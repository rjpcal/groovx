///////////////////////////////////////////////////////////////////////
//
// mtx.cc
//
// Copyright (c) 2001-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Mar 12 12:39:12 2001
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef MTX_CC_DEFINED
#define MTX_CC_DEFINED

#include "mtx.h"

#include "util/error.h"
#include "util/strings.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#ifdef WITH_MATLAB
#include <matrix.h>
#endif
#include <numeric>
#include <vector>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace
{
  inline void domemswap(double* buf1, double* buf2,
                        double* tempbuf1, size_t nelems) throw()
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
  void raise_exception(const fstring& msg, const char* f, int ln);
}

void range_checking::raise_exception(const fstring& msg,
                                     const char* f, int ln)
{
  dbgPrintNL(3, msg);
  fstring errmsg;
  errmsg.append("range check failed in file '", f, "' at line #");
  errmsg.append(ln, ": ", msg);
  throw Util::Error(errmsg);
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
  else raise_exception(fstring("geq: integer range error ",
                               x, " !>= ", lim),
                       f, ln);
}

void range_checking::lt(int x, int lim, const char* f, int ln)
{
  if (x<lim) ; // OK
  else raise_exception(fstring("less: integer range error ",
                               x, " !< ", lim),
                       f, ln);
}

void range_checking::leq(int x, int lim, const char* f, int ln)
{
  if (x<=lim) ; // OK
  else raise_exception(fstring("leq: integer range error ",
                               x, " !<= ", lim),
                       f, ln);
}

void range_checking::in_half_open(int x,
                                  int llim, int ulim,
                                  const char* f, int ln)
{
  if (x>=llim && x<ulim) ; // OK
  else raise_exception(fstring("in_half_open: integer range error ",
                               x, " !in [", llim, ", ", ulim, ")"),
                       f, ln);
}

void range_checking::in_full_open(int x,
                                  int llim, int ulim,
                                  const char* f, int ln)
{
  if (x>=llim && x<=ulim) ; // OK
  else raise_exception(fstring("in_full_open: integer range error ",
                               x, " !in [", llim, ", ", ulim, "]"),
                       f, ln);
}


///////////////////////////////////////////////////////////////////////
//
// slice member definitions
//
///////////////////////////////////////////////////////////////////////

slice slice::operator()(const index_range& rng) const
{
DOTRACE("slice::operator");
  RC_in_half_open(rng.begin(), 0, m_nelems);
  RC_geq(rng.count(), 0);
  RC_leq(rng.end(), m_nelems);

  return slice(m_data_source, storage_offset(rng.begin()),
               m_stride, rng.count());
}

void slice::print() const
{
DOTRACE("slice::print");
  for(mtx_const_iter iter = begin(); iter.has_more(); ++iter)
    {
      std::cout << std::setw(12) << std::setprecision(7) << double(*iter);
    }
  std::cout << std::endl;
}

double slice::sum() const
{
DOTRACE("slice::sum");
  double s = 0.0;
  for (mtx_const_iter i = begin(); i.has_more(); ++i)
    s += *i;
  return s;
}

double slice::min() const
{
DOTRACE("slice::min");
  mtx_const_iter i = begin();
  double mn = *i;
  for (; i.has_more(); ++i)
    if (*i < mn) mn = *i;
  return mn;
}

double slice::max() const
{
DOTRACE("slice::max");
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
DOTRACE("slice::get_sort_order");

  std::vector<val_index> buf(this->begin(), this->end());

  for (unsigned int i = 0; i < buf.size(); ++i)
    {
      buf[i].index = i;
    }

  std::sort(buf.begin(), buf.end());

  mtx index(1,nelems());

  for (int i = 0; i < nelems(); ++i)
    {
      Assert(buf[i].index < static_cast<unsigned int>(nelems()));
      index.at(0,i) = buf[i].index;
    }

  return index;
}

bool slice::operator==(const slice& other) const
{
DOTRACE("slice::operator==(const slice&)");
  if (m_nelems != other.m_nelems) return false;

  for (mtx_const_iter a = this->begin(), b = other.begin();
       a.has_more();
       ++a, ++b)
    if (*a != *b) return false;

  return true;
}

void slice::sort()
{
DOTRACE("slice::sort");
  std::sort(begin_nc(), end_nc());
}

void slice::reorder(const mtx& index_)
{
DOTRACE("slice::reorder");
  mtx index(index_.as_column());

  if (index.mrows() != nelems())
    throw Util::Error("dimension mismatch in slice::reorder");

  mtx neworder(nelems(), 1);

  for (int i = 0; i < nelems(); ++i)
    neworder.at(i,0) = (*this)[int(index.at(i,0))];

  *this = neworder.column(0);
}

slice& slice::operator+=(const slice& other)
{
DOTRACE("slice::operator+=(const slice&)");
  if (m_nelems != other.nelems())
    throw Util::Error("dimension mismatch in slice::operator+=");

  mtx_const_iter rhs = other.begin();

  for (mtx_iter lhs = begin_nc(); lhs.has_more(); ++lhs, ++rhs)
    *lhs += *rhs;

  return *this;
}

slice& slice::operator-=(const slice& other)
{
DOTRACE("slice::operator-=(const slice&)");
  if (m_nelems != other.nelems())
    throw Util::Error("dimension mismatch in slice::operator-=");

  mtx_const_iter rhs = other.begin();

  for (mtx_iter lhs = begin_nc(); lhs.has_more(); ++lhs, ++rhs)
    *lhs -= *rhs;

  return *this;
}

slice& slice::operator=(const slice& other)
{
DOTRACE("slice::operator=(const slice&)");
  if (m_nelems != other.nelems())
    throw Util::Error("dimension mismatch in slice::operator=");

  mtx_const_iter rhs = other.begin();

  for (mtx_iter lhs = begin_nc(); lhs.has_more(); ++lhs, ++rhs)
    *lhs = *rhs;

  return *this;
}

slice& slice::operator=(const mtx& other)
{
DOTRACE("slice::operator=(const mtx&)");
  if (m_nelems != other.nelems())
    throw Util::Error("dimension mismatch in slice::operator=");

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
DOTRACE("mtx_specs::as_shape");
  if (s.nelems() != this->nelems())
    {
      fstring msg;
      msg.append("as_shape(): dimension mismatch: ");
      msg.append("current nelems == ", nelems(), "; requested ");
      msg.append(s.mrows(), "x", s.ncols());
      throw Util::Error(msg);
    }

  if (m_rowstride != mrows())
    throw Util::Error("as_shape(): cannot reshape a submatrix");

  mtx_specs result = *this;
  result.m_shape = s;
  result.m_rowstride = s.mrows();

  return result;
}

void mtx_specs::select_rows(const row_index_range& rng)
{
DOTRACE("mtx_specs::select_rows");
  if (rng.begin() < 0)
    throw Util::Error("select_rows(): row index must be >= 0");

  if (rng.count() <= 0)
    throw Util::Error("select_rows(): number of rows must be > 0");

  if (rng.end() > mrows())
    throw Util::Error("select_rows(): upper row index out of range");

  m_offset += rng.begin();
  m_shape = mtx_shape(rng.count(), ncols());
}

void mtx_specs::select_cols(const col_index_range& rng)
{
DOTRACE("mtx_specs::select_cols");
  if (rng.begin() < 0)
    throw Util::Error("select_cols(): column index must be >= 0");

  if (rng.count() <= 0)
    throw Util::Error("select_cols(): number of columns must be > 0");

  if (rng.end() > ncols())
    throw Util::Error("select_cols(): upper column index out of range");

  m_offset += rng.begin()*m_rowstride;
  m_shape = mtx_shape(mrows(), rng.count());
}

///////////////////////////////////////////////////////////////////////
//
// data_holder member functions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  data_block* new_data_block(int mrows, int ncols,
                             mtx_policies::init_policy p)
  {
    if (p == mtx_policies::ZEROS)
      return data_block::make_zeros(mrows*ncols);
    // else...
    return data_block::make_uninitialized(mrows*ncols);
  }

  data_block* new_data_block(double* data,
                             int mrows, int ncols,
                             mtx_policies::storage_policy s)
  {
    switch (s)
      {
      case mtx_policies::BORROW:
        return data_block::make_borrowed(data, mrows*ncols);

      case mtx_policies::REFER:
        return data_block::make_referred(data, mrows*ncols);

      case mtx_policies::COPY:
      default:
        break;
      }

    return data_block::make_data_copy(data, mrows*ncols);
  }

#ifdef WITH_MATLAB
  data_block* new_data_block(mxArray* a,
                             mtx_policies::storage_policy s)
  {
    if (!mxIsDouble(a))
      throw Util::Error("cannot construct a mtx with a non-'double' mxArray");

    return new_data_block(mxGetPr(a), mxGetM(a), mxGetN(a), s);
  }

  data_block* new_data_block(const mxArray* a,
                             mtx_policies::storage_policy s)
  {
    if (!mxIsDouble(a))
      throw Util::Error("cannot construct a mtx with a non-'double' mxArray");

    if (s != mtx_policies::BORROW && s != mtx_policies::COPY)
      throw Util::Error("cannot construct a mtx from a const mxArray* "
                        "unless the storage_policy is COPY or BORROW");

    return new_data_block(mxGetPr(a), mxGetM(a), mxGetN(a), s);
  }
#endif
}

data_holder::data_holder(double* data, int mrows, int ncols, storage_policy s) :
  m_data(new_data_block(data, mrows, ncols, s))
{
  m_data->incr_refcount();
}

data_holder::data_holder(int mrows, int ncols, init_policy p) :
  m_data(new_data_block(mrows, ncols, p))
{
  m_data->incr_refcount();
}

#ifdef WITH_MATLAB
data_holder::data_holder(mxArray* a, storage_policy s) :
  m_data(new_data_block(a, s))
{
  m_data->incr_refcount();
}

data_holder::data_holder(const mxArray* a, storage_policy s) :
  m_data(new_data_block(a, s))
{
  m_data->incr_refcount();
}
#endif

data_holder::data_holder(const data_holder& other) :
  m_data(other.m_data)
{
  m_data->incr_refcount();
}

data_holder::~data_holder()
{
  m_data->decr_refcount();
}

void data_holder::swap(data_holder& other)
{
  std::swap(m_data, other.m_data);
}

///////////////////////////////////////////////////////////////////////
//
// data_ref_holder definitions
//
///////////////////////////////////////////////////////////////////////

void data_ref_holder::swap(data_ref_holder& other)
{
  std::swap(ref_, other.ref_);
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
DOTRACE("sub_mtx_ref::operator=(const sub_mtx_ref&)");
  if (this->nelems() != other.nelems())
    throw Util::Error("sub_mtx_ref::operator=(): dimension mismatch");

  std::copy(other.colmaj_begin(), other.colmaj_end(),
            this->colmaj_begin_nc());

  return *this;
}

sub_mtx_ref& sub_mtx_ref::operator=(const mtx& other)
{
DOTRACE("sub_mtx_ref::operator=(const mtx&)");
  if (this->nelems() != other.nelems())
    throw Util::Error("sub_mtx_ref::operator=(): dimension mismatch");

  std::copy(other.colmaj_begin(), other.colmaj_end(),
            this->colmaj_begin_nc());

  return *this;
}

///////////////////////////////////////////////////////////////////////
//
// mtx member definitions
//
///////////////////////////////////////////////////////////////////////

#ifdef WITH_MATLAB
mtx::mtx(mxArray* a, storage_policy s) :
  Base(mtx_specs(mxGetM(a), mxGetN(a)), data_holder(a, s))
{
DOTRACE("mtx::mtx(mxArray*, storage_policy)");
}

mtx::mtx(const mxArray* a, storage_policy s) :
  Base(mtx_specs(mxGetM(a), mxGetN(a)), data_holder(a, s))
{
DOTRACE("mtx::mtx(const mxArray*, storage_policy)");
}
#endif

mtx::mtx(double* data, int mrows, int ncols, storage_policy s) :
  Base(mrows, ncols, data_holder(data, mrows, ncols, s))
{
DOTRACE("mtx::mtx(double*, int, int, storage_policy)");
}

mtx::mtx(int mrows, int ncols, init_policy p) :
  Base(mrows, ncols, data_holder(mrows, ncols, p))
{
DOTRACE("mtx::mtx(int, int, init_policy)");
}

mtx::mtx(const mtx_shape& s, init_policy p) :
  Base(s.mrows(), s.ncols(), data_holder(s.mrows(), s.ncols(), p))
{
DOTRACE("mtx::mtx(const mtx_shape&, init_policy)");
}

const mtx& mtx::empty_mtx()
{
DOTRACE("mtx::empty_mtx");
  static mtx* m = 0;
  if (m == 0)
    {
      m = new mtx(0,0);
    }
  return *m;
}

mtx::mtx(const slice& s) :
  Base(s.nelems(), 1, data_holder(s.nelems(), 1, NO_INIT))
{
DOTRACE("mtx::mtx");
  std::copy(s.begin(), s.end(), this->colmaj_begin_nc());
}

mtx::~mtx() {}

#ifdef WITH_MATLAB
mxArray* mtx::make_mxarray() const
{
DOTRACE("mtx::make_mxarray");
  mxArray* result_mx = mxCreateDoubleMatrix(mrows(), ncols(), mxREAL);

  std::copy(colmaj_begin(), colmaj_end(), mxGetPr(result_mx));

  return result_mx;
}
#endif

void mtx::resize(int mrows_new, int ncols_new)
{
DOTRACE("mtx::resize");
  if (mrows() == mrows_new && ncols() == ncols_new)
    return;
  else
    {
      mtx newsize(mrows_new, ncols_new, ZEROS);
      this->swap(newsize);
    }
}

mtx mtx::contig() const
{
DOTRACE("mtx::contig");
  if (mrows() == rowstride())
    return *this;

  mtx result(this->mrows(), this->ncols(), NO_INIT);

  std::copy(this->colmaj_begin(), this->colmaj_end(),
            result.colmaj_begin_nc());

  return result;
}

void mtx::print() const
{
  std::cout << "mrows = " << mrows() << ", ncols = " << ncols() << '\n';
  for(int i = 0; i < mrows(); ++i)
    {
      for(int j = 0; j < ncols(); ++j)
        std::cout << std::setw(12) << std::setprecision(7) << at(i,j);
      std::cout << '\n';
    }
  std::cout << std::endl;
}

void mtx::print(const char* mtx_name) const
{
  std::cout << mtx_name << '\n';
  print();
}

void mtx::reorder_rows(const mtx& index_)
{
DOTRACE("mtx::reorder_rows");

  mtx index(index_.as_column());

  if (index.mrows() != mrows())
    throw Util::Error("dimension mismatch in mtx::reorder_rows");

  mtx neworder(mrows(), ncols());

  for (int r = 0; r < mrows(); ++r)
    neworder.row(r) = row(int(index.at(r,0)));

  *this = neworder;
}

void mtx::reorder_columns(const mtx& index_)
{
DOTRACE("mtx::reorder_columns");

  mtx index(index_.as_column());

  if (index.mrows() != ncols())
    throw Util::Error("dimension mismatch in mtx::reorder_columns");

  mtx neworder(mrows(), ncols());

  for (int c = 0; c < ncols(); ++c)
    neworder.column(c) = column(int(index.at(c,0)));

  *this = neworder;
}

void mtx::swap_columns(int c1, int c2)
{
DOTRACE("mtx::swap_columns");

  if (c1 == c2) return;

  memswap(address_nc(0,c1), address_nc(0,c2), mrows());
}

mtx mtx::mean_row() const
{
DOTRACE("mtx::mean_row");

  mtx res(1, ncols());

  mtx_iter resiter = res.row(0).begin_nc();

  for (int c = 0; c < ncols(); ++c, ++resiter)
    *resiter = column(c).mean();

  return res;
}

mtx mtx::mean_column() const
{
DOTRACE("mtx::mean_column");

  mtx res(mrows(), 1);

  mtx_iter resiter = res.column(0).begin_nc();

  for (int r = 0; r < mrows(); ++r, ++resiter)
    *resiter = row(r).mean();

  return res;
}

mtx::const_iterator mtx::find_min() const
{
DOTRACE("mtx::find_min");

  if (nelems() == 0)
    throw Util::Error("find_min(): the matrix must be non-empty");

  return std::min_element(begin(), end());
}

mtx::const_iterator mtx::find_max() const
{
DOTRACE("mtx::find_max");

  if (nelems() == 0)
    throw Util::Error("find_max(): the matrix must be non-empty");

  return std::max_element(begin(), end());
}

double mtx::min() const
{
DOTRACE("mtx::min");

  if (nelems() == 0)
    throw Util::Error("min(): the matrix must be non-empty");

  return *(std::min_element(colmaj_begin(), colmaj_end()));
}

double mtx::max() const
{
DOTRACE("mtx::max");

  if (nelems() == 0)
    throw Util::Error("max(): the matrix must be non-empty");

  return *(std::max_element(colmaj_begin(), colmaj_end()));
}

double mtx::sum() const
{
DOTRACE("mtx::sum");
  return std::accumulate(begin(), end(), 0.0);
}

mtx& mtx::operator+=(const mtx& other)
{
DOTRACE("mtx::operator+=(const mtx&)");
  if (ncols() != other.ncols())
    throw Util::Error("dimension mismatch in mtx::operator+=");

  for (int i = 0; i < ncols(); ++i)
    column(i) += other.column(i);

  return *this;
}

mtx& mtx::operator-=(const mtx& other)
{
DOTRACE("mtx::operator-=(const mtx&)");
  if (ncols() != other.ncols())
    throw Util::Error("dimension mismatch in mtx::operator-=");

  for (int i = 0; i < ncols(); ++i)
    column(i) -= other.column(i);

  return *this;
}

bool mtx::operator==(const mtx& other) const
{
DOTRACE("mtx::operator==(const mtx&)");
  if ( (mrows() != other.mrows()) || (ncols() != other.ncols()) )
    return false;
  for (int c = 0; c < ncols(); ++c)
    if ( column(c) != other.column(c) ) return false;
  return true;
}

void mtx::VMmul_assign(const slice& vec, const mtx& mtx,
                       slice& result)
{
DOTRACE("mtx::VMmul_assign");

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
    throw Util::Error("dimension mismatch in mtx::VMmul_assign");

  mtx_const_iter veciter = vec.begin();

  mtx_iter result_itr = result.begin_nc();

  for (int col = 0; col < mtx.ncols(); ++col, ++result_itr)
    *result_itr = inner_product(veciter, mtx.column_iter(col));
}

void mtx::assign_MMmul(const mtx& m1, const mtx& m2)
{
DOTRACE("mtx::assign_MMmul");
  if ( (m1.ncols() != m2.mrows()) ||
       (this->ncols() != m2.ncols()) )
    throw Util::Error("dimension mismatch in mtx::VMmul_assign");

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
    mtx result(src.mrows(), src.ncols(), mtx::NO_INIT);

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
      throw Util::Error("dimension mismatch in binary_op(mtx, mtx)");

    mtx result(m1.mrows(), m1.ncols(), mtx::NO_INIT);

    std::transform(m1.colmaj_begin(), m1.colmaj_end(),
                   m2.colmaj_begin(),
                   result.colmaj_begin_nc(),
                   op);

    return result;
  }
}

mtx operator+(const mtx& m1, const mtx& m2)
{
DOTRACE("operator+(mtx, mtx)");
  return binary_op(m1, m2, std::plus<double>());
}

mtx operator-(const mtx& m1, const mtx& m2)
{
DOTRACE("operator-(mtx, mtx)");
  return binary_op(m1, m2, std::minus<double>());
}

mtx arr_mul(const mtx& m1, const mtx& m2)
{
DOTRACE("arr_mul(mtx, mtx)");
  return binary_op(m1, m2, std::multiplies<double>());
}

mtx arr_div(const mtx& m1, const mtx& m2)
{
DOTRACE("arr_div(mtx, mtx)");
  return binary_op(m1, m2, std::divides<double>());
}

mtx min(const mtx& m1, const mtx& m2)
{
DOTRACE("min(mtx, mtx)");
  return binary_op(m1, m2, Min());
}

mtx max(const mtx& m1, const mtx& m2)
{
DOTRACE("max(mtx, mtx)");
  return binary_op(m1, m2, Max());
}

static const char vcid_mtx_cc[] = "$Header$";
#endif // !MTX_CC_DEFINED
