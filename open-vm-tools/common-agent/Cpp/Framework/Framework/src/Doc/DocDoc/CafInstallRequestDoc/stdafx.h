/*
 *  Author: bwilliams
 *  Created: February 25, 2016
 *
 *  Copyright (C) 2016 VMware, Inc.  All rights reserved. -- VMware Confidential
 *
 */

#ifndef STDAFX_H_
#define STDAFX_H_

#ifdef WIN32
   #define CAFINSTALLREQUESTDOC_LINKAGE __declspec(dllexport)
#else
   #define CAFINSTALLREQUESTDOC_LINKAGE
#endif

#include <CommonDefines.h>

#endif /* STDAFX_H_ */
