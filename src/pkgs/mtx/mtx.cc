///////////////////////////////////////////////////////////////////////
//
// mtx.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:39:12 2001
// written: Fri Apr  6 16:51:14 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MTX_CC_DEFINED
#define MTX_CC_DEFINED

#include "mtx.h"

#include "error.h"

#include "libmatlb.h"

#include "trace.h"
#include "debug.h"

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
  void raiseException(const char* msg);
}

void RC::raiseException(const char* msg)
{
  DebugPrintNL(msg);
  ErrorWithMsg err("Range check: ");
  err.appendMsg(msg);
  throw err;
}

void RC::less(const void* x, const void* lim)
{
  if (x<lim) ; // OK
  else raiseException("less: pointer range error");
}

void RC::leq(const void* x, const void* lim)
{
  if (x<=lim) ; // OK
  else raiseException("leq: pointer range error");
}

void RC::inHalfOpen(const void* x, const void* llim, const void* ulim)
{
  if (x>=llim && x<ulim) ; // OK
  else raiseException("inHalfOpen: pointer range error");
}

void RC::less(int x, int lim)
{
  if (x<lim) ; // OK
  else raiseException("less: integer range error");
}

void RC::leq(int x, int lim)
{
  if (x<=lim) ; // OK
  else raiseException("leq: integer range error");
}

void RC::inHalfOpen(int x, int llim, int ulim)
{
  if (x>=llim && x<ulim) ; // OK
  else raiseException("inHalfOpen: integer range error");
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
		mexPrintf("%7.4f   ", double(*iter));
    }
  mexPrintf("\n");
}

Slice& Slice::operator=(const Slice& other)
{
  if (itsNelems != other.nelems())
    throw ErrorWithMsg("dimension mismatch in Slice::operator=");

  MtxConstIter rhs = other.begin();

  for (MtxIter lhs = beginNC(); lhs.hasMore(); ++lhs, ++rhs)
    *lhs = *rhs;

  return *this;
}

Slice& Slice::operator=(const Mtx& other)
{
  if (itsNelems != other.nelems())
    throw ErrorWithMsg("dimension mismatch in Slice::operator=");

  int i = 0;
  for (  MtxIter lhs = beginNC(); lhs.hasMore(); ++lhs, ++i)
    *lhs = other.at(i);

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


///////////////////////////////////////////////////////////////////////
//
// MtxImpl member definitions
//
///////////////////////////////////////////////////////////////////////

Mtx::MtxImpl::MtxImpl(mxArray* a, StoragePolicy s)
{
  if (!mxIsNumeric(a))
	 throw ErrorWithMsg("cannot construct a Mtx with a non-numeric mxArray");

  init(mxGetPr(a), mxGetM(a), mxGetN(a), s);
}

void Mtx::MtxImpl::reshape(int mr, int nc)
{
  if (mr*nc != nelems())
	 {
		ErrorWithMsg err("dimension mismatch in Mtx::reshape: ");
		err.appendMsg("current nelems == ").appendNumber(nelems())
		  .appendMsg("; requested ")
		  .appendNumber(mr).appendMsg("x").appendNumber(nc);
		throw err;
	 }

  if (rowstride_ != mrows_)
    throw ErrorWithMsg("reshape not allowed for submatrix");

  mrows_ = mr;
  rowstride_ = mr;
  ncols_ = nc;
}

void Mtx::MtxImpl::selectRowRange(int r, int nr)
{
  if (r < 0 || nr < 0)
    throw ErrorWithMsg("attempted to select rows with negative indices");

  if ((r+nr) > mrows_)
    throw ErrorWithMsg("attempted to index more rows than are available");

  offset_ += r;
  mrows_ = nr;
}

void Mtx::MtxImpl::selectColumnRange(int c, int nc)
{
  if (c < 0 || nc < 0)
    throw ErrorWithMsg("attempted to select rows with negative indices");

  if ((c+nc) > ncols_)
    throw ErrorWithMsg("attempted to index more columns than are available");

  offset_ += c*rowstride_;
  ncols_ = nc;
}

void Mtx::MtxImpl::makeUnique()
{
  if ( !datablock_->isUnique() )
	 {
		DOTRACE("Mtx::MtxImpl::makeUnique");
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
  itsImpl(const_cast<double*>(s.dataStart()), s.nelems(), 1, BORROW)
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

Mtx Mtx::extractStructField(mxArray* structArray, const char* fieldName,
									 int indexIntoArray)
{
DOTRACE("Mtx::extractStructField");

  if (!structArray)
	 throw ErrorWithMsg("mxArray* was null");

  if (!mxIsStruct(structArray))
	 throw ErrorWithMsg("mxArray* is not a struct array");

  mxArray* field = mxGetField(structArray, indexIntoArray, fieldName);
  if (!field)
	 {
		ErrorWithMsg err("struct array does not contain field named '");
		err.appendMsg(fieldName).appendMsg("'");
		throw err;
	 }

  return Mtx(field, COPY);
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

void Mtx::swapColumns(int c1, int c2)
{
  if (c1 == c2) return;

  makeUnique();

  memswap(itsImpl.address(0,c1), itsImpl.address(0,c2), mrows());
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
