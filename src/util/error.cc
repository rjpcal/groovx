///////////////////////////////////////////////////////////////////////
//
// error.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 22 14:59:48 1999
// written: Mon Mar 13 13:58:38 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERROR_CC_DEFINED
#define ERROR_CC_DEFINED

#include "error.h"

#include "strings.h"

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
  itsInfo(new dynamic_string(""))
{}


ErrorWithMsg::ErrorWithMsg(const char* str) :
  itsInfo(new dynamic_string(str))
{
DOTRACE("ErrorWithMsg::ErrorWithMsg");
  DebugEvalNL(*str);
}

ErrorWithMsg::ErrorWithMsg(const ErrorWithMsg& other) :
  itsInfo(new dynamic_string(*(other.itsInfo)))
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
	 itsInfo = new dynamic_string(*(other.itsInfo));
  }
  return *this;
}

const char* ErrorWithMsg::msg_cstr() const {
DOTRACE("ErrorWithMsg::msg_cstr");
  return itsInfo->c_str();
}

void ErrorWithMsg::appendMsg(const char* addMsg) {
DOTRACE("ErrorWithMsg::appendMsg");
  *itsInfo += addMsg; 
}

void ErrorWithMsg::appendMsg(const char* addMsg1, const char* addMsg2) {
DOTRACE("ErrorWithMsg::appendMsg");
  *itsInfo += addMsg1; 
  *itsInfo += addMsg2; 
}

void ErrorWithMsg::appendMsg(const char* addMsg1, const char* addMsg2,
									  const char* addMsg3) {
DOTRACE("ErrorWithMsg::appendMsg");
  *itsInfo += addMsg1; 
  *itsInfo += addMsg2; 
  *itsInfo += addMsg3; 
}

void ErrorWithMsg::setMsg(const char* str) {
DOTRACE("ErrorWithMsg::setMsg");
  *itsInfo = str;
}

static const char vcid_error_cc[] = "$Header$";
#endif // !ERROR_CC_DEFINED
