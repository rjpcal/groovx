///////////////////////////////////////////////////////////////////////
//
// strings.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar  6 11:42:44 2000
// written: Mon Aug 20 13:28:29 2001
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

  string_rep new_rep(Util::max(itsAllocSize*2, bufsize), 0);

  new_rep.append(this->itsLength, this->itsText);

  Util::swap(itsAllocSize, new_rep.itsAllocSize);
  Util::swap(itsLength, new_rep.itsLength);
  Util::swap(itsText, new_rep.itsText);
}

void string_rep::make_space(std::size_t maxlen)
{
  realloc(maxlen+1); itsLength = maxlen;
}

void string_rep::append(std::size_t length, const char* text)
{
  Precondition(itsRefCount <= 1);
  Precondition(text != 0);

  if (itsLength + length + 1 <= itsAllocSize)
    {
      memcpy(itsText+itsLength, text, length);
      itsLength += length;
      itsText[itsLength] = '\0';
    }
  else
    {
      realloc(itsLength + length + 1);
      append(length, text);
    }

  Postcondition(itsLength+1 <= itsAllocSize);
}

string_rep::string_rep(std::size_t length, const char* text) :
  itsRefCount(0),
  itsAllocSize(length+1),
  itsLength(0),
  itsText(new char[itsAllocSize])
{
  if (text)
    append(length, text);
  else
    itsText[0] = '\0';
}

string_rep::string_rep(std::size_t length) :
  itsRefCount(0),
  itsAllocSize(length+1),
  itsLength(length),
  itsText(new char[itsAllocSize])
{
  itsText[0] = '\0';
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
      empty_rep = new string_rep(0);
      empty_rep->incrRefCount();
    }

  return empty_rep;
}

string_rep* string_rep::make(std::size_t length, const char* text)
{
  if (length == 0)
    return getEmptyRep();

  return new string_rep(length, text);
}

string_rep* string_rep::make(std::size_t length)
{
  if (length == 0)
    return getEmptyRep();

  return new string_rep(length);
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

//---------------------------------------------------------------------
//
// fstring member definitions
//
//---------------------------------------------------------------------

void fstring::init()
{
DOTRACE("fstring::init");
  Precondition(itsRep == 0);

  itsRep = string_rep::make(0);

  itsRep->incrRefCount();
}

fstring::fstring() :
  itsRep(0)
{
DOTRACE("fstring::fstring(int)");

  itsRep = string_rep::make(0);

  itsRep->incrRefCount();
}

fstring::fstring(const char* text) :
  itsRep(0)
{
DOTRACE("fstring::fstring(const char*)");

  itsRep = string_rep::make(safestrlen(text), text);

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

  string_rep* old_rep = itsRep;
  itsRep = string_rep::make(safestrlen(text), text);
  itsRep->incrRefCount();
  old_rep->decrRefCount();

  return *this;
}

fstring& fstring::operator=(const fstring& other)
{
DOTRACE("fstring::operator=(const fstring&)");

  string_rep* old_rep = itsRep;
  itsRep = other.itsRep;
  itsRep->incrRefCount();
  old_rep->decrRefCount();

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

STD_IO::istream& operator>>(STD_IO::istream& is, fstring& str)
{
DOTRACE("operator>>(std::istream&, fstring&)");
  str = "";
  is >> STD_IO::ws;
  while ( true )
    {
      int c = is.peek();
      if (c == EOF || isspace(c))
        break;
      str.append(char(is.get()));
    }
  return is;
}

STD_IO::ostream& operator<<(STD_IO::ostream& os, const fstring& str)
{
DOTRACE("operator<<(std::ostream&, fstring&)");
  os << str.c_str();
  return os;
}

STD_IO::istream& getline(STD_IO::istream& is, fstring& str)
{
DOTRACE("getline(std::istream&, fstring&)");
  return getline(is, str, '\n');
}

STD_IO::istream& getline(STD_IO::istream& is, fstring& str, char eol)
{
DOTRACE("getline(std::istream&, fstring&, char)");
  str = "";
  while ( true )
    {
      int c = is.get();
      if (c == EOF || c == eol)
        break;
      str.append(char(c));
    }
  return is;
}

static const char vcid_strings_cc[] = "$Header$";
#endif // !STRINGS_CC_DEFINED
