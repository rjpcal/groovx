///////////////////////////////////////////////////////////////////////
//
// strings.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Mar  6 11:16:48 2000
// written: Thu Mar  9 16:18:35 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGS_H_DEFINED
#define STRINGS_H_DEFINED


///////////////////////////////////////////////////////////////////////
/**
 *
 * \c string_literal is a simple string class that just holds a
 * pointer to a const char* that has been allocated elsewhere. The \c
 * char* used as an initializer must point to storage that will be
 * valid for the entire lifetime of the \c string_literal, since no
 * copy is made of the text. No non-const operations are provided for
 * \c string_literal.
 *
 **/
///////////////////////////////////////////////////////////////////////

class string_literal {
public:
  string_literal(const char* literal);

  const char* c_str() const { return itsText; }
  unsigned int length() const { return itsLength; }
  bool empty() const { return (length() == 0); }

private:
  const char* const itsText;
  const unsigned int itsLength;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * \c fixed_string is a simple string class that holds a pointer to a
 * dynamically-allocated char array. The initializer does not have to
 * reside in permanent storage, since a copy is made when the \c
 * fixed_string is constructed. Assignment is allowed, with copy
 * semantics. Also, a \c swap() operation is provided.
 *
 **/
///////////////////////////////////////////////////////////////////////

class fixed_string {
public:
  fixed_string(const char* text = "");
  fixed_string(const fixed_string& other);
  ~fixed_string();

  void swap(fixed_string& other);

  fixed_string& operator=(const char* text);
  fixed_string& operator=(const fixed_string& other);

  char* data() { return itsText; }

  const char* c_str() const { return itsText; }
  unsigned int length() const { return itsLength; }
  bool empty() const { return (length() == 0); }

private:
  char* itsText;
  unsigned int itsLength;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * \c dynamic_string is basically a wrapper around, and a limited
 * subset of the functionality of, \c std::string. \c dynamic_string
 * provides the functionality of \c fixed_string, and also allows
 * in-place concatenation.
 *
 **/
///////////////////////////////////////////////////////////////////////

class dynamic_string {
public:
  dynamic_string(const char* text = "");
  dynamic_string(const fixed_string& other);
  dynamic_string(const dynamic_string& other);
  ~dynamic_string();

  void swap(dynamic_string& other);

  dynamic_string& operator=(const char* text);
  dynamic_string& operator=(const fixed_string& other);
  dynamic_string& operator=(const dynamic_string& other);

  dynamic_string& operator+=(const char* text);
  dynamic_string& operator+=(const fixed_string& other);
  dynamic_string& operator+=(const dynamic_string& other);

  const char* c_str() const;
  unsigned int length() const;
  bool empty() const { return (length() == 0); }

private:
  struct Impl;
  Impl* const itsImpl;
};

///////////////////////////////////////////////////////////////////////
//
// Input/Output functions
//
///////////////////////////////////////////////////////////////////////

class istream;
class ostream;

istream& operator>>(istream& is, fixed_string& str);
istream& operator>>(istream& is, dynamic_string& str);

ostream& operator<<(ostream& os, const string_literal& str);
ostream& operator<<(ostream& os, const fixed_string& str);
ostream& operator<<(ostream& os, const dynamic_string& str);

istream& getline(istream& is, fixed_string& str);
istream& getline(istream& is, dynamic_string& str);

istream& getline(istream& is, fixed_string& str, char eol);
istream& getline(istream& is, dynamic_string& str, char eol);

static const char vcid_strings_h[] = "$Header$";
#endif // !STRINGS_H_DEFINED
