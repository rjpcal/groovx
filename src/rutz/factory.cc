/** @file rutz/factory.cc template class for object factories, which
    create objects belonging to some inheritance hierarchy given just
    a type name as a string */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat Nov 20 22:37:31 1999
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

#include "factory.h"

#include "rutz/trace.h"

namespace
{
  class function_fallback : public rutz::factory_fallback
  {
  public:
    typedef rutz::factory_base::fallback_t fallback_t;

    function_fallback(fallback_t* f) noexcept : m_func(f) {}

    virtual ~function_fallback() noexcept {}

    virtual void try_fallback(const rutz::fstring& key) const
    {
      if (m_func != nullptr)
        (*m_func)(key);
    }

  private:
    fallback_t* m_func;
  };
}

rutz::factory_fallback::factory_fallback() noexcept
{}

rutz::factory_fallback::~factory_fallback() noexcept
{}

rutz::factory_base::factory_base() noexcept
  :
  m_fallback()
{
GVX_TRACE("rutz::factory_base::factory_base");
}

rutz::factory_base::~factory_base() noexcept
{
GVX_TRACE("rutz::factory_base::~factory_base");
}

void rutz::factory_base::set_fallback(rutz::shared_ptr<factory_fallback> f)
{
GVX_TRACE("rutz::factory_base::set_fallback(object)");

  m_fallback = f;
}

void rutz::factory_base::set_fallback(fallback_t* fptr)
{
GVX_TRACE("rutz::factory_base::set_fallback");
  m_fallback = rutz::make_shared(new function_fallback(fptr));
}

void rutz::factory_base::try_fallback(const rutz::fstring& key) const
{
GVX_TRACE("rutz::factory_base::try_fallback");

  if (m_fallback.get() != nullptr)
    m_fallback->try_fallback(key);
}
