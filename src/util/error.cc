///////////////////////////////////////////////////////////////////////
//
// error.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 22 14:59:48 1999
// written: Sun Aug  5 20:12:47 2001
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

Error::Error()
{
DOTRACE("Error::Error");
#ifdef LOCAL_DEBUG
  Util::Trace::printStackTrace();
#endif

  if (std::uncaught_exception())
    {
      Util::Trace::printStackTrace();
      Assert(false);
    }
}

Error::~Error()
{
DOTRACE("Error::~Error ");
}

ErrorWithMsg::ErrorWithMsg() :
  Error(),
  itsInfo(new dynamic_string(""))
{

  DebugEvalNL(*itsInfo);
}


ErrorWithMsg::ErrorWithMsg(const char* str) :
  itsInfo(new dynamic_string(str))
{
DOTRACE("ErrorWithMsg::ErrorWithMsg");

  DebugEvalNL(*itsInfo);
}

ErrorWithMsg::ErrorWithMsg(const ErrorWithMsg& other) :
  itsInfo(new dynamic_string(*(other.itsInfo)))
{
DOTRACE("ErrorWithMsg::ErrorWithMsg");
  DebugEvalNL(*itsInfo);
}

ErrorWithMsg::~ErrorWithMsg()
{
DOTRACE("ErrorWithMsg::~ErrorWithMsg");
  delete itsInfo;
}

ErrorWithMsg& ErrorWithMsg::operator=(const ErrorWithMsg& other)
{
DOTRACE("ErrorWithMsg::operator=");

  *itsInfo = *(other.itsInfo);    DebugEvalNL(*itsInfo);

  return *this;
}

const char* ErrorWithMsg::msg_cstr() const
{
DOTRACE("ErrorWithMsg::msg_cstr");
  return itsInfo->c_str();
}

ErrorWithMsg& ErrorWithMsg::appendMsg(const char* addMsg)
{
DOTRACE("ErrorWithMsg::appendMsg");
  *itsInfo += addMsg;
  DebugEvalNL(*itsInfo);
  return *this;
}

ErrorWithMsg& ErrorWithMsg::appendMsg(const char* addMsg1, const char* addMsg2)
{
DOTRACE("ErrorWithMsg::appendMsg");
  *itsInfo += addMsg1;
  *itsInfo += addMsg2;
  DebugEvalNL(*itsInfo);
  return *this;
}

ErrorWithMsg& ErrorWithMsg::appendMsg(const char* addMsg1, const char* addMsg2,
                                      const char* addMsg3)
{
DOTRACE("ErrorWithMsg::appendMsg");
  *itsInfo += addMsg1;
  *itsInfo += addMsg2;
  *itsInfo += addMsg3;
  DebugEvalNL(*itsInfo);
  return *this;
}

void ErrorWithMsg::setMsg(const char* str)
{
DOTRACE("ErrorWithMsg::setMsg");
  *itsInfo = str;
  DebugEvalNL(*itsInfo);
}

static const char vcid_error_cc[] = "$Header$";
#endif // !ERROR_CC_DEFINED
