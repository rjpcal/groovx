///////////////////////////////////////////////////////////////////////
//
// enumeration.h
//
// Copyright (c) 2004-2005
// Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon May 31 14:19:07 2004
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

#ifndef ENUMERATION_H_DEFINED
#define ENUMERATION_H_DEFINED

#include "io/io.h"

#include "util/strings.h"

class EnumerationValue : public IO::IoObject
{
private:
  EnumerationValue(const EnumerationValue&);
  EnumerationValue& operator=(const EnumerationValue&);

public:
  EnumerationValue();
  EnumerationValue(fstring nm, int val);
  virtual ~EnumerationValue() throw();

  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  int value() const { return itsValue; }
  fstring name() const { return itsName; }

private:
  fstring itsName;
  int itsValue;
};

class Enumeration : public IO::IoObject
{
private:
  Enumeration(const Enumeration&);
  Enumeration& operator=(const Enumeration&);

public:
  Enumeration(const char* name);
  virtual ~Enumeration() throw();

  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  void add(fstring name, int val);
  fstring getName(int val) const;
  int getValue(fstring name) const;

private:
  class Impl;
  Impl* const rep;
};

static const char vcid_enumeration_h[] = "$Header$";
#endif // !ENUMERATION_H_DEFINED
