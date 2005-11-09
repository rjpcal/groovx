/** @file pkgs/mtx/mtx.h 2-dimensional numeric arrays, with iteration,
    subarrays, slicing, etc. a la matlab */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Mar 12 12:23:11 2001
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_PKGS_MTX_MTX_H_UTC20050626084022_DEFINED
#define GROOVX_PKGS_MTX_MTX_H_UTC20050626084022_DEFINED

#include "datablock.h"

#include "arithfunctor.h"

#include <iosfwd>
#include <iterator>

namespace rutz
{
  class fstring;
}

//  #######################################################
//  =======================================================
/// range-checking infrastructure

namespace range_checking
{
  void geq(const void* x, const void* lim, const char* f, int ln);
  void lt(const void* x, const void* lim, const char* f, int ln);
  void leq(const void* x, const void* lim, const char* f, int ln);
  void in_half_open(const void* x, const void* llim, const void* ulim,
                    const char* f, int ln);
  void in_full_open(const void* x, const void* llim, const void* ulim,
                    const char* f, int ln);

  void geq(int x, int lim, const char* f, int ln);
  void lt(int x, int lim, const char* f, int ln);
  void leq(int x, int lim, const char* f, int ln);
  void in_half_open(int x, int llim, int ulim, const char* f, int ln);
  void in_full_open(int x, int llim, int ulim, const char* f, int ln);
}

#ifdef RANGE_CHECK
// Range check
#  define RC_geq(x,lim) range_checking::geq((x),(lim),__FILE__,__LINE__)
#  define RC_less(x,lim) range_checking::lt((x),(lim),__FILE__,__LINE__)
#  define RC_leq(x,lim) range_checking::leq((x),(lim),__FILE__,__LINE__)
#  define RC_in_half_open(x,llim,ulim) range_checking::in_half_open((x),(llim),(ulim),__FILE__,__LINE__)
#  define RC_in_full_open(x,llim,ulim) range_checking::in_full_open((x),(llim),(ulim),__FILE__,__LINE__)

// Range check, and return the checked value
#  define RCR_geq(x,lim) (range_checking::geq((x),(lim),__FILE__,__LINE__), x)
#  define RCR_less(x,lim) (range_checking::lt((x),(lim),__FILE__,__LINE__), x)
#  define RCR_leq(x,lim) (range_checking::leq((x),(lim),__FILE__,__LINE__), x)
#  define RCR_in_half_open(x,llim,ulim) (range_checking::in_half_open((x),(llim),(ulim),__FILE__,__LINE__), x)
#  define RCR_in_full_open(x,llim,ulim) (range_checking::in_full_open((x),(llim),(ulim),__FILE__,__LINE__), x)

#else // !RANGE_CHECK

#  define RC_geq(x,lim)
#  define RC_less(x,lim)
#  define RC_leq(x,lim)
#  define RC_in_half_open(x,llim,ulim)
#  define RC_in_full_open(x,llim,ulim)

#  define RCR_geq(x,lim) (x)
#  define RCR_less(x,lim) (x)
#  define RCR_leq(x,lim) (x)
#  define RCR_in_half_open(x,llim,ulim) (x)
#  define RCR_in_full_open(x,llim,ulim) (x)
#endif


//  #######################################################
//  #######################################################
//  =======================================================
//  classes for representing ranges of indices

/// index_range class represents a half-open range of indices

class index_range
{
private:
  const int m_first;
  const int m_count;

public:
  index_range(int first, int count) : m_first(first), m_count(count) {}

  int begin() const { return m_first; }
  int end() const { return m_first+m_count; }
  int count() const { return m_count; }
};

/// A range class for row ranges only.
class row_index_range : public index_range
{
public:
  /// Construct with first+count.
  row_index_range(int first, int count) : index_range(first, count) {}
};

/// A range class for column ranges only.
class col_index_range : public index_range
{
public:
  /// Construct with first+count.
  col_index_range(int first, int count) : index_range(first, count) {}
};

inline index_range range(int i) { return index_range(i, 1); }
inline index_range range(int begin, int end) { return index_range(begin, end-begin); }
inline index_range range_n(int begin, int count) { return index_range(begin, count); }

inline row_index_range row_range(int r) { return row_index_range(r, 1); }
inline row_index_range row_range(int begin, int end) { return row_index_range(begin, end-begin); }
inline row_index_range row_range_n(int begin, int count) { return row_index_range(begin, count); }

inline col_index_range col_range(int c) { return col_index_range(c, 1); }
inline col_index_range col_range(int begin, int end) { return col_index_range(begin, end-begin); }
inline col_index_range col_range_n(int begin, int count) { return col_index_range(begin, count); }




//  #######################################################
//  #######################################################
//  =======================================================
//  iterators


class mtx;

/// Generic stride-based array iterator.
template <class T>
class stride_iterator_base
{
private:
  T* data;
  int stride;
  T* stop;

protected:
  // these friend declarations are to allow access to the protected
  // "raw" constructor that starts from a raw pointer plus
  // stride+length info
  friend class slice;
  friend class mtx;
  template <class U> friend class stride_iterator_base;

  stride_iterator_base(T* d, int str, int n) :
    data(d), stride(str), stop(data+str*n) {}

public:
  typedef std::random_access_iterator_tag iterator_category;

  stride_iterator_base(const stride_iterator_base& other) :
    data(other.data), stride(other.stride), stop(other.stop) {}

  template <class U>
  explicit stride_iterator_base(const stride_iterator_base<U>& other) :
    data(other.data), stride(other.stride), stop(other.stop) {}

  typedef T            value_type;
  typedef ptrdiff_t    difference_type;
  typedef T*           pointer;
  typedef T&           reference;

  stride_iterator_base end() const { stride_iterator_base e(*this); e.data = stop; return e; }

  bool has_more() const { return data < stop; }

  // Dereference

  reference operator*() const { RC_less(data, stop); return *data; }

  // Comparison

  bool operator==(const stride_iterator_base& other) const { return data == other.data; }

  bool operator!=(const stride_iterator_base& other) const { return data != other.data; }

  bool operator<(const stride_iterator_base& other) const { return data < other.data; }

  difference_type operator-(const stride_iterator_base& other) const
    { return (data - other.data) / stride; }

  // Increment/Decrement

  stride_iterator_base& operator++() { data += stride; return *this; }
  stride_iterator_base& operator--() { data -= stride; return *this; }

  stride_iterator_base operator++(int) { stride_iterator_base cpy(*this); data += stride; return cpy; }
  stride_iterator_base operator--(int) { stride_iterator_base cpy(*this); data -= stride; return cpy; }

  stride_iterator_base& operator+=(int x) { data += x*stride; return *this; }
  stride_iterator_base& operator-=(int x) { data -= x*stride; return *this; }

  stride_iterator_base operator+(int x) const { stride_iterator_base res(*this); res += x; return res; }
  stride_iterator_base operator-(int x) const { stride_iterator_base res(*this); res -= x; return res; }
};

typedef stride_iterator_base<double> mtx_iter;
typedef stride_iterator_base<const double> mtx_const_iter;


//  #######################################################
//  =======================================================

/// Represents a one dimensional sub-array of a matrix.
class slice
{
private:
  data_holder&          m_data_source;
  ptrdiff_t    const    m_offset;
  int          const    m_stride;
  int          const    m_nelems;

  const double* data_start() const
    { return m_data_source.storage() + m_offset; }

  double* data_start_nc()
    { return m_data_source.storage_nc() + m_offset; }

  ptrdiff_t data_offset(int i) const { return m_stride*i; }
  const double* address(int i) const { return data_start() + data_offset(i); }

  ptrdiff_t storage_offset(int i) const { return m_offset + m_stride*i; }

  slice(const data_holder& src, ptrdiff_t offset, int s, int n)
    :
    // by doing this const_cast, slice is promising to uphold the const
    // correctness of the data_holder
    m_data_source(const_cast<data_holder&>(src)),
    m_offset(offset),
    m_stride(s),
    m_nelems(n)
  {}

  friend class mtx; // so that mtx can get at slice's constructor

public:

  // Default copy constructor OK

  // No "default" assignment operator, since that would be assignment
  // of reference; instead we have an operator=() that does assignment
  // of value; see below

  //
  // Data access
  //

  double operator[](int i) const
  {
    RC_in_half_open(i, 0, m_nelems);
    return *(address(i));
  }

  int nelems() const { return m_nelems; }

  slice operator()(const index_range& rng) const;

  /// Print the slice to the given stream
  void print(std::ostream& s) const;

  /// Print the slice to stdout
  void print_stdout() const;

  //
  // Iteration
  //

  mtx_iter begin_nc()
    { return mtx_iter(data_start_nc(), m_stride, m_nelems); }

  mtx_iter end_nc()
    { return begin_nc().end(); }

  mtx_const_iter begin() const
    { return mtx_const_iter(data_start(), m_stride, m_nelems); }

  mtx_const_iter end() const
    { return begin().end(); }

  //
  // Const functions
  //

  double sum() const;
  double min() const;
  double max() const;

  double mean() const { return sum()/m_nelems; }


  // Returns an index matrix so that this->reorder(mtx) will put the
  // slice in sorted order
  mtx get_sort_order() const;

  bool operator==(const slice& other) const;

  bool operator!=(const slice& other) const
  { return !(operator==(other)); }

  //
  // Non-const functions
  //

  template <class F>
  void apply(F func)
  {
    for (mtx_iter i = begin_nc(); i.has_more(); ++i)
      *i = func(*i);
  }

  void sort();

  // Reorders the slice by applying *this[i] = *this[index[i]]
  void reorder(const mtx& index);

  void operator+=(double val) { apply(dash::add(val)); }
  void operator-=(double val) { apply(dash::sub(val)); }

  void operator*=(double factor) { apply(dash::mul(factor)); }
  void operator/=(double div) { apply(dash::div(div)); }

  slice& operator+=(const slice& other);
  slice& operator-=(const slice& other);

  /// All entries in this slice get the specified value
  slice& operator=(double val);

  /// This is assignment of value, not reference
  slice& operator=(const slice& other);

  /// This is assignment of value, not reference
  slice& operator=(const mtx& other);
};



//  #######################################################
//  =======================================================
/// mtx_shape class holds number-of-rows/number-of-columns info.

class mtx_shape
{
public:
  mtx_shape(int mr, int nc) : m_mrows(mr), m_ncols(nc) {}

  int mrows() const { return m_mrows; }
  int ncols() const { return m_ncols; }

  int max_dim() const { return (m_mrows > m_ncols) ? m_mrows : m_ncols; }

  int nelems() const { return m_mrows*m_ncols; }

private:
  int m_mrows;
  int m_ncols;
};


//  #######################################################
//  =======================================================
/// mtx_specs class holds shape plus storage offset and rowstride info.

class mtx_specs
{
public:
  mtx_specs() : m_shape(0, 0), m_rowstride(0), m_offset(0) {}

  mtx_specs(const mtx_shape& shape) :
    m_shape(shape),
    m_rowstride(shape.mrows()),
    m_offset(0)
  {}

  mtx_specs(int mrows, int ncols) :
    m_shape(mrows, ncols),
    m_rowstride(mrows),
    m_offset(0)
  {}

  mtx_specs(const mtx_specs& other) :
    m_shape(other.m_shape),
    m_rowstride(other.m_rowstride),
    m_offset(other.m_offset)
  {}

  void swap(mtx_specs& other);

  mtx_specs as_shape(const mtx_shape& s) const;

  mtx_specs as_shape(int mr, int nc) const { return as_shape(mtx_shape(mr,nc)); }

  void select_rows(const row_index_range& rng);
  void select_cols(const col_index_range& rng);

  mtx_specs sub_rows(const row_index_range& rng) const
  {
    mtx_specs result(*this); result.select_rows(rng); return result;
  }

  mtx_specs sub_cols(const col_index_range& rng) const
  {
    mtx_specs result(*this); result.select_cols(rng); return result;
  }

  const mtx_shape& shape() const { return m_shape; }

  ptrdiff_t offset() const { return m_offset; }

  int max_dim() const { return m_shape.max_dim(); }
  int nelems() const { return mrows()*ncols(); }

  int mrows() const { return m_shape.mrows(); }
  int rowstride() const { return m_rowstride; }

  int ncols() const { return m_shape.ncols(); }
  int colstride() const { return m_colstride; }

  unsigned int rowgap() const { return m_rowstride - mrows(); }

  int offset_from_start(int row, int col) const
  {
    // Strictly speaking, the only valid offsets are those that pass
    // RC_in_half_open(), but in order to allow "one-past-the-end"
    // indexing for iterators etc., we use the more permissive
    // RC_in_full_open() instead:
    RC_in_full_open(row, 0, mrows());
    RC_in_full_open(col, 0, ncols());

    return row + (col*m_rowstride);
  }

  int offset_from_start(int elem) const
  { return offset_from_start(elem%mrows(), elem/mrows()); }

  int offset_from_storage(int row, int col) const
  { return m_offset + offset_from_start(row, col); }

  int offset_from_storage(int elem) const
  { return m_offset + offset_from_start(elem); }

private:
  mtx_shape m_shape;
  int m_rowstride;
  static const int m_colstride = 1;
  ptrdiff_t m_offset;
};


//  #######################################################
//  #######################################################
//  =======================================================
//  Iterator templates for iterating over two-dimensional data


//  #######################################################
//  =======================================================
/// Index-based iterator for mtx class.

template <class M, class T>
class index_iterator_base
{
  M* m_src;
  int m_index;

public:
  index_iterator_base(M* m, int e) : m_src(m), m_index(e) {}

  typedef std::random_access_iterator_tag iterator_category;

  typedef T            value_type;
  typedef ptrdiff_t    difference_type;
  typedef T*           pointer;
  typedef T&           reference;

  index_iterator_base end() const { return index_iterator_base(m_src, m_src->nelems()); }

  bool has_more() const { return m_index < m_src->nelems(); }

private:
  // Need this pair of overloads to distinguish between const and
  // non-const M types, so that we can call either at() or at_nc()
  // as appropriate.
  template <class MM>
  static reference get_at(MM* m, int e) { return m->at_nc(e); }

  template <class MM>
  static reference get_at(const MM* m, int e) { return m->at(e); }

public:
  reference operator*() const { return get_at(m_src, m_index); }

  // Comparison

  bool operator==(const index_iterator_base& other) const { return m_index == other.m_index; }

  bool operator!=(const index_iterator_base& other) const { return m_index != other.m_index; }

  bool operator<(const index_iterator_base& other) const { return m_index < other.m_index; }

  difference_type operator-(const index_iterator_base& other) const
  { return m_index - other.m_index; }

  // Increment/Decrement

  index_iterator_base& operator++() { ++m_index; return *this; }
  index_iterator_base& operator--() { --m_index; return *this; }

  index_iterator_base operator++(int) { return index_iterator_base(m_src, m_index++); }
  index_iterator_base operator--(int) { return index_iterator_base(m_src, m_index--); }

  index_iterator_base& operator+=(int x) { m_index += x; return *this; }
  index_iterator_base& operator-=(int x) { m_index -= x; return *this; }

  index_iterator_base operator+(int x) const { return index_iterator_base(m_src, m_index+x); }
  index_iterator_base operator-(int x) const { return index_iterator_base(m_src, m_index-x); }
};


//  #######################################################
//  =======================================================
/// Column-major iterator for mtx class.

template <class T>
class colmaj_iterator_base
{
  int m_rowgap;
  int m_rowstride;
  T* m_ptr;
  T* m_current_end;

public:

  typedef std::forward_iterator_tag iterator_category;

  typedef T            value_type;
  typedef ptrdiff_t    difference_type;
  typedef T*           pointer;
  typedef T&           reference;

  colmaj_iterator_base(int rg, int rs, T* ptr) :
    m_rowgap(rg),
    m_rowstride(rs),
    m_ptr(ptr),
    m_current_end(m_ptr+(m_rowstride-m_rowgap))
  {}

  T& operator*() const { return *m_ptr; }

  colmaj_iterator_base& operator++()
  {
    if (++m_ptr == m_current_end)
      {
        m_ptr += m_rowgap;
        m_current_end += m_rowstride;
      }
    return *this;
  }

  bool operator==(const colmaj_iterator_base& other) const
  { return m_ptr == other.m_ptr; }

  bool operator!=(const colmaj_iterator_base& other) const
  { return m_ptr != other.m_ptr; }
};


//  #######################################################
//  =======================================================
/// Row-major iterator for mtx class.

template <class T>
class rowmaj_iterator_base
{
  int m_stride;
  T* m_current_start;
  T* m_ptr;
  T* m_current_end;

public:

  typedef std::forward_iterator_tag iterator_category;

  typedef T            value_type;
  typedef ptrdiff_t    difference_type;
  typedef T*           pointer;
  typedef T&           reference;

  rowmaj_iterator_base(int s, int ncols, T* ptr) :
    m_stride(s),
    m_current_start(ptr),
    m_ptr(ptr),
    m_current_end(m_ptr+(ncols*s))
  {}

  T& operator*() const { return *m_ptr; }

  rowmaj_iterator_base& operator++()
  {
    m_ptr += m_stride;
    if (m_ptr == m_current_end)
      {
        ++m_current_start;
        ++m_current_end;
        m_ptr = m_current_start;
      }
    return *this;
  }

  bool operator==(const rowmaj_iterator_base& other) const
  { return m_ptr == other.m_ptr; }

  bool operator!=(const rowmaj_iterator_base& other) const
  { return m_ptr != other.m_ptr; }
};


//  #######################################################
//  =======================================================
/// mtx_base class has shared infrastructure for mtx and sub_mtx_ref

template <class Data>
class mtx_base : public mtx_specs, public mtx_policies
{
private:
  mtx_base& operator=(const mtx_base& other); // not allowed

protected:
  Data m_data;

  void swap(mtx_base& other);

  mtx_base(const mtx_base& other);

  mtx_base(int mrows, int ncols, const Data& data);

  mtx_base(const mtx_specs& specs, const Data& data);

  ~mtx_base();

  ptrdiff_t offset_from_storage(int r, int c) const
  { return RCR_leq(mtx_specs::offset_from_storage(r, c), m_data.storage_length()); }

  double* address_nc(int row, int col)
  { return m_data.storage_nc() + offset_from_storage(row, col); }

  const double* address(int row, int col) const
  { return m_data.storage() + offset_from_storage(row, col); }

  // Does the same thing as address_nc(), but doesn't range-check, since
  // this result is assumed to be some kind of "one-past-the-end" offset.
  ptrdiff_t end_offset_from_storage(int r, int c) const
  { return mtx_specs::offset_from_storage(r, c); }

  // Does the same thing as address_nc(), but doesn't range-check, since
  // this result is assumed to be some kind of "one-past-the-end" address.
  double* end_address_nc(int row, int col)
  { return m_data.storage_nc() + end_offset_from_storage(row, col); }

  // Does the same thing as address(), but doesn't range-check, since
  // this result is assumed to be some kind of "one-past-the-end" address.
  const double* end_address(int row, int col) const
  { return m_data.storage() + end_offset_from_storage(row, col); }

  const double* storage() const { return m_data.storage(); }
  double* storage_nc() { return m_data.storage_nc(); }

public:

  const mtx_shape& shape() const { return specs().shape(); }
  const mtx_specs& specs() const { return *this; }

  const double& at(int i) const
  {
    RC_less(i+offset(), m_data.storage_length());
    return m_data.storage()[i+offset()];
  }

  double& at_nc(int i)
  {
    RC_less(i+offset(), m_data.storage_length());
    return m_data.storage_nc()[i+offset()];
  }

  template <class F>
  void apply(F func)
  {
    double* p = m_data.storage_nc() + offset();
    unsigned int gap = rowgap();

    if (gap == 0)
      {
        double* end = p + nelems();
        for (; p < end; ++p)
          *p = func(*p);
      }
    else
      {
        for (int c = 0; c < ncols(); ++c)
          {
            for (int r = 0; r < mrows(); ++r)
              {
                *p = func(*p);
                ++p;
              }
            p += gap;
          }
      }
  }

  //
  // Iterators
  //

  // Index-based iteration

  typedef index_iterator_base<mtx_base, double> iterator;
  typedef index_iterator_base<const mtx_base, const double> const_iterator;

  iterator begin_nc() { return iterator(this, 0); }
  iterator end_nc() { return iterator(this, nelems()); }

  const_iterator begin() const { return const_iterator(this, 0); }
  const_iterator end() const { return const_iterator(this, nelems()); }

  // Column-major iteration

  typedef colmaj_iterator_base<double> colmaj_iter;
  typedef colmaj_iterator_base<const double> const_colmaj_iter;

  colmaj_iter colmaj_begin_nc()
  { return colmaj_iter(rowgap(), rowstride(), address_nc(0,0)); }

  colmaj_iter colmaj_end_nc()
  { return colmaj_iter(rowgap(), rowstride(), end_address_nc(0,ncols())); }

  const_colmaj_iter colmaj_begin() const
  { return const_colmaj_iter(rowgap(), rowstride(), address(0,0)); }

  const_colmaj_iter colmaj_end() const
  { return const_colmaj_iter(rowgap(), rowstride(), end_address(0,ncols())); }

  // Row-major iteration

  typedef rowmaj_iterator_base<double> rowmaj_iter;
  typedef rowmaj_iterator_base<const double> const_rowmaj_iter;

  rowmaj_iter rowmaj_begin_nc()
  { return rowmaj_iter(rowstride(), ncols(), address_nc(0,0)); }

  rowmaj_iter rowmaj_end_nc()
  { return rowmaj_iter(rowstride(), ncols(), address_nc(mrows(),0)); }

  const_rowmaj_iter rowmaj_begin() const
  { return const_rowmaj_iter(rowstride(), ncols(), address(0,0)); }

  const_rowmaj_iter rowmaj_end() const
  { return const_rowmaj_iter(rowstride(), ncols(), address(mrows(),0)); }


  //
  // Functions
  //

  void clear(double x = 0.0) { apply(dash::setter(x)); }
};


//  #######################################################
//  =======================================================
/// sub_mtx_ref class

class sub_mtx_ref : public mtx_base<data_ref_holder>
{
public:
  typedef mtx_base<data_ref_holder> Base;

  sub_mtx_ref(const mtx_specs& specs, data_holder& ref) :
    Base(specs, data_ref_holder(&ref))
  {}

  sub_mtx_ref& operator=(const sub_mtx_ref& other);
  sub_mtx_ref& operator=(const mtx& other);
};


//  #######################################################
//  =======================================================
/// mtx class

class mtx : public mtx_base<data_holder>
{
public:

  typedef mtx_base<data_holder> Base;

  //
  // Constructors + Conversion
  //

  static const mtx& empty_mtx();

  mtx(const mtx_specs& specs, const data_holder& data) :
    Base(specs, data)
  {}

  /// Set up a mtx with a storage policy of COPY.
  static mtx colmaj_copy_of(const double* data, int mrows, int ncols);

  /// Set up a mtx with a storage_policy of BORROW.
  static mtx colmaj_borrow_from(double* data, int mrows, int ncols);

  /// Set up a mtx with a storage_policy of REFER.
  static mtx colmaj_refer_to(double* data, int mrows, int ncols);

  static mtx zeros(const mtx_shape& s);

  static mtx uninitialized(const mtx_shape& s);

  static mtx zeros(int mrows, int ncols)
  { return zeros(mtx_shape(mrows, ncols)); }

  static mtx uninitialized(int mrows, int ncols)
  { return uninitialized(mtx_shape(mrows, ncols)); }

  static mtx from_stream(std::istream& s);

  static mtx from_string(const char* s);

  mtx(const slice& s);

  mtx(const mtx& other) : Base(other) {}

  virtual ~mtx();

  mtx& operator=(const mtx& other)
  {
    mtx temp(other);
    Base::swap(temp);
    return *this;
  }

  // This will destroy any data in the process of changing the size of
  // the mtx to the specified dimensions; its only advantage over just
  // declaring a new mtx is that it will avoid a deallocate/allocate
  // cycle if the new dimensions are the same as the current dimensions.
  void resize(int mrows_new, int ncols_new);

  /** Makes sure that the data are in contiguous storage; if called on a
      submatrix, contig() will make a new matrix of the proper size and copy
      the elements there; otherwise, it will just return the current
      matrix. */
  mtx contig() const;

  data_holder& get_data_holder() { return m_data; }


  //
  // I/O
  //

  /// Print the mtx to the given ostream, including the matrix name if non-empty
  void print(std::ostream& s, const char* mtx_name = "") const;

  /// Print the mtx to stdout
  void print_stdout() const;

  /// Print the mtx to stdout along with a name for the mtx
  void print_stdout_named(const char* mtx_name) const;

  /// Convert the mtx to a string, by printing it to a string stream.
  rutz::fstring as_string() const;

  /// Read the mtx from the given istream
  void scan(std::istream& s);

  /// Read the mtx from the given string
  void scan_string(const char* s);

  //
  // Iteration
  //


  //
  // Data access
  //

  double& at(int row, int col)
    { return Base::at_nc(offset_from_start(row, col)); }

  const double& at(int row, int col) const
    { return Base::at(offset_from_start(row, col)); }

  double& at(int elem)
    { return Base::at_nc(offset_from_start(elem)); }

  const double& at(int elem) const
    { return Base::at(offset_from_start(elem)); }

  bool same_size(const mtx& x) const
  { return (mrows() == x.mrows()) && (ncols() == x.ncols()); }

  //
  // Slices, submatrices
  //

  sub_mtx_ref sub(const row_index_range& rng)
  {
    return sub_mtx_ref(this->specs().sub_rows(rng), this->m_data);
  }

  sub_mtx_ref sub(const col_index_range& rng)
  {
    return sub_mtx_ref(this->specs().sub_cols(rng), this->m_data);
  }

  sub_mtx_ref sub(const row_index_range& rr, const col_index_range& cc)
  {
    return sub_mtx_ref(this->specs().sub_rows(rr).sub_cols(cc), this->m_data);
  }


  mtx sub(const row_index_range& rng) const
  {
    return mtx(this->specs().sub_rows(rng), this->m_data);
  }

  mtx sub(const col_index_range& rng) const
  {
    return mtx(this->specs().sub_cols(rng), this->m_data);
  }

  mtx sub(const row_index_range& rr, const col_index_range& cc) const
  {
    return mtx(this->specs().sub_rows(rr).sub_cols(cc), this->m_data);
  }


  mtx operator()(const row_index_range& rng) const { return sub(rng); }

  mtx operator()(const col_index_range& rng) const { return sub(rng); }

  mtx operator()(const row_index_range& rr, const col_index_range& cc) const
  { return sub(rr, cc); }


  mtx as_shape(const mtx_shape& s) const
  { return mtx(this->specs().as_shape(s), this->m_data); }

  mtx as_shape(int mr, int nc) const
  { return mtx(this->specs().as_shape(mr, nc), this->m_data); }

  slice row(int r) const
    { return slice(this->m_data, offset_from_storage(r,0), rowstride(), ncols()); }

  mtx_iter row_iter(int r)
    { return mtx_iter(address_nc(r,0), rowstride(), ncols()); }

  mtx_const_iter row_iter(int r) const
    { return mtx_const_iter(address(r,0), rowstride(), ncols()); }

  mtx as_row() const { return as_shape(1, nelems()); }

  void reorder_rows(const mtx& index);



  slice column(int c) const
    { return slice(this->m_data, offset_from_storage(0,c), colstride(), mrows()); }

  mtx_iter column_iter(int c)
    { return mtx_iter(address_nc(0,c), colstride(), mrows()); }

  mtx_const_iter column_iter(int c) const
    { return mtx_const_iter(address(0,c), colstride(), mrows()); }

  mtx as_column() const { return as_shape(nelems(), 1); }

  void reorder_columns(const mtx& index);

  void swap_columns(int c1, int c2);


  //
  // Functions
  //

  mtx mean_row() const;

  mtx mean_column() const;

  const_iterator find_min() const;
  const_iterator find_max() const;

  double min() const;
  double max() const;

  double sum() const;

  double mean() const { return sum() / nelems(); }

  mtx& operator+=(double x) { apply(dash::add(x)); return *this; }
  mtx& operator-=(double x) { apply(dash::sub(x)); return *this; }
  mtx& operator*=(double fac) { apply(dash::mul(fac)); return *this; }
  mtx& operator/=(double div) { apply(dash::div(div)); return *this; }

  mtx& operator+=(const mtx& other);
  mtx& operator-=(const mtx& other);

  bool operator==(const mtx& other) const;

  bool operator!=(const mtx& other) const
  { return !(operator==(other)); }

  // result = vec * mtx;
  static void VMmul_assign(const slice& vec, const mtx& mtx,
                           slice& result);

  // this = m1 * m2;
  void assign_MMmul(const mtx& m1, const mtx& m2);

  void make_unique() { Base::m_data.make_unique(); }

private:
  friend class slice;
};



//  #######################################################
//  #######################################################
//  #######################################################
//  =======================================================
//  Inline free functions


inline double inner_product(mtx_const_iter s1, mtx_const_iter s2)
{
  double result = 0.0;

  for (; s1.has_more(); ++s1, ++s2)
    result += (*s1) * (*s2);

  return result;
}

mtx operator+(const mtx& m, double x);
mtx operator-(const mtx& m, double x);
mtx operator*(const mtx& m, double x);
mtx operator/(const mtx& m, double x);

mtx operator+(const mtx& m1, const mtx& m2);
mtx operator-(const mtx& m1, const mtx& m2);

// Simple element-by-element multiplication (i.e. NOT matrix multiplication)
mtx arr_mul(const mtx& m1, const mtx& m2);

// Simple element-by-element division (i.e. NOT matrix division)
mtx arr_div(const mtx& m1, const mtx& m2);

mtx min(const mtx& m1, const mtx& m2);
mtx max(const mtx& m1, const mtx& m2);

static const char vcid_groovx_pkgs_mtx_mtx_h_utc20050626084022[] = "$Id$ $HeadURL$";
#endif // !GROOVX_PKGS_MTX_MTX_H_UTC20050626084022_DEFINED