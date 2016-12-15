/** @file nub/types.h enum types for reference-counted handles */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Nov 16 08:57:40 2005
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

#ifndef GROOVX_NUB_TYPES_H_UTC20051116165740_DEFINED
#define GROOVX_NUB_TYPES_H_UTC20051116165740_DEFINED

namespace nub
{
  enum class ref_type { WEAK, STRONG };

  // type tags
  struct ref_vis_private {};   //! nub::objectdb gets no reference to the object
  struct ref_vis_protected {}; //! nub::objectdb gets a weak reference to the object
  struct ref_vis_public {};    //! nub::objectdb gets a strong reference to the object
}

#endif // !GROOVX_NUB_TYPES_H_UTC20051116165740DEFINED
