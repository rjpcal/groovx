///////////////////////////////////////////////////////////////////////
//
// mtx.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:39:12 2001
// written: Mon Mar 12 14:00:37 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MTX_CC_DEFINED
#define MTX_CC_DEFINED

#include "mtx.h"

#define LOCAL_ASSERT
#include "debug.h"

#define NO_TRACE
#include "trace.h"

#include <cstdlib>

#include "libmatlb.h"

namespace {
  struct FreeNode {
	 FreeNode* next;
  };

  FreeNode* fsFreeList = 0;
}

void* DataBlock::operator new(size_t bytes) {
DOTRACE("DataBlock::operator new");

  Assert(bytes == sizeof(DataBlock));
  if (fsFreeList == 0)
	 return ::operator new(bytes);
  FreeNode* node = fsFreeList;
  fsFreeList = fsFreeList->next;
  return (void*)node;
}

void DataBlock::operator delete(void* space) {
DOTRACE("DataBlock::operator delete");

  ((FreeNode*)space)->next = fsFreeList;
  fsFreeList = (FreeNode*)space;
}

DataBlock::DataBlock() {
DOTRACE("DataBlock::DataBlock");
}

DataBlock::~DataBlock() {
DOTRACE("DataBlock::~DataBlock");
  delete [] itsData;
}

DataBlock* DataBlock::getEmptyDataBlock() {
DOTRACE("DataBlock::getEmptyDataBlock");
  static DataBlock* empty_rep = 0;
  if (empty_rep == 0)
	 {
		empty_rep = new DataBlock;

		empty_rep->itsRefCount = 1; 
		empty_rep->itsLength = 0;
		empty_rep->itsData = new double[1];
	 }

  return empty_rep;
}

DataBlock* DataBlock::makeDataCopy(const double* data, int data_length)
{
DOTRACE("DataBlock::makeDataCopy");
  if (data == 0)
	 return getEmptyDataBlock();

  DataBlock* p = new DataBlock;

  p->itsRefCount = 0; 
  p->itsLength = data_length;
  p->itsData = new double[data_length];

  memcpy(p->itsData, data, data_length*sizeof(double));

  return p;
}

DataBlock* DataBlock::makeBlank(int length) {
DOTRACE("DataBlock::makeBlank");
  if (length <= 0)
	 return getEmptyDataBlock();

  DataBlock* p = new DataBlock;

  p->itsRefCount = 0;
  p->itsLength = length;
  p->itsData = new double[p->itsLength];

  return p;
}

void DataBlock::makeUnique(DataBlock*& rep) {
DOTRACE("DataBlock::makeUnique");
  if (rep->itsRefCount <= 1) return;

  DataBlock* new_rep = new DataBlock;

  new_rep->itsRefCount = 0;
  new_rep->itsLength = rep->itsLength;
  new_rep->itsData = new double[new_rep->itsLength];

  memcpy(new_rep->itsData, rep->itsData, sizeof(double)*new_rep->itsLength);

  rep->decrRefCount();
  new_rep->incrRefCount();

  rep = new_rep;

  Postcondition(new_rep->itsRefCount == 1);
}


///////////////////////////////////////////////////////////////////////
//
// Mtx member definitions
//
///////////////////////////////////////////////////////////////////////

Mtx::Mtx(mxArray* a) :
  block_(DataBlock::makeDataCopy(mxGetPr(a), mxGetM(a)*mxGetN(a))),
  mrows_(mxGetM(a)),
  ncols_(mxGetN(a)),
  start_(block_->itsData)
{
DOTRACE("Mtx::Mtx");
  block_->incrRefCount();
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

static const char vcid_mtx_cc[] = "$Header$";
#endif // !MTX_CC_DEFINED
