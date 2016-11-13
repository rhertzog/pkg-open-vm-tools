/*
 *  Author: bwilliams
 *  Created: April 6, 2012
 *
 *  Copyright (C) 2012-2016 VMware, Inc.  All rights reserved. -- VMware Confidential
 *
 *  This code was generated by the script "build/dev/codeGen/genCppXml". Please
 *  speak to Brian W. before modifying it by hand.
 *
 */

#ifndef ErrorResponseXml_h_
#define ErrorResponseXml_h_


#include "Doc/ResponseDoc/CErrorResponseDoc.h"

#include "Doc/DocXml/ResponseXml/ResponseXmlLink.h"
#include "Xml/XmlUtils/CXmlElement.h"

namespace Caf {

	/// Streams the ErrorResponse class to/from XML
	namespace ErrorResponseXml {

		/// Adds the ErrorResponseDoc into the XML.
		void RESPONSEXML_LINKAGE add(
			const SmartPtrCErrorResponseDoc errorResponseDoc,
			const SmartPtrCXmlElement thisXml);

		/// Parses the ErrorResponseDoc from the XML.
		SmartPtrCErrorResponseDoc RESPONSEXML_LINKAGE parse(
			const SmartPtrCXmlElement thisXml);
	}
}

#endif
