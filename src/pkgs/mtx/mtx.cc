///////////////////////////////////////////////////////////////////////
//
// mtx.cc
//
// Copyright (c) 2001-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:39:12 2001
// written: Wed Feb 27 15:39:58 2002
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

void Slice::print() const
{
  for(MtxConstIter iter = begin(); iter.hasMore(); ++iter)
    {
      std::cout << std::setw(12) << std::setprecision(7) << double(*iter);
    }
  std::cout << std::endl;
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
// MtxImpl member definitions
//
///////////////////////////////////////////////////////////////////////

void MtxImpl::init(double* data, int mrows, int ncols, StoragePolicy s)
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

  mrows_ = mrows;
  rowstride_ = mrows;
  ncols_ = ncols;
  offset_ = 0;
}

void MtxImpl::swap(MtxImpl& other)
{
  doswap(datablock_, other.datablock_);
  doswap(mrows_, other.mrows_);
  doswap(rowstride_, other.rowstride_);
  doswap(ncols_, other.ncols_);
  doswap(offset_, other.offset_);
}

MtxImpl::MtxImpl(const MtxImpl& other) :
  datablock_(other.datablock_),
  mrows_(other.mrows_),
  rowstride_(other.rowstride_),
  ncols_(other.ncols_),
  offset_(other.offset_)
{
  datablock_->incrRefCount();
}

namespace
{
  DataBlock* newDataBlock(int mrows, int ncols, MtxImpl::InitPolicy p)
  {
    if (p == MtxImpl::ZEROS) return DataBlock::makeBlank(mrows*ncols);
    return DataBlock::makeUninitialized(mrows*ncols);
  }
}

MtxImpl::MtxImpl(int mrows, int ncols, InitPolicy p) :
  datablock_(newDataBlock(mrows, ncols, p)),
  mrows_(mrows),
  rowstride_(mrows),
  ncols_(ncols),
  offset_(0)
{
  datablock_->incrRefCount();
}

#ifdef HAVE_MATLAB
MtxImpl::MtxImpl(mxArray* a, StoragePolicy s)
{
  if (!mxIsDouble(a))
    throw Util::Error("cannot construct a Mtx with a non-'double' mxArray");

  init(mxGetPr(a), mxGetM(a), mxGetN(a), s);
}

MtxImpl::MtxImpl(const mxArray* a, StoragePolicy s)
{
  if (s != BORROW && s != COPY)
    throw Util::Error("cannot construct a Mtx from a const mxArray* "
                      "unless the StoragePolicy is COPY or BORROW");

  if (!mxIsDouble(a))
    throw Util::Error("cannot construct a Mtx with a non-'double' mxArray");

  init(mxGetPr(a), mxGetM(a), mxGetN(a), s);
}
#endif

MtxImpl::~MtxImpl() { datablock_->decrRefCount(); }

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

void MtxImpl::selectRowRange(int r, int nr)
{
  if (r < 0 || nr < 0)
    throw Util::Error("attempted to select rows with negative indices");

  if ((r+nr) > mrows_)
    throw Util::Error("attempted to index more rows than are available");

  offset_ += r;
  mrows_ = nr;
}

void MtxImpl::selectColumnRange(int c, int nc)
{
  if (c < 0 || nc < 0)
    throw Util::Error("attempted to select rows with negative indices");

  if ((c+nc) > ncols_)
    throw Util::Error("attempted to index more columns than are available");

  offset_ += c*rowstride_;
  ncols_ = nc;
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

Mtx Mtx::rows(int r, int nr) const
{
  Mtx result(*this);

  result.itsImpl.selectRowRange(r, nr);

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

Mtx Mtx::columns(int c, int nc) const
{
  Mtx result(*this);

  result.itsImpl.selectColumnRange(c, nc);

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

Mtx::const_iterator Mtx::find_min() const
{
  return std::min_element(begin(), end());
}

Mtx::const_iterator Mtx::find_max() const
{
  return std::max_element(begin(), end());
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

    std::transform(src.begin(), src.end(),
                   result.begin_nc(),
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

    std::transform(m1.begin(), m1.end(),
                   m2.begin(),
                   result.begin_nc(),
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
