///////////////////////////////////////////////////////////////////////
//
// factory.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat Nov 20 22:37:31 1999
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

#ifndef GROOVX_RUTZ_FACTORY_CC_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_FACTORY_CC_UTC20050626084020_DEFINED

#include "factory.h"

#include "rutz/trace.h"

rutz::factory_base::factory_base() throw()
  :
  m_fallback(0)
{
GVX_TRACE("rutz::factory_base::factory_base");
}

rutz::factory_base::~factory_base() throw()
{
GVX_TRACE("rutz::factory_base::~factory_base");
}

void rutz::factory_base::set_fallback(fallback_t* fptr) throw()
{
GVX_TRACE("rutz::factory_base::set_fallback");
  m_fallback = fptr;
}

void rutz::factory_base::try_fallback(const rutz::fstring& type) const
{
GVX_TRACE("rutz::factory_base::try_fallback");
  if (m_fallback != 0)
    {
      (*m_fallback)(type);
    }
}

static const char vcid_groovx_rutz_factory_cc_utc20050626084020[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_FACTORY_CC_UTC20050626084020_DEFINED
