///////////////////////////////////////////////////////////////////////
//
// strings.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar  6 11:42:44 2000
// written: Sun Aug  5 20:14:54 2001
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

string_rep::string_rep(std::size_t length, const char* text) :
  itsRefCount(0),
  itsText(new char[length+1]),
  itsLength(length)
{
  if (text)
    memcpy(itsText, text, itsLength+1);
  else
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

  string_rep* p = new string_rep(length, text);

  return p;
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
  return ( itsRep->itsText == other ||
           strcmp(itsRep->itsText, other) == 0 );
}

bool fixed_string::equals(const string_literal& other) const
{
DOTRACE("fixed_string::equals(const string_literal&");
  return ( itsRep->itsLength == other.length() &&
           strcmp(itsRep->itsText, other.c_str()) == 0 );
}

bool fixed_string::equals(const fixed_string& other) const
{
DOTRACE("fixed_string::equals(const fixed_string&)");
  return itsRep->itsText == other.itsRep->itsText ||
    ( itsRep->itsLength == other.itsRep->itsLength &&
      strcmp(itsRep->itsText, other.itsRep->itsText) == 0 );
}

bool fixed_string::ends_with(const fixed_string& ext) const
{
DOTRACE("fixed_string::ends_with");
  if (ext.length() > this->length())
    return false;

  unsigned int skip = this->length() - ext.length();

  return ext.equals(this->c_str() + skip);
}

//---------------------------------------------------------------------
//
// dynamic_string member definitions
//
//---------------------------------------------------------------------

struct dynamic_string::Impl {
  Impl(const char* s) : text(s == 0 ? "" : s) {}
  std::string text;
};

dynamic_string::dynamic_string(const char* text) :
  itsImpl(new Impl(text))
{
DOTRACE("dynamic_string::dynamic_string");
}

dynamic_string::dynamic_string(const fixed_string& other) :
  itsImpl(new Impl(other.c_str()))
{
DOTRACE("dynamic_string::dynamic_string");
}

dynamic_string::dynamic_string(const dynamic_string& other) :
  itsImpl(new Impl(*(other.itsImpl)))
{
DOTRACE("dynamic_string::dynamic_string");
}

dynamic_string::~dynamic_string()
{
DOTRACE("dynamic_string::~dynamic_string");
  delete itsImpl;
}

void dynamic_string::swap(dynamic_string& other)
{
DOTRACE("dynamic_string::swap");
  itsImpl->text.swap(other.itsImpl->text);
}

dynamic_string& dynamic_string::operator=(const char* text)
{
DOTRACE("dynamic_string::operator=(const char*)");
  itsImpl->text = (text) ? text : "";
  return *this;
}

dynamic_string& dynamic_string::operator=(const fixed_string& other)
{
DOTRACE("dynamic_string::operator=(const fixed_string&)");
  itsImpl->text = other.c_str();
  return *this;
}

dynamic_string& dynamic_string::operator=(const dynamic_string& other)
{
DOTRACE("dynamic_string::operator=(const dynamic_string&)");
  itsImpl->text = other.itsImpl->text;
  return *this;
}

void dynamic_string::appendCstring(const char* text)
{
DOTRACE("dynamic_string::appendCstring");
  if (text) itsImpl->text += text;
}

bool dynamic_string::equals(const char* other) const
{
DOTRACE("dynamic_string::equals(const char*)");
  return (other != 0) && ( itsImpl->text == other );
}

bool dynamic_string::equals(const string_literal& other) const
{
DOTRACE("dynamic_string::equals(const string_literal&)");
  return ( itsImpl->text.length() == other.itsLength &&
           itsImpl->text == other.itsText );
}

bool dynamic_string::equals(const fixed_string& other) const
{
DOTRACE("dynamic_string::equals(const fixed_string&)");
  return ( itsImpl->text.length() == other.length() &&
           itsImpl->text == other.c_str() );
}

bool dynamic_string::equals(const dynamic_string& other) const
{
DOTRACE("dynamic_string::equals(const dynamic_string&)");
  return ( itsImpl->text == other.itsImpl->text );
}

const char* dynamic_string::c_str() const
{
  return itsImpl->text.c_str();
}

unsigned int dynamic_string::length() const
{
  return itsImpl->text.length();
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

STD_IO::istream& operator>>(STD_IO::istream& is, dynamic_string& str)
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

STD_IO::ostream& operator<<(STD_IO::ostream& os, const dynamic_string& str)
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

STD_IO::istream& getline(STD_IO::istream& is, dynamic_string& str)
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

STD_IO::istream& getline(STD_IO::istream& is, dynamic_string& str, char eol)
{
  std::string temp;
  std::getline(is, temp, eol);
  str = temp.c_str();
  return is;
}

static const char vcid_strings_cc[] = "$Header$";
#endif // !STRINGS_CC_DEFINED
