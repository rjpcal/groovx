///////////////////////////////////////////////////////////////////////
//
// error.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Jun 22 14:59:48 1999
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

#ifndef GROOVX_RUTZ_ERROR_CC_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_ERROR_CC_UTC20050626084020_DEFINED

#include "rutz/error.h"

#include "rutz/backtrace.h"
#include "rutz/fstring.h"

#include <cstdlib>
#include <new>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  static rutz::backtrace* last = 0;
}

rutz::error::error(const rutz::file_pos& pos) :
  std::exception(),
  m_msg(),
  m_file_pos(pos),
  m_backtrace(new rutz::backtrace(rutz::backtrace::current()))
{
GVX_TRACE("rutz::error::error()");

  dbg_dump(4, m_msg);

  if (last == 0)
    last = new rutz::backtrace(*m_backtrace);
  else
    *last = *m_backtrace;

  if (GVX_DBG_LEVEL() >= 4)
    {
      m_backtrace->print();
    }
}

rutz::error::error(const rutz::fstring& msg,
                   const rutz::file_pos& pos) :
  std::exception(),
  m_msg(msg),
  m_file_pos(pos),
  m_backtrace(new rutz::backtrace(rutz::backtrace::current()))
{
GVX_TRACE("rutz::error::error(fstring)");

  dbg_dump(4, m_msg);

  if (last == 0)
    last = new rutz::backtrace(*m_backtrace);
  else
    *last = *m_backtrace;

  if (GVX_DBG_LEVEL() >= 4)
    {
      m_backtrace->print();
    }
}

rutz::error::error(const rutz::error& other) throw() :
  std::exception(other),
  m_msg(other.m_msg),
  m_file_pos(other.m_file_pos),
  m_backtrace(0)
{
GVX_TRACE("rutz::error::error(copy)");

  dbg_dump(4, m_msg);

  if (other.m_backtrace != 0)
    m_backtrace =
      new (std::nothrow) rutz::backtrace(*other.m_backtrace);
}

rutz::error::~error() throw()
{
GVX_TRACE("rutz::error::~error");
  delete m_backtrace;
}

const char* rutz::error::what() const throw()
{
  m_what = rutz::fstring("at ", m_file_pos.m_file_name,
                         ":", m_file_pos.m_line_no, ":\n",
                         m_msg);
  return m_what.c_str();
}

const rutz::backtrace& rutz::error::last_backtrace()
{
  if (last == 0)
    last = new rutz::backtrace();

  return *last;
}

static const char vcid_groovx_rutz_error_cc_utc20050626084020[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_ERROR_CC_UTC20050626084020_DEFINED
