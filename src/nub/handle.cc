/** @file nub/handle.cc strong reference-counted handle for nub::object */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Nov 16 09:01:10 2005
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef GROOVX_NUB_HANDLE_CC_UTC20051116170110_DEFINED
#define GROOVX_NUB_HANDLE_CC_UTC20051116170110_DEFINED

#include "nub/handle.h"

#include "rutz/demangle.h"
#include "rutz/error.h"

void nub::detail::throw_ref_null(const std::type_info& info,
                                 const rutz::file_pos& pos)
{
  throw rutz::error(rutz::cat("attempted to construct a ref<",
                              rutz::demangled_name(info),
                              "> with a null pointer"),
                    pos);
}

void nub::detail::throw_ref_unshareable(const std::type_info& info,
                                        const rutz::file_pos& pos)
{
  throw rutz::error(rutz::cat("attempted to construct a ref<",
                              rutz::demangled_name(info),
                              "> with an unshareable object"),
                    pos);
}

static const char __attribute__((used)) vcid_groovx_nub_handle_cc_utc20051116170110[] = "$Id$ $HeadURL$";
#endif // !GROOVX_NUB_HANDLE_CC_UTC20051116170110DEFINED