///////////////////////////////////////////////////////////////////////
//
// strings.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Mar  6 11:42:44 2000
// written: Thu Nov  2 18:23:26 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGS_CC_DEFINED
#define STRINGS_CC_DEFINED

#include "util/strings.h"

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
// fixed_string member definitions
//
//---------------------------------------------------------------------

namespace {
  struct FreeNode {
	 FreeNode* next;
  };

  FreeNode* fsFreeList = 0;
}

void* fixed_string::Rep::operator new(size_t bytes) {
  Assert(bytes == sizeof(Rep));
  if (fsFreeList == 0)
	 return ::operator new(bytes);
  FreeNode* node = fsFreeList;
  fsFreeList = fsFreeList->next;
  return (void*)node;
}

void fixed_string::Rep::operator delete(void* space) {
  ((FreeNode*)space)->next = fsFreeList;
  fsFreeList = (FreeNode*)space;
}

fixed_string::Rep::Rep(const char* text)
{
  if (text == 0) text = "";

  itsRefCount = 0; 
  itsLength = strlen(text);
  itsText = new char[itsLength+1];

  memcpy(itsText, text, itsLength+1);
}

fixed_string::Rep::~Rep()
{ delete [] itsText; }

fixed_string::fixed_string(const char* text) :
  itsRep(0)
{
DOTRACE("fixed_string::fixed_string(const char*)");

  static Rep* empty_rep = 0;
  if (text == 0 || text[0] == '\0')
	 {
		if (empty_rep == 0)
		  {
			 empty_rep = Rep::make("");
			 empty_rep->incrRefCount();
		  }

		itsRep = empty_rep;
	 }
  else {
	 itsRep = Rep::make(text);
  }

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
  Rep* other_rep = other.itsRep;
  other.itsRep = this->itsRep;
  this->itsRep = other_rep;
}

fixed_string& fixed_string::operator=(const char* text)
{
DOTRACE("fixed_string::operator=(const char*)");

  Rep* old_rep = itsRep;
  itsRep = Rep::make(text);
  itsRep->incrRefCount();
  old_rep->decrRefCount();

  return *this;
}

fixed_string& fixed_string::operator=(const fixed_string& other)
{
DOTRACE("fixed_string::operator=(const fixed_string&)");

  Rep* old_rep = itsRep;
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
  itsImpl->text = text;
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

dynamic_string& dynamic_string::append(const char* text)
{
DOTRACE("dynamic_string::append(const char*)");
  itsImpl->text += text;
  return *this;
}

dynamic_string& dynamic_string::append(const fixed_string& other)
{
DOTRACE("dynamic_string::append(const fixed_string&)");
  itsImpl->text += other.c_str();
  return *this;
}

dynamic_string& dynamic_string::append(const dynamic_string& other)
{
DOTRACE("dynamic_string::append(const dynamic_string&)");
  itsImpl->text += other.itsImpl->text;
  return *this;
}

bool dynamic_string::equals(const char* other) const
{
DOTRACE("dynamic_string::equals(const char*)");
  return ( itsImpl->text == other );
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

template <>
std::string& dynamic_string::rep(std::string*)
{
  return itsImpl->text;
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
