///////////////////////////////////////////////////////////////////////
//
// error.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 22 14:59:48 1999
// written: Wed Aug  8 15:26:49 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERROR_CC_DEFINED
#define ERROR_CC_DEFINED

#include "util/error.h"

#include "util/strings.h"

#include <exception>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"


Util::Error::Error(const Util::Error& other) :
  itsInfo(other.itsInfo),
  itsCount(other.itsCount)
{
DOTRACE("Util::Error::Error(copy)");
  DebugEvalNL(*itsInfo);

  ++*itsCount;
}

Util::Error::~Error()
{
DOTRACE("Util::Error::~Error");

  if (--*itsCount == 0)
    {
      DOTRACE("Util::Error::~Error--delete");
      delete itsInfo;
      delete itsCount;
    }
}

const char* Util::Error::msg_cstr() const
{
DOTRACE("Util::Error::msg_cstr");
  return itsInfo->c_str();
}

void Util::Error::setMsg(const char* str)
{
DOTRACE("Util::Error::setMsg");
  *itsInfo = str;
  DebugEvalNL(*itsInfo);
}

void Util::Error::doAppend(const char* text)
{
DOTRACE("Util::Error::doAppend");
  itsInfo->append(text);
  DebugEvalNL(*itsInfo);
}

void Util::Error::init()
{
DOTRACE("Util::Error::init");

  itsInfo = new fixed_string("");
  itsCount = new ushort(1);
}

static const char vcid_error_cc[] = "$Header$";
#endif // !ERROR_CC_DEFINED
