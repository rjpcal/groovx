///////////////////////////////////////////////////////////////////////
//
// strings.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Mar  6 11:42:44 2000
// written: Sat Sep 23 15:32:24 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGS_CC_DEFINED
#define STRINGS_CC_DEFINED

#include "util/strings.h"

#include <cstring>
#include <iostream.h>
#include <string>

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

fixed_string::fixed_string(const char* text) :
  itsText(text == 0 ? new char[1] : new char[strlen(text)+1]),
  itsLength(text == 0 ? 0 : strlen(text))
{
  if (text != 0)
	 strcpy(itsText, text);
  else
	 itsText[0] = 0;
}

fixed_string::fixed_string(const fixed_string& other) :
  itsText(new char[other.length() + 1]),
  itsLength(other.length())
{
  strcpy(itsText, other.itsText);
}

fixed_string::~fixed_string()
{
  delete [] itsText;
}

void fixed_string::swap(fixed_string& other)
{
  char* other_data = other.itsText;
  other.itsText = this->itsText;
  this->itsText = other_data;

  unsigned int other_len = other.itsLength;
  other.itsLength = this->itsLength;
  this->itsLength = other_len;
}

fixed_string& fixed_string::operator=(const char* text)
{
  if (itsText != text)
	 {
		fixed_string rhs_copy(text);
		this->swap(rhs_copy);
	 }
  return *this;
}

fixed_string& fixed_string::operator=(const fixed_string& other)
{
  if (itsText != other.itsText)
	 {
		fixed_string rhs_copy(other);
		this->swap(rhs_copy);
	 }
  return *this;
}

bool fixed_string::equals(const char* other) const
{
  return ( strcmp(itsText, other) == 0 );
}

bool fixed_string::equals(const string_literal& other) const
{
  return ( itsLength == other.itsLength &&
			  strcmp(itsText, other.itsText) == 0 );
}

bool fixed_string::equals(const fixed_string& other) const
{
  return ( itsLength == other.itsLength &&
			  strcmp(itsText, other.itsText) == 0 );
}

//---------------------------------------------------------------------
//
// dynamic_string member definitions
//
//---------------------------------------------------------------------

struct dynamic_string::Impl {
  Impl(const char* s) : text(s == 0 ? "" : s) {}
  string text;
};

dynamic_string::dynamic_string(const char* text) :
  itsImpl(new Impl(text))
{}

dynamic_string::dynamic_string(const fixed_string& other) :
  itsImpl(new Impl(other.c_str()))
{}

dynamic_string::dynamic_string(const dynamic_string& other) :
  itsImpl(new Impl(*(other.itsImpl)))
{}

dynamic_string::~dynamic_string()
{
  delete itsImpl;
}

void dynamic_string::swap(dynamic_string& other)
{
  itsImpl->text.swap(other.itsImpl->text);
}

dynamic_string& dynamic_string::operator=(const char* text)
{
  itsImpl->text = text;
  return *this;
}

dynamic_string& dynamic_string::operator=(const fixed_string& other)
{
  itsImpl->text = other.c_str();
  return *this;
}

dynamic_string& dynamic_string::operator=(const dynamic_string& other)
{
  itsImpl->text = other.itsImpl->text;
  return *this;
}

dynamic_string& dynamic_string::append(const char* text)
{
  itsImpl->text += text;
  return *this;
}

dynamic_string& dynamic_string::append(const fixed_string& other)
{
  itsImpl->text += other.c_str();
  return *this;
}

dynamic_string& dynamic_string::append(const dynamic_string& other)
{
  itsImpl->text += other.itsImpl->text;
  return *this;
}

bool dynamic_string::equals(const char* other) const
{
  return ( itsImpl->text == other );
}

bool dynamic_string::equals(const string_literal& other) const
{
  return ( itsImpl->text.length() == other.itsLength &&
			  itsImpl->text == other.itsText );
}

bool dynamic_string::equals(const fixed_string& other) const
{
  return ( itsImpl->text.length() == other.itsLength &&
			  itsImpl->text == other.itsText );
}

bool dynamic_string::equals(const dynamic_string& other) const
{
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
string& dynamic_string::rep(string*)
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
  string temp; is >> temp;
  str = temp.c_str();
  return is;
}

STD_IO::istream& operator>>(STD_IO::istream& is, dynamic_string& str)
{
  string temp; is >> temp;
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
  string temp;
  getline(is, temp);
  str = temp.c_str();
  return is;
}

STD_IO::istream& getline(STD_IO::istream& is, dynamic_string& str)
{
  string temp;
  getline(is, temp);
  str = temp.c_str();
  return is;
}

STD_IO::istream& getline(STD_IO::istream& is, fixed_string& str, char eol)
{
  string temp;
  getline(is, temp, eol);
  str = temp.c_str();
  return is;
}

STD_IO::istream& getline(STD_IO::istream& is, dynamic_string& str, char eol)
{
  string temp;
  getline(is, temp, eol);
  str = temp.c_str();
  return is;
}

static const char vcid_strings_cc[] = "$Header$";
#endif // !STRINGS_CC_DEFINED
