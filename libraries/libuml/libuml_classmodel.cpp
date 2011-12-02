/*
 * libuml - Class model part of the UML library
 * 
 * Copyright (c) 2011, Mathijs van de Nes <mathijs.nes@gmail.com>
 * All rights reserved.
 * 
 * See README for more details.
 */

#include "libuml_classmodel.hpp"

#include <fstream>
#include <sstream>

using std::ofstream;
using std::stringstream;

namespace libuml {

	TiXmlElement* uml_classm_operation_t::getXmi(const string &id) const {
		TiXmlElement *op = new TiXmlElement("UML:Operation");
		op->SetAttribute("name", name.c_str());
		op->SetAttribute("xmi.id", id);
		op->SetAttribute("visibility", visToStr(visibility));
		
		TiXmlElement *parameters = new TiXmlElement("UML:BehavorialFeature.parameter");

		stringstream ss;
		// parameters
		for (size_t i = 0; i < arguments.size(); i++) {
			TiXmlElement *parameter = new TiXmlElement("UML:Parameter");
			ss << id << ":p" << i;
			parameter->SetAttribute("xmi.id", ss.str().c_str());
			ss.str("");
			parameter->SetAttribute("name", arguments[i].first.c_str());
			parameter->SetAttribute("kind", "in");

			TiXmlElement *umltype = new TiXmlElement("UML:Parameter.type");
			TiXmlElement *datatype = new TiXmlElement("UML:DataType");
			datatype->SetAttribute("name", arguments[i].second.c_str());
			umltype->LinkEndChild(datatype);
			parameter->LinkEndChild(umltype);

			parameters->LinkEndChild(parameter);
		}

		// Return type
		TiXmlElement *parameter = new TiXmlElement("UML:Parameter");
		string paramid = id + ":ret";
		parameter->SetAttribute("xmi.id", paramid.c_str());
		parameter->SetAttribute("name", "return");
		parameter->SetAttribute("kind", "return");
		TiXmlElement *umltype = new TiXmlElement("UML:Parameter.type");
		TiXmlElement *datatype = new TiXmlElement("UML:DataType");
		datatype->SetAttribute("name", returntype.c_str());
		umltype->LinkEndChild(datatype);
		parameter->LinkEndChild(umltype);
		parameters->LinkEndChild(parameter);

		op->LinkEndChild(parameters);

		return op;
	}

	TiXmlElement* uml_classm_attribute_t::getXmi(const string &id) const {
		TiXmlElement *attr = new TiXmlElement("UML:Attribute");
		attr->SetAttribute("name", name.c_str());
		attr->SetAttribute("xmi.id", id);
		attr->SetAttribute("visibility", visToStr(visibility));
		
		TiXmlElement *umltype = new TiXmlElement("UML:StructuralFeature.type");
		TiXmlElement *datatype = new TiXmlElement("UML:DataType");
		datatype->SetAttribute("name", type.c_str());
		umltype->LinkEndChild(datatype);
		attr->LinkEndChild(umltype);

		return attr;
	} //uml_classm_attribute_t::getXmi
	

	void uml_classm_class_t::addOperation(const uml_classm_operation_t &op) {
		operations.push_back(op);
	} //uml_classm_class_t::addOperation

	void uml_classm_class_t::addAttribute(const uml_classm_attribute_t &at) {
		attributes.push_back(at);
	} //uml_classm_class_t::addAttribute

	uml_classm_class_t::uml_classm_class_t(const string &classname) {
		name = classname;
	} //uml_classm_class_t::uml_classm_class_t

	void uml_classm_t::addClass(const uml_classm_class_t &c) {
		classes.push_back(c);
	} //uml_classm_t::addClass

	void uml_classm_t::clearClasses() {
		classes.clear();
	} //uml_classm_t::clearClasses

	TiXmlElement* uml_classm_class_t::getXmi(const string &classid) const {
		TiXmlElement *tiny_xml_class = new TiXmlElement("UML:Class");
		tiny_xml_class->SetAttribute("name", name.c_str());
		tiny_xml_class->SetAttribute("xmi_id", classid);
		tiny_xml_class->SetAttribute("visibility", "public");
		tiny_xml_class->SetAttribute("isAbstract", "false");

		TiXmlElement *xmi_classifier = new TiXmlElement("UML:Classifier.feature");
		tiny_xml_class->LinkEndChild(xmi_classifier);

		int count = 0;
		stringstream subid;
		for (vector<uml_classm_operation_t>::const_iterator op = operations.begin(); op != operations.end(); ++op)
		{
			subid << classid << ":o" << count;
			xmi_classifier->LinkEndChild(op->getXmi(subid.str().c_str()));
			count++;
			subid.str("");
		}
		for (vector<uml_classm_attribute_t>::const_iterator at = attributes.begin(); at != attributes.end(); ++at)
		{
			subid << classid << ":a" << count;
			xmi_classifier->LinkEndChild(at->getXmi(subid.str().c_str()));
			count++;
			subid.str("");
		}

		return tiny_xml_class;
	}

	int uml_classm_t::toXmi(char* fileName){
		//return -1;

		// header
		TiXmlDocument doc;
		doc.LinkEndChild(new TiXmlDeclaration("1.0", "", ""));


		TiXmlElement *xmi_declaration = new TiXmlElement( "XMI" );
		xmi_declaration->SetAttribute("xmi.version", "1.2");
		xmi_declaration->SetAttribute("xmlns:UML", "org.omg.xmi.namespace.UML");
		doc.LinkEndChild(xmi_declaration);

		TiXmlElement *xmi_header = new TiXmlElement("XMI.header");
		xmi_declaration->LinkEndChild(xmi_header);

		TiXmlElement *xmi_documentation = new TiXmlElement("XMI.documentation");
		xmi_header->LinkEndChild(xmi_documentation);

		TiXmlElement *xmi_exporter = new TiXmlElement("XMI.exporter");
		xmi_documentation->LinkEndChild(xmi_exporter);
		xmi_exporter->LinkEndChild(new TiXmlText("libuml"));

		TiXmlElement *xmi_metamodel = new TiXmlElement("XMI.metamodel");
		xmi_metamodel->SetAttribute("XMI.name", "UML");
		xmi_metamodel->SetAttribute("XMI.version", "1.4");
		xmi_header->LinkEndChild(xmi_metamodel);

		TiXmlElement *content;
		content = new TiXmlElement("XMI.content");
		xmi_declaration->LinkEndChild(content);

		TiXmlElement *model = new TiXmlElement("UML:Model");
		model->SetAttribute("name", fileName); // TODO?
		model->SetAttribute("xmi.id", 1);
		content->LinkEndChild(model);

		TiXmlElement *namespace_ownedelement = new TiXmlElement("UML:Namespace.ownedElement");
		model->LinkEndChild(namespace_ownedelement);

		stringstream ss;
		for (size_t i = 0; i < classes.size(); i++)
		{
			ss << i;
			content->LinkEndChild(classes[i].getXmi(ss.str()));
			ss.str("");
		} //for

		ofstream outfile;
		outfile.open(fileName);
		outfile << doc;
		outfile.close();

		return 0;
	} //uml_classm_t::toXmi

} //namespace libuml
