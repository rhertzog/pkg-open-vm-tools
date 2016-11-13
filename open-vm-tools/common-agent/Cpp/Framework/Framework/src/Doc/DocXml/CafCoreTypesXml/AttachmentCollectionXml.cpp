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

#include "stdafx.h"

#include "Doc/CafCoreTypesDoc/CAttachmentCollectionDoc.h"
#include "Doc/CafCoreTypesDoc/CAttachmentDoc.h"
#include "Xml/XmlUtils/CXmlElement.h"
#include "Doc/DocXml/CafCoreTypesXml/AttachmentCollectionXml.h"
#include "Doc/DocXml/CafCoreTypesXml/AttachmentXml.h"

using namespace Caf;

void AttachmentCollectionXml::add(
	const SmartPtrCAttachmentCollectionDoc attachmentCollectionDoc,
	const SmartPtrCXmlElement thisXml) {
	CAF_CM_STATIC_FUNC_VALIDATE("AttachmentCollectionXml", "add");

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(attachmentCollectionDoc);
		CAF_CM_VALIDATE_SMARTPTR(thisXml);

		const std::deque<SmartPtrCAttachmentDoc> attachmentVal =
			attachmentCollectionDoc->getAttachment();
		if (! attachmentVal.empty()) {
			for (TConstIterator<std::deque<SmartPtrCAttachmentDoc> > attachmentIter(attachmentVal);
				attachmentIter; attachmentIter++) {
				const SmartPtrCXmlElement attachmentXml =
					thisXml->createAndAddElement("attachment");
				AttachmentXml::add(*attachmentIter, attachmentXml);
			}
		}
	}
	CAF_CM_EXIT;
}

SmartPtrCAttachmentCollectionDoc AttachmentCollectionXml::parse(
	const SmartPtrCXmlElement thisXml) {
	CAF_CM_STATIC_FUNC_VALIDATE("AttachmentCollectionXml", "parse");

	SmartPtrCAttachmentCollectionDoc attachmentCollectionDoc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(thisXml);

		const CXmlElement::SmartPtrCElementCollection attachmentChildrenXml =
			thisXml->findOptionalChildren("attachment");

		std::deque<SmartPtrCAttachmentDoc> attachmentVal;
		if (! attachmentChildrenXml.IsNull() && ! attachmentChildrenXml->empty()) {
			for (TConstIterator<CXmlElement::CElementCollection> attachmentXmlIter(*attachmentChildrenXml);
				attachmentXmlIter; attachmentXmlIter++) {
				const SmartPtrCXmlElement attachmentXml = attachmentXmlIter->second;
				const SmartPtrCAttachmentDoc attachmentDoc =
					AttachmentXml::parse(attachmentXml);
				attachmentVal.push_back(attachmentDoc);
			}
		}

		attachmentCollectionDoc.CreateInstance();
		attachmentCollectionDoc->initialize(
			attachmentVal);
	}
	CAF_CM_EXIT;

	return attachmentCollectionDoc;
}

