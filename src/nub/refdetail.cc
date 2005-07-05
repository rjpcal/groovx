///////////////////////////////////////////////////////////////////////
//
// ref.cc
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Oct 27 17:07:31 2000
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

#ifndef GROOVX_NUB_REFDETAIL_CC_UTC20050705190649_DEFINED
#define GROOVX_NUB_REFDETAIL_CC_UTC20050705190649_DEFINED

#include "ref.h"

#include "nub/objdb.h"

#include "rutz/error.h"
#include "rutz/demangle.h"

#include "rutz/debug.h"
GVX_DBG_REGISTER

#ifndef GVX_DEFAULT_REFVIS
#  define GVX_DEFAULT_REFVIS PRIVATE
#endif

namespace
{
  nub::ref_vis default_vis = nub::GVX_DEFAULT_REFVIS;
}

nub::ref_vis nub::get_default_ref_vis()
{
  return default_vis;
}

void nub::set_default_ref_vis(nub::ref_vis vis)
{
  default_vis = vis;
}

bool nub::detail::is_valid_uid(nub::uid id) throw()
{
  return nub::objectdb::instance().is_valid_uid(id);
}

nub::object* nub::detail::get_checked_item(nub::uid id)
{
  return nub::objectdb::instance().get_checked_obj(id);
}

void nub::detail::insert_item(nub::object* obj, ref_vis vis)
{
  if (vis == DEFAULT)
    {
      vis = default_vis;
    }

  switch (vis)
    {
    case PUBLIC:    nub::objectdb::instance().insert_obj(obj); break;
    case PROTECTED: nub::objectdb::instance().insert_obj_weak(obj); break;
    case PRIVATE:   /* nothing */ break;
    default:
      GVX_PANIC("unknown ref_vis enum value");
    }
}

void nub::detail::throw_ref_null(const std::type_info& info,
                               const rutz::file_pos& pos)
{
  throw rutz::error(rutz::fstring("attempted to construct a ref<",
                                  rutz::demangled_name(info),
                                  "> with a null pointer"),
                    pos);
}

void nub::detail::throw_ref_unshareable(const std::type_info& info,
                                      const rutz::file_pos& pos)
{
  throw rutz::error(rutz::fstring("attempted to construct a ref<",
                                  rutz::demangled_name(info),
                                  "> with an unshareable object"),
                    pos);
}

void nub::detail::throw_soft_ref_invalid(const std::type_info& info,
                                      const rutz::file_pos& pos)
{
  throw rutz::error(rutz::fstring("attempted to access invalid object "
                                  "in soft_ref<", rutz::demangled_name(info), ">"),
                    pos);
}

static const char vcid_groovx_nub_refdetail_cc_utc20050705190649[] = "$Id$ $HeadURL$";
#endif // !GROOVX_NUB_REFDETAIL_CC_UTC20050705190649_DEFINED
