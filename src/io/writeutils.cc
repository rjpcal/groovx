///////////////////////////////////////////////////////////////////////
//
// writeutils.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 16 14:21:32 1999
// written: Wed Jun 20 18:30:22 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITEUTILS_CC_DEFINED
#define WRITEUTILS_CC_DEFINED

#include "io/writeutils.h"

#include "io/readutils.h"

const char* IO::WriteUtils::makeElementNameString(const char* seq_name,
                                                  int element_num)
{
  return ReadUtils::makeElementNameString(seq_name, element_num);
}

const char* IO::WriteUtils::makeSeqCountString(const char* seq_name)
{
  return ReadUtils::makeSeqCountString(seq_name);
}

static const char vcid_writeutils_cc[] = "$Header$";
#endif // !WRITEUTILS_CC_DEFINED
