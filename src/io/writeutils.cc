///////////////////////////////////////////////////////////////////////
//
// writeutils.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 16 14:21:32 1999
// written: Fri Jan 18 16:07:06 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITEUTILS_CC_DEFINED
#define WRITEUTILS_CC_DEFINED

#include "io/writeutils.h"

#include "io/readutils.h"

fstring IO::WriteUtils::makeElementNameString(const fstring& seq_name,
                                              int element_num)
{
  return ReadUtils::makeElementNameString(seq_name, element_num);
}

fstring IO::WriteUtils::makeSeqCountString(const fstring& seq_name)
{
  return ReadUtils::makeSeqCountString(seq_name);
}

static const char vcid_writeutils_cc[] = "$Header$";
#endif // !WRITEUTILS_CC_DEFINED
