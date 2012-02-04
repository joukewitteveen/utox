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

void scan( char* &inFile ) {
	writeLog( "Scanning...\n" );
	inFile = "input.tiff";
} //scan
