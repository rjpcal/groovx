///////////////////////////////////////////////////////////////////////
//
// janitor.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Mar 23 17:35:40 2000
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

#ifndef JANITOR_H_DEFINED
#define JANITOR_H_DEFINED

namespace Util
{

/// Generic class for scope-based cleanup.
template <class C, class T>
class Janitor
{
public:
  typedef T (C::* Getter)() const;
  typedef void (C::* Setter)(T);

  Janitor(C& obj, Getter g, Setter s) :
    itsObj(obj), itsGetter(g), itsSetter(s),
    itsOrigVal( (itsObj.*itsGetter)() ) {}

  ~Janitor() { (itsObj.*itsSetter)(itsOrigVal); }
private:
  C& itsObj;
  Getter itsGetter;
  Setter itsSetter;
  T itsOrigVal;
};

} // end namespace Util

static const char vcid_janitor_h[] = "$Header$";
#endif // !JANITOR_H_DEFINED
