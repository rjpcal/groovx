///////////////////////////////////////////////////////////////////////
//
// mtx.cc
//
// Copyright (c) 2001-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:39:12 2001
// written: Mon Mar  4 13:25:38 2002
// $Id$
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
#ifdef HAVE_MATLAB
#include <libmatlb.h>
#endif
#include <numeric>

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  inline void domemswap(double* buf1, double* buf2,
                        double* tempbuf1, size_t nelems)
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
        static double swapbuffer[BUFSIZE];
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
  void raiseException(const char* msg, const char* f, int ln);
}

void RC::raiseException(const char* msg, const char* f, int ln)
{
  DebugPrintNL(msg);
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

void RC::less(const void* x, const void* lim, const char* f, int ln)
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

void RC::geq(int x, int lim, const char* f, int ln)
{
  if (x>=lim) ; // OK
  else raiseException("geq: integer range error", f, ln);
}

void RC::less(int x, int lim, const char* f, int ln)
{
  if (x<lim) ; // OK
  else raiseException("less: integer range error", f, ln);
}

void RC::leq(int x, int lim, const char* f, int ln)
{
  if (x<=lim) ; // OK
  else raiseException("leq: integer range error", f, ln);
}

void RC::inHalfOpen(int x, int llim, int ulim, const char* f, int ln)
{
  if (x>=llim && x<ulim) ; // OK
  else raiseException("inHalfOpen: integer range error", f, ln);
}


///////////////////////////////////////////////////////////////////////
//
// Slice member definitions
//
///////////////////////////////////////////////////////////////////////

Slice Slice::operator()(const Range& rng) const
{
  RC_inHalfOpen(rng.begin(), 0, itsNelems);
  RC_geq(rng.count(), 0);
  RC_leq(rng.end(), itsNelems);

  return Slice(itsOwner, storageOffset(rng.begin()), itsStride, rng.count());
}

void Slice::print() const
{
  for(MtxConstIter iter = begin(); iter.hasMore(); ++iter)
    {
      std::cout << std::setw(12) << std::setprecision(7) << double(*iter);
    }
  std::cout << std::endl;
}

double Slice::sum() const
{
  double s = 0.0;
  for (MtxConstIter i = begin(); i.hasMore(); ++i)
    s += *i;
  return s;
}

double Slice::min() const
{
  MtxConstIter i = begin();
  double mn = *i;
  for (; i.hasMore(); ++i)
    if (*i < mn) mn = *i;
  return mn;
}

double Slice::max() const
{
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
  if (itsNelems != other.itsNelems) return false;

  for (MtxConstIter a = this->begin(), b = other.begin();
       a.hasMore();
       ++a, ++b)
    if (*a != *b) return false;

  return true;
}

void Slice::sort()
{
  std::sort(beginNC(), endNC());
}

void Slice::reorder(const Mtx& index_)
{
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
  if (itsNelems != other.nelems())
    throw Util::Error("dimension mismatch in Slice::operator+=");

  MtxConstIter rhs = other.begin();

  for (MtxIter lhs = beginNC(); lhs.hasMore(); ++lhs, ++rhs)
    *lhs += *rhs;

  return *this;
}

Slice& Slice::operator-=(const Slice& other)
{
  if (itsNelems != other.nelems())
    throw Util::Error("dimension mismatch in Slice::operator-=");

  MtxConstIter rhs = other.begin();

  for (MtxIter lhs = beginNC(); lhs.hasMore(); ++lhs, ++rhs)
    *lhs -= *rhs;

  return *this;
}

Slice& Slice::operator=(const Slice& other)
{
  if (itsNelems != other.nelems())
    throw Util::Error("dimension mismatch in Slice::operator=");

  MtxConstIter rhs = other.begin();

  for (MtxIter lhs = beginNC(); lhs.hasMore(); ++lhs, ++rhs)
    *lhs = *rhs;

  return *this;
}

Slice& Slice::operator=(const Mtx& other)
{
  if (itsNelems != other.nelems())
    throw Util::Error("dimension mismatch in Slice::operator=");

  int i = 0;
  for (  MtxIter lhs = beginNC(); lhs.hasMore(); ++lhs, ++i)
    *lhs = other.at(i);

  return *this;
}

///////////////////////////////////////////////////////////////////////
//
// DataHolder member functions
//
///////////////////////////////////////////////////////////////////////

DataHolder::DataHolder(DataBlock* db) :
  datablock_(db)
{
  datablock_->incrRefCount();
}

DataHolder::DataHolder(double* data, int mrows, int ncols, StoragePolicy s) :
  datablock_(0)
{
  switch (s)
    {
    case BORROW:
      datablock_ = DataBlock::makeBorrowed(data, mrows*ncols);
      break;
    case REFER:
      datablock_ = DataBlock::makeReferred(data, mrows*ncols);
      break;
    case COPY:
    default:
      datablock_ = DataBlock::makeDataCopy(data, mrows*ncols);
      break;
    }

  datablock_->incrRefCount();
}

DataHolder::DataHolder(const DataHolder& other) :
  datablock_(other.datablock_)
{
  datablock_->incrRefCount();
}

DataHolder::~DataHolder()
{
  datablock_->decrRefCount();
}

///////////////////////////////////////////////////////////////////////
//
// MtxImpl member definitions
//
///////////////////////////////////////////////////////////////////////

void MtxImpl::swap(MtxImpl& other)
{
  std::swap(datablock_, other.datablock_);
  std::swap(mrows_, other.mrows_);
  std::swap(rowstride_, other.rowstride_);
  std::swap(ncols_, other.ncols_);
  std::swap(offset_, other.offset_);
}

MtxImpl::MtxImpl(const MtxImpl& other) :
  MtxStorage(other),
  DataHolder(other)
{}

namespace
{
  DataBlock* newDataBlock(int mrows, int ncols, MtxImpl::InitPolicy p)
  {
    if (p == MtxImpl::ZEROS) return DataBlock::makeBlank(mrows*ncols);
    return DataBlock::makeUninitialized(mrows*ncols);
  }
}

MtxImpl::MtxImpl(int mrows, int ncols, InitPolicy p) :
  MtxStorage(mrows, ncols),
  DataHolder(newDataBlock(mrows, ncols, p))
{}

#ifdef HAVE_MATLAB
MtxImpl::MtxImpl(mxArray* a, StoragePolicy s) :
  MtxStorage(mxGetM(a), mxGetN(a)),
  DataHolder(mxGetPr(a), mxGetM(a), mxGetN(a), s)
{
  if (!mxIsDouble(a))
    throw Util::Error("cannot construct a Mtx with a non-'double' mxArray");
}

MtxImpl::MtxImpl(const mxArray* a, StoragePolicy s) :
  MtxStorage(mxGetM(a), mxGetN(a)),
  DataHolder(mxGetPr(a), mxGetM(a), mxGetN(a), s)
{
  if (s != BORROW && s != COPY)
    throw Util::Error("cannot construct a Mtx from a const mxArray* "
                      "unless the StoragePolicy is COPY or BORROW");

  if (!mxIsDouble(a))
    throw Util::Error("cannot construct a Mtx with a non-'double' mxArray");
}
#endif

MtxImpl::~MtxImpl() {}

void MtxImpl::reshape(int mr, int nc)
{
  if (mr*nc != nelems())
    {
      fstring msg;
      msg.append("dimension mismatch in Mtx::reshape: ");
      msg.append("current nelems == ", nelems(), "; requested ");
      msg.append(mr, "x", nc);
      throw Util::Error(msg);
    }

  if (rowstride_ != mrows_)
    throw Util::Error("reshape not allowed for submatrix");

  mrows_ = mr;
  rowstride_ = mr;
  ncols_ = nc;
}

void MtxImpl::selectRows(const RowRange& rng)
{
  if (rng.begin() < 0)
    throw Util::Error("selectRows(): row index must be >= 0");

  if (rng.count() <= 0)
    throw Util::Error("selectRows(): number of rows must be > 0");

  if (rng.end() > mrows_)
    throw Util::Error("selectRows(): upper row index out of range");

  offset_ += rng.begin();
  mrows_ = rng.count();
}

void MtxImpl::selectCols(const ColRange& rng)
{
  if (rng.begin() < 0)
    throw Util::Error("selectCols(): column index must be >= 0");

  if (rng.count() <= 0)
    throw Util::Error("selectCols(): number of columns must be > 0");

  if (rng.end() > ncols_)
    throw Util::Error("selectCols(): upper column index out of range");

  offset_ += rng.begin()*rowstride_;
  ncols_ = rng.count();
}

///////////////////////////////////////////////////////////////////////
//
// Mtx member definitions
//
///////////////////////////////////////////////////////////////////////

const Mtx& Mtx::emptyMtx()
{
  static Mtx m(0,0);
  return m;
}

Mtx::Mtx(const Slice& s) :
  itsImpl(const_cast<double*>(s.dataStart()), s.nelems(), 1, COPY)
{
  if (s.itsStride != 1)
    throw Util::Error("can't initialize Mtx from Slice with stride != 1");
}

Mtx::~Mtx() {}

#ifdef HAVE_MATLAB
mxArray* Mtx::makeMxArray() const
{
  mxArray* result_mx = mxCreateDoubleMatrix(mrows(), ncols(), mxREAL);

  const int n = nelems();

  double* matdata = mxGetPr(result_mx);

  for (int i = 0; i < n; ++i)
    matdata[i] = itsImpl.at(itsImpl.offsetFromStart(i));

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
      MtxImpl newImpl(mrowsNew, ncolsNew, ZEROS);
      this->itsImpl.swap(newImpl);
    }
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

Mtx Mtx::operator()(const RowRange& rng) const
{
  Mtx result(*this);

  result.itsImpl.selectRows(rng);

  return result;
}

void Mtx::reorderRows(const Mtx& index_)
{
  Mtx index(index_.asColumn());

  if (index.mrows() != mrows())
    throw Util::Error("dimension mismatch in Mtx::reorderRows");

  Mtx neworder(mrows(), ncols());

  for (int r = 0; r < mrows(); ++r)
    neworder.row(r) = row(int(index.at(r,0)));

  *this = neworder;
}

Mtx Mtx::operator()(const ColRange& rng) const
{
  Mtx result(*this);

  result.itsImpl.selectCols(rng);

  return result;
}

void Mtx::reorderColumns(const Mtx& index_)
{
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
  if (c1 == c2) return;

  memswap(itsImpl.address_nc(0,c1), itsImpl.address_nc(0,c2), mrows());
}

Mtx Mtx::meanRow() const
{
  Mtx res(1, ncols());

  MtxIter resiter = res.row(0).beginNC();

  for (int c = 0; c < ncols(); ++c, ++resiter)
    *resiter = column(c).mean();

  return res;
}

Mtx Mtx::meanColumn() const
{
  Mtx res(mrows(), 1);

  MtxIter resiter = res.column(0).beginNC();

  for (int r = 0; r < mrows(); ++r, ++resiter)
    *resiter = row(r).mean();

  return res;
}

Mtx::const_iterator Mtx::find_min() const
{
  if (nelems() == 0)
    throw Util::Error("find_min(): the matrix must be non-empty");

  return std::min_element(begin(), end());
}

Mtx::const_iterator Mtx::find_max() const
{
  if (nelems() == 0)
    throw Util::Error("find_max(): the matrix must be non-empty");

  return std::max_element(begin(), end());
}

double Mtx::min() const
{
  if (nelems() == 0)
    throw Util::Error("min(): the matrix must be non-empty");

  return *(std::min_element(colmaj_begin(), colmaj_end()));
}

double Mtx::max() const
{
  if (nelems() == 0)
    throw Util::Error("max(): the matrix must be non-empty");

  return *(std::max_element(colmaj_begin(), colmaj_end()));
}

double Mtx::sum() const
{
  return std::accumulate(begin(), end(), 0.0);
}

Mtx& Mtx::operator+=(const Mtx& other)
{
  if (ncols() != other.ncols())
    throw Util::Error("dimension mismatch in Mtx::operator+=");

  for (int i = 0; i < ncols(); ++i)
    column(i) += other.column(i);

  return *this;
}

Mtx& Mtx::operator-=(const Mtx& other)
{
  if (ncols() != other.ncols())
    throw Util::Error("dimension mismatch in Mtx::operator-=");

  for (int i = 0; i < ncols(); ++i)
    column(i) -= other.column(i);

  return *this;
}

bool Mtx::operator==(const Mtx& other) const
{
  if ( (mrows() != other.mrows()) || (ncols() != other.ncols()) )
    return false;
  for (int c = 0; c < ncols(); ++c)
    if ( column(c) != other.column(c) ) return false;
  return true;
}

void Mtx::VMmul_assign(const Slice& vec, const Mtx& mtx,
                       Slice& result)
{
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
  return binaryOp(m1, m2, std::plus<double>());
}

Mtx operator-(const Mtx& m1, const Mtx& m2)
{
  return binaryOp(m1, m2, std::minus<double>());
}

Mtx arr_mul(const Mtx& m1, const Mtx& m2)
{
  return binaryOp(m1, m2, std::multiplies<double>());
}

Mtx arr_div(const Mtx& m1, const Mtx& m2)
{
  return binaryOp(m1, m2, std::divides<double>());
}

Mtx min(const Mtx& m1, const Mtx& m2)
{
  return binaryOp(m1, m2, Min());
}

Mtx max(const Mtx& m1, const Mtx& m2)
{
  return binaryOp(m1, m2, Max());
}

static const char vcid_mtx_cc[] = "$Header$";
#endif // !MTX_CC_DEFINED
