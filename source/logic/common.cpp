/*
 * Utox - Detection logic common functionality
 * 
 * Copyright (c) 2011, Jouke Witteveen <j.witteveen@gmail.com>
 * All rights reserved.
 * 
 * See README for more details.
 */

#include "common.hpp"
#include "../detector.hpp"
#include "../reader.hpp"
using detector::nested_rectangles;


// TODO: supersede me.
void treePrint( vector<nested_rectangles> &r, int l ){
  for( vector<nested_rectangles>::iterator s = r.begin(); s != r.end(); ++s ){
    for( int i = 0; i < l; ++i ) writeLog( "|" );
    writeLog( "+*\n" );
    treePrint( s->children, l + 1 );
  } //for
} //treePrint
void leafRead( reader::image_object &i, vector<nested_rectangles> &r ){
  for( vector<nested_rectangles>::iterator s = r.begin(); s != r.end(); ++s ){
    if( s->descendants == 0 ){
      writeLog( i.readText( rectToRect( s->rectangle ) ) );
    } else leafRead( i, s->children );
  } //for
} //leafRead


// To improve OCR quality (invoke the adaptive classifier).
void testRead( reader::image_object &i, const vector<nested_rectangles> &nr ){
  for( vector<nested_rectangles>::const_iterator r = nr.begin(); r != nr.end();
       ++r ){
    if( r->descendants == 0 ) i.readText( rectToRect( r->rectangle ) );
    else testRead( i, r->children );
  } //for
} //testRead


reader::Rect rectToRect( const detector::Rect r ){
  return reader::Rect( r.x, r.y, r.width, r.height );
} //rectToRect


bool yOrderR( const detector::Rect &r1, const detector::Rect &r2 ){
  return r1.y < r2.y;
} //yOrderR
