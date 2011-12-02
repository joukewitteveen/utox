/*
 * Utox - Detection logic for class diagrams
 * 
 * Copyright (c) 2011, Jouke Witteveen <j.witteveen@gmail.com>
 * Copyright (c) 2011, Mathijs van de Nes <mathijs.nes@gmail.com>
 * All rights reserved.
 * 
 * See README for more details.
 */

#include <algorithm>
#include <string>
#include "class.hpp"

using std::string;


// Add rectangles to the bottom of a rectangle vector when they align nicely.
void classRectExtend( vector<nested_rectangles> &classRect,
                      list<detector::Rect> &haystack,
                      list<detector::Rect>::iterator begin ){
  const int bottom = classRect.back().rectangle.y
                     + classRect.back().rectangle.height;
  bool mismatch;
  for( list<detector::Rect>::iterator r = begin; r != haystack.end(); ++r ){
    if( r->y < bottom ) continue;
    else if( r->y >= bottom + 2 * VDELTA ) break;
    mismatch = false;
    for( vector<nested_rectangles>::iterator nr = classRect.begin();
         nr != classRect.end(); ++nr ){
      if( !detector::matchRectX( *r, nr->rectangle ) ){
        mismatch = true;
        break;
      } //if
    } //for
    if( !mismatch ){
      classRect.push_back( nested_rectangles( *r ) );
      begin = haystack.erase( r );
      classRectExtend( classRect, haystack, begin );
      return;
    } //if
  } //for
} //classRectExtend


// Gather all classes in all levels of the hierarchy and place them at top level
// in the returned hierarchy.
// No limit is placed on the number of subrectangles of a class.
vector<nested_rectangles> filterClassRect(
  vector<nested_rectangles> &hierarchy ){
  vector<nested_rectangles> filtered,
                            intermediate;
  list<detector::Rect> haystack;

  // PART I: Pile up the haystack.
  for( vector<nested_rectangles>::iterator nr = hierarchy.begin();
       nr != hierarchy.end(); ++nr ){

    if( nr->descendants == 0 ){  // Add leafs to the search space.
      haystack.push_back( nr->rectangle );
      continue;
    } else if( nr->descendants == 1 ){  // Ignore lone rectangles.
       continue;
    } else if( nr->descendants == (int) nr->children.size() ){
      // A node might represent an intact class.
      // We are mild and accept based on widths only.
      vector<nested_rectangles>::iterator child = nr->children.begin(),
                                          child_end = nr->children.end();
      do{
        if( !detector::matchRectX( nr->rectangle, *child ) ) break;
      } while( ++child < child_end );
      if( child == child_end ){  // All children match.
        std::sort( nr->children.begin(), child_end, yOrderR );
        filtered.push_back( *nr );
        continue;
      } //if
    } //if

    // Recurse
    intermediate = filterClassRect( nr->children );
    filtered.insert( filtered.end(), intermediate.begin(), intermediate.end() );
  } //for

  // PART II: Mine the haystack.
  haystack.sort( yOrderR );

  // Try to find a class under each rectangle.
  while( !haystack.empty() ){
    intermediate.clear();
    intermediate.push_back( nested_rectangles( haystack.front() ) );
    haystack.pop_front();
    classRectExtend( intermediate, haystack, haystack.begin() );
    if( intermediate.size() == 1 ) continue;
    filtered.push_back( detector::fosterRect( intermediate ) );
  } //while

  return filtered;
} //filterClassRect


// Class model extraction entry routine. [development is in the libuml branch]
uml_classm_t* extractClassM( char* fileName, const uml_type umlType ){
  vector<nested_rectangles> detectedClassRects;
  vector<detector::Vec2i> relations;
  vector<uml_arrow_type> types;
  detector::image_object image( fileName, umlType );

  if( image.detectRectangles() != 0 ){
    writeLog( "Error running detectRectangles.\n", true );
    return NULL;
  } //if
  image.detection.rectangles = filterClassRect( image.detection.rectangles );
  writeLog( image.detection.rectangles.size() );
  writeLog( " classes found.\n", true );

  reader::image_object i( fileName );
  testRead( i, image.detection.rectangles );

  detector::lineWidth = image.detectLineWidth();
  image.detectRelations( relations, types );
  char *className;
  for( size_t n = 0; n < relations.size(); ++n ){
    writeLog( "Class \"" );
    className = i.readText( rectToRect(
      image.detection.rectangles[relations[n][0]].children[0].rectangle ) );
    reader::trimEnd( className );
    writeLog( className );
    writeLog( "\" is " );
    switch( types[n] ){
      case Association:
        writeLog( "associated with" );
        break;
      case Generalization:
        writeLog( "a generalization of" );
        break;
      case Composition:
        writeLog( "composed of" );
        break;
      case Aggregation:
        writeLog( "an aggregation of" );
        break;
      default:
        writeLog( "linked with" );
    } //switch
    writeLog( " class \"" );
    className = i.readText( rectToRect(
      image.detection.rectangles[relations[n][1]].children[0].rectangle ) );
    reader::trimEnd( className );
    writeLog( className );
    writeLog( "\".\n", true );
  } //for

  // TODO: merge libuml branch (further development is done there).
/*treePrint( image.detection.rectangles );
leafRead( i, image.detection.rectangles );*/

  return NULL;
} //extractClassM
