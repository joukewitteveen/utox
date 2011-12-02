/*
 * Utox - Detection logic common functionality
 * 
 * Copyright (c) 2011, Jouke Witteveen <j.witteveen@gmail.com>
 * All rights reserved.
 * 
 * See README for more details.
 */

#ifndef LOGIC_COMMON_HPP
#define LOGIC_COMMON_HPP

#include "../detector.hpp"
#include "../reader.hpp"
#include "../logging.hpp"
 
using std::vector;
using detector::nested_rectangles;

void treePrint( vector<nested_rectangles>&, int = 0 );
void leafRead( reader::image_object&, vector<nested_rectangles>& );

void testRead( reader::image_object&, const vector<nested_rectangles>& );

reader::Rect rectToRect( const detector::Rect );

bool yOrderR( const detector::Rect&, const detector::Rect& );

bool yOrderNR( const nested_rectangles&, const nested_rectangles& );

#endif //LOGIC_COMMON_HPP
