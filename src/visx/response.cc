///////////////////////////////////////////////////////////////////////
//
// response.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 13 18:33:17 2000
// written: Tue Aug  7 11:31:00 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSE_CC_DEFINED
#define RESPONSE_CC_DEFINED

#include "response.h"

#include "util/strings.h"

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

Value* Response::clone() const
{
DOTRACE("Response::clone");
  return new Response(*this);
}

const char* Response::getNativeTypeName() const
{
DOTRACE("Response::getNativeTypeName");
  return "Response";
}

void Response::printTo(STD_IO::ostream& os) const
{
DOTRACE("Response::printTo");
  os << itsVal << " " << itsMsec;
}

void Response::scanFrom(STD_IO::istream& is)
{
DOTRACE("Response::scanFrom");
  is >> itsVal >> itsMsec;
}

const char* Response::get(Util::TypeCue<const char*>) const
{
DOTRACE("Response::get(const char*)");
  static dynamic_string str;
  str = "";
  str.append(itsVal).append(" ").append(itsMsec);
  return str.c_str();
}

void Response::assignTo(Value& other) const
{
DOTRACE("Response::assignTo");

  other.set(this->get(Util::TypeCue<const char*>()));
}

static const char vcid_response_cc[] = "$Header$";
#endif // !RESPONSE_CC_DEFINED
