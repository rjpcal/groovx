///////////////////////////////////////////////////////////////////////
//
// strings.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Mar  6 11:16:48 2000
// written: Thu Nov  2 17:15:54 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGS_H_DEFINED
#define STRINGS_H_DEFINED

#include <cstring>

class string_literal;
class fixed_string;
class dynamic_string;

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
  friend class fixed_string;
  friend class dynamic_string;

  string_literal(const char* literal);

  bool equals(const char* other) const;
  bool equals(const string_literal& other) const;

  const char* c_str() const { return itsText; }
  unsigned int length() const { return itsLength; }
  bool empty() const { return (length() == 0); }

private:
  string_literal(const string_literal&);
  string_literal& operator=(const string_literal&);

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
  friend class string_literal;
  friend class dynamic_string;

  fixed_string(const char* text = "");
  fixed_string(const fixed_string& other);
  ~fixed_string();

  void swap(fixed_string& other);

  fixed_string& operator=(const char* text);
  fixed_string& operator=(const fixed_string& other);

  bool equals(const char* other) const;
  bool equals(const string_literal& other) const;
  bool equals(const fixed_string& other) const;

  char* data() { itsRep = itsRep->makeUnique(); return itsRep->itsText; }

  const char* c_str() const { return itsRep->itsText; }
  unsigned int length() const { return itsRep->itsLength; }
  bool empty() const { return (length() == 0); }

  //
  // Comparison operators
  //

  bool operator<(const char* other) const
	 { return strcmp(itsRep->itsText, other) < 0; }

  template <class StrType>
  bool operator<(const StrType& other) const
	 {
		// Check if we are pointing to the same string
		if (itsRep->itsText == other.c_str()) return false;
		// ...otherwise do a string compare
		return strcmp(itsRep->itsText, other.c_str()) < 0;
	 }

  bool operator>(const char* other) const
	 { return strcmp(itsRep->itsText, other) < 0; }

  template <class StrType>
  bool operator>(const StrType& other) const
	 {
		// Check if we are pointing to the same string
		if (itsRep->itsText == other.c_str()) return false;
		// ...otherwise do a string compare
		return strcmp(itsRep->itsText, other.c_str()) > 0;
	 }

private:
  class Rep {
  private:
	 Rep(const Rep& other); // not implemented
	 Rep& operator=(const Rep& other); // not implemented

	 Rep(const char* text);
	 ~Rep();

	 int itsRefCount;

  public:
	 static Rep* make(const char* text) { return new Rep(text); }

	 void incrRefCount() { ++itsRefCount; }
	 void decrRefCount() { if (--itsRefCount <= 0) delete this; }

	 Rep* makeUnique()
		{
		  if (itsRefCount <= 1) return this;

		  // It is safe to do this without worring about 'delete-ing
		  // this', since the previous statement tells us that
		  // itsRefCount is at least 2
		  decrRefCount();

		  return make(itsText);
		}

	 char* itsText;
	 unsigned int itsLength;
  };

  Rep* itsRep;
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
  friend class string_literal;
  friend class fixed_string;

  dynamic_string(const char* text = "");
  dynamic_string(const fixed_string& other);
  dynamic_string(const dynamic_string& other);
  ~dynamic_string();

  void swap(dynamic_string& other);

  dynamic_string& operator=(const char* text);
  dynamic_string& operator=(const fixed_string& other);
  dynamic_string& operator=(const dynamic_string& other);

  dynamic_string& append(const char* text);
  dynamic_string& append(const fixed_string& other);
  dynamic_string& append(const dynamic_string& other);

  dynamic_string& operator+=(const char* text)
	 { return append(text); }
  dynamic_string& operator+=(const fixed_string& other)
	 { return append(other); }
  dynamic_string& operator+=(const dynamic_string& other)
	 { return append(other); }

  bool equals(const char* other) const;
  bool equals(const string_literal& other) const;
  bool equals(const fixed_string& other) const;
  bool equals(const dynamic_string& other) const;

  const char* c_str() const;
  unsigned int length() const;
  bool empty() const { return (length() == 0); }

  //
  // Comparison operators
  //

  bool operator<(const char* other) const
	 { return strcmp(this->c_str(), other) < 0; }

  template <class StrType>
  bool operator<(const StrType& other) const
	 { return strcmp(this->c_str(), other.c_str()) < 0; }

  bool operator>(const char* other) const
	 { return strcmp(this->c_str(), other) > 0; }

  template <class StrType>
  bool operator>(const StrType& other) const
	 { return strcmp(this->c_str(), other.c_str()) > 0; }

protected:
  /** This template is instantiated only for std::string, to allow
      subclasses to get a handle to the internal representation with
      out having to explicitly name std::string in this file. */
  template <class StringType>
  StringType& rep(StringType* /*dummy*/ = 0);

private:
  struct Impl;
  Impl* const itsImpl;
};

///////////////////////////////////////////////////////////////////////
//
// Overloaded operator==
//
///////////////////////////////////////////////////////////////////////

// With const char* on left

inline bool operator==(const char* lhs, const string_literal& rhs)
  { return rhs.equals(lhs); }

inline bool operator==(const char* lhs, const fixed_string& rhs)
  { return rhs.equals(lhs); }

inline bool operator==(const char* lhs, const dynamic_string& rhs)
  { return rhs.equals(lhs); }

// With string_literal on left

inline bool operator==(const string_literal& lhs, const char* rhs)
  { return lhs.equals(rhs); }

inline bool operator==(const string_literal& lhs, const string_literal& rhs)
  { return lhs.equals(rhs); }

inline bool operator==(const string_literal& lhs, const fixed_string& rhs)
  { return rhs.equals(lhs); }

inline bool operator==(const string_literal& lhs, const dynamic_string& rhs)
  { return rhs.equals(lhs); }

// With fixed_string on left

inline bool operator==(const fixed_string& lhs, const char* rhs)
  { return lhs.equals(rhs); }

inline bool operator==(const fixed_string& lhs, const string_literal& rhs)
  { return lhs.equals(rhs); }

inline bool operator==(const fixed_string& lhs, const fixed_string& rhs)
  { return lhs.equals(rhs); }

inline bool operator==(const fixed_string& lhs, const dynamic_string& rhs)
  { return rhs.equals(lhs); }

// With dynamic_string on left

inline bool operator==(const dynamic_string& lhs, const char* rhs)
  { return lhs.equals(rhs); }

inline bool operator==(const dynamic_string& lhs, const string_literal& rhs)
  { return lhs.equals(rhs); }

inline bool operator==(const dynamic_string& lhs, const fixed_string& rhs)
  { return lhs.equals(rhs); }

inline bool operator==(const dynamic_string& lhs, const dynamic_string& rhs)
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

STD_IO::istream& operator>>(STD_IO::istream& is, fixed_string& str);
STD_IO::istream& operator>>(STD_IO::istream& is, dynamic_string& str);

STD_IO::ostream& operator<<(STD_IO::ostream& os, const string_literal& str);
STD_IO::ostream& operator<<(STD_IO::ostream& os, const fixed_string& str);
STD_IO::ostream& operator<<(STD_IO::ostream& os, const dynamic_string& str);

STD_IO::istream& getline(STD_IO::istream& is, fixed_string& str);
STD_IO::istream& getline(STD_IO::istream& is, dynamic_string& str);

STD_IO::istream& getline(STD_IO::istream& is, fixed_string& str, char eol);
STD_IO::istream& getline(STD_IO::istream& is, dynamic_string& str, char eol);

static const char vcid_strings_h[] = "$Header$";
#endif // !STRINGS_H_DEFINED
