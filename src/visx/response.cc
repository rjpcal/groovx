///////////////////////////////////////////////////////////////////////
//
// response.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Mar 13 18:33:17 2000
// written: Fri Jul  7 15:22:06 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSE_CC_DEFINED
#define RESPONSE_CC_DEFINED

#include "response.h"

#include <iostream.h>

#define NO_TRACE
#include "util/trace.h"

///////////////////////////////////////////////////////////////////////
//
// Response member definitions
//
///////////////////////////////////////////////////////////////////////

const int Response::INVALID_VALUE;
const int Response::ALWAYS_CORRECT;

Response::~Response() {}

Value* Response::clone() const {
DOTRACE("Response::clone");
  return new Response(*this); 
}

Value::Type Response::getNativeType() const {
DOTRACE("Response::getNativeType");
  return Value::UNKNOWN;
}

const char* Response::getNativeTypeName() const {
DOTRACE("Response::getNativeTypeName");
  return "Response";
}

void Response::printTo(ostream& os) const {
DOTRACE("Response::printTo");
  os << itsVal << " " << itsMsec; 
}

void Response::scanFrom(istream& is) {
DOTRACE("Response::scanFrom");
  is >> itsVal >> itsMsec;
}

static const char vcid_response_cc[] = "$Header$";
#endif // !RESPONSE_CC_DEFINED
