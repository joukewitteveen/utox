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
#include <stdio.h>
#include <string.h>
#include <fstream>

// Scan a image
bool scan( char* &scanFile, const bool &overwrite ){
	
	// Send action to the log.
	writeLog( "Scanning...\n" );

	// Read file
	std::ifstream file( std::string( scanFile ).c_str() );

	if( file ){ // File exists
		writeLog( std::string( scanFile ) + std::string( " already exists.\n" ) );
		if( overwrite ){ // Must overwrite
			writeLog( "But will overwrite original file.\n" );
		} else{ // Must not overwrite
			writeLog( "Will not overwrite original file,\nPlease give an other name in the instructions.\n" );
			return 0;
		} //if
	} else{ // File not exists
		writeLog( std::string( std::string( scanFile ) + std::string( " does not exist yet.\n" ) ) );
	} //if
	
	if( !file || overwrite ){ // File not exists or must overwrite.
		FILE *in;
		char buff[512];

		// Try to scan
	  in = popen( std::string( std::string( "scanimage --format=tiff " ) + std::string( scanFile ) + std::string( " 2>&1" ) ).c_str(), "r" );
		while( fgets( buff, sizeof( buff ), in ) != NULL ) {
		} //while
		pclose( in );
	
		std::string result = buff; // Output from scanimage

		// Check if the image is scanned or if there is no scan device
		if( result.compare( "scanimage: no SANE devices found" ) == 1 ){
			writeLog( "No scanners found.\n" );
			return 0;
		} else{
			writeLog( "Image is scanned.\n" );
		} //if
	} //if

	return 1;
} //scan
