/*
 * Utox - Shape (rectangle and relation) detection
 * 
 * Copyright (c) 2011, Jouke Witteveen <j.witteveen@gmail.com>
 * All rights reserved.
 * 
 * See README for more details.
 */

#include <cassert>
#include <queue>
#include <opencv2/highgui/highgui.hpp>  // Needed for imread().
#include <opencv2/imgproc/imgproc.hpp>
#include "detector.hpp"
#include "logging.hpp"

using std::queue;
namespace detector{


// TODO: Use the image-object dependant lineWidth instead of the one below.
int lineWidth = LINEWIDTH;


// NB: We use a squared Euclidean distance.
// Its result is integer and its computation does not involve a square root.
inline int distance( const Point &p1, const Point &p2 ){
  const int dx = p1.x - p2.x,
            dy = p1.y - p2.y;
  return dx * dx + dy * dy;
} //distance


// Converting a rectangle to a contour.
Contour rtoc( const Rect r ){
  Contour c;
  c.push_back( r.tl() );
  c.push_back( Point( r.x + r.width, r.y ) );
  c.push_back( r.br() );
  c.push_back( Point( r.x, r.y + r.height ) );
  return c;
} //rtoc


// Try to convert a contour to a rectangle.
bool ctor( const Contour c, Rect &r ){
  if( c.size() < 4 ) return false;
  Contour approx( 2 );
  int d, dMax = 0;

  // Find two points at maximum distance.
  for( Contour::const_iterator a = c.begin(); a + 1 < c.end(); ++a ){
    for( Contour::const_iterator b = a + 1; b < c.end(); ++b ){
      d = distance( *a, *b );
      if( d > dMax ) approx[0] = *a, approx[1] = *b, dMax = d;
    } //for
  } //for

  r = boundingRect( approx );
  int cArea = contourArea( c ),
      rArea = r.area();
  if( cArea < rArea * MINCOVER || rArea < cArea * MINCOVER ) return false;
  return true;
} //ctor


// Expand each side of a rectangle by a given amount.
Rect biggerRect( Rect r, int d ){
  r.x -= d;
  r.y -= d;
  r.width += 2 * d;
  r.height += 2 * d;
  return r;
} //biggerRect


// Determine the type of an arrow head at a point.
// The signature of an arrow head is a vector of numbers of corners of the
// contour of the arrow head between consecutive visits.
uml_arrow_type arrowType( const Point &p, const vector<int> &signature,
                          const vector<Point> &loose ){
  const int arrowsize2 = ARROWSIZE * ARROWSIZE;
  int count = 0;

  if( signature.size() == 0 ){
    for( vector<Point>::const_iterator h = loose.begin(); h != loose.end();
         ++h ){
      // NB arrowsize2 / 4 equals ARROWSIZE / 2 in Eucledian distance.
      if( 4 * distance( p, *h ) <= arrowsize2 ) ++count;
    } //for
    if( count < 2 ) return Link;
    else return Association;
  } //if

  for( vector<int>::const_iterator i = signature.begin(); i != signature.end();
       ++i ){
    if( *i > count ) count = *i;
  } //for
  switch( count ){
    case 0:
      return Composition;
    case 1:
      return Association;
    case 2:
      return Generalization;
    case 3:
    default:
      return Aggregation;
  } //switch
} //arrowType


#ifdef DEBUG  // TODO: remove me
vector<Contour> debugPrintC;
vector<Contour> debugPrintR;
#endif
// Recursively fill `children` with the children of `parent`, setting `parent`
// to the innermost representation of its equivalence class.
// Returns the number of descendants of `parent`.
int gatherChildren( int &parent, vector<nested_rectangles> &children,
                    vector<Rect> &rectangles, const vector<Contour> &contours,
                    const vector<Vec4i> &hierarchy ){
  if( hierarchy[parent][2] < 0 ) return 0;  // No children.
  int descendants = 0,
      index;
  queue<int> q;

  for( q.push( hierarchy[parent][2] ); !q.empty(); q.pop() ){
    index = q.front();
    if( hierarchy[index][0] >= 0 ) q.push( hierarchy[index][0] );

    if( !ctor( contours[index], rectangles[index] ) ){
      if( hierarchy[index][2] >= 0 ) q.push( hierarchy[index][2] );
      continue;  // Not rectangular.      
    } else if( rectangles[index].width < MINWIDTH
               || rectangles[index].height < MINHEIGHT ){
      continue;  // Too small.
    } else if( matchRect( rectangles[parent], rectangles[index] ) ){
      parent = index;  // Prefer the inner-most representation of the class.
      if( hierarchy[index][2] >= 0 ) q.push( hierarchy[index][2] );
      continue;  // Equivalent to parent.
    } //if

    children.push_back( nested_rectangles() );
    // The following line could change index to another class representative.
    children.back().descendants
      = gatherChildren( index, children.back().children, rectangles, contours,
                        hierarchy );
    children.back().rectangle = rectangles[index];
#ifdef DEBUG  // TODO: remove me
debugPrintC.push_back( contours[index] );
debugPrintR.push_back( rtoc( rectangles[index] ) );
#endif
    descendants += children.back().descendants + 1;
  } //for

  return descendants;
} //gatherChildren


///////////////////////////// Advertised functions /////////////////////////////

// Whether two rectangles are actually the same horizontal-wise.
bool matchRectX( const Rect &r1, const Rect &r2 ){
  return abs( r1.x - r2.x ) < HDELTA
         && abs( ( r1.x + r1.width ) - ( r2.x + r2.width ) ) < HDELTA;
} //matchRectX


// Whether two rectangles are actually the same vertical-wise.
bool matchRectY( const Rect &r1, const Rect &r2 ){
  return abs( r1.y - r2.y ) < VDELTA
         && abs( ( r1.y + r1.height ) - ( r2.y + r2.height ) ) < VDELTA;
} //matchRectY


// Collect rectangles in a bounding parent rectangle.
nested_rectangles fosterRect( vector<nested_rectangles> &children ){
  int descendants = children.size();
  Contour extremities;
  for( vector<nested_rectangles>::iterator c = children.begin();
       c != children.end(); ++c ){
    descendants += c->descendants;
    extremities.push_back( c->rectangle.tl() );
    extremities.push_back( c->rectangle.br() );
  } //for
  nested_rectangles foster( boundingRect( extremities ) );
  foster.children = children;
  foster.descendants = descendants;
  return foster;
} //fosterRect


////////////////////////////// image_object class //////////////////////////////

image_object::image_object( const char* fileName, const uml_type _umlType )
  : umlType( _umlType ){
  Mat _image = imread( fileName, 0 );
  assert( !_image.empty() );  // TODO: Find a better way to fail.

  // We work on a binary version of the image.
  adaptiveThreshold( _image, image, 255, THRESHMETH, THRESHPARITY, THRESHBLOCK,
                     THRESHDELTA );
} //image_object (constructor)


// Detected rectangles are stored in detection.rectangles in tree form.
// Child rectangles in the tree lie within their parent in the image.
int image_object::detectRectangles(){
  int index = 0;
  vector<Contour> contours;
  vector<Vec4i> hierarchy;

  findContours( image, contours, hierarchy, CV_RETR_TREE,
                CV_CHAIN_APPROX_SIMPLE );
  if( contours.empty() ) return -1;

  // Find the first root index in the hierarchy.
  while( hierarchy[index][3] >= 0 ) index = hierarchy[index][3];
  while( hierarchy[index][1] >= 0 ) index = hierarchy[index][1];
  // Add a super-parent.
  hierarchy.push_back( Vec4i( -1, -1, index, -1 ) );
  index = hierarchy.size() - 1;  // The index of our super-parent.
  vector<Rect> rectangles( hierarchy.size() );
  rectangles[index] = Rect( 0, 0, image.size().width, image.size().height );

  // Analyze the hierarchy of rectangles and store it as detection feature.
  int count =  gatherChildren( index, detection.rectangles, rectangles,
                               contours, hierarchy );
  writeLog( count );
  writeLog( " rectangles found.\n", true );

#ifdef DEBUG // TODO: remove me
Mat _image1 = image.clone(), _image2 = image.clone();
drawContours( _image1, debugPrintC, -1, 200, 2, CV_AA );
drawContours( _image2, debugPrintR, -1, 200, 2, CV_AA );
imshow( "Accepted contours", _image1 );
imshow( "Accepted rectangles", _image2 );
waitKey();
#endif
  return 0;
} //image_object::detectRectangles


int image_object::detectLineWidth(){
  int sum = 0,
      count = 0,
      bottom;
  for( vector<nested_rectangles>::iterator nr = detection.rectangles.begin();
       nr != detection.rectangles.end(); ++nr ){
    bottom = nr->children.front().rectangle.y
             + nr->children.front().rectangle.height;
    for( vector<nested_rectangles>::iterator r = nr->children.begin();
         ++r != nr->children.end();
         bottom = r->rectangle.y + r->rectangle.height ){
      if( r->rectangle.y - bottom < VDELTA ){
        sum += r->rectangle.y - bottom;
        ++count;
      } //if
    } //for
    if( nr->rectangle.y < nr->children.front().rectangle.y ){
      sum += nr->children.front().rectangle.y + nr->children.back().rectangle.y
             + nr->children.back().rectangle.height - 2 * nr->rectangle.y
             - nr->rectangle.height;
      count += 2;
    } //if
  } //for
  detection.lineWidth = ceil( (float) sum / count ) + 2;
  writeLog( detection.lineWidth );
  writeLog( " is the detected line width.\n", true );
  return detection.lineWidth;
} //image_object::detectLineWidth


// Fill a vector of tuples of indices of toplevel rectangles that are connected
// and record their types (the arrow is on the first index).
void image_object::detectRelations( vector<Vec2i> &relations,
                                    vector<uml_arrow_type> &types ){
  const int arrowsize2 = ARROWSIZE * ARROWSIZE,
            linewidth2 = LINEWIDTH * LINEWIDTH;
  int i, last, sinceLast;
  Point previous;
  Contour c;
  vector<Contour> contours;
  // Features that touch only one class.
  vector<vector<Point> > looseVisits( detection.rectangles.size() );
  vector<int> visits;  // Classes a feature touches.
  vector<vector<int> > visitSignature;  // Characteristicts of each touch.
  vector<Point> visitHandle;  // Location of the touch.
  vector<vector<int> > relationSignature0,
                       relationSignature1;
  vector<Point> relationHandle0,
                relationHandle1;
#ifdef DEBUG  // TODO: remove me
vector<Contour> poly;
#endif

  relations.clear();
  types.clear();
  // Clear the rectangles.
  for( vector<nested_rectangles>::iterator r = detection.rectangles.begin();
       r != detection.rectangles.end(); ++r ){
    rectangle( image, biggerRect( r->rectangle, LINEWIDTH ), 0, CV_FILLED );
  } //for

  findContours( image, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );

  // For each contour, find out which rectangles it touches.
  // Visits to the same rectangle are different if the path between them is at
  // some point more than ARROWSIZE away from the first visit.
  for( vector<Contour>::iterator ci = contours.begin(); ci != contours.end();
       ++ci ){
    approxPolyDP( *ci, c, 1, true );
#ifdef DEBUG  // TODO: remove me
poly.push_back( Contour() );
#endif
    last = -1;
    for( Contour::iterator p = c.begin(); p != c.end(); ++p ){
      if( last != -1 ){
        if( distance( visitHandle.back(), *p ) > arrowsize2 ) last = -1;
        else ++sinceLast;
      } //if
      i = atRect( *p );
      if( i == -1 ){
        // NB: 2 * linewidth2 equals sqrt(2) * LINEWIDTH in Eucledian distance.
        if( last != -1 && sinceLast > 0
            && distance( previous, *p ) <= 2 * linewidth2 ){
          --sinceLast;
        } else previous = *p;
#ifdef DEBUG  // TODO: remove me
poly.back().push_back( previous );
#endif
        continue;
      } //if
      if( i != last ){
        visits.push_back( i );
        visitSignature.push_back( vector<int>() );
        visitHandle.push_back( *p );
        last = i;
      } else if( sinceLast > 0 || distance( previous, *p ) > linewidth2 ){
        visitSignature.back().push_back( sinceLast );
      } //if
      previous = *p;
      sinceLast = -1;
#ifdef DEBUG  // TODO: remove me
poly.back().push_back( previous );
#endif
    } //for
    // The last and first visit might be the same visit.
    if( visits.size() > 1 && visits.front() == visits.back() && last != -1
        && distance( visitHandle.back(), visitHandle.front() ) <= arrowsize2 ){
      for( Contour::iterator p = c.begin(); *p != visitHandle.front(); ++p ){
        if( distance( visitHandle.back(), *p ) > arrowsize2 ){
          last = -1;
          break;
        } else if( distance( previous, *p ) > linewidth2 ){
          ++sinceLast;
          previous = *p;
        } //if
      } //for
      if( last != -1 ){
        visits.pop_back();
        visitSignature.back().push_back( sinceLast + 1 );
        visitSignature.front().insert( visitSignature.front().begin(),
                                       visitSignature.back().begin(),
                                       visitSignature.back().end() );
        visitSignature.pop_back();
        visitHandle.front() = visitHandle.back();
        visitHandle.pop_back();
      } //if
    } //if

    if( visits.size() == 0 ){  // Isolated contour.
    } else if( visits.size() == 1 ){
      looseVisits[visits.front()].push_back( visitHandle.front() );
    } else if( visits.size() == 2 ){
      writeLog( "Simple connection found.\n" );
      relations.push_back( Vec2i( visits.front(), visits.back() ) );
      relationSignature0.push_back( visitSignature.front() );
      relationHandle0.push_back( visitHandle.front() );
      relationSignature1.push_back( visitSignature.back() );
      relationHandle1.push_back( visitHandle.back() );
    } else if( ( i = treeRoot( visits ) ) != -1 ){
      writeLog( "Tree form connection found.\n" );
      last = visits.size();
      for( int j = 0; j < last; ++j ){
        if( j == i ) continue;
        relations.push_back( Vec2i( visits[i], visits[j] ) );
        relationSignature0.push_back( visitSignature[i] );
        relationHandle0.push_back( visitHandle[i] );
        relationSignature1.push_back( visitSignature[j] );
        relationHandle1.push_back( visitHandle[j] );
      } //for
    } else if( visits.size() == 4 ){
      writeLog( "Crossing connection found.\n" );
      relations.push_back( Vec2i( visits[0], visits[2] ) );
      relations.push_back( Vec2i( visits[1], visits[3] ) );
      relationSignature0.push_back( visitSignature[0] );
      relationHandle0.push_back( visitHandle[0] );
      relationSignature0.push_back( visitSignature[2] );
      relationHandle0.push_back( visitHandle[2] );
      relationSignature1.push_back( visitSignature[1] );
      relationHandle1.push_back( visitHandle[1] );
      relationSignature1.push_back( visitSignature[3] );
      relationHandle1.push_back( visitHandle[3] );
    } else{
      // TODO: Try line detection in the unprocessed contour.
      writeLog( "A feature could not be processed.\n" );
    } //if
    visits.clear();
    visitSignature.clear();
    visitHandle.clear();
  } //for
#ifdef DEBUG  // TODO: remove me
drawContours( image, poly, -1, 250, 1, CV_AA );
imshow( "Considered connections", image );
waitKey();
#endif

  // Determine relation types.
  for( size_t n = 0; n < relations.size(); ++n ){
    types.push_back( arrowType( relationHandle0[n], relationSignature0[n],
                                looseVisits[relations[n][0]] ) );
    if( types.back() == Link ){
      types.back() = arrowType( relationHandle1[n], relationSignature1[n],
                                looseVisits[relations[n][1]] );
      int swap = relations[n][0];
      relations[n][0] = relations[n][1];
      relations[n][1] = swap;
    } //if
  } //for
} //image_object::detectRelations


// Find the index of a rectangle neighbouring a point.
int image_object::atRect( const Point p ){
  for( size_t n = 0; n < detection.rectangles.size(); ++n ){
    if( p.inside( biggerRect( detection.rectangles[n].rectangle,
                              LINEWIDTH + 2 ) ) ){
      return n;
    } //if
  } //for
  return -1;
} //atRect


// When all rectangles but one are at the same vertical position, we speak of a
// tree. At the moment only downward trees are supported.
// Return the index of the root or -1 if the rectangles do not form a tree.
int image_object::treeRoot( vector<int> &indices ){
  size_t root,
         top,  // Of the children.
         bottom;
  if( detection.rectangles[indices[0]].rectangle.y
        < detection.rectangles[indices[1]].rectangle.y ){
    root = 0;
    top = 1;
    bottom = 1;
  } else{
    root = 1;
    top = 0;
    bottom = 0;
  } //if

  for( size_t i = 2; i < indices.size(); ++i ){
    if( detection.rectangles[indices[i]].rectangle.y
          < detection.rectangles[indices[root]].rectangle.y ){
      top = root;
      root = i;
    } else if( detection.rectangles[indices[i]].rectangle.y
                 < detection.rectangles[indices[top]].rectangle.y ){
      top = i;
    } else if( detection.rectangles[indices[i]].rectangle.y
                 > detection.rectangles[indices[bottom]].rectangle.y ){
      bottom = i;
    } //if
  } //for
  if( detection.rectangles[indices[bottom]].rectangle.y
        - detection.rectangles[indices[top]].rectangle.y < VDELTA
      && detection.rectangles[indices[top]].rectangle.y
           - detection.rectangles[indices[root]].rectangle.y >= VDELTA ){
    return root;
  } //if
  return -1;
} //image_object::isTree


} //namespace detector
