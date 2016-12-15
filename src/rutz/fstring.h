/** @file rutz/fstring.h ref-counted string type that allows much
    faster compile times than std::string */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri Oct 15 15:40:18 2004
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_RUTZ_FSTRING_H_UTC20050626084021_DEFINED
#define GROOVX_RUTZ_FSTRING_H_UTC20050626084021_DEFINED

#include "rutz/debug.h"

#include <atomic>
#include <cstddef>
#include <cstring>
#include <iosfwd>

namespace rutz
{
  ///////////////////////////////////////////////////////////
  /**
   *
   * \c string_rep is a helper class for fstring that handles memory
   * management and reference-counting. \c string_rep should not be
   * used by public clients.
   *
   **/
  ///////////////////////////////////////////////////////////

  class string_rep
  {
  public:
    static string_rep* make(std::size_t length, const char* text,
                            std::size_t capacity=0);

    static string_rep* read_from_stream(std::istream& is);

    static string_rep* readsome_from_stream(std::istream& is, unsigned int count);

    static string_rep* readline_from_stream(std::istream& is, char eol = '\n');

    void incr_ref_count() noexcept { ++m_refcount; }

    int decr_ref_count() noexcept
    {
      const int c = --m_refcount;
      if (c <= 0)
        delete this;
      return c;
    }

    std::size_t length() const noexcept { return m_length; }
    std::size_t capacity() const noexcept { return m_capacity; }
    const char* text() const noexcept { return m_text; }

    void debug_dump() const noexcept;

  private:
    // Class-specific operator new.
    void* operator new(std::size_t bytes);

    // Class-specific operator delete.
    void operator delete(void* space);

    // Constructor builds a string_rep with ref-count 0. 'length' here
    // does NOT need to "+1" for a null-terminator
    string_rep(std::size_t length, const char* text, std::size_t capacity=0);

    ~string_rep() noexcept;

    // To be called once via std::call_once()
    static void initialize_empty_rep();

    // Member functions whose names are prefixed with "uniq_" require
    // as a precondition that the string_rep object be unshared. In
    // order to ensure thread safety, such mutating calls are private
    // and can be made only during the process of construction of a
    // string_rep object.

    void uniq_append_no_terminate(char c);
    void add_terminator() noexcept;
    void uniq_set_length(std::size_t length) noexcept;
    void uniq_append(std::size_t length, const char* text);
    void uniq_realloc(std::size_t capacity);

    string_rep(const string_rep& other); // not implemented
    string_rep& operator=(const string_rep& other); // not implemented

    std::atomic<int> m_refcount;

    std::size_t m_capacity;
    std::size_t m_length;
    char* m_text;
  };

  struct char_range
  {
    char_range(const char* t, size_t n) : text(t), len(n) {}

    // default copy, dtor, assignment OK

    const char*  const text;
    size_t       const len;
  };

  ///////////////////////////////////////////////////////////
  /**
   *
   * \c fstring is a simple string class that holds a pointer to a
   * dynamically-allocated char array. The initializer does not have
   * to reside in permanent storage, since a copy is made when the \c
   * fstring is constructed. Assignment is allowed, with copy
   * semantics. Also, a \c swap() operation is provided. The internal
   * implementation uses reference counting to allow for efficient
   * copies; however, to allow safe multi-threaded access to fstring,
   * fstring's interface is read-only, except for a few functions
   * (assignment operator, swap(), clear() read(), readline(),
   * readsome()) which safely replace the entire string.
   *
   **/
  ///////////////////////////////////////////////////////////

  class fstring
  {
  public:
    /// Construct an empty string.
    fstring();

    /// Copy constructor.
    fstring(const fstring& other) noexcept;

    /// Destructory.
    ~fstring() noexcept;

    /// Construct by copying from a C-style null-terminated char array.
    fstring(const char* s) :
      m_rep(nullptr)
    {
      init_range(char_range(s, s ? strlen(s) : 0));
    }

    /// Construct from a character range (pointer plus length).
    explicit fstring(char_range r) :
      m_rep(nullptr)
    {
      init_range(r);
    }

    /// Swap contents with another fstring object.
    void swap(fstring& other) noexcept;

    /// Assign from a C-style null-terminated char array.
    fstring& operator=(const char* text);

    /// Assignment operator.
    fstring& operator=(const fstring& other) noexcept;

    /// Get a pointer to the const underlying data array.
    const char* c_str() const noexcept { return m_rep->text(); }

    /// Get the number of characters in the string (NOT INCLUDING the null terminator).
    std::size_t length() const noexcept { return m_rep->length(); }

    /// Query whether the length of the string is 0.
    bool is_empty() const noexcept { return (length() == 0); }

    /// Same as is_empty(); for compatibility with std::string interface.
    bool empty() const noexcept { return is_empty(); }

    /// Return the character at position i.
    char operator[](unsigned int i) const { return m_rep->text()[i]; }

    /// Reset to an empty string.
    void clear();

    //
    // Substring operations
    //

    /// Query whether the terminal substring matches the given string.
    bool ends_with(const fstring& ext) const noexcept;

    //
    // Comparison operators
    //

    /// Query for equality with a C-style string.
    bool equals(const char* other) const noexcept;
    /// Query for equality with another fstring object.
    bool equals(const fstring& other) const noexcept;

    /// Query if string is lexicographically less-than another string.
    bool operator<(const char* other) const noexcept;

    /// Query if string is lexicographically less-than another string.
    template <class string_type>
    bool operator<(const string_type& other) const noexcept
    {
      return operator<(other.c_str());
    }

    /// Query if string is lexicographically greater-than another string.
    bool operator>(const char* other) const noexcept;

    /// Query if string is lexicographically greater-than another string.
    template <class string_type>
    bool operator>(const string_type& other) const noexcept
    {
      return operator>(other.c_str());
    }

    //
    // Input/Output
    //

    /// Set the string by reading consecutive non-whitespace characters.
    void read(std::istream& is);

    /// Set the string by reading exactly count characters.
    void readsome(std::istream& is, unsigned int count);

    /// Write the string's contents to the ostream.
    void write(std::ostream& os) const;

    /// Set the string by reading characters up until newline or EOF.
    void readline(std::istream& is, char eol = '\n');

    //
    // Operators
    //

    /// Equality operator.
    bool operator==(const char* rhs)    const noexcept { return equals(rhs); }
    /// Equality operator.
    bool operator==(const fstring& rhs) const noexcept { return equals(rhs); }

    /// Inequality operator.
    bool operator!=(const char* rhs)    const noexcept { return !equals(rhs); }
    /// Inequality operator.
    bool operator!=(const fstring& rhs) const noexcept { return !equals(rhs); }

    /// Dump contents for debugging.
    void debug_dump() const noexcept;

  private:
    void init_empty();

    void init_range(char_range r);

    /// Construct from an existing string_rep object
    /** The string_rep should have refcount==0 on entry. */
    fstring(string_rep* r);

    string_rep* m_rep;
  };


  fstring sconvert(char x);            ///< Convert char -> fstring
  fstring sconvert(const char* x);     ///< Convert c string -> fstring
  fstring sconvert(const fstring& x);  ///< Convert fstring -> fstring

  fstring sconvert(bool x);            ///< Convert bool -> fstring
  fstring sconvert(int x);             ///< Convert int -> fstring
  fstring sconvert(unsigned int x);    ///< Convert uint -> fstring
  fstring sconvert(long x);            ///< Convert long -> fstring
  fstring sconvert(unsigned long x);   ///< Convert ulong -> fstring
  fstring sconvert(double x);          ///< Convert double -> fstring


  ///////////////////////////////////////////////////////////
  //
  // Overloaded operators
  //
  ///////////////////////////////////////////////////////////

  // operator ==

  inline bool operator==(const char* lhs, const fstring& rhs) noexcept
  { return rhs.equals(lhs); }

  // operator !=

  inline bool operator!=(const char* lhs, const fstring& rhs) noexcept
  { return !rhs.equals(lhs); }

  ///////////////////////////////////////////////////////////
  //
  // Input/Output functions
  //
  ///////////////////////////////////////////////////////////

  inline std::istream& operator>>(std::istream& is, fstring& str)
  {
    str.read(is); return is;
  }

  inline std::ostream& operator<<(std::ostream& os, const fstring& str)
  {
    str.write(os); return os;
  }

  inline std::istream& getline(std::istream& is, fstring& str)
  {
    str.readline(is); return is;
  }

  inline std::istream& getline(std::istream& is, fstring& str, char eol)
  {
    str.readline(is, eol); return is;
  }

  ///////////////////////////////////////////////////////////
  //
  // Overload of rutz::debug::eval() (used in dbg_eval macros)
  //
  ///////////////////////////////////////////////////////////

  namespace debug
  {
    inline void eval (const char* what, int level, const char* where,
                      int line_no, bool nl, rutz::fstring expr) noexcept
    {
      eval(what, level, where, line_no, nl, expr.c_str());
    }
  }

} // end namespace rutz

#endif // !GROOVX_RUTZ_FSTRING_H_UTC20050626084021_DEFINED
