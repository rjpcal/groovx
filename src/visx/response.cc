///////////////////////////////////////////////////////////////////////
//
// response.cc
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Mar 13 18:33:17 2000
// written: Wed Mar 19 12:46:29 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSE_CC_DEFINED
#define RESPONSE_CC_DEFINED

#include "visx/response.h"

#include "util/strings.h"

#include <iostream>

#include "util/trace.h"

///////////////////////////////////////////////////////////////////////
//
// Response member definitions
//
///////////////////////////////////////////////////////////////////////

const int Response::INVALID_VALUE;
const int Response::ALWAYS_CORRECT;

Response::~Response() {}

fstring Response::getNativeTypeName() const
{
DOTRACE("Response::getNativeTypeName");
  static fstring name("Response"); return name;
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

static const char vcid_response_cc[] = "$Header$";
#endif // !RESPONSE_CC_DEFINED
