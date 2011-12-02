/*
 * libuml - Class model part of the UML library
 * 
 * Copyright (c) 2011, Mathijs van de Nes <mathijs.nes@gmail.com>
 * All rights reserved.
 * 
 * See README for more details.
 */

#ifndef LIBUML_CLASSMODEL_HPP
#define LIBUML_CLASSMODEL_HPP

#include "libuml.hpp"

#include <string>
#include <vector>
#include <utility> //pair

#include <tinyxml/tinyxml.h>

using std::string;
using std::vector;
using std::pair;

namespace libuml {

	struct uml_classm_operation_t {
		uml_visibility_type visibility;
		string name;
		vector< pair<string, string> > arguments;
		string returntype;
		TiXmlElement* getXmi(const string &) const;
	};

	struct uml_classm_attribute_t {
		uml_visibility_type visibility;
		string name;
		string type;
		TiXmlElement* getXmi(const string &) const;
	};

	class uml_classm_class_t {
	public:
		// functions
		uml_classm_class_t(const string &);
		void addOperation(const uml_classm_operation_t &);
		void addAttribute(const uml_classm_attribute_t &);
		TiXmlElement* getXmi(const string &) const;
		// attributes
		string name;
	private:
		vector<uml_classm_operation_t> operations;
		vector<uml_classm_attribute_t> attributes;
	};

	// UML Class diagram type
	class uml_classm_t : public uml_t {
	public:
		// functions
		uml_classm_t() : uml_t(Class) {}
		void addClass(const uml_classm_class_t&);
		void clearClasses();
		virtual int toXmi(char*);
	private:
		//attributes
		vector<uml_classm_class_t> classes;
	};

} //namespace libuml

#endif // LIBUML_CLASSMODEL_HPP
