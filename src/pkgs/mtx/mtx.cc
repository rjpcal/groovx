///////////////////////////////////////////////////////////////////////
//
// mtx.cc
//
// Copyright (c) 2001-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:39:12 2001
// written: Tue Feb 19 14:09:00 2002
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
#include <iostream.h>
#include <iomanip.h>
#ifdef HAVE_MATLAB
#include <libmatlb.h>
#endif

#include "util/trace.h"
#include "util/debug.h"

namespace {
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
      cout << setw(12) << setprecision(7) << double(*iter);
    }
  cout << endl;
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

  ::sort(buf.begin(), buf.end());

  Mtx index(1,nelems());

  for (int i = 0; i < nelems(); ++i)
    index.at(0,i) = buf[i].index;

  return index;
}

void Slice::sort()
{
  ::sort(beginNC(), endNC());
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

#ifdef HAVE_MATLAB
MtxImpl::MtxImpl(mxArray* a, StoragePolicy s)
{
  if (!mxIsDouble(a))
    throw Util::Error("cannot construct a Mtx with a non-'double' mxArray");

  init(mxGetPr(a), mxGetM(a), mxGetN(a), s);
}
#endif

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

void MtxImpl::makeUnique()
{
  if ( !datablock_->isUnique() )
    {
      DOTRACE("MtxImpl::makeUnique");
      DataBlock::makeUnique(datablock_);
    }
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
      MtxImpl newImpl(mrowsNew, ncolsNew);
      this->itsImpl.swap(newImpl);
    }
}

void Mtx::print() const
{
  cout << "mrows = " << mrows() << ", ncols = " << ncols() << '\n';
  for(int i = 0; i < mrows(); ++i)
    {
      for(int j = 0; j < ncols(); ++j)
        cout << setw(12) << setprecision(7) << at(i,j);
      cout << '\n';
    }
  cout << '\n';
}

void Mtx::print(const char* mtxName) const
{
  cout << mtxName << '\n';
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

  makeUnique();

  memswap(itsImpl.address_nc(0,c1), itsImpl.address_nc(0,c2), mrows());
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

static const char vcid_mtx_cc[] = "$Header$";
#endif // !MTX_CC_DEFINED
