/** @file rutz/assocarray.cc generic associative arrays, implemented as
    a thin wrapper around std::map<rutz::fstring, void*> */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Oct 14 18:42:05 2004
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

#ifndef GROOVX_RUTZ_ASSOCARRAY_CC_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_ASSOCARRAY_CC_UTC20050626084020_DEFINED

#include "assocarray.h"

#include "rutz/error.h"
#include "rutz/fstring.h"

#include <map>

#include "rutz/trace.h"

struct rutz::assoc_array_base::impl
{
  impl(kill_func_t* f, const char* descr)
    :
    values(),
    kill_func(f),
    key_description(descr)
  {}

  typedef std::map<rutz::fstring, void*> map_t;

  map_t         values;
  kill_func_t*  kill_func;
  rutz::fstring key_description;
};

rutz::assoc_array_base::assoc_array_base(kill_func_t* f,
                                         const char* descr) :
  rep(new impl(f, descr))
{
GVX_TRACE("rutz::assoc_array_base::assoc_array_base");
}

rutz::assoc_array_base::~assoc_array_base()
{
GVX_TRACE("rutz::assoc_array_base::~assoc_array_base");
  clear();
}

rutz::fstring rutz::assoc_array_base::
get_known_keys(const char* sep) const
{
  rutz::fstring result;

  bool first = true;

  for (impl::map_t::iterator ii = rep->values.begin();
       ii != rep->values.end();
       ++ii)
    {
      if (ii->second != 0)
        {
          if (!first) result.append(sep);

          result.append(ii->first);

          first = false;
        }
    }

  return result;
}

void rutz::assoc_array_base::
throw_for_key(const char* key, const rutz::file_pos& pos) const
{
  throw rutz::error(rutz::cat("known keys are:\n\t",
                              get_known_keys("\n\t"),
                              "\nunknown ",
                              rep->key_description,
                              " '", key, "'"),
                    pos);
}

void rutz::assoc_array_base::
throw_for_key(const rutz::fstring& key, const rutz::file_pos& pos) const
{
  throw_for_key(key.c_str(), pos);
}

void rutz::assoc_array_base::clear()
{
GVX_TRACE("rutz::assoc_array_base::clear");
  for (impl::map_t::iterator ii = rep->values.begin();
       ii != rep->values.end();
       ++ii)
    {
      if (rep->kill_func != 0) rep->kill_func(ii->second);
      ii->second = 0;
    }

  delete rep;
}

void* rutz::assoc_array_base::get_value_for_key(const rutz::fstring& key) const
{
GVX_TRACE("rutz::assoc_array_base::get_value_for_key");
  return rep->values[key];
}

void* rutz::assoc_array_base::get_value_for_key(const char* key) const
{
  return get_value_for_key(rutz::fstring(key));
}

void rutz::assoc_array_base::set_value_for_key(const char* key, void* ptr)
{
GVX_TRACE("rutz::assoc_array_base::set_value_for_key");
  rutz::fstring skey(key);
  void*& ptr_slot = rep->values[skey];
  if (rep->kill_func != 0) rep->kill_func(ptr_slot);
  ptr_slot = ptr;
}

static const char __attribute__((used)) vcid_groovx_rutz_assocarray_cc_utc20050626084020[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_ASSOCARRAY_CC_UTC20050626084020_DEFINED
