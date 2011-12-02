/*
 * Utox - Detection logic for class diagrams
 * 
 * Copyright (c) 2011, Jouke Witteveen <j.witteveen@gmail.com>
 * Copyright (c) 2011, Mathijs van de Nes <mathijs.nes@gmail.com>
 * All rights reserved.
 * 
 * See README for more details.
 */

#ifndef LOGIC_CLASS_HPP
#define LOGIC_CLASS_HPP

#include <list>
#include "common.hpp"

using std::list;

void classRectExtend( vector<nested_rectangles>&, list<detector::Rect>&,
                      list<detector::Rect>::iterator );

vector<nested_rectangles> filterClassRect( vector<nested_rectangles>& );

uml_classm_t* extractClassM( char*, const uml_type );

#endif //LOGIC_CLASS_HPP
