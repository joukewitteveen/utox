/*
 * Utox - Shape (rectangle and relation) detection
 * 
 * Copyright (c) 2011, Jouke Witteveen <j.witteveen@gmail.com>
 * All rights reserved.
 * 
 * See README for more details.
 */

#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <vector>
#include <opencv2/core/core.hpp>
#include <libuml/libuml.hpp>

// Parameter values. These values are best guesses.
// Any of these is eligible for being tweakable at runtime.
#define LINEWIDTH 4  // A priori line width of the borders of rectangles.
// Maximal diameter of arrowheads.
#define ARROWSIZE ( 8 * detector::lineWidth )
// Minimum horizontal distance to be considered different.
#define HDELTA ( 3 * detector::lineWidth )
#define VDELTA HDELTA  // As above, but vertical.
// Equivalence of rectangles is well behaved if the following exceed their
// respective delta's.
#define MINWIDTH ( 6 * HDELTA )  // Minimum width of a rectangle.
#define MINHEIGHT ( VDELTA + 2 )  // Minimum height of a rectangle.
#define MINCOVER .92  // Minimum overlap of contours and derived rectangles.
// You should really know what you are doing when changing the following values.
#define THRESHMETH ADAPTIVE_THRESH_GAUSSIAN_C  // Image threshold method.
#define THRESHPARITY THRESH_BINARY_INV  // Parity (binary or binary inverse).
#define THRESHBLOCK 13  // Threshold calculation block size.
#define THRESHDELTA 12  // Additive constant for the threshold method.

namespace detector {
using namespace cv;

typedef std::vector<Point> Contour;

struct nested_rectangles {
  nested_rectangles(){}
  nested_rectangles( const Rect r ) : rectangle( r ), descendants( 0 ){}
  operator const Rect&() const { return rectangle; }
  Rect rectangle;
  int descendants;
  std::vector<nested_rectangles> children;
}; //nested_rectangles

struct features {
  features() : lineWidth( LINEWIDTH ){}
  std::vector<nested_rectangles> rectangles;
  int lineWidth;
}; //features
// TODO: Use the image-object dependant lineWidth instead of the one below.
extern int lineWidth;  // A thread-unsafe, global version of the above.

class image_object {
  public:
    image_object( const char*, const uml_type );
    int detectRectangles();
    int detectLineWidth();
    void detectRelations( vector<Vec2i>&, vector<uml_arrow_type>& );
    const uml_type umlType;
    features detection;
  private:
    int atRect( const Point );
    int treeRoot( vector<int>& );
    Mat image;
}; //image_object

bool matchRectX( const Rect&, const Rect& );

bool matchRectY( const Rect&, const Rect& );

inline bool matchRect( const Rect &r1, const Rect &r2 ){
  return matchRectX( r1, r2 ) && matchRectY( r1, r2 );
} //matchRect

nested_rectangles fosterRect( vector<nested_rectangles>& );

} //namespace detector

#endif //DETECTOR_HPP
