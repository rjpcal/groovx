///////////////////////////////////////////////////////////////////////
//
// strings.cc
//
// Copyright (c) 2000-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar  6 11:42:44 2000
// written: Mon Jan 13 11:08:25 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGS_CC_DEFINED
#define STRINGS_CC_DEFINED

#include "util/strings.h"

#include "util/algo.h"
#include "util/freelist.h"

#include <cctype>
#include <iostream>

#define NO_PROF
#include "util/trace.h"
#include "util/debug.h"

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

void string_rep::add_terminator()
{
  itsText[itsLength] = '\0';
}

void string_rep::set_length(std::size_t length)
{
  Precondition(length+1 < itsCapacity);
  itsLength = length;
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
DOTRACE("string_rep::~string_rep");

  delete [] itsText;
  itsText = (char*)0xdeadbeef;
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

void string_rep::debugDump() const
{
  dbgEvalNL(0, (void*)this);
  dbgEvalNL(0, itsRefCount);
  dbgEvalNL(0, itsLength);
  dbgEvalNL(0, (void*)itsText);
  dbgEvalNL(0, itsText);
  for (unsigned int i = 0; i < itsLength; ++i)
    dbgPrint(0, (void*)itsText[i]);
  dbgPrintNL(0, "");
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

  dbgDump(7, *this);

  if (itsRep->decrRefCount() == 0)
    itsRep = (string_rep*)0xdeadbeef;
}

void fstring::swap(fstring& other)
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
DOTRACE("fstring::operator<");
  // Check if we are pointing to the same string
  if (c_str() == other) return false;
  // ...otherwise do a string compare
  return strcmp(c_str(), other) < 0;
}

bool fstring::operator>(const char* other) const
{
DOTRACE("fstring::operator>");
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
      itsRep->append_no_terminate(char(c));
    }

  itsRep->add_terminator();
}

void fstring::readsome(STD_IO::istream& is, unsigned int count)
{
DOTRACE("fstring::readsome");

  if (count > 0)
    {
      string_rep::makeUnique(itsRep);
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

void fstring::debugDump() const
{
  dbgEvalNL(0, (void*)this);
  itsRep->debugDump();
}

static const char vcid_strings_cc[] = "$Header$";
#endif // !STRINGS_CC_DEFINED
