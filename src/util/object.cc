///////////////////////////////////////////////////////////////////////
//
// object.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun  5 10:26:14 2001
// written: Tue Sep  4 07:31:50 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJECT_CC_DEFINED
#define OBJECT_CC_DEFINED

#include "util/object.h"

#include "util/trace.h"

namespace
{
  Util::UID idCounter = 0;
}

Util::Object::Object() : itsId(++idCounter)
{
DOTRACE("Util::Object::Object");
}

Util::Object::~Object()
{
DOTRACE("Util::Object::~Object");
}

Util::UID Util::Object::id() const { return itsId; }

static const char vcid_object_cc[] = "$Header$";
#endif // !OBJECT_CC_DEFINED
