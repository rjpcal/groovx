///////////////////////////////////////////////////////////////////////
//
// error.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 22 14:59:48 1999
// written: Sat Mar  4 00:47:58 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERROR_CC_DEFINED
#define ERROR_CC_DEFINED

#include "error.h"

#include <string>

#define NO_TRACE
#include "trace.h"
#include "debug.h"

Error::Error () {
DOTRACE("Error::Error ");
}

Error::~Error () {
DOTRACE("Error::~Error ");
}

ErrorWithMsg::ErrorWithMsg() :
  Error(),
  itsInfo(new string(""))
{}


ErrorWithMsg::ErrorWithMsg(const char* str) :
  itsInfo(new string(str))
{
DOTRACE("ErrorWithMsg::ErrorWithMsg");
  DebugEvalNL(*str);
}

ErrorWithMsg::ErrorWithMsg(const string& str) :
  itsInfo(new string(str))
{
DOTRACE("ErrorWithMsg::ErrorWithMsg");
  DebugEvalNL(*str);
}

ErrorWithMsg::ErrorWithMsg(const ErrorWithMsg& other) :
  itsInfo(new string(*(other.itsInfo)))
{
DOTRACE("ErrorWithMsg::ErrorWithMsg");
}

ErrorWithMsg::~ErrorWithMsg() {
DOTRACE("ErrorWithMsg::~ErrorWithMsg");
  delete itsInfo;
}

ErrorWithMsg& ErrorWithMsg::operator=(const ErrorWithMsg& other) {
DOTRACE("ErrorWithMsg::operator=");
  if (&other != this) { 
	 delete itsInfo;
	 itsInfo = new string(*(other.itsInfo));
  }
  return *this;
}

const char* ErrorWithMsg::msg_cstr() const {
DOTRACE("ErrorWithMsg::msg_cstr");
  return itsInfo->c_str();
}

const string& ErrorWithMsg::msg() const {
DOTRACE("ErrorWithMsg::msg");
  return *itsInfo;
}

void ErrorWithMsg::appendMsg(const char* addMsg) {
DOTRACE("ErrorWithMsg::appendMsg");
  *itsInfo += addMsg; 
}

void ErrorWithMsg::appendMsg(const string& addMsg) {
DOTRACE("ErrorWithMsg::appendMsg");
  *itsInfo += addMsg;
}

void ErrorWithMsg::setMsg(const string& str) {
DOTRACE("ErrorWithMsg::setMsg");
  *itsInfo = str;
}

static const char vcid_error_cc[] = "$Header$";
#endif // !ERROR_CC_DEFINED
