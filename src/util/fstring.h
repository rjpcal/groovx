///////////////////////////////////////////////////////////////////////
//
// fstring.h
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Oct 15 15:40:18 2004
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef FSTRING_H_DEFINED
#define FSTRING_H_DEFINED

#include <cstddef>

#ifdef HAVE_IOSFWD
#  include <iosfwd>
#else
class istream;
class ostream;
#endif

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
  private:
    string_rep(const string_rep& other); // not implemented
    string_rep& operator=(const string_rep& other); // not implemented

    // Class-specific operator new.
    void* operator new(std::size_t bytes);

    // Class-specific operator delete.
    void operator delete(void* space);

    // Constructor builds a string_rep with ref-count 0. 'length' here
    // does NOT need to "+1" for a null-terminator
    string_rep(std::size_t length, const char* text, std::size_t capacity=0);

    ~string_rep() throw();

    static string_rep* get_empty_rep();

  public:
    static string_rep* make(std::size_t length, const char* text,
                            std::size_t capacity=0);

    static void make_unique(string_rep*& rep);

    string_rep* clone() const;

    bool is_unique() const throw() { return m_refcount == 1; }

    void incr_ref_count() throw() { ++m_refcount; }

    std::size_t decr_ref_count() throw()
    {
      std::size_t c = --m_refcount;
      if (c <= 0)
        delete this;
      return c;
    }

    std::size_t length() const throw() { return m_length; }
    std::size_t capacity() const throw() { return m_capacity; }
    const char* text() const throw() { return m_text; }

    // Member functions whose names are prefixed with "uniq_" require as
    // a precondition that the string_rep object be unshared, so callers
    // are required to call string_rep::make_unique() on the string_rep
    // before calling such a member function. It's not possible for
    // string_rep to handle this itself, because the act of doing a
    // make_unique() requires actually substituting one string_rep
    // object for another, which is something that can only done by the
    // caller.

    char* uniq_data() throw();

    void uniq_clear() throw();

    void uniq_append_no_terminate(char c);

    void add_terminator() throw();

    void uniq_set_length(std::size_t length) throw();

    void uniq_append(std::size_t length, const char* text);

    void uniq_realloc(std::size_t capacity);

    void uniq_reserve(std::size_t capacity);

    void debug_dump() const throw();

  private:
    unsigned int m_refcount;

    std::size_t m_capacity;
    std::size_t m_length;
    char* m_text;
  };

  struct char_range
  {
    char_range(const char* t, unsigned int n) : text(t), len(n) {}

    // default copy, dtor, assignment OK

    const char*  const text;
    unsigned int const len;
  };

  ///////////////////////////////////////////////////////////
  /**
   *
   * \c fstring is a simple string class that holds a pointer to a
   * dynamically-allocated char array. The initializer does not have to
   * reside in permanent storage, since a copy is made when the \c
   * fstring is constructed. Assignment is allowed, with copy
   * semantics. Also, a \c swap() operation is provided.
   *
   **/
  ///////////////////////////////////////////////////////////

  class fstring
  {
  public:
    /// Construct an empty string.
    fstring();

    /// Copy constructor.
    fstring(const fstring& other) throw();

    /// Destructory.
    ~fstring() throw();

    /// Construct by copying from a C-style null-terminated char array.
    fstring(const char* s) :
      m_rep(0)
    {
      init_empty(); append(s);
    }

    /// Construct from a character range (pointer plus length).
    fstring(char_range r) :
      m_rep(0)
    {
      init_range(r);
    }

    /// Construct by converting args to strings and concatenating.
    template <class T1>
    explicit fstring(const T1& part1) :
      m_rep(0)
    {
      init_empty(); append(part1);
    }

    /// Construct by converting args to strings and concatenating.
    template <class T1, class T2>
    fstring(const T1& part1, const T2& part2) :
      m_rep(0)
    {
      init_empty(); append(part1); append(part2);
    }

    /// Construct by converting args to strings and concatenating.
    template <class T1, class T2, class T3>
    fstring(const T1& part1, const T2& part2, const T3& part3) :
      m_rep(0)
    {
      init_empty(); append(part1); append(part2, part3);
    }

    /// Construct by converting args to strings and concatenating.
    template <class T1, class T2, class T3, class T4>
    fstring(const T1& part1, const T2& part2, const T3& part3,
            const T4& part4) :
      m_rep(0)
    {
      init_empty(); append(part1); append(part2, part3, part4);
    }

    /// Construct by converting args to strings and concatenating.
    template <class T1, class T2, class T3, class T4, class T5>
    fstring(const T1& part1, const T2& part2, const T3& part3,
            const T4& part4, const T5& part5) :
      m_rep(0)
    {
      init_empty(); append(part1); append(part2, part3, part4, part5);
    }

    /// Construct by converting args to strings and concatenating.
    template <class T1, class T2, class T3, class T4, class T5, class T6>
    fstring(const T1& part1, const T2& part2, const T3& part3,
            const T4& part4, const T5& part5, const T6& part6) :
      m_rep(0)
    {
      init_empty(); append(part1); append(part2, part3, part4, part5, part6);
    }

    /// Construct by converting args to strings and concatenating.
    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    fstring(const T1& part1, const T2& part2, const T3& part3,
            const T4& part4, const T5& part5, const T6& part6,
            const T7& part7) :
      m_rep(0)
    {
      init_empty(); append(part1); append(part2, part3, part4, part5, part6, part7);
    }

    /// Swap contents with another fstring object.
    void swap(fstring& other) throw();

    /// Assign from a C-style null-terminated char array.
    fstring& operator=(const char* text);

    /// Assignment operator.
    fstring& operator=(const fstring& other) throw();

    /// Get a pointer to the non-const underlying data array.
    char* data()
    {
      string_rep::make_unique(m_rep); return m_rep->uniq_data();
    }

    /// Get a pointer to the const underlying data array.
    const char* c_str() const throw() { return m_rep->text(); }

    /// Get the number of characters in the string (NOT INCLUDING the null terminator).
    std::size_t length() const throw() { return m_rep->length(); }

    /// Query whether the length of the string is 0.
    bool is_empty() const throw() { return (length() == 0); }

    /// Same as is_empty(); for compatibility with std::string interface.
    bool empty() const throw() { return is_empty(); }

    /// Reset to an empty string.
    void clear();

    //
    // Substring operations
    //

    /// Query whether the terminal substring matches the given string.
    bool ends_with(const fstring& ext) const throw();

    //
    // Comparison operators
    //

    /// Query for equality with a C-style string.
    bool equals(const char* other) const throw();
    /// Query for equality with another fstring object.
    bool equals(const fstring& other) const throw();

    /// Query if string is lexicographically less-than another string.
    bool operator<(const char* other) const throw();

    /// Query if string is lexicographically less-than another string.
    template <class string_type>
    bool operator<(const string_type& other) const throw()
    {
      return operator<(other.c_str());
    }

    /// Query if string is lexicographically greater-than another string.
    bool operator>(const char* other) const throw();

    /// Query if string is lexicographically greater-than another string.
    template <class string_type>
    bool operator>(const string_type& other) const throw()
    {
      return operator>(other.c_str());
    }

    //
    // Appending
    //

    /// Append a range of characters.
    void append_range(const char* text, std::size_t length);

    /// Convert args to string and append to the fstring.
    template <class T1>
    fstring& append(const T1& part1)
    {
      do_append(part1); return *this;
    }

    /// Convert args to string and append to the fstring.
    template <class T1, class T2>
    fstring& append(const T1& part1, const T2& part2)
    {
      do_append(part1); do_append(part2); return *this;
    }

    /// Convert args to string and append to the fstring.
    template <class T1, class T2, class T3>
    fstring& append(const T1& part1, const T2& part2, const T3& part3)
    {
      do_append(part1); do_append(part2); do_append(part3); return *this;
    }

    /// Convert args to string and append to the fstring.
    template <class T1, class T2, class T3, class T4>
    fstring& append(const T1& part1, const T2& part2, const T3& part3,
                    const T4& part4)
    {
      do_append(part1); do_append(part2); do_append(part3);
      do_append(part4); return *this;
    }

    /// Convert args to string and append to the fstring.
    template <class T1, class T2, class T3, class T4, class T5>
    fstring& append(const T1& part1, const T2& part2, const T3& part3,
                    const T4& part4, const T5& part5)
    {
      do_append(part1); do_append(part2); do_append(part3);
      do_append(part4); do_append(part5); return *this;
    }

    /// Convert args to string and append to the fstring.
    template <class T1, class T2, class T3, class T4, class T5, class T6>
    fstring& append(const T1& part1, const T2& part2, const T3& part3,
                    const T4& part4, const T5& part5, const T6& part6)
    {
      do_append(part1); do_append(part2); do_append(part3);
      do_append(part4); do_append(part5); do_append(part6); return *this;
    }

    //
    // Input/Output
    //

    /// Set the string by reading consecutive non-whitespace characters.
    void read(STD_IO::istream& is);

    /// Set the string by reading exactly count characters.
    void readsome(STD_IO::istream& is, unsigned int count);

    /// Write the string's contents to the ostream.
    void write(STD_IO::ostream& os) const;

    /// Set the string by reading characters up until newline or EOF.
    void readline(STD_IO::istream& is);

    /// Set the string by reading characters up until \a eol or EOF.
    void readline(STD_IO::istream& is, char eol);

    //
    // Operators -- note that these are member functions rather than free
    // functions in order to avoid problems with buggy compilers that are too
    // loose about implicitly converting anything to an fstring to match an ==
    // or != call.
    //

    /// Equality operator.
    bool operator==(const char* rhs)    const throw() { return equals(rhs); }
    /// Equality operator.
    bool operator==(const fstring& rhs) const throw() { return equals(rhs); }

    /// Inequality operator.
    bool operator!=(const char* rhs)    const throw() { return !equals(rhs); }
    /// Inequality operator.
    bool operator!=(const fstring& rhs) const throw() { return !equals(rhs); }

    /// Dump contents for debugging.
    void debug_dump() const throw();

  private:
    void do_append(char c);
    void do_append(const char* s);
    void do_append(const fstring& s);

    void do_append(bool x);
    void do_append(int x);
    void do_append(unsigned int x);
    void do_append(long x);
    void do_append(unsigned long x);
    void do_append(double x);

    void init_empty();

    void init_range(char_range r);

    string_rep* m_rep;
  };



  ///////////////////////////////////////////////////////////
  //
  // Overloaded operators
  //
  ///////////////////////////////////////////////////////////

  // operator ==

  inline bool operator==(const char* lhs, const fstring& rhs) throw()
  { return rhs.equals(lhs); }

  // operator !=

  inline bool operator!=(const char* lhs, const fstring& rhs) throw()
  { return !rhs.equals(lhs); }

  ///////////////////////////////////////////////////////////
  //
  // Input/Output functions
  //
  ///////////////////////////////////////////////////////////

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

} // end namespace rutz

static const char vcid_fstring_h[] = "$Id$ $URL$";
#endif // !FSTRING_H_DEFINED
