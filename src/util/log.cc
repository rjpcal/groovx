///////////////////////////////////////////////////////////////////////
//
// log.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun 20 17:49:28 2001
// written: Fri Jan 25 10:48:25 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef LOG_CC_DEFINED
#define LOG_CC_DEFINED

#include "util/log.h"

#include "util/strings.h"

#include <iostream.h>

void Util::log(const char* msg)
{
  std::cerr << msg << '\n';
}

void Util::log(const fstring& msg)
{
  std::cerr << msg << '\n';
}

static const char vcid_log_cc[] = "$Header$";
#endif // !LOG_CC_DEFINED
