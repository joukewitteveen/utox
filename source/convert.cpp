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
#include <fstream>

// Convert the input file (for at least Linux Mint 12.)
bool convert( char* &inFile, char* &convertFile, const bool &overwrite ){

	// Send action to the log.
	writeLog( "Converting...\n" );

	// Read possible already converted file
	std::ifstream file( std::string( convertFile ).c_str( ) );

	if( file ){ // Converted file exists
		writeLog( std::string( convertFile ) + std::string( " already exists.\n" ) );
		if( overwrite ){ // But must overwrite
			writeLog( "But will overwrite original file.\n" );
		} else{ // Must not overwrite
			writeLog( "Will not overwrite original file,\nPlease give an other name in the instructions.\n" );
			return 0;
		} //if
	} else{ // File not exists
		writeLog( std::string( convertFile ) + std::string( " does not exist yet.\n" ) );
	} //if
		 

	if( inFile != NULL && ( !file || overwrite ) ){ // inFile is not NULL and File not exists or must overwrite.
		// Read input file
		std::ifstream file( std::string( inFile ).c_str() );

		if( !file ){ // File not exists
			writeLog( std::string( std::string( inFile ) + std::string( " doesn't exists.\n" ) ) );
			return 0;
		} else{ // File exists, so convert
			// Make a system call to convert (with ImageMagic) the input into a readable input file for Utox.
			system( std::string( std::string( "convert " ) + std::string( inFile ) + std::string( " -background white -flatten " ) + std::string( convertFile ) ).c_str( ) );
			writeLog( std::string( "Converted to " ) + std::string( convertFile ) + std::string( ".\n" ) );
		} //if

	} //if
	return 1;
} //convert
