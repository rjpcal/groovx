///////////////////////////////////////////////////////////////////////
//
// strings.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar  6 11:42:44 2000
// written: Wed Aug  8 20:53:55 2001
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
#include <string>

#define NO_PROF
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

//---------------------------------------------------------------------
//
// string_literal member definitions
//
//---------------------------------------------------------------------

string_literal::string_literal(const char* literal) :
  itsText(literal),
  itsLength(strlen(literal))
{}

bool string_literal::equals(const char* other) const
{
  return ( strcmp(itsText, other) == 0 );
}

bool string_literal::equals(const string_literal& other) const
{
  return ( itsLength == other.itsLength &&
           strcmp(itsText, other.itsText) == 0 );
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
      memcpy(itsText+itsLength, text, length+1);
      itsLength += length;
    }
  else
    {
      realloc(itsLength + length + 1);
      append(length, text);
    }
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

  itsRep = string_rep::make(strlen(text), text);

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
  itsRep = string_rep::make(strlen(text), text);
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

bool fstring::equals(const string_literal& other) const
{
DOTRACE("fstring::equals(const string_literal&");
  return ( length() == other.length() &&
           strcmp(c_str(), other.c_str()) == 0 );
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

void fstring::append_text(std::size_t length, const char* text)
{
DOTRACE("fstring::append_text");

  string_rep::makeUnique(itsRep);
  itsRep->append(length, text);
}

//---------------------------------------------------------------------
//
// Input/Output function definitions
//
//---------------------------------------------------------------------

STD_IO::istream& operator>>(STD_IO::istream& is, fstring& str)
{
  std::string temp; is >> temp;
  str = temp.c_str();
  return is;
}

STD_IO::ostream& operator<<(STD_IO::ostream& os, const string_literal& str)
{
  os << str.c_str();
  return os;
}

STD_IO::ostream& operator<<(STD_IO::ostream& os, const fstring& str)
{
  os << str.c_str();
  return os;
}

STD_IO::istream& getline(STD_IO::istream& is, fstring& str)
{
  std::string temp;
  std::getline(is, temp);
  str = temp.c_str();
  return is;
}

STD_IO::istream& getline(STD_IO::istream& is, fstring& str, char eol)
{
  std::string temp;
  std::getline(is, temp, eol);
  str = temp.c_str();
  return is;
}

static const char vcid_strings_cc[] = "$Header$";
#endif // !STRINGS_CC_DEFINED
