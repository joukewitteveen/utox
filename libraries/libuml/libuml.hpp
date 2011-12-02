/*
 * libuml - stopgap code awaiting implementation
 * 
 * Copyright (c) 2011, Jouke Witteveen <j.witteveen@gmail.com>
 * All rights reserved.
 * 
 * See README for more details.
 */

#ifndef LIBUML_HPP
#define LIBUML_HPP

// Types of UML diagrams
enum uml_type {
  Class,
  Sequence
}; //uml_type

enum uml_arrow_type {
  Link,  // Undirected association.
  Association,  // Directed association.
  Generalization,
  Composition,
  Aggregation
}; //uml_arrow_type

// General UML type
class uml_t {
  public:
    uml_t( const uml_type t ) : umlType( t ){}
    uml_type umlType;
    virtual int toXmi( char* );
};

// UML Class diagram type
class uml_classm_t : public uml_t {
  public:
    uml_classm_t() : uml_t( Class ){}
    int toXmi( char * );
};

#endif //LIBUML_HPP
