/*
 * Utox - Convert
 * 
 * Copyright (c) 2012, Michiel Vos <michiel@fastfox.nl>
 * All rights reserved.
 *
 * See README for more details.
 */

#include "logging.hpp"
#include "convert.hpp"
#include <stdlib.h>

// Convert the input file (for at least Linux Mint 12.)
void convert( char* &inFile ){
  writeLog( std::string( "Converting ") + std::string( inFile ) + std::string( " to input.png\n" ) );

	// Make a system call to convert (with ImageMagic) the input into a readable input file for Utox.
	system( std::string( std::string( "convert " ) + std::string( inFile ) + std::string( " -background white -flatten input.png" ) ).c_str( ) );

  // Replace the original file with the converted file.
	inFile = "input.png";
} //convert
