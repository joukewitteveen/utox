/*
 * Utox - image-to-uml LIACS Software Enginneering project
 * 
 * Copyright (c) 2011, Jouke Witteveen <j.witteveen@gmail.com>
 * All rights reserved.
 * 
 * See README for more details.
 */

#include <iostream>
#include "logic.hpp"
#include "logging.hpp"
#include "convert.hpp"
#include "scan.hpp"

using namespace std;

bool loggingVerbose;
bool overwrite = false;

void usage(){
  cout << "Usage: utox [-v] [-w] [-f input-file] -o output-file [-c convertFile] [-a scanFile]" << endl
       << "Detect the UML structure depicted in input-file and write it to" << endl
       << "output-file in XMI representation." << endl << endl
       << "  -v\tincrease verbosity" << endl
			 << "  -w\toverwrite image if already exists\n" << endl
			 << "  -f\tconvert input-file to output-file" << endl
			 << "  -c\tconvert input-file to convert-file (without extension)" << endl
			 << "  -a\tscan image and save as save-File (without extension)" << endl;
} //usage


int main( int argc, char *argv[] ){
  char* inFile = NULL,
      * outFile = NULL,
			* scanFile = NULL,
			* convertFile = NULL;
			
  int opt;
  uml_type umlType = Class;  // Default for now.
  extern char* optarg;
  extern int optind,
             optopt;

  // More options to come! (Type of UML diagram)
  while( ( opt = getopt( argc, argv, "f:o:vha:c:w" ) ) != -1 ){
    switch( opt ){
      case 'f':  // Input file.
        // TODO: What if already specified? Currently: use latest.
        inFile = optarg;
        // TODO: Do some checks (existence)?
        break;
      case 'o':  // Output file.
        // TODO: What if already specified? Currently: use latest.
        outFile = optarg;
        // TODO: Do some checks?
        break;
      case 'v':  // Verbose.
        loggingVerbose = true;
        break;
      // Note: getopt yields '?' on missing arguments so don't use it for help.
      case 'h':  // Help.
        usage();
        return 0;
      case 'c':  // Convert the image 
				convertFile = optarg;
				if( !convert( inFile, convertFile, overwrite ) ){ // No input-file
					return 1;
				} //if
				inFile = convertFile;
				break;
			case 'a': // Use the scanner and convert the image
				scanFile = optarg;
				if( scan( scanFile, overwrite ) ){
					// convert( scanFile, convertFile, overwrite );
					// inFile = convertFile;
					inFile = scanFile;
				} else{ // No scan device
					return 1;
				} //if
				break;
			case 'w': // force overwrite
				overwrite = true;
				break;
      default:
        cerr << "Ouch, what just happend? (" << (char) optopt << ")" << endl;
        //return 1  // Fatal.
    } //switch
  } //while
  if( optind < argc ){
    cerr << "Unprocessed options present." << endl;
    usage();
    return 1;  // Fatal.
  } //if
  if( inFile == NULL || outFile == NULL ){
    usage();
    return 1;
  } //if

  uml_t* uml = extractUml( inFile, umlType );
  //TODO: Check result
  writeLog( "Writing XMI to \"" );
  writeLog( outFile );
  writeLog( "\".\n", true );
  //uml->toXmi( outFile );
  //TODO: Check result

  return 0;
} //main
