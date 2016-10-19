/** @file pkgs/mtx/mtx.cc 2-dimensional numeric arrays, with iteration,
    subarrays, slicing, etc. a la matlab */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Mar 12 12:39:12 2001
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

#ifndef GROOVX_PKGS_MTX_MTX_CC_UTC20050626084022_DEFINED
#define GROOVX_PKGS_MTX_MTX_CC_UTC20050626084022_DEFINED

#include "mtx.h"

#include "rutz/cstrstream.h"
#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <vector>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;

namespace
{
  inline void domemswap(double* buf1, double* buf2,
                        double* tempbuf1, size_t nelems) noexcept
  {
    memcpy(tempbuf1, buf1, nelems*sizeof(double));
    memcpy(buf1, buf2, nelems*sizeof(double));
    memcpy(buf2, tempbuf1, nelems*sizeof(double));
  }

  inline void memswap(double* buf1, double* buf2, size_t nelems)
  {
    const size_t BUFSIZE = 512;
    if (nelems <= BUFSIZE)
      {
        double swapbuffer[BUFSIZE];
        domemswap(buf1, buf2, swapbuffer, nelems);
      }
    else
      {
        double* tempbuf1 = new double[nelems];
        domemswap(buf1, buf2, tempbuf1, nelems);
        delete [] tempbuf1;
      }
  }
}

namespace range_checking
{
  [[noreturn]] void raise_exception(const fstring& msg, const char* f, int ln);
}

void range_checking::raise_exception(const fstring& msg,
                                     const char* f, int ln)
{
  dbg_print_nl(3, msg);
  const fstring errmsg =
    rutz::sfmt("range check failed in file '%s' at line #%d: %s",
               f, ln, msg.c_str());
  throw rutz::error(errmsg, SRC_POS);
}

void range_checking::geq(const void* x, const void* lim,
                         const char* f, int ln)
{
  if (x>=lim) ; // OK
  else raise_exception("geq: pointer range error", f, ln);
}

void range_checking::lt(const void* x, const void* lim,
                        const char* f, int ln)
{
  if (x<lim) ; // OK
  else raise_exception("less: pointer range error", f, ln);
}

void range_checking::leq(const void* x, const void* lim,
                         const char* f, int ln)
{
  if (x<=lim) ; // OK
  else raise_exception("leq: pointer range error", f, ln);
}

void range_checking::in_half_open(const void* x,
                                  const void* llim, const void* ulim,
                                  const char* f, int ln)
{
  if (x>=llim && x<ulim) ; // OK
  else raise_exception("in_half_open: pointer range error", f, ln);
}

void range_checking::in_full_open(const void* x,
                                  const void* llim, const void* ulim,
                                  const char* f, int ln)
{
  if (x>=llim && x<=ulim) ; // OK
  else raise_exception("in_full_open: pointer range error", f, ln);
}

void range_checking::geq(int x, int lim, const char* f, int ln)
{
  if (x>=lim) ; // OK
  else raise_exception(rutz::sfmt("geq: integer range error "
                                  "%d !>= %d", x, lim),
                       f, ln);
}

void range_checking::lt(int x, int lim, const char* f, int ln)
{
  if (x<lim) ; // OK
  else raise_exception(rutz::sfmt("less: integer range error "
                                  "%d !< %d", x, lim),
                       f, ln);
}

void range_checking::leq(int x, int lim, const char* f, int ln)
{
  if (x<=lim) ; // OK
  else raise_exception(rutz::sfmt("leq: integer range error "
                                  "%d !<= %d", x, lim),
                       f, ln);
}

void range_checking::in_half_open(int x,
                                  int llim, int ulim,
                                  const char* f, int ln)
{
  if (x>=llim && x<ulim) ; // OK
  else raise_exception(rutz::sfmt("in_half_open: integer range error "
                                  "%d !in [%d, %d[", x, llim, ulim),
                       f, ln);
}

void range_checking::in_full_open(int x,
                                  int llim, int ulim,
                                  const char* f, int ln)
{
  if (x>=llim && x<=ulim) ; // OK
  else raise_exception(rutz::sfmt("in_full_open: integer range error "
                                  "%d !in [%d, %d]", x, llim, ulim),
                       f, ln);
}


///////////////////////////////////////////////////////////////////////
//
// slice member definitions
//
///////////////////////////////////////////////////////////////////////

slice slice::operator()(const index_range& rng) const
{
GVX_TRACE("slice::operator");
  RC_in_half_open(rng.begin(), 0, m_nelems);
  RC_geq(rng.count(), 0);
  RC_leq(rng.end(), m_nelems);

  return slice(m_data_source, storage_offset(rng.begin()),
               m_stride, rng.count());
}

void slice::print(std::ostream& s) const
{
GVX_TRACE("slice::print");
  for (mtx_const_iter iter = begin(); iter.has_more(); ++iter)
    {
      s << std::setw(12) << std::setprecision(7) << double(*iter);
    }
  s << std::endl;
}

void slice::print_stdout() const
{
GVX_TRACE("slice::print_stdout");
  print(std::cout);
}

double slice::sum() const
{
GVX_TRACE("slice::sum");
  double s = 0.0;
  for (mtx_const_iter i = begin(); i.has_more(); ++i)
    s += *i;
  return s;
}

double slice::min() const
{
GVX_TRACE("slice::min");
  mtx_const_iter i = begin();
  double mn = *i;
  for (; i.has_more(); ++i)
    if (*i < mn) mn = *i;
  return mn;
}

double slice::max() const
{
GVX_TRACE("slice::max");
  mtx_const_iter i = begin();
  double m = *i;
  for (; i.has_more(); ++i)
    if (*i > m) m = *i;
  return m;
}

namespace
{
  struct val_index
  {
    double val;
    unsigned int index;

    val_index(double v=0.0) : val(v) {}

    bool operator<(const val_index& v2) const { return val < v2.val; }
  };
}

mtx slice::get_sort_order() const
{
GVX_TRACE("slice::get_sort_order");

  std::vector<val_index> buf(this->begin(), this->end());

  for (unsigned int i = 0; i < buf.size(); ++i)
    {
      buf[i].index = i;
    }

  std::sort(buf.begin(), buf.end());

  mtx index = mtx::uninitialized(1, this->nelems());

  for (int i = 0; i < nelems(); ++i)
    {
      GVX_ASSERT(buf[i].index < static_cast<unsigned int>(nelems()));
      index.at(0,i) = buf[i].index;
    }

  return index;
}

bool slice::operator==(const slice& other) const
{
GVX_TRACE("slice::operator==(const slice&)");
  if (m_nelems != other.m_nelems) return false;

  for (mtx_const_iter a = this->begin(), b = other.begin();
       a.has_more();
       ++a, ++b)
    if (*a != *b) return false;

  return true;
}

void slice::sort()
{
GVX_TRACE("slice::sort");
  std::sort(begin_nc(), end_nc());
}

void slice::reorder(const mtx& index_)
{
GVX_TRACE("slice::reorder");
  mtx index(index_.as_column());

  if (index.mrows() != nelems())
    throw rutz::error("dimension mismatch in slice::reorder", SRC_POS);

  mtx neworder = mtx::uninitialized(this->nelems(), 1);

  for (int i = 0; i < nelems(); ++i)
    neworder.at(i,0) = (*this)[int(index.at(i,0))];

  *this = neworder.column(0);
}

slice& slice::operator+=(const slice& other)
{
GVX_TRACE("slice::operator+=(const slice&)");
  if (m_nelems != other.nelems())
    throw rutz::error("dimension mismatch in slice::operator+=", SRC_POS);

  mtx_const_iter rhs = other.begin();

  for (mtx_iter lhs = begin_nc(); lhs.has_more(); ++lhs, ++rhs)
    *lhs += *rhs;

  return *this;
}

slice& slice::operator-=(const slice& other)
{
GVX_TRACE("slice::operator-=(const slice&)");
  if (m_nelems != other.nelems())
    throw rutz::error("dimension mismatch in slice::operator-=", SRC_POS);

  mtx_const_iter rhs = other.begin();

  for (mtx_iter lhs = begin_nc(); lhs.has_more(); ++lhs, ++rhs)
    *lhs -= *rhs;

  return *this;
}

slice& slice::operator=(double val)
{
GVX_TRACE("slice::operator=(double)");
  for (mtx_iter itr = begin_nc(); itr.has_more(); ++itr)
    *itr = val;

  return *this;
}

slice& slice::operator=(const slice& other)
{
GVX_TRACE("slice::operator=(const slice&)");
  if (m_nelems != other.nelems())
    throw rutz::error("dimension mismatch in slice::operator=", SRC_POS);

  mtx_const_iter rhs = other.begin();

  for (mtx_iter lhs = begin_nc(); lhs.has_more(); ++lhs, ++rhs)
    *lhs = *rhs;

  return *this;
}

slice& slice::operator=(const mtx& other)
{
GVX_TRACE("slice::operator=(const mtx&)");
  if (m_nelems != other.nelems())
    throw rutz::error("dimension mismatch in slice::operator=", SRC_POS);

  int i = 0;
  for (mtx_iter lhs = begin_nc(); lhs.has_more(); ++lhs, ++i)
    *lhs = other.at(i);

  return *this;
}

void mtx_specs::swap(mtx_specs& other)
{
  std::swap(m_shape, other.m_shape);
  std::swap(m_rowstride, other.m_rowstride);
  std::swap(m_offset, other.m_offset);
}

mtx_specs mtx_specs::as_shape(const mtx_shape& s) const
{
GVX_TRACE("mtx_specs::as_shape");
  if (s.nelems() != this->nelems())
    {
      const fstring msg =
        rutz::sfmt("as_shape(): dimension mismatch: "
                   "current nelems == %d; requested %dx%d",
                   nelems(), s.mrows(), s.ncols());
      throw rutz::error(msg, SRC_POS);
    }

  if (m_rowstride != mrows())
    throw rutz::error("as_shape(): cannot reshape a submatrix", SRC_POS);

  mtx_specs result = *this;
  result.m_shape = s;
  result.m_rowstride = s.mrows();

  return result;
}

void mtx_specs::select_rows(const row_index_range& rng)
{
GVX_TRACE("mtx_specs::select_rows");
  if (rng.begin() < 0)
    throw rutz::error("select_rows(): row index must be >= 0", SRC_POS);

  if (rng.count() <= 0)
    throw rutz::error("select_rows(): number of rows must be > 0", SRC_POS);

  if (rng.end() > mrows())
    throw rutz::error("select_rows(): upper row index out of range", SRC_POS);

  m_offset += rng.begin();
  m_shape = mtx_shape(rng.count(), ncols());
}

void mtx_specs::select_cols(const col_index_range& rng)
{
GVX_TRACE("mtx_specs::select_cols");
  if (rng.begin() < 0)
    throw rutz::error("select_cols(): column index must be >= 0", SRC_POS);

  if (rng.count() <= 0)
    throw rutz::error("select_cols(): number of columns must be > 0", SRC_POS);

  if (rng.end() > ncols())
    throw rutz::error("select_cols(): upper column index out of range", SRC_POS);

  m_offset += rng.begin()*m_rowstride;
  m_shape = mtx_shape(mrows(), rng.count());
}

///////////////////////////////////////////////////////////////////////
//
// mtx_base member definitions
//
///////////////////////////////////////////////////////////////////////

template <class Data>
void mtx_base<Data>::swap(mtx_base& other)
{
  mtx_specs::swap(other);
  m_data.swap(other.m_data);
}

template <class Data>
mtx_base<Data>::mtx_base(const mtx_base& other) :
  mtx_specs(other),
  m_data(other.m_data)
{}

template <class Data>
mtx_base<Data>::mtx_base(mtx_base&& other) :
  mtx_specs(other),
  m_data(other.m_data)
{}

template <class Data>
mtx_base<Data>::mtx_base(int mrows, int ncols, const Data& data) :
  mtx_specs(mrows, ncols),
  m_data(data)
{}

template <class Data>
mtx_base<Data>::mtx_base(const mtx_specs& specs, const Data& data) :
  mtx_specs(specs),
  m_data(data)
{}

template <class Data>
mtx_base<Data>::~mtx_base() {}

template class mtx_base<data_holder>;

template class mtx_base<data_ref_holder>;

///////////////////////////////////////////////////////////////////////
//
// sub_mtx_ref member definitions
//
///////////////////////////////////////////////////////////////////////

sub_mtx_ref& sub_mtx_ref::operator=(const sub_mtx_ref& other)
{
GVX_TRACE("sub_mtx_ref::operator=(const sub_mtx_ref&)");
  if (this->nelems() != other.nelems())
    throw rutz::error("sub_mtx_ref::operator=(): dimension mismatch",
                      SRC_POS);

  std::copy(other.colmaj_begin(), other.colmaj_end(),
            this->colmaj_begin_nc());

  return *this;
}

sub_mtx_ref& sub_mtx_ref::operator=(const mtx& other)
{
GVX_TRACE("sub_mtx_ref::operator=(const mtx&)");
  if (this->nelems() != other.nelems())
    throw rutz::error("sub_mtx_ref::operator=(): dimension mismatch",
                      SRC_POS);

  std::copy(other.colmaj_begin(), other.colmaj_end(),
            this->colmaj_begin_nc());

  return *this;
}

///////////////////////////////////////////////////////////////////////
//
// mtx member definitions
//
///////////////////////////////////////////////////////////////////////

mtx mtx::colmaj_copy_of(const double* data, int mrows, int ncols)
{
GVX_TRACE("mtx::colmaj_copy_of");

  return mtx(mtx_shape(mrows, ncols),
             data_holder(const_cast<double*>(data),
                         mrows, ncols, COPY));
}

mtx mtx::colmaj_borrow_from(double* data, int mrows, int ncols)
{
GVX_TRACE("mtx::colmaj_borrow_from");

  return mtx(mtx_shape(mrows, ncols),
             data_holder(data, mrows, ncols, BORROW));
}

mtx mtx::colmaj_refer_to(double* data, int mrows, int ncols)
{
GVX_TRACE("mtx::colmaj_refer_to");

  return mtx(mtx_shape(mrows, ncols),
             data_holder(data, mrows, ncols, REFER));
}

mtx mtx::zeros(const mtx_shape& s)
{
  return mtx(s, data_holder(s.mrows(), s.ncols(), ZEROS));
}

mtx mtx::uninitialized(const mtx_shape& s)
{
  return mtx(s, data_holder(s.mrows(), s.ncols(), NO_INIT));
}

mtx mtx::from_stream(std::istream& s)
{
GVX_TRACE("mtx::from_stream");

  fstring buf;
  int mrows = -1;
  int ncols = -1;

  s >> buf;
  if (buf != "mrows")
    throw rutz::error(rutz::sfmt("parse error while scanning mtx "
                                 "from stream: expected 'mrows', got '%s'",
                                 buf.c_str()),
                      SRC_POS);

  s >> mrows;
  if (mrows < 0)
    throw rutz::error("parse error while scanning mtx "
                      "from stream: expected mrows>=0", SRC_POS);

  s >> buf;
  if (buf != "ncols")
    throw rutz::error(rutz::sfmt("parse error while scanning mtx "
                                 "from stream: expected 'ncols', got '%s'",
                                 buf.c_str()),
                      SRC_POS);

  s >> ncols;
  if (ncols < 0)
    throw rutz::error("parse error while scanning mtx "
                      "from stream: expected ncols>=0", SRC_POS);

  mtx result = mtx::zeros(mrows, ncols);

  for (int r = 0; r < mrows; ++r)
    for (int c = 0; c < ncols; ++c)
      {
        if (s.eof())
          throw rutz::error("premature EOF while scanning mtx "
                            "from stream", SRC_POS);
        double d = 0.0;
        s >> d;
        result.at(r,c) = d;
      }

  if (s.fail())
    throw rutz::error("error while scanning mtx from stream", SRC_POS);

  return result;
}

mtx mtx::from_string(const char* s)
{
GVX_TRACE("mtx::from_string");

  rutz::imemstream ms(s);
  return mtx::from_stream(ms);
}


const mtx& mtx::empty_mtx()
{
GVX_TRACE("mtx::empty_mtx");
  static mtx* m = 0;
  if (m == 0)
    {
      m = new mtx(mtx::zeros(0,0));
    }
  return *m;
}

mtx::mtx(const slice& s) :
  Base(s.nelems(), 1, data_holder(s.nelems(), 1, NO_INIT))
{
GVX_TRACE("mtx::mtx");
  std::copy(s.begin(), s.end(), this->colmaj_begin_nc());
}

mtx::~mtx() {}

void mtx::resize(int mrows_new, int ncols_new)
{
GVX_TRACE("mtx::resize");
  if (mrows() == mrows_new && ncols() == ncols_new)
    return;
  else
    {
      mtx newsize = mtx::zeros(mrows_new, ncols_new);
      this->swap(newsize);
    }
}

mtx mtx::contig() const
{
GVX_TRACE("mtx::contig");
  if (mrows() == rowstride())
    return *this;

  mtx result = mtx::uninitialized(this->shape());

  std::copy(this->colmaj_begin(), this->colmaj_end(),
            result.colmaj_begin_nc());

  return result;
}

namespace
{
  void format_mtx(const mtx& m,
                  std::ostream& s,
                  const char* mtx_name,
                  bool trailing_newline)
  {
    if (mtx_name != 0 && mtx_name[0] != '\0')
      s << '[' << mtx_name << "] ";

    s << "mrows " << m.mrows() << " ncols " << m.ncols();
    for(int i = 0; i < m.mrows(); ++i)
      {
        s << '\n';
        for(int j = 0; j < m.ncols(); ++j)
          s << ' '
            << std::setw(18)
            << std::setprecision(17)
            << m.at(i,j);
      }

    if (trailing_newline)
      s << '\n';
  }
}

void mtx::print(std::ostream& s, const char* mtx_name) const
{
GVX_TRACE("mtx::print");
  format_mtx(*this, s, mtx_name, true);
}

void mtx::print_stdout() const
{
GVX_TRACE("mtx::print_stdout");
  format_mtx(*this, std::cout, 0, true);
}

void mtx::print_stdout_named(const char* mtx_name) const
{
GVX_TRACE("mtx::print_stdout_named");
  format_mtx(*this, std::cout, mtx_name, true);
}

rutz::fstring mtx::as_string() const
{
GVX_TRACE("mtx::as_string");
  std::ostringstream oss;

  format_mtx(*this, oss, 0, false);

  return rutz::fstring(oss.str().c_str());
}

void mtx::scan(std::istream& s)
{
GVX_TRACE("mtx::scan");

  *this = mtx::from_stream(s);
}

void mtx::scan_string(const char* s)
{
GVX_TRACE("mtx::scan_string");

  *this = mtx::from_string(s);
}

void mtx::reorder_rows(const mtx& index_)
{
GVX_TRACE("mtx::reorder_rows");

  mtx index(index_.as_column());

  if (index.mrows() != mrows())
    throw rutz::error("dimension mismatch in mtx::reorder_rows",
                      SRC_POS);

  mtx neworder = mtx::uninitialized(this->shape());

  for (int r = 0; r < mrows(); ++r)
    neworder.row(r) = row(int(index.at(r,0)));

  *this = neworder;
}

void mtx::reorder_columns(const mtx& index_)
{
GVX_TRACE("mtx::reorder_columns");

  mtx index(index_.as_column());

  if (index.mrows() != ncols())
    throw rutz::error("dimension mismatch in mtx::reorder_columns",
                      SRC_POS);

  mtx neworder = mtx::uninitialized(this->shape());

  for (int c = 0; c < ncols(); ++c)
    neworder.column(c) = column(int(index.at(c,0)));

  *this = neworder;
}

void mtx::swap_columns(int c1, int c2)
{
GVX_TRACE("mtx::swap_columns");

  if (c1 == c2) return;

  memswap(address_nc(0,c1), address_nc(0,c2), mrows());
}

mtx mtx::mean_row() const
{
GVX_TRACE("mtx::mean_row");

  mtx res = mtx::uninitialized(1, ncols());

  mtx_iter resiter = res.row(0).begin_nc();

  for (int c = 0; c < ncols(); ++c, ++resiter)
    *resiter = column(c).mean();

  return res;
}

mtx mtx::mean_column() const
{
GVX_TRACE("mtx::mean_column");

  mtx res = mtx::uninitialized(mrows(), 1);

  mtx_iter resiter = res.column(0).begin_nc();

  for (int r = 0; r < mrows(); ++r, ++resiter)
    *resiter = row(r).mean();

  return res;
}

mtx::const_iterator mtx::find_min() const
{
GVX_TRACE("mtx::find_min");

  if (nelems() == 0)
    throw rutz::error("find_min(): the matrix must be non-empty",
                      SRC_POS);

  return std::min_element(begin(), end());
}

mtx::const_iterator mtx::find_max() const
{
GVX_TRACE("mtx::find_max");

  if (nelems() == 0)
    throw rutz::error("find_max(): the matrix must be non-empty",
                      SRC_POS);

  return std::max_element(begin(), end());
}

double mtx::min() const
{
GVX_TRACE("mtx::min");

  if (nelems() == 0)
    throw rutz::error("min(): the matrix must be non-empty",
                      SRC_POS);

  return *(std::min_element(colmaj_begin(), colmaj_end()));
}

double mtx::max() const
{
GVX_TRACE("mtx::max");

  if (nelems() == 0)
    throw rutz::error("max(): the matrix must be non-empty",
                      SRC_POS);

  return *(std::max_element(colmaj_begin(), colmaj_end()));
}

double mtx::sum() const
{
GVX_TRACE("mtx::sum");
  return std::accumulate(begin(), end(), 0.0);
}

mtx& mtx::operator+=(const mtx& other)
{
GVX_TRACE("mtx::operator+=(const mtx&)");
  if (ncols() != other.ncols())
    throw rutz::error("dimension mismatch in mtx::operator+=",
                      SRC_POS);

  for (int i = 0; i < ncols(); ++i)
    column(i) += other.column(i);

  return *this;
}

mtx& mtx::operator-=(const mtx& other)
{
GVX_TRACE("mtx::operator-=(const mtx&)");
  if (ncols() != other.ncols())
    throw rutz::error("dimension mismatch in mtx::operator-=",
                      SRC_POS);

  for (int i = 0; i < ncols(); ++i)
    column(i) -= other.column(i);

  return *this;
}

bool mtx::operator==(const mtx& other) const
{
GVX_TRACE("mtx::operator==(const mtx&)");
  if ( (mrows() != other.mrows()) || (ncols() != other.ncols()) )
    return false;
  for (int c = 0; c < ncols(); ++c)
    if ( column(c) != other.column(c) ) return false;
  return true;
}

void mtx::VMmul_assign(const slice& vec, const mtx& mtx,
                       slice& result)
{
GVX_TRACE("mtx::VMmul_assign");

  // e.g mrows == vec.nelems == 3   ncols == 4
  //
  //               | e11  e12  e13  e14 |
  // [w1 w2 w3] *  | e21  e22  e23  e24 | =
  //               | e31  e32  e33  e34 |
  //
  //
  // [ w1*e11+w2*e21+w3*e31  w1*e12+w2*e22+w3*e32  ... ]

  if ( (vec.nelems() != mtx.mrows()) ||
       (result.nelems() != mtx.ncols()) )
    throw rutz::error("dimension mismatch in mtx::VMmul_assign",
                      SRC_POS);

  mtx_const_iter veciter = vec.begin();

  mtx_iter result_itr = result.begin_nc();

  for (int col = 0; col < mtx.ncols(); ++col, ++result_itr)
    *result_itr = inner_product(veciter, mtx.column_iter(col));
}

void mtx::assign_MMmul(const mtx& m1, const mtx& m2)
{
GVX_TRACE("mtx::assign_MMmul");
  if ( (m1.ncols() != m2.mrows()) ||
       (this->ncols() != m2.ncols()) )
    throw rutz::error("dimension mismatch in mtx::VMmul_assign",
                      SRC_POS);

  for (int n = 0; n < mrows(); ++n)
    {
      mtx_iter row_element = this->row_iter(n);

      mtx_const_iter veciter = m1.row_iter(n);

      for (int col = 0; col < m2.ncols(); ++col, ++row_element)
        *row_element = inner_product(veciter, m2.column_iter(col));
    }
}

namespace
{
  template <class Op>
  mtx unary_op(const mtx& src, Op op)
  {
    mtx result = mtx::uninitialized(src.shape());

    std::transform(src.colmaj_begin(), src.colmaj_end(),
                   result.colmaj_begin_nc(),
                   op);

    return result;
  }
}

mtx operator+(const mtx& m, double x)
{
  return unary_op(m, std::bind2nd(std::plus<double>(), x));
}

mtx operator-(const mtx& m, double x)
{
  return unary_op(m, std::bind2nd(std::minus<double>(), x));
}

mtx operator*(const mtx& m, double x)
{
  return unary_op(m, std::bind2nd(std::multiplies<double>(), x));
}

mtx operator/(const mtx& m, double x)
{
  return unary_op(m, std::bind2nd(std::divides<double>(), x));
}


namespace
{
  template <class Op>
  mtx binary_op(const mtx& m1, const mtx& m2, Op op)
  {
    if (! m1.same_size(m2) )
      throw rutz::error("dimension mismatch in binary_op(mtx, mtx)",
                        SRC_POS);

    mtx result = mtx::uninitialized(m1.shape());

    std::transform(m1.colmaj_begin(), m1.colmaj_end(),
                   m2.colmaj_begin(),
                   result.colmaj_begin_nc(),
                   op);

    return result;
  }
}

mtx operator+(const mtx& m1, const mtx& m2)
{
GVX_TRACE("operator+(mtx, mtx)");
  return binary_op(m1, m2, std::plus<double>());
}

mtx operator-(const mtx& m1, const mtx& m2)
{
GVX_TRACE("operator-(mtx, mtx)");
  return binary_op(m1, m2, std::minus<double>());
}

mtx arr_mul(const mtx& m1, const mtx& m2)
{
GVX_TRACE("arr_mul(mtx, mtx)");
  return binary_op(m1, m2, std::multiplies<double>());
}

mtx arr_div(const mtx& m1, const mtx& m2)
{
GVX_TRACE("arr_div(mtx, mtx)");
  return binary_op(m1, m2, std::divides<double>());
}

mtx min(const mtx& m1, const mtx& m2)
{
GVX_TRACE("min(mtx, mtx)");
  return binary_op(m1, m2, dash::min());
}

mtx max(const mtx& m1, const mtx& m2)
{
GVX_TRACE("max(mtx, mtx)");
  return binary_op(m1, m2, dash::max());
}

#endif // !GROOVX_PKGS_MTX_MTX_CC_UTC20050626084022_DEFINED
