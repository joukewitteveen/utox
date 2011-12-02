/*
 * Utox - Text detection
 * 
 * Copyright (c) 2011, Jouke Witteveen <j.witteveen@gmail.com>
 * All rights reserved.
 * 
 * See README for more details.
 */

#include <cassert>
#include "reader.hpp"

#define TESSDATAPATH "./"  // The parent directory of tessdata ending in '/'.
#define PIXSCALE 2.5  // Tesseract expects high dpi.

namespace reader{


image_object::image_object( const char* fileName, const char* lang )
  : image( pixScale( pixRead( fileName ), PIXSCALE, PIXSCALE ) ){
  assert( image != NULL );  // TODO: Find a better way to fail.
  tesseract.Init( TESSDATAPATH, lang );
  tesseract.SetImage( image );
  tesseract.SetVariable( "tessedit_char_whitelist",
    "+-#~/:(),<>&* _0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" );
} //image_object (constructor)


image_object::~image_object(){
  tesseract.End();
  delete image;
} //image_object (destructor)


char* image_object::readText( Rect r ){
  tesseract.SetRectangle( PIXSCALE * ( r.left + 2 ), PIXSCALE * ( r.top + 2 ),
                          PIXSCALE * ( r.width - 4 ),
                          PIXSCALE * ( r.height - 4 ) );
  return tesseract.GetUTF8Text();
} //image_object::readText


// In place exploding a string into its lines similar to the old strtok.
// This is not thread-safe (it uses a static variable).
char* explodeLines( char *current ){
  static char *next = NULL;
  if( current ) next = current;
  else current = next;

  if( !next || !*next ) return NULL;
  while( *++next ){
    if( *next == '\n' ){
      *next = '\0';
      while( *++next == '\n' );  // The next non-empty line.
      break;
    } //if
  } //while
  return current;
} //explodeLines


void trimEnd( char *s ){
  if( !s ) return;
  for( char *p = s; *p; ++p ){
    switch( *p ){
      case '\n':
      case ' ':
        break;
      default:
        s = p + 1;  // After the last seen non-whitespace character.
    } //switch
  } //for
  *s = '\0';
} //trimEnd


} //namespace reader
