///////////////////////////////////////////////////////////////////////
//
// assocarray.cc
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Oct 14 18:42:05 2004
// commit: $Id$
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

#ifndef ASSOCARRAY_CC_DEFINED
#define ASSOCARRAY_CC_DEFINED

#include "assocarray.h"

#include "util/error.h"
#include "util/strings.h"

#include <map>

#include "util/trace.h"

struct rutz::assoc_array_base::impl
{
  impl(kill_func_t* f, const char* descr)
    :
    values(),
    kill_func(f),
    key_description(descr)
  {}

  typedef std::map<fstring, void*> map_t;

  map_t        values;
  kill_func_t* kill_func;
  fstring      key_description;
};

rutz::assoc_array_base::assoc_array_base(kill_func_t* f,
                                         const char* descr) :
  rep(new impl(f, descr))
{
DOTRACE("rutz::assoc_array_base::assoc_array_base");
}

rutz::assoc_array_base::~assoc_array_base()
{
DOTRACE("rutz::assoc_array_base::~assoc_array_base");
  clear();
}

void rutz::assoc_array_base::throw_for_key(const char* key,
                                           const rutz::file_pos& pos)
{
  fstring keylist("known keys are:");

  for (impl::map_t::iterator ii = rep->values.begin();
       ii != rep->values.end();
       ++ii)
    {
      if (ii->second != 0)
        keylist.append("\n\t", ii->first);
    }

  throw rutz::error(fstring(keylist, "\nunknown ", rep->key_description,
                            " '", key, "'"), pos);
}

void rutz::assoc_array_base::throw_for_key(const fstring& key,
                                           const rutz::file_pos& pos)
{
  throw_for_key(key.c_str(), pos);
}

void rutz::assoc_array_base::clear()
{
DOTRACE("rutz::assoc_array_base::clear");
  for (impl::map_t::iterator ii = rep->values.begin();
       ii != rep->values.end();
       ++ii)
    {
      if (rep->kill_func != 0) rep->kill_func(ii->second);
      ii->second = 0;
    }

  delete rep;
}

void* rutz::assoc_array_base::get_value_for_key(const fstring& key) const
{
DOTRACE("rutz::assoc_array_base::get_value_for_key");
  return rep->values[key];
}

void* rutz::assoc_array_base::get_value_for_key(const char* key) const
{
  return get_value_for_key(fstring(key));
}

void rutz::assoc_array_base::set_value_for_key(const char* key, void* ptr)
{
DOTRACE("rutz::assoc_array_base::set_value_for_key");
  fstring skey(key);
  void*& ptr_slot = rep->values[skey];
  if (rep->kill_func != 0) rep->kill_func(ptr_slot);
  ptr_slot = ptr;
}

static const char vcid_assocarray_cc[] = "$Header$";
#endif // !ASSOCARRAY_CC_DEFINED