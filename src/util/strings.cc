///////////////////////////////////////////////////////////////////////
//
// strings.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Mar  6 11:42:44 2000
// written: Mon Mar  6 11:43:02 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGS_CC_DEFINED
#define STRINGS_CC_DEFINED

#include "strings.h"

#include <cstring>
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

//---------------------------------------------------------------------
//
// fixed_string member definitions
//
//---------------------------------------------------------------------

fixed_string::fixed_string(const char* text) :
  itsText(new char[strlen(text)+1]),
  itsLength(strlen(text))
{
  strcpy(itsText, text);
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

//---------------------------------------------------------------------
//
// dynamic_string member definitions
//
//---------------------------------------------------------------------

struct dynamic_string::Impl {
  Impl(const char* s) : text(s) {}
  string text;
};

dynamic_string::dynamic_string(const char* text) :
  itsImpl(new Impl(text))
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

dynamic_string& dynamic_string::operator=(const dynamic_string& other)
{
  itsImpl->text = other.itsImpl->text;
  return *this;
}

dynamic_string& dynamic_string::operator+=(const char* text)
{
  itsImpl->text += text;
  return *this;
}

dynamic_string& dynamic_string::operator+=(const dynamic_string& other)
{
  itsImpl->text += other.itsImpl->text;
  return *this;
}

const char* dynamic_string::c_str() const
{
  return itsImpl->text.c_str();
}

unsigned int dynamic_string::length() const
{
  return itsImpl->text.length();
}

static const char vcid_strings_cc[] = "$Header$";
#endif // !STRINGS_CC_DEFINED
