///////////////////////////////////////////////////////////////////////
//
// enumeration.cc
//
// Copyright (c) 2004-2005
// Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon May 31 14:22:48 2004
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef ENUMERATION_CC_DEFINED
#define ENUMERATION_CC_DEFINED

#include "io/enumeration.h"

#include "io/reader.h"
#include "io/readutils.h"
#include "io/writer.h"
#include "io/writeutils.h"

#include "util/error.h"
#include "util/ref.h"

#include <vector>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

EnumerationValue::EnumerationValue() :
  itsName(),
  itsValue()
{
DOTRACE("EnumerationValue::EnumerationValue(default)");
}

EnumerationValue::EnumerationValue(fstring nm, int val) :
  itsName(nm),
  itsValue(val)
{
DOTRACE("EnumerationValue::EnumerationValue(int, fstring)");
}

EnumerationValue::~EnumerationValue() throw()
{
DOTRACE("EnumerationValue::~EnumerationValue");
}

void EnumerationValue::readFrom(IO::Reader& reader)
{
DOTRACE("EnumerationValue::readFrom");

  reader.readValue("name", itsName);
  reader.readValue("value", itsValue);
}

void EnumerationValue::writeTo(IO::Writer& writer) const
{
DOTRACE("EnumerationValue::writeTo");

  writer.writeValue("name", itsName);
  writer.writeValue("value", itsValue);
}

typedef Util::Ref<EnumerationValue> ValRef;

struct Enumeration::Impl
{
  fstring name;
  std::vector<ValRef> values;
};

Enumeration::Enumeration(const char* name) :
  rep(new Impl)
{
DOTRACE("Enumeration::Enumeration");

  rep->name = name;
}

Enumeration::~Enumeration() throw()
{
DOTRACE("Enumeration::~Enumeration");
  delete rep;
}

void Enumeration::readFrom(IO::Reader& reader)
{
DOTRACE("Enumeration::readFrom");

  reader.readValue("name", rep->name);

  std::vector<ValRef> newvalues;
  IO::ReadUtils::readObjectSeq<EnumerationValue>
    (reader, "values", std::back_inserter(newvalues));

  newvalues.swap(rep->values);
}

void Enumeration::writeTo(IO::Writer& writer) const
{
DOTRACE("Enumeration::writeTo");

  writer.writeValue("name", rep->name);

  IO::WriteUtils::writeObjectSeq
    (writer, "values", rep->values.begin(), rep->values.end());
}

void Enumeration::add(fstring name, int val)
{
DOTRACE("Enumeration::add");

  rep->values.push_back(ValRef(new EnumerationValue(name, val)));
}

fstring Enumeration::getName(int val) const
{
DOTRACE("Enumeration::getName");

  for (unsigned int i = 0; i < rep->values.size(); ++i)
    {
      if (rep->values[i]->value() == val)
        return rep->values[i]->name();
    }

  throw Util::Error(fstring("no entry with value '", val,
                            "' in enumeration ", rep->name));
  Assert(0); return fstring();
}

int Enumeration::getValue(fstring name) const
{
DOTRACE("Enumeration::getValue");

  for (unsigned int i = 0; i < rep->values.size(); ++i)
    {
      if (rep->values[i]->name() == name)
        return rep->values[i]->value();
    }

  throw Util::Error(fstring("no entry with name '", name,
                            "' in enumeration ", rep->name));
  Assert(0); return 0;
}

static const char vcid_enumeration_cc[] = "$Header$";
#endif // !ENUMERATION_CC_DEFINED
