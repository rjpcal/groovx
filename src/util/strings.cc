///////////////////////////////////////////////////////////////////////
//
// strings.cc
//
// Copyright (c) 2000-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Mar  6 11:42:44 2000
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGS_CC_DEFINED
#define STRINGS_CC_DEFINED

#include "util/strings.h"

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
// string_rep member definitions
//
//---------------------------------------------------------------------

namespace
{
  FreeList<string_rep>* repList;
}

void* string_rep::operator new(size_t bytes)
{
  if (repList == 0) repList = new FreeList<string_rep>;
  return repList->allocate(bytes);
}

void string_rep::operator delete(void* space)
{
  repList->deallocate(space);
}

string_rep::string_rep(std::size_t length, const char* text,
                       std::size_t capacity) :
  itsRefCount(0),
  itsCapacity(Util::max(length+1, capacity)),
  itsLength(0),
  itsText(new char[itsCapacity])
{
  if (text)
    append(length, text);
  else
    add_terminator();
}

string_rep::~string_rep() throw()
{
DOTRACE("string_rep::~string_rep");

  delete [] itsText;
  itsText = (char*)0xdeadbeef;
}

string_rep* string_rep::get_empty_rep()
{
  static string_rep* empty_rep = 0;
  if (empty_rep == 0)
    {
      empty_rep = new string_rep(0, 0);
      empty_rep->incr_ref_count();
    }

  return empty_rep;
}

string_rep* string_rep::make(std::size_t length, const char* text,
                             std::size_t capacity)
{
  if (length == 0)
    return get_empty_rep();

  return new string_rep(length, text, capacity);
}

string_rep* string_rep::clone() const
{
  return new string_rep(itsLength, itsText, itsCapacity);
}

void string_rep::make_unique(string_rep*& rep)
{
  if (rep->itsRefCount <= 1) return;

  string_rep* new_rep = rep->clone();

  rep->decr_ref_count();
  new_rep->incr_ref_count();

  rep = new_rep;

  Postcondition(new_rep->itsRefCount == 1);
}

char* string_rep::data() throw()
{
  Precondition(itsRefCount <= 1);

  return itsText;
}

void string_rep::clear() throw()
{
  Precondition(itsRefCount <= 1);

  itsLength = 0;
  add_terminator();
}

void string_rep::append_no_terminate(char c)
{
  if (itsLength + 2 <= itsCapacity)
    {
      itsText[itsLength++] = c;
    }
  else
    {
      realloc(itsLength + 2);
      itsText[itsLength++] = c;
    }
}

void string_rep::add_terminator() throw()
{
  itsText[itsLength] = '\0';
}

void string_rep::set_length(std::size_t length) throw()
{
  Precondition(length+1 < itsCapacity);
  itsLength = length;
  add_terminator();
}

void string_rep::append(std::size_t length, const char* text)
{
  Precondition(itsRefCount <= 1);
  Precondition(text != 0);

  if (itsLength + length + 1 <= itsCapacity)
    {
      memcpy(itsText+itsLength, text, length);
      itsLength += length;
      add_terminator();
    }
  else
    {
      realloc(itsLength + length + 1);
      append(length, text);
    }

  Postcondition(itsLength+1 <= itsCapacity);
}

void string_rep::realloc(std::size_t capacity)
{
  Precondition(itsRefCount <= 1);

  string_rep new_rep(Util::max(itsCapacity*2 + 32, capacity), 0);

  new_rep.append(this->itsLength, this->itsText);

  Util::swap2(itsCapacity, new_rep.itsCapacity);
  Util::swap2(itsLength, new_rep.itsLength);
  Util::swap2(itsText, new_rep.itsText);
}

void string_rep::reserve(std::size_t capacity)
{
  if (itsCapacity < capacity)
    realloc(capacity);
}

void string_rep::debugDump() const throw()
{
  dbgEvalNL(0, (void*)this);
  dbgEvalNL(0, itsRefCount);
  dbgEvalNL(0, itsLength);
  dbgEvalNL(0, (void*)itsText);
  dbgEvalNL(0, itsText);
  for (unsigned int i = 0; i < itsLength; ++i)
    dbgPrint(0, (void*)(int)itsText[i]);
  dbgPrintNL(0, "");
}

//---------------------------------------------------------------------
//
// fstring member definitions
//
//---------------------------------------------------------------------

void fstring::init_empty()
{
DOTRACE("fstring::init_empty");
  Precondition(itsRep == 0);

  itsRep = string_rep::make(0, 0);

  itsRep->incr_ref_count();
}

void fstring::init_range(char_range r)
{
DOTRACE("fstring::init");
  Precondition(itsRep == 0);

  itsRep = string_rep::make(r.len, r.text);

  itsRep->incr_ref_count();
}

fstring::fstring() :
  itsRep(string_rep::make(0,0))
{
DOTRACE("fstring::fstring");

  itsRep->incr_ref_count();
}

fstring::fstring(const fstring& other) throw() :
  itsRep(other.itsRep)
{
DOTRACE("fstring::fstring(const fstring&)");
  itsRep->incr_ref_count();
}

fstring::~fstring() throw()
{
DOTRACE("fstring::~fstring");

  dbgDump(7, *this);

  if (itsRep->decr_ref_count() == 0)
    itsRep = (string_rep*)0xdeadbeef;
}

void fstring::swap(fstring& other) throw()
{
DOTRACE("fstring::swap");

  Util::swap2(itsRep, other.itsRep);
}

fstring& fstring::operator=(const char* text)
{
DOTRACE("fstring::operator=(const char*)");

  fstring copy(text);
  this->swap(copy);
  return *this;
}

fstring& fstring::operator=(const fstring& other) throw()
{
DOTRACE("fstring::operator=(const fstring&)");

  fstring copy(other);
  this->swap(copy);
  return *this;
}

bool fstring::ends_with(const fstring& ext) const throw()
{
DOTRACE("fstring::ends_with");
  if (ext.length() > this->length())
    return false;

  unsigned int skip = this->length() - ext.length();

  return ext.equals(this->c_str() + skip);
}

bool fstring::equals(const char* other) const throw()
{
DOTRACE("fstring::equals(const char*)");
  return ( c_str() == other ||
           strcmp(c_str(), other) == 0 );
}

bool fstring::equals(const fstring& other) const throw()
{
DOTRACE("fstring::equals(const fstring&)");

  return c_str() == other.c_str() ||
    ( length() == other.length() &&
      strcmp(c_str(), other.c_str()) == 0 );
}

bool fstring::operator<(const char* other) const throw()
{
DOTRACE("fstring::operator<");
  // Check if we are pointing to the same string
  if (c_str() == other) return false;
  // ...otherwise do a string compare
  return strcmp(c_str(), other) < 0;
}

bool fstring::operator>(const char* other) const throw()
{
DOTRACE("fstring::operator>");
  // Check if we are pointing to the same string
  if (c_str() == other) return false;
  // ...otherwise do a string compare
  return strcmp(c_str(), other) > 0;
}

namespace
{
  const int SZ = 64;
}

void fstring::do_append(char c) { append_range(&c, 1); }
void fstring::do_append(const char* s) { if (s != 0) append_range(s, strlen(s)); }
void fstring::do_append(const fstring& s) { append_range(s.c_str(), s.length()); }

#define APPEND(fmt, val) \
  char buf[SZ]; int n = snprintf(buf, SZ, fmt, (val)); Assert(n > 0 && n < SZ); append_range(&buf[0], std::size_t(n))

void fstring::do_append(bool x)          { APPEND("%d", int(x)); }
void fstring::do_append(int x)           { APPEND("%d", x); }
void fstring::do_append(unsigned int x)  { APPEND("%u", x); }
void fstring::do_append(long x)          { APPEND("%ld", x); }
void fstring::do_append(unsigned long x) { APPEND("%lu", x); }
void fstring::do_append(double x)        { APPEND("%g", x); }

#undef APPEND

void fstring::append_range(const char* text, std::size_t len)
{
DOTRACE("fstring::append_range");

  if (len == 0)
    return;

  // special case for when the current string is empty: just make a new
  // rep with the to-be-appended text and swap
  if (itsRep->length() == 0)
    {
      string_rep* newrep = string_rep::make(len, text);
      string_rep* oldrep = itsRep;
      itsRep = newrep;
      oldrep->decr_ref_count();
      itsRep->incr_ref_count();
    }
  else
    {
      string_rep::make_unique(itsRep);
      itsRep->append(len, text);
    }
}

//---------------------------------------------------------------------
//
// Input/Output function definitions
//
//---------------------------------------------------------------------

void fstring::read(STD_IO::istream& is)
{
DOTRACE("fstring::read");
  clear();
  is >> STD_IO::ws;
  while ( true )
    {
      int c = is.get();
      if (c == EOF || isspace(c))
        {
          is.unget();
          break;
        }
      itsRep->append_no_terminate(char(c));
    }

  itsRep->add_terminator();
}

void fstring::readsome(STD_IO::istream& is, unsigned int count)
{
DOTRACE("fstring::readsome");

  if (count > 0)
    {
      string_rep::make_unique(itsRep);
      itsRep->reserve(count+1);
#ifndef PRESTANDARD_IOSTREAMS
      unsigned int numread = is.readsome(itsRep->data(), count);
      itsRep->set_length(numread);
#else
      is.read(itsRep->data(), count);
      if (is.eof()) is.clear();
      unsigned int numread = is.gcount();
      itsRep->set_length(numread);
#endif
    }
}

void fstring::write(STD_IO::ostream& os) const
{
DOTRACE("fstring::write");
  os.write(c_str(), length());
}

void fstring::readline(STD_IO::istream& is)
{
  readline(is, '\n');
}

void fstring::readline(STD_IO::istream& is, char eol)
{
DOTRACE("fstring::readline");
  clear();
  while ( true )
    {
      int c = is.get();
      if (c == EOF || c == eol)
        break;
      itsRep->append_no_terminate(char(c));
    }

  itsRep->add_terminator();
}

void fstring::debugDump() const throw()
{
  dbgEvalNL(0, (void*)this);
  itsRep->debugDump();
}

static const char vcid_strings_cc[] = "$Header$";
#endif // !STRINGS_CC_DEFINED
