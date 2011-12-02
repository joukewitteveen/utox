/*
 * Utox - Text detection
 * 
 * Copyright (c) 2011, Jouke Witteveen <j.witteveen@gmail.com>
 * All rights reserved.
 * 
 * See README for more details.
 */

#ifndef READER_HPP
#define READER_HPP

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

namespace reader{
using namespace tesseract;

struct Rect {
  Rect(){}
  Rect( int l, int t, int w, int h ) : left( l ), top( t ), width( w ),
                                       height( h ){}
  union {
    int left;
    int x;
  };
  union {
    int top;
    int y;
  };
  int width;
  int height;
};

class image_object {
  public:
    image_object( const char*, const char* = NULL );
    ~image_object();
    char* readText( Rect );
  private:
    Pix *image;
    TessBaseAPI tesseract;
}; //image_object

char* explodeLines( char* = NULL );

void trimEnd( char* );

} //namespace reader

#endif //READER_HPP
