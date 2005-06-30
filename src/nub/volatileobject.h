///////////////////////////////////////////////////////////////////////
//
// volatileobject.h
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Aug 21 17:16:15 2001
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_NUB_VOLATILEOBJECT_H_UTC20050626084019_DEFINED
#define GROOVX_NUB_VOLATILEOBJECT_H_UTC20050626084019_DEFINED

#include "nub/object.h"

namespace nub
{
  class volatile_object;
}

/// Subclass of nub::object for inherently un-shareable objects.
/** This typically applies to objects who must control their own
    lifetime, or whose lifetime is controlled by some external
    mechanism (such as a windowing system, for example). */
class nub::volatile_object : public virtual nub::object
{
public:
  volatile_object();
  virtual ~volatile_object() throw();

  void destroy();
};

static const char vcid_groovx_nub_volatileobject_h_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_NUB_VOLATILEOBJECT_H_UTC20050626084019_DEFINED
