///////////////////////////////////////////////////////////////////////
//
// mtx.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:39:12 2001
// written: Wed Mar 14 16:38:17 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MTX_CC_DEFINED
#define MTX_CC_DEFINED

#include "mtx.h"

#include "error.h"

#include "libmatlb.h"

#include "trace.h"

Slice& Slice::operator=(const Slice& other)
{
  if (itsNelems != other.nelems())
    throw ErrorWithMsg("dimension mismatch in Slice::operator=");

  MtxConstIter rhs = other.begin();

  for (MtxIter lhs = beginNC(); lhs.hasMore(); ++lhs, ++rhs)
    *lhs = *rhs;

  return *this;
}

MtxIter::MtxIter(Mtx& m, ptrdiff_t storageOffset, int s, int n) :
  data(0), stride(0), stop(0)
{
  // Make sure that the data storage is unique since this is a
  // non-const iterator
  m.makeUnique();

  data = m.storageStart() + storageOffset;
  stride = s;
  stop = data + s*n;
}

Mtx::MtxImpl::MtxImpl(mxArray* a, StoragePolicy s)
{
  init(mxGetPr(a), mxGetM(a), mxGetN(a), s);
}

void Mtx::MtxImpl::reshape(int mr, int nc)
{
  if (mr*nc != nelems())
    throw ErrorWithMsg("dimension mismatch in Mtx::reshape");

  if (rowstride_ != mrows_)
    throw ErrorWithMsg("reshape not allowed for submatrix");

  mrows_ = mr;
  rowstride_ = mr;
  ncols_ = nc;
}

void Mtx::MtxImpl::selectRowRange(int r, int nr)
{
  if ((r+nr) > mrows_)
    throw ErrorWithMsg("attempted to index more rows than are available");

  offset_ += r;
  mrows_ = nr;
}

void Mtx::MtxImpl::selectColumnRange(int c, int nc)
{
  if ((c+nc) > ncols_)
    throw ErrorWithMsg("attempted to index more columns than are available");

  offset_ += c*rowstride_;
  ncols_ = nc;
}

void Mtx::MtxImpl::makeUnique()
{
  if ( !storage_->isUnique() )
	 {
		DOTRACE("Mtx::MtxImpl::makeUnique");
  		DataBlock::makeUnique(storage_);
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
  itsImpl(s.dataStart(), s.nelems(), 1, BORROW)
{
  if (s.itsStride != 1)
    throw ErrorWithMsg("can't initialize Mtx from Slice with stride != 1");
}

mxArray* Mtx::makeMxArray() const
{
  mxArray* result_mx = mxCreateDoubleMatrix(mrows(), ncols(), mxREAL);

  const int n = nelems();

  double* matdata = mxGetPr(result_mx);

  for (int i = 0; i < n; ++i)
    matdata[i] = itsImpl.at(itsImpl.offsetFromStart(i));

  return result_mx;
}

void Mtx::print() const
{
  mexPrintf("mrows = %d, ncols = %d\n", mrows(), ncols());
  for(int i = 0; i < mrows(); ++i)
    {
      for(int j = 0; j < ncols(); ++j)
        mexPrintf("%7.4f   ", at(i,j));
      mexPrintf("\n");
    }
  mexPrintf("\n");
}

Mtx Mtx::rows(int r, int nr) const
{
  Mtx result(*this);

  result.itsImpl.selectRowRange(r, nr);

  return result;
}

Mtx Mtx::columns(int c, int nc) const
{
  Mtx result(*this);

  result.itsImpl.selectColumnRange(c, nc);

  return result;
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
    throw ErrorWithMsg("dimension mismatch in Mtx::VMmul_assign");

  MtxConstIter veciter = vec.begin();

  MtxIter resultIter = result.beginNC();

  for (int col = 0; col < mtx.ncols(); ++col, ++resultIter)
    *resultIter = innerProduct(veciter, mtx.colIter(col));
}

void Mtx::assign_MMmul(const Mtx& m1, const Mtx& m2)
{
DOTRACE("Mtx::assign_MMmul");
  if ( (m1.ncols() != m2.mrows()) ||
		 (this->ncols() != m2.ncols()) )
	 throw ErrorWithMsg("dimension mismatch in Mtx::VMmul_assign");

  for (int n = 0; n < mrows(); ++n)
    {
		MtxIter rowElement = this->rowIter(n);

		MtxConstIter veciter = m1.rowIter(n);

		for (int col = 0; col < m2.ncols(); ++col, ++rowElement)
		  *rowElement = innerProduct(veciter, m2.colIter(col));
    }
}

static const char vcid_mtx_cc[] = "$Header$";
#endif // !MTX_CC_DEFINED
