///////////////////////////////////////////////////////////////////////
//
// strings.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar  6 11:42:44 2000
// written: Wed Aug  8 18:58:58 2001
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
      string_rep new_rep(Util::max(itsAllocSize*2, itsLength + length + 1), 0);

      new_rep.append(this->itsLength, this->itsText);
      new_rep.append(length, text);

      Util::swap(itsAllocSize, new_rep.itsAllocSize);
      Util::swap(itsLength, new_rep.itsLength);
      Util::swap(itsText, new_rep.itsText);
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
// fixed_string member definitions
//
//---------------------------------------------------------------------

fixed_string::fixed_string(std::size_t length) :
  itsRep(0)
{
DOTRACE("fixed_string::fixed_string(int)");

  itsRep = string_rep::make(length);

  itsRep->incrRefCount();
}

fixed_string::fixed_string(const char* text) :
  itsRep(0)
{
DOTRACE("fixed_string::fixed_string(const char*)");

  itsRep = string_rep::make(strlen(text), text);

  itsRep->incrRefCount();
}

fixed_string::fixed_string(const fixed_string& other) :
  itsRep(other.itsRep)
{
DOTRACE("fixed_string::fixed_string(const fixed_string&)");
  itsRep->incrRefCount();
}

fixed_string::~fixed_string()
{
DOTRACE("fixed_string::~fixed_string");
  itsRep->decrRefCount();
}

void fixed_string::swap(fixed_string& other)
{
DOTRACE("fixed_string::swap");

  Util::swap(itsRep, other.itsRep);
}

fixed_string& fixed_string::operator=(const char* text)
{
DOTRACE("fixed_string::operator=(const char*)");

  string_rep* old_rep = itsRep;
  itsRep = string_rep::make(strlen(text), text);
  itsRep->incrRefCount();
  old_rep->decrRefCount();

  return *this;
}

fixed_string& fixed_string::operator=(const fixed_string& other)
{
DOTRACE("fixed_string::operator=(const fixed_string&)");

  string_rep* old_rep = itsRep;
  itsRep = other.itsRep;
  itsRep->incrRefCount();
  old_rep->decrRefCount();

  return *this;
}

bool fixed_string::equals(const char* other) const
{
DOTRACE("fixed_string::equals(const char*)");
  return ( c_str() == other ||
           strcmp(c_str(), other) == 0 );
}

bool fixed_string::equals(const string_literal& other) const
{
DOTRACE("fixed_string::equals(const string_literal&");
  return ( length() == other.length() &&
           strcmp(c_str(), other.c_str()) == 0 );
}

bool fixed_string::equals(const fixed_string& other) const
{
DOTRACE("fixed_string::equals(const fixed_string&)");
  return c_str() == other.c_str() ||
    ( length() == other.length() &&
      strcmp(c_str(), other.c_str()) == 0 );
}

bool fixed_string::ends_with(const fixed_string& ext) const
{
DOTRACE("fixed_string::ends_with");
  if (ext.length() > this->length())
    return false;

  unsigned int skip = this->length() - ext.length();

  return ext.equals(this->c_str() + skip);
}

void fixed_string::append_text(std::size_t length, const char* text)
{
DOTRACE("fixed_string::append_text");

  string_rep::makeUnique(itsRep);
  itsRep->append(length, text);
}

//---------------------------------------------------------------------
//
// Input/Output function definitions
//
//---------------------------------------------------------------------

STD_IO::istream& operator>>(STD_IO::istream& is, fixed_string& str)
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

STD_IO::ostream& operator<<(STD_IO::ostream& os, const fixed_string& str)
{
  os << str.c_str();
  return os;
}

STD_IO::istream& getline(STD_IO::istream& is, fixed_string& str)
{
  std::string temp;
  std::getline(is, temp);
  str = temp.c_str();
  return is;
}

STD_IO::istream& getline(STD_IO::istream& is, fixed_string& str, char eol)
{
  std::string temp;
  std::getline(is, temp, eol);
  str = temp.c_str();
  return is;
}

static const char vcid_strings_cc[] = "$Header$";
#endif // !STRINGS_CC_DEFINED
