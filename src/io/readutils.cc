///////////////////////////////////////////////////////////////////////
//
// readutils.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 16 14:25:40 1999
// written: Wed Aug  8 20:16:39 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef READUTILS_CC_DEFINED
#define READUTILS_CC_DEFINED

#include "io/readutils.h"

#include "util/strings.h"

const char* IO::ReadUtils::makeElementNameString(const char* seq_name,
                                                 int element_num)
{
  static fstring result("");
  result = seq_name;
  result.append( element_num );
  return result.c_str();
}

const char* IO::ReadUtils::makeSeqCountString(const char* seq_name)
{
  static fstring result("");
  result = seq_name;
  result += "Count";
  return result.c_str();
}

static const char vcid_readutils_cc[] = "$Header$";
#endif // !READUTILS_CC_DEFINED
