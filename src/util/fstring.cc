///////////////////////////////////////////////////////////////////////
//
// fstring.cc
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Oct 15 15:43:41 2004
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

#ifndef FSTRING_CC_DEFINED
#define FSTRING_CC_DEFINED

#include "fstring.h"

#include "util/algo.h"
#include "util/freelist.h"

#include <cctype>
#include <cstring>
#include <iostream>

#define NO_PROF
#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

//---------------------------------------------------------------------
//
// rutz::string_rep member definitions
//
//---------------------------------------------------------------------

namespace
{
  rutz::free_list<rutz::string_rep>* rep_list;
}

void* rutz::string_rep::operator new(size_t bytes)
{
  if (rep_list == 0) rep_list = new rutz::free_list<rutz::string_rep>;
  return rep_list->allocate(bytes);
}

void rutz::string_rep::operator delete(void* space)
{
  rep_list->deallocate(space);
}

rutz::string_rep::string_rep(std::size_t length, const char* text,
                             std::size_t capacity) :
  m_refcount(0),
  m_capacity(rutz::max(length+1, capacity)),
  m_length(0),
  m_text(new char[m_capacity])
{
  if (text)
    uniq_append(length, text);
  else
    add_terminator();
}

rutz::string_rep::~string_rep() throw()
{
DOTRACE("rutz::string_rep::~string_rep");

  delete [] m_text;
  m_text = (char*)0xdeadbeef;
}

rutz::string_rep* rutz::string_rep::get_empty_rep()
{
  static rutz::string_rep* empty_rep = 0;
  if (empty_rep == 0)
    {
      empty_rep = new rutz::string_rep(0, 0);
      empty_rep->incr_ref_count();
    }

  return empty_rep;
}

rutz::string_rep* rutz::string_rep::make(std::size_t length,
                                         const char* text,
                                         std::size_t capacity)
{
  if (length == 0)
    return get_empty_rep();

  return new rutz::string_rep(length, text, capacity);
}

rutz::string_rep* rutz::string_rep::clone() const
{
  return new rutz::string_rep(m_length, m_text, m_capacity);
}

void rutz::string_rep::make_unique(rutz::string_rep*& rep)
{
  if (rep->m_refcount <= 1) return;

  rutz::string_rep* new_rep = rep->clone();

  rep->decr_ref_count();
  new_rep->incr_ref_count();

  rep = new_rep;

  POSTCONDITION(new_rep->m_refcount == 1);
}

char* rutz::string_rep::uniq_data() throw()
{
  PRECONDITION(m_refcount <= 1);

  return m_text;
}

void rutz::string_rep::uniq_clear() throw()
{
  PRECONDITION(m_refcount <= 1);

  m_length = 0;
  add_terminator();
}

void rutz::string_rep::uniq_append_no_terminate(char c)
{
  if (m_length + 2 <= m_capacity)
    {
      m_text[m_length++] = c;
    }
  else
    {
      uniq_realloc(m_length + 2);
      m_text[m_length++] = c;
    }
}

void rutz::string_rep::add_terminator() throw()
{
  m_text[m_length] = '\0';
}

void rutz::string_rep::uniq_set_length(std::size_t length) throw()
{
  PRECONDITION(m_refcount <= 1);
  PRECONDITION(length+1 < m_capacity);
  m_length = length;
  add_terminator();
}

void rutz::string_rep::uniq_append(std::size_t length, const char* text)
{
  PRECONDITION(m_refcount <= 1);
  PRECONDITION(text != 0);

  if (m_length + length + 1 <= m_capacity)
    {
      memcpy(m_text+m_length, text, length);
      m_length += length;
      add_terminator();
    }
  else
    {
      uniq_realloc(m_length + length + 1);
      uniq_append(length, text);
    }

  POSTCONDITION(m_length+1 <= m_capacity);
  POSTCONDITION(m_text[m_length] == '\0');
}

void rutz::string_rep::uniq_realloc(std::size_t capacity)
{
  PRECONDITION(m_refcount <= 1);

  rutz::string_rep new_rep(rutz::max(m_capacity*2 + 32, capacity), 0);

  new_rep.uniq_append(this->m_length, this->m_text);

  rutz::swap2(m_capacity, new_rep.m_capacity);
  rutz::swap2(m_length, new_rep.m_length);
  rutz::swap2(m_text, new_rep.m_text);
}

void rutz::string_rep::uniq_reserve(std::size_t capacity)
{
  if (m_capacity < capacity)
    uniq_realloc(capacity);
}

void rutz::string_rep::debug_dump() const throw()
{
  dbg_eval_nl(0, (void*)this);
  dbg_eval_nl(0, m_refcount);
  dbg_eval_nl(0, m_length);
  dbg_eval_nl(0, (void*)m_text);
  dbg_eval_nl(0, m_text);
  for (unsigned int i = 0; i < m_length; ++i)
    dbg_print(0, (void*)(int)m_text[i]);
  dbg_print_nl(0, "");
}

//---------------------------------------------------------------------
//
// fstring member definitions
//
//---------------------------------------------------------------------

void rutz::fstring::init_empty()
{
DOTRACE("rutz::fstring::init_empty");
  PRECONDITION(m_rep == 0);

  m_rep = string_rep::make(0, 0);

  m_rep->incr_ref_count();
}

void rutz::fstring::init_range(char_range r)
{
DOTRACE("rutz::fstring::init");
  PRECONDITION(m_rep == 0);

  m_rep = string_rep::make(r.len, r.text);

  m_rep->incr_ref_count();
}

rutz::fstring::fstring() :
  m_rep(string_rep::make(0,0))
{
DOTRACE("rutz::fstring::fstring");

  m_rep->incr_ref_count();
}

rutz::fstring::fstring(const rutz::fstring& other) throw() :
  m_rep(other.m_rep)
{
DOTRACE("rutz::fstring::fstring(const fstring&)");
  m_rep->incr_ref_count();
}

rutz::fstring::~fstring() throw()
{
DOTRACE("rutz::fstring::~fstring");

  dbg_dump(7, *this);

  if (m_rep->decr_ref_count() == 0)
    m_rep = (string_rep*)0xdeadbeef;
}

void rutz::fstring::swap(rutz::fstring& other) throw()
{
DOTRACE("rutz::fstring::swap");

  rutz::swap2(m_rep, other.m_rep);
}

rutz::fstring& rutz::fstring::operator=(const char* text)
{
DOTRACE("rutz::fstring::operator=(const char*)");

  fstring copy(text);
  this->swap(copy);
  return *this;
}

rutz::fstring& rutz::fstring::operator=(const fstring& other) throw()
{
DOTRACE("rutz::fstring::operator=(const fstring&)");

  rutz::fstring copy(other);
  this->swap(copy);
  return *this;
}

bool rutz::fstring::ends_with(const fstring& ext) const throw()
{
DOTRACE("rutz::fstring::ends_with");
  if (ext.length() > this->length())
    return false;

  unsigned int skip = this->length() - ext.length();

  return ext.equals(this->c_str() + skip);
}

void rutz::fstring::clear()
{
DOTRACE("rutz::fstring::clear");

  if (m_rep->is_unique())
    {
      m_rep->uniq_clear();
    }
  else
    {
      fstring().swap(*this);
    }
}

bool rutz::fstring::equals(const char* other) const throw()
{
DOTRACE("rutz::fstring::equals(const char*)");
  return ( c_str() == other ||
           strcmp(c_str(), other) == 0 );
}

bool rutz::fstring::equals(const fstring& other) const throw()
{
DOTRACE("rutz::fstring::equals(const fstring&)");

  return c_str() == other.c_str() ||
    ( length() == other.length() &&
      strcmp(c_str(), other.c_str()) == 0 );
}

bool rutz::fstring::operator<(const char* other) const throw()
{
DOTRACE("rutz::fstring::operator<");
  // Check if we are pointing to the same string
  if (c_str() == other) return false;
  // ...otherwise do a string compare
  return strcmp(c_str(), other) < 0;
}

bool rutz::fstring::operator>(const char* other) const throw()
{
DOTRACE("rutz::fstring::operator>");
  // Check if we are pointing to the same string
  if (c_str() == other) return false;
  // ...otherwise do a string compare
  return strcmp(c_str(), other) > 0;
}

void rutz::fstring::do_append(char c) { append_range(&c, 1); }
void rutz::fstring::do_append(const char* s) { if (s != 0) append_range(s, strlen(s)); }
void rutz::fstring::do_append(const fstring& s) { append_range(s.c_str(), s.length()); }

#define APPEND(fmt, val)                        \
  const int SZ = 64;                            \
  char buf[SZ];                                 \
  int n = snprintf(buf, SZ, fmt, (val));        \
  ASSERT(n > 0 && n < SZ);                      \
  append_range(&buf[0], std::size_t(n))

void rutz::fstring::do_append(bool x)          { APPEND("%d", int(x)); }
void rutz::fstring::do_append(int x)           { APPEND("%d", x); }
void rutz::fstring::do_append(unsigned int x)  { APPEND("%u", x); }
void rutz::fstring::do_append(long x)          { APPEND("%ld", x); }
void rutz::fstring::do_append(unsigned long x) { APPEND("%lu", x); }
void rutz::fstring::do_append(double x)        { APPEND("%g", x); }

#undef APPEND

void rutz::fstring::append_range(const char* text, std::size_t len)
{
DOTRACE("rutz::fstring::append_range");

  if (len == 0)
    return;

  // special case for when the current string is empty: just make a new
  // rep with the to-be-appended text and swap
  if (m_rep->length() == 0)
    {
      string_rep* newrep = string_rep::make(len, text);
      string_rep* oldrep = m_rep;
      m_rep = newrep;
      oldrep->decr_ref_count();
      m_rep->incr_ref_count();
    }
  else
    {
      string_rep::make_unique(m_rep);
      m_rep->uniq_append(len, text);
    }
}

//---------------------------------------------------------------------
//
// Input/Output function definitions
//
//---------------------------------------------------------------------

void rutz::fstring::read(STD_IO::istream& is)
{
DOTRACE("rutz::fstring::read");
  clear();
  string_rep::make_unique(m_rep);
  is >> STD_IO::ws;
  while ( true )
    {
      int c = is.get();
      if (c == EOF || isspace(c))
        {
          is.unget();
          break;
        }
      m_rep->uniq_append_no_terminate(char(c));
    }

  m_rep->add_terminator();
}

void rutz::fstring::readsome(STD_IO::istream& is, unsigned int count)
{
DOTRACE("rutz::fstring::readsome");

  if (count > 0)
    {
      string_rep::make_unique(m_rep);

      m_rep->uniq_reserve(count+1);
#ifndef PRESTANDARD_IOSTREAMS
      unsigned int numread = is.readsome(m_rep->uniq_data(), count);
      m_rep->uniq_set_length(numread);
#else
      is.read(m_rep->uniq_data(), count);
      if (is.eof()) is.clear();
      unsigned int numread = is.gcount();
      m_rep->uniq_set_length(numread);
#endif
    }
}

void rutz::fstring::write(STD_IO::ostream& os) const
{
DOTRACE("rutz::fstring::write");
  os.write(c_str(), length());
}

void rutz::fstring::readline(STD_IO::istream& is)
{
  readline(is, '\n');
}

void rutz::fstring::readline(STD_IO::istream& is, char eol)
{
DOTRACE("rutz::fstring::readline");
  clear();
  string_rep::make_unique(m_rep);
  while ( true )
    {
      int c = is.get();
      if (c == EOF || c == eol)
        break;
      m_rep->uniq_append_no_terminate(char(c));
    }

  m_rep->add_terminator();
}

void rutz::fstring::debug_dump() const throw()
{
  dbg_eval_nl(0, (void*)this);
  m_rep->debug_dump();
}

static const char vcid_fstring_cc[] = "$Header$";
#endif // !FSTRING_CC_DEFINED