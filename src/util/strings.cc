///////////////////////////////////////////////////////////////////////
//
// strings.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar  6 11:42:44 2000
// written: Mon Aug 20 14:43:09 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGS_CC_DEFINED
#define STRINGS_CC_DEFINED

#include "util/strings.h"

#include "util/algo.h"
#include "util/freelist.h"

#include <cstring>
#include <iostream.h>

#define NO_PROF
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace
{
  unsigned int safestrlen(const char* str)
  {
    return str ? strlen(str) : 0;
  }
}

//---------------------------------------------------------------------
//
// string_rep member definitions
//
//---------------------------------------------------------------------

namespace
{
  FreeList<string_rep> repList;
}

void* string_rep::operator new(size_t bytes)
{
  return repList.allocate(bytes);
}

void string_rep::operator delete(void* space)
{
  repList.deallocate(space);
}

void string_rep::realloc(std::size_t bufsize)
{
  Precondition(itsRefCount <= 1);

  string_rep new_rep(Util::max(itsCapacity*2 + 32, bufsize), 0);

  new_rep.append(this->itsLength, this->itsText);

  Util::swap(itsCapacity, new_rep.itsCapacity);
  Util::swap(itsLength, new_rep.itsLength);
  Util::swap(itsText, new_rep.itsText);
}

void string_rep::make_space(std::size_t maxlen)
{
  realloc(maxlen+1); itsLength = maxlen;
}

void string_rep::add_terminator()
{
  itsText[itsLength] = '\0';
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

string_rep::~string_rep()
{
  delete [] itsText;
}

string_rep* string_rep::getEmptyRep()
{
  static string_rep* empty_rep = 0;
  if (empty_rep == 0)
    {
      empty_rep = new string_rep(0, 0);
      empty_rep->incrRefCount();
    }

  return empty_rep;
}

string_rep* string_rep::make(std::size_t length, const char* text,
                             std::size_t capacity)
{
  if (length == 0)
    return getEmptyRep();

  return new string_rep(length, text, capacity);
}

string_rep* string_rep::clone() const
{
  return new string_rep(itsLength, itsText, itsCapacity);
}

void string_rep::makeUnique(string_rep*& rep)
{
  if (rep->itsRefCount <= 1) return;

  string_rep* new_rep = rep->clone();

  rep->decrRefCount();
  new_rep->incrRefCount();

  rep = new_rep;

  Postcondition(new_rep->itsRefCount == 1);
}

char* string_rep::data()
{
  Precondition(itsRefCount <= 1);

  return itsText;
}

void string_rep::clear()
{
  Precondition(itsRefCount <= 1);

  itsLength = 0;
  add_terminator();
}

//---------------------------------------------------------------------
//
// fstring member definitions
//
//---------------------------------------------------------------------

void fstring::init(Util::CharData cdata)
{
DOTRACE("fstring::init");
  Precondition(itsRep == 0);

  itsRep = string_rep::make(cdata.len, cdata.text);

  itsRep->incrRefCount();
}

fstring::fstring() :
  itsRep(string_rep::make(0,0))
{
DOTRACE("fstring::fstring");

  itsRep->incrRefCount();
}

fstring::fstring(const fstring& other) :
  itsRep(other.itsRep)
{
DOTRACE("fstring::fstring(const fstring&)");
  itsRep->incrRefCount();
}

fstring::~fstring()
{
DOTRACE("fstring::~fstring");
  itsRep->decrRefCount();
}

void fstring::swap(fstring& other)
{
DOTRACE("fstring::swap");

  Util::swap(itsRep, other.itsRep);
}

fstring& fstring::operator=(const char* text)
{
DOTRACE("fstring::operator=(const char*)");

  fstring copy(text);
  this->swap(copy);
  return *this;
}

fstring& fstring::operator=(const fstring& other)
{
DOTRACE("fstring::operator=(const fstring&)");

  fstring copy(other);
  this->swap(copy);
  return *this;
}

bool fstring::equals(const char* other) const
{
DOTRACE("fstring::equals(const char*)");
  return ( c_str() == other ||
           strcmp(c_str(), other) == 0 );
}

bool fstring::equals(const fstring& other) const
{
DOTRACE("fstring::equals(const fstring&)");

  return c_str() == other.c_str() ||
    ( length() == other.length() &&
      strcmp(c_str(), other.c_str()) == 0 );
}

bool fstring::ends_with(const fstring& ext) const
{
DOTRACE("fstring::ends_with");
  if (ext.length() > this->length())
    return false;

  unsigned int skip = this->length() - ext.length();

  return ext.equals(this->c_str() + skip);
}

bool fstring::operator<(const char* other) const
{
  // Check if we are pointing to the same string
  if (c_str() == other) return false;
  // ...otherwise do a string compare
  return strcmp(c_str(), other) < 0;
}

bool fstring::operator>(const char* other) const
{
  // Check if we are pointing to the same string
  if (c_str() == other) return false;
  // ...otherwise do a string compare
  return strcmp(c_str(), other) > 0;
}

void fstring::append_text(std::size_t length, const char* text)
{
DOTRACE("fstring::append_text");

  if (length > 0)
    {
      string_rep::makeUnique(itsRep);
      itsRep->append(length, text);
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
//        append(char(c));
      itsRep->append_no_terminate(char(c));
    }

  itsRep->add_terminator();
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
//        append(char(c));
      itsRep->append_no_terminate(char(c));
    }

  itsRep->add_terminator();
}

static const char vcid_strings_cc[] = "$Header$";
#endif // !STRINGS_CC_DEFINED
