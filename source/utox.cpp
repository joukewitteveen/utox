/*
 * Utox - image-to-uml LIACS Software Enginneering project
 * 
 * Copyright (c) 2011, Jouke Witteveen <j.witteveen@gmail.com>
 * All rights reserved.
 * 
 * See README for more details.
 */

#include <unistd.h>
#include <iostream>
#include "logic.hpp"
#include "logging.hpp"

using namespace std;


bool loggingVerbose;


void usage(){
  cout << "Usage: utox [-v] -f input-file -o output-file" << endl
       << "Detect the UML structure depicted in input-file and write it to" << endl
       << "output-file in XMI representation." << endl << endl
       << "  -v\tincrease verbosity" << endl;
} //usage


int main( int argc, char *argv[] ){
  char* inFile = NULL,
      * outFile = NULL;
  int opt;
  uml_type umlType = Class;  // Default for now.
  extern char* optarg;
  extern int optind,
             optopt;

  // More options to come! (Type of UML diagram)
  while( ( opt = getopt( argc, argv, "f:o:vh" ) ) != -1 ){
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
