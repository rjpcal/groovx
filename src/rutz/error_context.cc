/** @file rutz/error_context.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Nov 17 16:14:11 2005
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

#include "rutz/error_context.h"

#include <new>
#include <sstream>

#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  // FIXME this "thread-local singleton" code is duplicated here and
  // in rutz/backtrace.cc; can we abstract out the common code?

  // returns a mutable object (but note that
  // rutz::error_context::current() returns a const object)
  rutz::error_context* get_current_context()
  {
    static thread_local rutz::error_context* current_context = nullptr;

    if (!current_context)
      {
        current_context = new (std::nothrow) rutz::error_context;
        if (!current_context)
          GVX_ABORT("memory allocation failed");
      }

    return current_context;
  }
}

rutz::error_context::error_context() {}

rutz::error_context::~error_context() {}

const rutz::error_context& rutz::error_context::current()
{
  return *(get_current_context());
}

bool rutz::error_context::add_entry(const error_context_entry* e)
{
  return m_entries.push(e);
}

void rutz::error_context::remove_entry(const error_context_entry* e)
{
  GVX_ASSERT(m_entries.top() == e);
  m_entries.pop();
}

rutz::fstring rutz::error_context::get_text() const
{
  std::ostringstream result;

  const char* prefix = "==> while ";

  for (unsigned int i = 0; i < m_entries.size(); ++i)
    {
      result << prefix << m_entries[i]->text() << ":";
      prefix = "\n==> while ";
    }

  return rutz::fstring(result.str().c_str());
}

rutz::error_context_entry::error_context_entry(const rutz::fstring& msg)
  :
  m_text(msg),
  m_context(get_current_context())
{
  if (!m_context->add_entry(this))
    m_context = nullptr;
}

rutz::error_context_entry::~error_context_entry()
{
  if (m_context)
    m_context->remove_entry(this);
}
