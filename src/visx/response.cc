///////////////////////////////////////////////////////////////////////
//
// response.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 13 18:33:17 2000
// written: Thu Sep 13 11:32:12 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSE_CC_DEFINED
#define RESPONSE_CC_DEFINED

#include "visx/response.h"

#include "util/strings.h"

#include <iostream.h>

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
