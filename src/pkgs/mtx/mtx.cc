///////////////////////////////////////////////////////////////////////
//
// mtx.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:39:12 2001
// written: Wed Mar 14 15:54:21 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MTX_CC_DEFINED
#define MTX_CC_DEFINED

#include "mtx.h"

#include "error.h"

#include "libmatlb.h"

#define LOCAL_TRACE
#include "trace.h"

namespace {
  Mtx dummyMtx(0,0);
}

ConstSlice::ConstSlice() :
  itsStorage(dummyMtx.storage_),
  itsOffset(0),
  itsStride(1),
  itsNelems(0)
{}

Slice& Slice::operator=(const ConstSlice& other)
{
  if (itsNelems != other.nelems())
	 throw ErrorWithMsg("dimension mismatch in Slice::operator=");

  ConstIterator rhs = other.begin();

  for (Iterator lhs = begin(); lhs.hasMore(); ++lhs, ++rhs)
	 *lhs = *rhs;

  return *this;
}

///////////////////////////////////////////////////////////////////////
//
// Mtx member definitions
//
///////////////////////////////////////////////////////////////////////

void Mtx::initialize(double* data, int mrows, int ncols, StoragePolicy s)
{
  if (s == COPY)
	 storage_ = DataBlock::makeDataCopy(data, mrows*ncols);
  else
	 storage_ = DataBlock::makeBorrowed(data, mrows*ncols);

  mrows_ = mrows;
  ncols_ = ncols;
  start_ = storage_->itsData;

  storage_->incrRefCount();
}

const Mtx& Mtx::emptyMtx()
{
  static Mtx m(0,0);
  return m;
}

Mtx::Mtx(mxArray* a, StoragePolicy s)
{
  initialize(mxGetPr(a), mxGetM(a), mxGetN(a), s);
}

Mtx::Mtx(int mrows, int ncols) :
  storage_(DataBlock::makeBlank(mrows*ncols)),
  mrows_(mrows),
  ncols_(ncols),
  start_(storage_->itsData)
{
  storage_->incrRefCount();
}  

Mtx::Mtx(const ConstSlice& s)
{
  if (s.itsStride != 1)
	 throw ErrorWithMsg("can't initialize Mtx from Slice with stride != 1");

  initialize(s.data(), s.nelems(), 1, BORROW);
}

Mtx::~Mtx()
{
  storage_->decrRefCount();
}

mxArray* Mtx::makeMxArray() const
{
  mxArray* result_mx = mxCreateDoubleMatrix(mrows_, ncols_, mxREAL);

  const int n = nelems();

  double* matdata = mxGetPr(result_mx);

  for (int i = 0; i < n; ++i)
	 matdata[i] = start_[i];

  return result_mx;
}

void Mtx::print() const
{
  mexPrintf("mrows = %d, ncols = %d\n", mrows_, ncols_);
  for(int i = 0; i < mrows_; ++i)
	 {
		for(int j = 0; j < ncols_; ++j)
		  mexPrintf("%7.4f   ", at(i,j));
		mexPrintf("\n");
	 }
  mexPrintf("\n");
}

void Mtx::reshape(int mrows, int ncols)
{
  if (mrows*ncols != mrows_*ncols_)
	 throw ErrorWithMsg("dimension mismatch in Mtx::reshape");

  mrows_ = mrows;
  ncols_ = ncols;
}

void Mtx::leftMultAndAssign(const ConstSlice& vec, Slice& result) const
{
  // e.g mrows == vec.nelems == 3   ncols == 4
  //
  //               | e11  e12  e13  e14 |
  // [w1 w2 w3] *  | e21  e22  e23  e24 | = 
  //               | e31  e32  e33  e34 |
  //
  //
  // [ w1*e11+w2*e21+w3*e31  w1*e12+w2*e22+w3*e32  ... ]

  if ( (vec.nelems() != mrows_) ||
		 (result.nelems() != ncols_) )
	 throw ErrorWithMsg("dimension mismatch in Mtx::leftMultAndAssign");

  ConstSlice::ConstIterator veciter = vec.begin();

  for (int col = 0; col < ncols_; ++col)
	 result[col] = Slice::dot(veciter, this->colIter(col));
}

void Mtx::makeUnique()
{
  if ( !storage_->isUnique() )
	 {
		ptrdiff_t offset = start_ - storage_->itsData;
		DataBlock::makeUnique(storage_);
		start_ = storage_->itsData + offset;
	 }
}

static const char vcid_mtx_cc[] = "$Header$";
#endif // !MTX_CC_DEFINED
