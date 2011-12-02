/*
 * libuml - stopgap code awaiting implementation
 * 
 * Copyright (c) 2011, Jouke Witteveen <j.witteveen@gmail.com>
 * All rights reserved.
 * 
 * See README for more details.
 */

#include "libuml.hpp"


int uml_t::toXmi( char* fileName ){
  switch( umlType ){
    case Class:
      return ( (uml_classm_t*) this )->toXmi( fileName );
    default:
      return -1;
  } //switch
} //uml_t::toXmi


int uml_classm_t::toXmi( char* fileName ){
  return -1;
} //uml_classm_t::toXmi
