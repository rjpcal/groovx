/** @file nub/volatileobject.h nub::object subclass for inherently
    unshareable objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue Aug 21 17:16:15 2001
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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
  virtual ~volatile_object() noexcept;

  void destroy();
};

#endif // !GROOVX_NUB_VOLATILEOBJECT_H_UTC20050626084019_DEFINED
