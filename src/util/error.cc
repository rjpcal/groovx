///////////////////////////////////////////////////////////////////////
//
// error.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 22 14:59:48 1999
// written: Wed Aug  8 12:27:56 2001
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

Util::Error::Error() :
  itsInfo(new dynamic_string(""))
{

  DebugEvalNL(*itsInfo);
}


Util::Error::Error(const char* str) :
  itsInfo(new dynamic_string(str))
{
DOTRACE("Util::Error::Error");

  DebugEvalNL(*itsInfo);
}

Util::Error::Error(const Util::Error& other) :
  itsInfo(new dynamic_string(*(other.itsInfo)))
{
DOTRACE("Util::Error::Error");
  DebugEvalNL(*itsInfo);
}

Util::Error::~Error()
{
DOTRACE("Util::Error::~Error");
  delete itsInfo;
}

Util::Error& Util::Error::operator=(const Util::Error& other)
{
DOTRACE("Util::Error::operator=");

  *itsInfo = *(other.itsInfo);    DebugEvalNL(*itsInfo);

  return *this;
}

const char* Util::Error::msg_cstr() const
{
DOTRACE("Util::Error::msg_cstr");
  return itsInfo->c_str();
}

Util::Error& Util::Error::appendMsg(const char* addMsg)
{
DOTRACE("Util::Error::appendMsg");
  *itsInfo += addMsg;
  DebugEvalNL(*itsInfo);
  return *this;
}

Util::Error& Util::Error::appendMsg(const char* addMsg1, const char* addMsg2)
{
DOTRACE("Util::Error::appendMsg");
  *itsInfo += addMsg1;
  *itsInfo += addMsg2;
  DebugEvalNL(*itsInfo);
  return *this;
}

Util::Error& Util::Error::appendMsg(const char* addMsg1, const char* addMsg2,
                                    const char* addMsg3)
{
DOTRACE("Util::Error::appendMsg");
  *itsInfo += addMsg1;
  *itsInfo += addMsg2;
  *itsInfo += addMsg3;
  DebugEvalNL(*itsInfo);
  return *this;
}

void Util::Error::setMsg(const char* str)
{
DOTRACE("Util::Error::setMsg");
  *itsInfo = str;
  DebugEvalNL(*itsInfo);
}

static const char vcid_error_cc[] = "$Header$";
#endif // !ERROR_CC_DEFINED
