///////////////////////////////////////////////////////////////////////
//
// strings.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar  6 11:16:48 2000
// written: Wed Aug  8 21:03:21 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGS_H_DEFINED
#define STRINGS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CSTRING_DEFINED)
#include <cstring>
#define CSTRING_DEFINED
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TOSTRING_H_DEFINED)
#include "util/tostring.h"
#define CSTRING_DEFINED
#endif

class string_literal;
class fstring;

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
  friend class fstring;

  string_literal(const char* literal);

  bool equals(const char* other) const;
  bool equals(const string_literal& other) const;

  const char* c_str() const { return itsText; }
  std::size_t length() const { return itsLength; }
  bool empty() const { return (length() == 0); }

private:
  string_literal(const string_literal&);
  string_literal& operator=(const string_literal&);

  const char* const itsText;
  const std::size_t itsLength;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c string_rep is a helper class for fstring that handles
 * memory management and reference-counting. \c string_rep should not
 * be used by public clients.
 *
 **/
///////////////////////////////////////////////////////////////////////

class string_rep {
private:
  string_rep(const string_rep& other); // not implemented
  string_rep& operator=(const string_rep& other); // not implemented

  // Class-specific operator new.
  void* operator new(std::size_t bytes);

  // Class-specific operator delete.
  void operator delete(void* space);

  friend class DummyFriend; // to eliminate compiler warning

  // Constructor builds a string_rep with ref-count 0. 'length' here
  // does NOT need to "+1" for a null-terminator
  string_rep(std::size_t length, const char* text);
  string_rep(std::size_t length);
  ~string_rep();

  static string_rep* getEmptyRep();

public:
  static string_rep* make(std::size_t length, const char* text);

  static string_rep* make(std::size_t length);

  string_rep* clone() const { return new string_rep(itsLength, itsText); }

  static void makeUnique(string_rep*& rep);

  void incrRefCount() { ++itsRefCount; }
  void decrRefCount() { if (--itsRefCount <= 0) delete this; }

  std::size_t length() const { return itsLength; }
  const char* text() const { return itsText; }
  char* data();

  void append(std::size_t length, const char* text);

  void realloc(std::size_t bufsize);

  void make_space(std::size_t len);

private:
  unsigned int itsRefCount;

  std::size_t itsAllocSize;
  std::size_t itsLength;
  char* itsText;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c fstring is a simple string class that holds a pointer to a
 * dynamically-allocated char array. The initializer does not have to
 * reside in permanent storage, since a copy is made when the \c
 * fstring is constructed. Assignment is allowed, with copy
 * semantics. Also, a \c swap() operation is provided.
 *
 **/
///////////////////////////////////////////////////////////////////////

class fstring {
public:
  friend class string_literal;

  fstring();
  fstring(const char* text);
  fstring(const fstring& other);
  ~fstring();

  template <class T1>
  fstring(const T1& part1) :
    itsRep(0)
  {
    init(); append(part1);
  }

  template <class T1, class T2>
  fstring(const T1& part1, const T2& part2) :
    itsRep(0)
  {
    init(); append(part1, part2);
  }

  template <class T1, class T2, class T3>
  fstring(const T1& part1, const T2& part2, const T3& part3) :
    itsRep(0)
  {
    init(); append(part1, part2, part3);
  }

  template <class T1, class T2, class T3, class T4>
  fstring(const T1& part1, const T2& part2, const T3& part3,
          const T4& part4) :
    itsRep(0)
  {
    init(); append(part1, part2, part3, part4);
  }

  void swap(fstring& other);

  fstring& operator=(const char* text);
  fstring& operator=(const fstring& other);

  bool equals(const char* other) const;
  bool equals(const string_literal& other) const;
  bool equals(const fstring& other) const;

  void make_space(std::size_t len)
  {
    string_rep::makeUnique(itsRep); itsRep->make_space(len);
  }

  char* data() { string_rep::makeUnique(itsRep); return itsRep->data(); }

  const char* c_str() const { return itsRep->text(); }
  std::size_t length() const { return itsRep->length(); }
  bool empty() const { return (length() == 0); }

  bool ends_with(const fstring& ext) const;

  //
  // Comparison operators
  //

  bool operator<(const char* other) const
    { return strcmp(itsRep->text(), other) < 0; }

  template <class StrType>
  bool operator<(const StrType& other) const
    {
      // Check if we are pointing to the same string
      if (c_str() == other.c_str()) return false;
      // ...otherwise do a string compare
      return strcmp(c_str(), other.c_str()) < 0;
    }

  bool operator>(const char* other) const
    { return strcmp(c_str(), other) < 0; }

  template <class StrType>
  bool operator>(const StrType& other) const
    {
      // Check if we are pointing to the same string
      if (c_str() == other.c_str()) return false;
      // ...otherwise do a string compare
      return strcmp(c_str(), other.c_str()) > 0;
    }

  //
  // Appending
  //

  /** Append additional text to the error message. */
  template <class T1>
  fstring& append(const T1& part1)
  {
    do_append(part1); return *this;
  }

  /** Append additional text to the error message. */
  template <class T1, class T2>
  fstring& append(const T1& part1, const T2& part2)
  {
    do_append(part1); do_append(part2); return *this;
  }

  /** Append additional text to the error message. */
  template <class T1, class T2, class T3>
  fstring& append(const T1& part1, const T2& part2, const T3& part3)
  {
    do_append(part1); do_append(part2); do_append(part3); return *this;
  }

  /** Append additional text to the error message. */
  template <class T1, class T2, class T3, class T4>
  fstring& append(const T1& part1, const T2& part2, const T3& part3,
                  const T4& part4)
  {
    do_append(part1); do_append(part2); do_append(part3);
    do_append(part4); return *this;
  }

  template <class T>
  fstring& operator+=(const T& x) { return append(x); }

private:
  void do_append(const char* text)
  { if (text) append_text(strlen(text), text); }

  void do_append(const fstring& other)
  { append_text(other.length(), other.c_str()); }

  void do_append(const string_literal& other)
  { append_text(other.length(), other.c_str()); }

  template <class T>
  void do_append(const T& x)
  {
    do_append(Util::Convert<T>::toString(x));
  }

  void append_text(std::size_t length, const char* text);

  void init();

  string_rep* itsRep;
};



namespace Util
{
  template <>
  struct Convert<string_literal>
  {
    static const char* toString(const string_literal& x) { return x.c_str(); }
  };

  template <>
  struct Convert<fstring>
  {
    static const char* toString(const fstring& x) { return x.c_str(); }
  };
}

///////////////////////////////////////////////////////////////////////
//
// Overloaded operator==
//
///////////////////////////////////////////////////////////////////////

// With const char* on left

inline bool operator==(const char* lhs, const string_literal& rhs)
  { return rhs.equals(lhs); }

inline bool operator==(const char* lhs, const fstring& rhs)
  { return rhs.equals(lhs); }

// With string_literal on left

inline bool operator==(const string_literal& lhs, const char* rhs)
  { return lhs.equals(rhs); }

inline bool operator==(const string_literal& lhs, const string_literal& rhs)
  { return lhs.equals(rhs); }

inline bool operator==(const string_literal& lhs, const fstring& rhs)
  { return rhs.equals(lhs); }

// With fstring on left

inline bool operator==(const fstring& lhs, const char* rhs)
  { return lhs.equals(rhs); }

inline bool operator==(const fstring& lhs, const string_literal& rhs)
  { return lhs.equals(rhs); }

inline bool operator==(const fstring& lhs, const fstring& rhs)
  { return lhs.equals(rhs); }

///////////////////////////////////////////////////////////////////////
//
// Input/Output functions
//
///////////////////////////////////////////////////////////////////////

#ifdef PRESTANDARD_IOSTREAMS
class istream;
class ostream;
#else
#  if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOSFWD_DEFINED)
#    include <iosfwd>
#    define IOSFWD_DEFINED
#  endif
#endif

STD_IO::istream& operator>>(STD_IO::istream& is, fstring& str);

STD_IO::ostream& operator<<(STD_IO::ostream& os, const string_literal& str);
STD_IO::ostream& operator<<(STD_IO::ostream& os, const fstring& str);

STD_IO::istream& getline(STD_IO::istream& is, fstring& str);

STD_IO::istream& getline(STD_IO::istream& is, fstring& str, char eol);

static const char vcid_strings_h[] = "$Header$";
#endif // !STRINGS_H_DEFINED
