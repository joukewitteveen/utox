/*
 * Utox - Logging functionality
 * 
 * Copyright (c) 2011, Jouke Witteveen <j.witteveen@gmail.com>
 * All rights reserved.
 * 
 * See README for more details.
 */

#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <iostream>


extern bool loggingVerbose;


template <typename T>
void writeLog( const T& message, bool flush = false ){
  if( loggingVerbose ){
    std::cout << message;
    if( flush ) std::cout << std::flush;
  } //if
} //writeLog

#endif //LOGGING_HPP
