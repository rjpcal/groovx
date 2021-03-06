/** @file rutz/cstrstream.cc like std::strstream, but not deprecated */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri Oct  8 15:45:46 2004
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

#include "cstrstream.h"

#include <cstdio> // for EOF

rutz::imembuf::imembuf(const char* s) :
  m_len(strlen(s)),
  m_buf(s),
  m_owned_mem(nullptr)
{
  setg(const_cast<char*>(m_buf),
       const_cast<char*>(m_buf),
       const_cast<char*>(m_buf) + m_len);
}

rutz::imembuf::imembuf(const char* s, size_t len) :
  m_len(len),
  m_buf(s),
  m_owned_mem(nullptr)
{
  setg(const_cast<char*>(m_buf),
       const_cast<char*>(m_buf),
       const_cast<char*>(m_buf) + m_len);
}

void rutz::imembuf::make_owning()
{
  if (m_owned_mem == nullptr)
    {
      m_owned_mem = new char[m_len+1];
      memcpy(m_owned_mem, m_buf, m_len+1);
      m_buf = nullptr;
      setg(m_owned_mem, m_owned_mem, m_owned_mem + m_len);
    }
}

rutz::imembuf::~imembuf()
{
  delete [] m_owned_mem;
}

int rutz::imembuf::underflow()
{
  if (gptr() < egptr())
    {
      return *gptr();
    }

  // Since there's no "external data source", if we've come to the end
  // of our current buffer, then we're just plain out of data.
  return EOF;
}

rutz::imemstream::imemstream(const char* s)
  :
  std::istream(&m_buf), m_buf(s)
{}

rutz::imemstream::imemstream(const char* s, size_t len)
  :
  std::istream(&m_buf), m_buf(s, len)
{}

rutz::imemstream::~imemstream() {}

rutz::icstrstream::icstrstream(const char* s)
  :
  std::istream(&m_buf), m_buf(s)
{
  m_buf.make_owning();
}

rutz::icstrstream::~icstrstream() {}
