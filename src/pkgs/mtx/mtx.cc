///////////////////////////////////////////////////////////////////////
//
// mtx.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
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
#include "util/minivec.h"
#include "util/strings.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#ifdef WITH_MATLAB
#include <libmatlb.h>
#endif
#include <numeric>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER;

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

namespace RC
{
  void raiseException(const fstring& msg, const char* f, int ln);
}

void RC::raiseException(const fstring& msg, const char* f, int ln)
{
  dbgPrintNL(3, msg);
  fstring errmsg;
  errmsg.append("Range check failed in file '", f, "' at line #");
  errmsg.append(ln, ": ", msg);
  throw Util::Error(errmsg);
}

void RC::geq(const void* x, const void* lim, const char* f, int ln)
{
  if (x>=lim) ; // OK
  else raiseException("geq: pointer range error", f, ln);
}

void RC::lt(const void* x, const void* lim, const char* f, int ln)
{
  if (x<lim) ; // OK
  else raiseException("less: pointer range error", f, ln);
}

void RC::leq(const void* x, const void* lim, const char* f, int ln)
{
  if (x<=lim) ; // OK
  else raiseException("leq: pointer range error", f, ln);
}

void RC::inHalfOpen(const void* x, const void* llim, const void* ulim,
                    const char* f, int ln)
{
  if (x>=llim && x<ulim) ; // OK
  else raiseException("inHalfOpen: pointer range error", f, ln);
}

void RC::inFullOpen(const void* x, const void* llim, const void* ulim,
                    const char* f, int ln)
{
  if (x>=llim && x<=ulim) ; // OK
  else raiseException("inFullOpen: pointer range error", f, ln);
}

void RC::geq(int x, int lim, const char* f, int ln)
{
  if (x>=lim) ; // OK
  else raiseException(fstring("geq: integer range error ",
                              x, " !>= ", lim),
                      f, ln);
}

void RC::lt(int x, int lim, const char* f, int ln)
{
  if (x<lim) ; // OK
  else raiseException(fstring("less: integer range error ",
                              x, " !< ", lim),
                      f, ln);
}

void RC::leq(int x, int lim, const char* f, int ln)
{
  if (x<=lim) ; // OK
  else raiseException(fstring("leq: integer range error ",
                              x, " !<= ", lim),
                      f, ln);
}

void RC::inHalfOpen(int x, int llim, int ulim, const char* f, int ln)
{
  if (x>=llim && x<ulim) ; // OK
  else raiseException(fstring("inHalfOpen: integer range error ",
                              x, " !in [", llim, ", ", ulim, ")"),
                      f, ln);
}

void RC::inFullOpen(int x, int llim, int ulim, const char* f, int ln)
{
  if (x>=llim && x<=ulim) ; // OK
  else raiseException(fstring("inFullOpen: integer range error ",
                              x, " !in [", llim, ", ", ulim, "]"),
                      f, ln);
}


///////////////////////////////////////////////////////////////////////
//
// Slice member definitions
//
///////////////////////////////////////////////////////////////////////

Slice Slice::operator()(const Range& rng) const
{
DOTRACE("Slice::operator");
  RC_inHalfOpen(rng.begin(), 0, itsNelems);
  RC_geq(rng.count(), 0);
  RC_leq(rng.end(), itsNelems);

  return Slice(itsOwner, storageOffset(rng.begin()), itsStride, rng.count());
}

void Slice::print() const
{
DOTRACE("Slice::print");
  for(MtxConstIter iter = begin(); iter.hasMore(); ++iter)
    {
      std::cout << std::setw(12) << std::setprecision(7) << double(*iter);
    }
  std::cout << std::endl;
}

double Slice::sum() const
{
DOTRACE("Slice::sum");
  double s = 0.0;
  for (MtxConstIter i = begin(); i.hasMore(); ++i)
    s += *i;
  return s;
}

double Slice::min() const
{
DOTRACE("Slice::min");
  MtxConstIter i = begin();
  double mn = *i;
  for (; i.hasMore(); ++i)
    if (*i < mn) mn = *i;
  return mn;
}

double Slice::max() const
{
DOTRACE("Slice::max");
  MtxConstIter i = begin();
  double mx = *i;
  for (; i.hasMore(); ++i)
    if (*i > mx) mx = *i;
  return mx;
}

namespace
{
  struct ValIndex
  {
    static int counter;
    double val;
    int index;

    ValIndex(double v) : val(v), index(counter++) {}

    bool operator<(const ValIndex& v2) const { return val < v2.val; }
  };

  int ValIndex::counter = 0;
}

Mtx Slice::getSortOrder() const
{
DOTRACE("Slice::getSortOrder");

  ValIndex::counter = 0;

  minivec<ValIndex> buf(begin(), end());

  std::sort(buf.begin(), buf.end());

  Mtx index(1,nelems());

  for (int i = 0; i < nelems(); ++i)
    index.at(0,i) = buf[i].index;

  return index;
}

bool Slice::operator==(const Slice& other) const
{
DOTRACE("Slice::operator==(const Slice&)");
  if (itsNelems != other.itsNelems) return false;

  for (MtxConstIter a = this->begin(), b = other.begin();
       a.hasMore();
       ++a, ++b)
    if (*a != *b) return false;

  return true;
}

void Slice::sort()
{
DOTRACE("Slice::sort");
  std::sort(beginNC(), endNC());
}

void Slice::reorder(const Mtx& index_)
{
DOTRACE("Slice::reorder");
  Mtx index(index_.asColumn());

  if (index.mrows() != nelems())
    throw Util::Error("dimension mismatch in Slice::reorder");

  Mtx neworder(nelems(), 1);

  for (int i = 0; i < nelems(); ++i)
    neworder.at(i,0) = (*this)[int(index.at(i,0))];

  *this = neworder.column(0);
}

Slice& Slice::operator+=(const Slice& other)
{
DOTRACE("Slice::operator+=(const Slice&)");
  if (itsNelems != other.nelems())
    throw Util::Error("dimension mismatch in Slice::operator+=");

  MtxConstIter rhs = other.begin();

  for (MtxIter lhs = beginNC(); lhs.hasMore(); ++lhs, ++rhs)
    *lhs += *rhs;

  return *this;
}

Slice& Slice::operator-=(const Slice& other)
{
DOTRACE("Slice::operator-=(const Slice&)");
  if (itsNelems != other.nelems())
    throw Util::Error("dimension mismatch in Slice::operator-=");

  MtxConstIter rhs = other.begin();

  for (MtxIter lhs = beginNC(); lhs.hasMore(); ++lhs, ++rhs)
    *lhs -= *rhs;

  return *this;
}

Slice& Slice::operator=(const Slice& other)
{
DOTRACE("Slice::operator=(const Slice&)");
  if (itsNelems != other.nelems())
    throw Util::Error("dimension mismatch in Slice::operator=");

  MtxConstIter rhs = other.begin();

  for (MtxIter lhs = beginNC(); lhs.hasMore(); ++lhs, ++rhs)
    *lhs = *rhs;

  return *this;
}

Slice& Slice::operator=(const Mtx& other)
{
DOTRACE("Slice::operator=(const Mtx&)");
  if (itsNelems != other.nelems())
    throw Util::Error("dimension mismatch in Slice::operator=");

  int i = 0;
  for (MtxIter lhs = beginNC(); lhs.hasMore(); ++lhs, ++i)
    *lhs = other.at(i);

  return *this;
}

void MtxSpecs::swap(MtxSpecs& other)
{
  std::swap(shape_, other.shape_);
  std::swap(rowstride_, other.rowstride_);
  std::swap(offset_, other.offset_);
}

MtxSpecs MtxSpecs::as_shape(const MtxShape& s) const
{
DOTRACE("MtxSpecs::as_shape");
  if (s.nelems() != this->nelems())
    {
      fstring msg;
      msg.append("as_shape(): dimension mismatch: ");
      msg.append("current nelems == ", nelems(), "; requested ");
      msg.append(s.mrows(), "x", s.ncols());
      throw Util::Error(msg);
    }

  if (rowstride_ != mrows())
    throw Util::Error("as_shape(): cannot reshape a submatrix");

  MtxSpecs result = *this;
  result.shape_ = s;
  result.rowstride_ = s.mrows();

  return result;
}

void MtxSpecs::selectRows(const RowRange& rng)
{
DOTRACE("MtxSpecs::selectRows");
  if (rng.begin() < 0)
    throw Util::Error("selectRows(): row index must be >= 0");

  if (rng.count() <= 0)
    throw Util::Error("selectRows(): number of rows must be > 0");

  if (rng.end() > mrows())
    throw Util::Error("selectRows(): upper row index out of range");

  offset_ += rng.begin();
  shape_ = MtxShape(rng.count(), ncols());
}

void MtxSpecs::selectCols(const ColRange& rng)
{
DOTRACE("MtxSpecs::selectCols");
  if (rng.begin() < 0)
    throw Util::Error("selectCols(): column index must be >= 0");

  if (rng.count() <= 0)
    throw Util::Error("selectCols(): number of columns must be > 0");

  if (rng.end() > ncols())
    throw Util::Error("selectCols(): upper column index out of range");

  offset_ += rng.begin()*rowstride_;
  shape_ = MtxShape(mrows(), rng.count());
}

///////////////////////////////////////////////////////////////////////
//
// DataHolder member functions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  DataBlock* newDataBlock(int mrows, int ncols, WithPolicies::InitPolicy p)
  {
    if (p == WithPolicies::ZEROS) return DataBlock::makeBlank(mrows*ncols);
    return DataBlock::makeUninitialized(mrows*ncols);
  }

  DataBlock* newDataBlock(double* data,
                          int mrows, int ncols, WithPolicies::StoragePolicy s)
  {
    switch (s)
      {
      case WithPolicies::BORROW:
        return DataBlock::makeBorrowed(data, mrows*ncols);
        break;
      case WithPolicies::REFER:
        return DataBlock::makeReferred(data, mrows*ncols);
        break;
      case WithPolicies::COPY:
      default:
        break;
      }

    return DataBlock::makeDataCopy(data, mrows*ncols);
  }

#ifdef WITH_MATLAB
  DataBlock* newDataBlock(mxArray* a, WithPolicies::StoragePolicy s)
  {
    if (!mxIsDouble(a))
      throw Util::Error("cannot construct a Mtx with a non-'double' mxArray");

    return newDataBlock(mxGetPr(a), mxGetM(a), mxGetN(a), s);
  }

  DataBlock* newDataBlock(const mxArray* a, WithPolicies::StoragePolicy s)
  {
    if (!mxIsDouble(a))
      throw Util::Error("cannot construct a Mtx with a non-'double' mxArray");

    if (s != WithPolicies::BORROW && s != WithPolicies::COPY)
      throw Util::Error("cannot construct a Mtx from a const mxArray* "
                        "unless the StoragePolicy is COPY or BORROW");

    return newDataBlock(mxGetPr(a), mxGetM(a), mxGetN(a), s);
  }
#endif
}

DataHolder::DataHolder(double* data, int mrows, int ncols, StoragePolicy s) :
  datablock_(newDataBlock(data, mrows, ncols, s))
{
  datablock_->incrRefCount();
}

DataHolder::DataHolder(int mrows, int ncols, InitPolicy p) :
  datablock_(newDataBlock(mrows, ncols, p))
{
  datablock_->incrRefCount();
}

#ifdef WITH_MATLAB
DataHolder::DataHolder(mxArray* a, StoragePolicy s) :
  datablock_(newDataBlock(a, s))
{
  datablock_->incrRefCount();
}

DataHolder::DataHolder(const mxArray* a, StoragePolicy s) :
  datablock_(newDataBlock(a, s))
{
  datablock_->incrRefCount();
}
#endif

DataHolder::DataHolder(const DataHolder& other) :
  datablock_(other.datablock_)
{
  datablock_->incrRefCount();
}

DataHolder::~DataHolder()
{
  datablock_->decrRefCount();
}

void DataHolder::swap(DataHolder& other)
{
  std::swap(datablock_, other.datablock_);
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
  MtxSpecs::swap(other);
  data_.swap(other.data_);
}

template <class Data>
MtxBase<Data>::MtxBase(const MtxBase& other) :
  MtxSpecs(other),
  data_(other.data_)
{}

template <class Data>
MtxBase<Data>::MtxBase(int mrows, int ncols, const Data& data) :
  MtxSpecs(mrows, ncols),
  data_(data)
{}

template <class Data>
MtxBase<Data>::MtxBase(const MtxSpecs& specs, const Data& data) :
  MtxSpecs(specs),
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
Mtx::Mtx(mxArray* a, StoragePolicy s) :
  Base(MtxSpecs(mxGetM(a), mxGetN(a)), DataHolder(a, s))
{
DOTRACE("Mtx::Mtx(mxArray*, StoragePolicy)");
}

Mtx::Mtx(const mxArray* a, StoragePolicy s) :
  Base(MtxSpecs(mxGetM(a), mxGetN(a)), DataHolder(a, s))
{
DOTRACE("Mtx::Mtx(const mxArray*, StoragePolicy)");
}
#endif

Mtx::Mtx(double* data, int mrows, int ncols, StoragePolicy s) :
  Base(mrows, ncols, DataHolder(data, mrows, ncols, s))
{
DOTRACE("Mtx::Mtx(double*, int, int, StoragePolicy)");
}

Mtx::Mtx(int mrows, int ncols, InitPolicy p) :
  Base(mrows, ncols, DataHolder(mrows, ncols, p))
{
DOTRACE("Mtx::Mtx(int, int, InitPolicy)");
}

Mtx::Mtx(const MtxShape& s, InitPolicy p) :
  Base(s.mrows(), s.ncols(), DataHolder(s.mrows(), s.ncols(), p))
{
DOTRACE("Mtx::Mtx(const MtxShape&, InitPolicy)");
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

Mtx::Mtx(const Slice& s) :
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

  MtxIter resiter = res.row(0).beginNC();

  for (int c = 0; c < ncols(); ++c, ++resiter)
    *resiter = column(c).mean();

  return res;
}

Mtx Mtx::meanColumn() const
{
DOTRACE("Mtx::meanColumn");

  Mtx res(mrows(), 1);

  MtxIter resiter = res.column(0).beginNC();

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

void Mtx::VMmul_assign(const Slice& vec, const Mtx& mtx,
                       Slice& result)
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

  MtxConstIter veciter = vec.begin();

  MtxIter resultIter = result.beginNC();

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
      MtxIter rowElement = this->rowIter(n);

      MtxConstIter veciter = m1.rowIter(n);

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
