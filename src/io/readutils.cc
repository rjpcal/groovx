///////////////////////////////////////////////////////////////////////
//
// readutils.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Nov 16 14:25:40 1999
// written: Wed Mar 19 12:45:49 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef READUTILS_CC_DEFINED
#define READUTILS_CC_DEFINED

#include "io/readutils.h"

#include "util/strings.h"

fstring IO::ReadUtils::makeElementNameString(const fstring& seq_name,
                                             int element_num)
{
  fstring result(seq_name);
  result.append( element_num );
  return result;
}

fstring IO::ReadUtils::makeSeqCountString(const fstring& seq_name)
{
  fstring result(seq_name);
  result.append( "Count" );
  return result;
}

static const char vcid_readutils_cc[] = "$Header$";
#endif // !READUTILS_CC_DEFINED
