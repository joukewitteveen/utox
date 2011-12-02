/*
 * Utox - Detection logic core
 * 
 * Copyright (c) 2011, Jouke Witteveen <j.witteveen@gmail.com>
 * All rights reserved.
 * 
 * See README for more details.
 */

#include "logic.hpp"
#include "logic/class.hpp"
#include "logging.hpp"


uml_t* extractUml( char* fileName, const uml_type umlType ){
  switch( umlType ){
    case Class:
      return extractClassM( fileName, umlType );
    default:
      writeLog( "Not implemented.\n" );
      return NULL;
  } //switch
} //extractUml
