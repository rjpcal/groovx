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
  void raiseException(const fstring& msg, const char* f, int ln);
}

void range_checking::raiseException(const fstring& msg,
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
  else raiseException("geq: pointer range error", f, ln);
}

void range_checking::lt(const void* x, const void* lim,
                        const char* f, int ln)
{
  if (x<lim) ; // OK
  else raiseException("less: pointer range error", f, ln);
}

void range_checking::leq(const void* x, const void* lim,
                         const char* f, int ln)
{
  if (x<=lim) ; // OK
  else raiseException("leq: pointer range error", f, ln);
}

void range_checking::in_half_open(const void* x,
                                  const void* llim, const void* ulim,
                                  const char* f, int ln)
{
  if (x>=llim && x<ulim) ; // OK
  else raiseException("in_half_open: pointer range error", f, ln);
}

void range_checking::in_full_open(const void* x,
                                  const void* llim, const void* ulim,
                                  const char* f, int ln)
{
  if (x>=llim && x<=ulim) ; // OK
  else raiseException("in_full_open: pointer range error", f, ln);
}

void range_checking::geq(int x, int lim, const char* f, int ln)
{
  if (x>=lim) ; // OK
  else raiseException(fstring("geq: integer range error ",
                              x, " !>= ", lim),
                      f, ln);
}

void range_checking::lt(int x, int lim, const char* f, int ln)
{
  if (x<lim) ; // OK
  else raiseException(fstring("less: integer range error ",
                              x, " !< ", lim),
                      f, ln);
}

void range_checking::leq(int x, int lim, const char* f, int ln)
{
  if (x<=lim) ; // OK
  else raiseException(fstring("leq: integer range error ",
                              x, " !<= ", lim),
                      f, ln);
}

void range_checking::in_half_open(int x,
                                  int llim, int ulim,
                                  const char* f, int ln)
{
  if (x>=llim && x<ulim) ; // OK
  else raiseException(fstring("in_half_open: integer range error ",
                              x, " !in [", llim, ", ", ulim, ")"),
                      f, ln);
}

void range_checking::in_full_open(int x,
                                  int llim, int ulim,
                                  const char* f, int ln)
{
  if (x>=llim && x<=ulim) ; // OK
  else raiseException(fstring("in_full_open: integer range error ",
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

  return slice(m_owner, storage_offset(rng.begin()), m_stride, rng.count());
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
  double mx = *i;
  for (; i.has_more(); ++i)
    if (*i > mx) mx = *i;
  return mx;
}

namespace
{
  struct ValIndex
  {
    double val;
    unsigned int index;

    ValIndex(double v=0.0) : val(v) {}

    bool operator<(const ValIndex& v2) const { return val < v2.val; }
  };
}

Mtx slice::get_sort_order() const
{
DOTRACE("slice::get_sort_order");

  std::vector<ValIndex> buf(this->begin(), this->end());

  for (unsigned int i = 0; i < buf.size(); ++i)
    {
      buf[i].index = i;
    }

  std::sort(buf.begin(), buf.end());

  Mtx index(1,nelems());

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

void slice::reorder(const Mtx& index_)
{
DOTRACE("slice::reorder");
  Mtx index(index_.asColumn());

  if (index.mrows() != nelems())
    throw Util::Error("dimension mismatch in slice::reorder");

  Mtx neworder(nelems(), 1);

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

slice& slice::operator=(const Mtx& other)
{
DOTRACE("slice::operator=(const Mtx&)");
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
// DataHolder member functions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  data_block* newDataBlock(int mrows, int ncols, mtx_policies::init_policy p)
  {
    if (p == mtx_policies::ZEROS)
      return data_block::make_zeros(mrows*ncols);
    // else...
    return data_block::make_uninitialized(mrows*ncols);
  }

  data_block* newDataBlock(double* data,
                           int mrows, int ncols, mtx_policies::storage_policy s)
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
  data_block* newDataBlock(mxArray* a, mtx_policies::storage_policy s)
  {
    if (!mxIsDouble(a))
      throw Util::Error("cannot construct a Mtx with a non-'double' mxArray");

    return newDataBlock(mxGetPr(a), mxGetM(a), mxGetN(a), s);
  }

  data_block* newDataBlock(const mxArray* a, mtx_policies::storage_policy s)
  {
    if (!mxIsDouble(a))
      throw Util::Error("cannot construct a Mtx with a non-'double' mxArray");

    if (s != mtx_policies::BORROW && s != mtx_policies::COPY)
      throw Util::Error("cannot construct a Mtx from a const mxArray* "
                        "unless the storage_policy is COPY or BORROW");

    return newDataBlock(mxGetPr(a), mxGetM(a), mxGetN(a), s);
  }
#endif
}

DataHolder::DataHolder(double* data, int mrows, int ncols, storage_policy s) :
  m_data(newDataBlock(data, mrows, ncols, s))
{
  m_data->incr_refcount();
}

DataHolder::DataHolder(int mrows, int ncols, init_policy p) :
  m_data(newDataBlock(mrows, ncols, p))
{
  m_data->incr_refcount();
}

#ifdef WITH_MATLAB
DataHolder::DataHolder(mxArray* a, storage_policy s) :
  m_data(newDataBlock(a, s))
{
  m_data->incr_refcount();
}

DataHolder::DataHolder(const mxArray* a, storage_policy s) :
  m_data(newDataBlock(a, s))
{
  m_data->incr_refcount();
}
#endif

DataHolder::DataHolder(const DataHolder& other) :
  m_data(other.m_data)
{
  m_data->incr_refcount();
}

DataHolder::~DataHolder()
{
  m_data->decr_refcount();
}

void DataHolder::swap(DataHolder& other)
{
  std::swap(m_data, other.m_data);
}

///////////////////////////////////////////////////////////////////////
//
// DataHolderRef definitions
//
///////////////////////////////////////////////////////////////////////

void DataHolderRef::swap(DataHolderRef& other)
{
  std::swap(ref_, other.ref_);
}

///////////////////////////////////////////////////////////////////////
//
// MtxBase member definitions
//
///////////////////////////////////////////////////////////////////////

template <class Data>
void MtxBase<Data>::swap(MtxBase& other)
{
  mtx_specs::swap(other);
  data_.swap(other.data_);
}

template <class Data>
MtxBase<Data>::MtxBase(const MtxBase& other) :
  mtx_specs(other),
  data_(other.data_)
{}

template <class Data>
MtxBase<Data>::MtxBase(int mrows, int ncols, const Data& data) :
  mtx_specs(mrows, ncols),
  data_(data)
{}

template <class Data>
MtxBase<Data>::MtxBase(const mtx_specs& specs, const Data& data) :
  mtx_specs(specs),
  data_(data)
{}

template <class Data>
MtxBase<Data>::~MtxBase() {}

template class MtxBase<DataHolder>;

template class MtxBase<DataHolderRef>;

///////////////////////////////////////////////////////////////////////
//
// SubMtxRef member definitions
//
///////////////////////////////////////////////////////////////////////

SubMtxRef& SubMtxRef::operator=(const SubMtxRef& other)
{
DOTRACE("SubMtxRef::operator=(const SubMtxRef&)");
  if (this->nelems() != other.nelems())
    throw Util::Error("SubMtxRef::operator=(): dimension mismatch");

  std::copy(other.colmaj_begin(), other.colmaj_end(),
            this->colmaj_begin_nc());

  return *this;
}

SubMtxRef& SubMtxRef::operator=(const Mtx& other)
{
DOTRACE("SubMtxRef::operator=(const Mtx&)");
  if (this->nelems() != other.nelems())
    throw Util::Error("SubMtxRef::operator=(): dimension mismatch");

  std::copy(other.colmaj_begin(), other.colmaj_end(),
            this->colmaj_begin_nc());

  return *this;
}

///////////////////////////////////////////////////////////////////////
//
// Mtx member definitions
//
///////////////////////////////////////////////////////////////////////

#ifdef WITH_MATLAB
Mtx::Mtx(mxArray* a, storage_policy s) :
  Base(mtx_specs(mxGetM(a), mxGetN(a)), DataHolder(a, s))
{
DOTRACE("Mtx::Mtx(mxArray*, storage_policy)");
}

Mtx::Mtx(const mxArray* a, storage_policy s) :
  Base(mtx_specs(mxGetM(a), mxGetN(a)), DataHolder(a, s))
{
DOTRACE("Mtx::Mtx(const mxArray*, storage_policy)");
}
#endif

Mtx::Mtx(double* data, int mrows, int ncols, storage_policy s) :
  Base(mrows, ncols, DataHolder(data, mrows, ncols, s))
{
DOTRACE("Mtx::Mtx(double*, int, int, storage_policy)");
}

Mtx::Mtx(int mrows, int ncols, init_policy p) :
  Base(mrows, ncols, DataHolder(mrows, ncols, p))
{
DOTRACE("Mtx::Mtx(int, int, init_policy)");
}

Mtx::Mtx(const mtx_shape& s, init_policy p) :
  Base(s.mrows(), s.ncols(), DataHolder(s.mrows(), s.ncols(), p))
{
DOTRACE("Mtx::Mtx(const mtx_shape&, init_policy)");
}

const Mtx& Mtx::emptyMtx()
{
DOTRACE("Mtx::emptyMtx");
  static Mtx* m = 0;
  if (m == 0)
    {
      m = new Mtx(0,0);
    }
  return *m;
}

Mtx::Mtx(const slice& s) :
  Base(s.nelems(), 1, DataHolder(s.nelems(), 1, NO_INIT))
{
DOTRACE("Mtx::Mtx");
  std::copy(s.begin(), s.end(), this->colmaj_begin_nc());
}

Mtx::~Mtx() {}

#ifdef WITH_MATLAB
mxArray* Mtx::makeMxArray() const
{
DOTRACE("Mtx::makeMxArray");
  mxArray* result_mx = mxCreateDoubleMatrix(mrows(), ncols(), mxREAL);

  std::copy(colmaj_begin(), colmaj_end(), mxGetPr(result_mx));

  return result_mx;
}
#endif

void Mtx::resize(int mrowsNew, int ncolsNew)
{
DOTRACE("Mtx::resize");
  if (mrows() == mrowsNew && ncols() == ncolsNew)
    return;
  else
    {
      Mtx newsize(mrowsNew, ncolsNew, ZEROS);
      this->swap(newsize);
    }
}

Mtx Mtx::contig() const
{
DOTRACE("Mtx::contig");
  if (mrows() == rowstride())
    return *this;

  Mtx result(this->mrows(), this->ncols(), NO_INIT);

  std::copy(this->colmaj_begin(), this->colmaj_end(),
            result.colmaj_begin_nc());

  return result;
}

void Mtx::print() const
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

void Mtx::print(const char* mtxName) const
{
  std::cout << mtxName << '\n';
  print();
}

void Mtx::reorderRows(const Mtx& index_)
{
DOTRACE("Mtx::reorderRows");

  Mtx index(index_.asColumn());

  if (index.mrows() != mrows())
    throw Util::Error("dimension mismatch in Mtx::reorderRows");

  Mtx neworder(mrows(), ncols());

  for (int r = 0; r < mrows(); ++r)
    neworder.row(r) = row(int(index.at(r,0)));

  *this = neworder;
}

void Mtx::reorderColumns(const Mtx& index_)
{
DOTRACE("Mtx::reorderColumns");

  Mtx index(index_.asColumn());

  if (index.mrows() != ncols())
    throw Util::Error("dimension mismatch in Mtx::reorderColumns");

  Mtx neworder(mrows(), ncols());

  for (int c = 0; c < ncols(); ++c)
    neworder.column(c) = column(int(index.at(c,0)));

  *this = neworder;
}

void Mtx::swapColumns(int c1, int c2)
{
DOTRACE("Mtx::swapColumns");

  if (c1 == c2) return;

  memswap(address_nc(0,c1), address_nc(0,c2), mrows());
}

Mtx Mtx::meanRow() const
{
DOTRACE("Mtx::meanRow");

  Mtx res(1, ncols());

  mtx_iter resiter = res.row(0).begin_nc();

  for (int c = 0; c < ncols(); ++c, ++resiter)
    *resiter = column(c).mean();

  return res;
}

Mtx Mtx::meanColumn() const
{
DOTRACE("Mtx::meanColumn");

  Mtx res(mrows(), 1);

  mtx_iter resiter = res.column(0).begin_nc();

  for (int r = 0; r < mrows(); ++r, ++resiter)
    *resiter = row(r).mean();

  return res;
}

Mtx::const_iterator Mtx::find_min() const
{
DOTRACE("Mtx::find_min");

  if (nelems() == 0)
    throw Util::Error("find_min(): the matrix must be non-empty");

  return std::min_element(begin(), end());
}

Mtx::const_iterator Mtx::find_max() const
{
DOTRACE("Mtx::find_max");

  if (nelems() == 0)
    throw Util::Error("find_max(): the matrix must be non-empty");

  return std::max_element(begin(), end());
}

double Mtx::min() const
{
DOTRACE("Mtx::min");

  if (nelems() == 0)
    throw Util::Error("min(): the matrix must be non-empty");

  return *(std::min_element(colmaj_begin(), colmaj_end()));
}

double Mtx::max() const
{
DOTRACE("Mtx::max");

  if (nelems() == 0)
    throw Util::Error("max(): the matrix must be non-empty");

  return *(std::max_element(colmaj_begin(), colmaj_end()));
}

double Mtx::sum() const
{
DOTRACE("Mtx::sum");
  return std::accumulate(begin(), end(), 0.0);
}

Mtx& Mtx::operator+=(const Mtx& other)
{
DOTRACE("Mtx::operator+=(const Mtx&)");
  if (ncols() != other.ncols())
    throw Util::Error("dimension mismatch in Mtx::operator+=");

  for (int i = 0; i < ncols(); ++i)
    column(i) += other.column(i);

  return *this;
}

Mtx& Mtx::operator-=(const Mtx& other)
{
DOTRACE("Mtx::operator-=(const Mtx&)");
  if (ncols() != other.ncols())
    throw Util::Error("dimension mismatch in Mtx::operator-=");

  for (int i = 0; i < ncols(); ++i)
    column(i) -= other.column(i);

  return *this;
}

bool Mtx::operator==(const Mtx& other) const
{
DOTRACE("Mtx::operator==(const Mtx&)");
  if ( (mrows() != other.mrows()) || (ncols() != other.ncols()) )
    return false;
  for (int c = 0; c < ncols(); ++c)
    if ( column(c) != other.column(c) ) return false;
  return true;
}

void Mtx::VMmul_assign(const slice& vec, const Mtx& mtx,
                       slice& result)
{
DOTRACE("Mtx::VMmul_assign");

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
    throw Util::Error("dimension mismatch in Mtx::VMmul_assign");

  mtx_const_iter veciter = vec.begin();

  mtx_iter resultIter = result.begin_nc();

  for (int col = 0; col < mtx.ncols(); ++col, ++resultIter)
    *resultIter = innerProduct(veciter, mtx.columnIter(col));
}

void Mtx::assign_MMmul(const Mtx& m1, const Mtx& m2)
{
DOTRACE("Mtx::assign_MMmul");
  if ( (m1.ncols() != m2.mrows()) ||
       (this->ncols() != m2.ncols()) )
    throw Util::Error("dimension mismatch in Mtx::VMmul_assign");

  for (int n = 0; n < mrows(); ++n)
    {
      mtx_iter rowElement = this->rowIter(n);

      mtx_const_iter veciter = m1.rowIter(n);

      for (int col = 0; col < m2.ncols(); ++col, ++rowElement)
        *rowElement = innerProduct(veciter, m2.columnIter(col));
    }
}

namespace
{
  template <class Op>
  Mtx unaryOp(const Mtx& src, Op op)
  {
    Mtx result(src.mrows(), src.ncols(), Mtx::NO_INIT);

    std::transform(src.colmaj_begin(), src.colmaj_end(),
                   result.colmaj_begin_nc(),
                   op);

    return result;
  }
}

Mtx operator+(const Mtx& m, double x)
{
  return unaryOp(m, std::bind2nd(std::plus<double>(), x));
}

Mtx operator-(const Mtx& m, double x)
{
  return unaryOp(m, std::bind2nd(std::minus<double>(), x));
}

Mtx operator*(const Mtx& m, double x)
{
  return unaryOp(m, std::bind2nd(std::multiplies<double>(), x));
}

Mtx operator/(const Mtx& m, double x)
{
  return unaryOp(m, std::bind2nd(std::divides<double>(), x));
}


namespace
{
  template <class Op>
  Mtx binaryOp(const Mtx& m1, const Mtx& m2, Op op)
  {
    if (! m1.sameSize(m2) )
      throw Util::Error("dimension mismatch in binaryOp(Mtx, Mtx)");

    Mtx result(m1.mrows(), m1.ncols(), Mtx::NO_INIT);

    std::transform(m1.colmaj_begin(), m1.colmaj_end(),
                   m2.colmaj_begin(),
                   result.colmaj_begin_nc(),
                   op);

    return result;
  }
}

Mtx operator+(const Mtx& m1, const Mtx& m2)
{
DOTRACE("operator+(Mtx, Mtx)");
  return binaryOp(m1, m2, std::plus<double>());
}

Mtx operator-(const Mtx& m1, const Mtx& m2)
{
DOTRACE("operator-(Mtx, Mtx)");
  return binaryOp(m1, m2, std::minus<double>());
}

Mtx arr_mul(const Mtx& m1, const Mtx& m2)
{
DOTRACE("arr_mul(Mtx, Mtx)");
  return binaryOp(m1, m2, std::multiplies<double>());
}

Mtx arr_div(const Mtx& m1, const Mtx& m2)
{
DOTRACE("arr_div(Mtx, Mtx)");
  return binaryOp(m1, m2, std::divides<double>());
}

Mtx min(const Mtx& m1, const Mtx& m2)
{
DOTRACE("min(Mtx, Mtx)");
  return binaryOp(m1, m2, Min());
}

Mtx max(const Mtx& m1, const Mtx& m2)
{
DOTRACE("max(Mtx, Mtx)");
  return binaryOp(m1, m2, Max());
}

static const char vcid_mtx_cc[] = "$Header$";
#endif // !MTX_CC_DEFINED
