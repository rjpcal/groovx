///////////////////////////////////////////////////////////////////////
//
// mtx.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:39:12 2001
// written: Mon Mar 12 19:18:54 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MTX_CC_DEFINED
#define MTX_CC_DEFINED

#include "mtx.h"

#include "error.h"

#include "libmatlb.h"

#define NO_TRACE
#include "trace.h"

///////////////////////////////////////////////////////////////////////
//
// Mtx member definitions
//
///////////////////////////////////////////////////////////////////////

void Mtx::initialize(double* data, int mrows, int ncols, StoragePolicy s)
{
  if (s == COPY)
	 block_ = DataBlock::makeDataCopy(data, mrows*ncols);
  else
	 block_ = DataBlock::makeBorrowed(data, mrows*ncols);

  mrows_ = mrows;
  ncols_ = ncols;
  start_ = block_->itsData;

  block_->incrRefCount();
}

Mtx::Mtx(mxArray* a, StoragePolicy s)
{
  initialize(mxGetPr(a), mxGetM(a), mxGetN(a), s);
}

Mtx::Mtx(int mrows, int ncols) :
  block_(DataBlock::makeBlank(mrows*ncols)),
  mrows_(mrows),
  ncols_(ncols),
  start_(block_->itsData)
{
DOTRACE("Mtx::Mtx");
  block_->incrRefCount();
}  

Mtx::~Mtx()
{
DOTRACE("Mtx::~Mtx");
  block_->decrRefCount();
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

static const char vcid_mtx_cc[] = "$Header$";
#endif // !MTX_CC_DEFINED
