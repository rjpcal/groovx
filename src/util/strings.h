///////////////////////////////////////////////////////////////////////
//
// strings.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar  6 11:16:48 2000
// written: Fri Feb  1 11:09:35 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGS_H_DEFINED
#define STRINGS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CSTDDEF_DEFINED)
#include <cstddef>
#define CSTDDEF_DEFINED
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TOSTRING_H_DEFINED)
#include "util/tostring.h"
#endif

#ifdef PRESTANDARD_IOSTREAMS
class istream;
class ostream;
#else
#  if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOSFWD_DEFINED)
#    include <iosfwd>
#    define IOSFWD_DEFINED
#  endif
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c string_rep is a helper class for fstring that handles
 * memory management and reference-counting. \c string_rep should not
 * be used by public clients.
 *
 **/
///////////////////////////////////////////////////////////////////////

class string_rep
{
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
  string_rep(std::size_t length, const char* text, std::size_t capacity=0);
  ~string_rep();

  static string_rep* getEmptyRep();

public:
  static string_rep* make(std::size_t length, const char* text,
                          std::size_t capacity=0);

  string_rep* clone() const;

  static void makeUnique(string_rep*& rep);

  void incrRefCount() { ++itsRefCount; }
  void decrRefCount() { if (--itsRefCount <= 0) delete this; }

  std::size_t length() const { return itsLength; }
  std::size_t capacity() const { return itsCapacity; }
  const char* text() const { return itsText; }
  char* data();

  void clear();

  void append_no_terminate(char c);

  void add_terminator();

  void set_length(std::size_t length);

  void append(std::size_t length, const char* text);

  void realloc(std::size_t capacity);

  void reserve(std::size_t capacity);

private:
  unsigned int itsRefCount;

  std::size_t itsCapacity;
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

class fstring
{
public:
  fstring();
  fstring(const fstring& other);
  ~fstring();

  fstring(const char* s) :
    itsRep(0)
  {
    init(Util::CharData(s));
  }

  template <class T1>
  explicit fstring(const T1& part1) :
    itsRep(0)
  {
    do_init(part1);
  }

  template <class T1, class T2>
  fstring(const T1& part1, const T2& part2) :
    itsRep(0)
  {
    do_init(part1); append(part2);
  }

  template <class T1, class T2, class T3>
  fstring(const T1& part1, const T2& part2, const T3& part3) :
    itsRep(0)
  {
    do_init(part1); append(part2, part3);
  }

  template <class T1, class T2, class T3, class T4>
  fstring(const T1& part1, const T2& part2, const T3& part3,
          const T4& part4) :
    itsRep(0)
  {
    do_init(part1); append(part2, part3, part4);
  }

  void swap(fstring& other);

  fstring& operator=(const char* text);
  fstring& operator=(const fstring& other);

  char* data()
  {
    string_rep::makeUnique(itsRep); return itsRep->data();
  }

  const char* c_str() const { return itsRep->text(); }
  std::size_t length() const { return itsRep->length(); }
  bool is_empty() const { return (length() == 0); }

  void clear() { string_rep::makeUnique(itsRep); itsRep->clear(); }

  //
  // Substring operations
  //

  bool ends_with(const fstring& ext) const;

  //
  // Comparison operators
  //

  bool equals(const char* other) const;
  bool equals(const fstring& other) const;

  bool operator<(const char* other) const;

  template <class StrType>
  bool operator<(const StrType& other) const
  {
    return operator<(other.c_str());
  }

  bool operator>(const char* other) const;

  template <class StrType>
  bool operator>(const StrType& other) const
  {
    return operator>(other.c_str());
  }

  //
  // Appending
  //

  /// Append additional text to the fstring.
  template <class T1>
  fstring& append(const T1& part1)
  {
    do_append(part1); return *this;
  }

  /// Append additional text to the fstring.
  template <class T1, class T2>
  fstring& append(const T1& part1, const T2& part2)
  {
    do_append(part1); do_append(part2); return *this;
  }

  /// Append additional text to the fstring.
  template <class T1, class T2, class T3>
  fstring& append(const T1& part1, const T2& part2, const T3& part3)
  {
    do_append(part1); do_append(part2); do_append(part3); return *this;
  }

  /// Append additional text to the fstring.
  template <class T1, class T2, class T3, class T4>
  fstring& append(const T1& part1, const T2& part2, const T3& part3,
                  const T4& part4)
  {
    do_append(part1); do_append(part2); do_append(part3);
    do_append(part4); return *this;
  }

  //
  // Input/Output
  //

  void read(STD_IO::istream& is);

  /// Read exactly count characters from the stream
  void readsome(STD_IO::istream& is, unsigned int count);

  void write(STD_IO::ostream& os) const;

  void readline(STD_IO::istream& is);

  void readline(STD_IO::istream& is, char eol);

  //
  // Operators -- note that these are member functions rather than free
  // functions in order to avoid problems with buggy compilers that are too
  // loose about implicitly converting anything to an fstring to match an ==
  // or != call.
  //

  bool operator==(const char* rhs) const    { return equals(rhs); }
  bool operator==(const fstring& rhs) const { return equals(rhs); }

  bool operator!=(const char* rhs) const    { return !equals(rhs); }
  bool operator!=(const fstring& rhs) const { return !equals(rhs); }

private:
  void do_append(const char& c)
  { append_text(1, &c); }

  void do_append(Util::CharData cdata)
  { append_text(cdata.len, cdata.text); }

  void do_append(const fstring& other)
  { append_text(other.length(), other.c_str()); }

  template <class T>
  void do_append(const T& x)
  {
    do_append(Util::Convert<T>::toString(x));
  }

  void append_text(std::size_t length, const char* text);

  void init(Util::CharData cdata);

  template <class T>
  void do_init(const T& x)
  {
    init(Util::Convert<T>::toString(x));
  }

  string_rep* itsRep;
};



namespace Util
{
  template <>
  struct Convert<fstring>
  {
    static CharData toString(const fstring& x)
    { return CharData(x.c_str(), x.length()); }
  };
}

///////////////////////////////////////////////////////////////////////
//
// Overloaded operators
//
///////////////////////////////////////////////////////////////////////

// operator ==

inline bool operator==(const char* lhs, const fstring& rhs)
  { return rhs.equals(lhs); }

// operator !=

inline bool operator!=(const char* lhs, const fstring& rhs)
  { return !rhs.equals(lhs); }

///////////////////////////////////////////////////////////////////////
//
// Input/Output functions
//
///////////////////////////////////////////////////////////////////////

inline STD_IO::istream& operator>>(STD_IO::istream& is, fstring& str)
{
  str.read(is); return is;
}

inline STD_IO::ostream& operator<<(STD_IO::ostream& os, const fstring& str)
{
  str.write(os); return os;
}

inline STD_IO::istream& getline(STD_IO::istream& is, fstring& str)
{
  str.readline(is); return is;
}

inline STD_IO::istream& getline(STD_IO::istream& is, fstring& str, char eol)
{
  str.readline(is, eol); return is;
}

static const char vcid_strings_h[] = "$Header$";
#endif // !STRINGS_H_DEFINED
