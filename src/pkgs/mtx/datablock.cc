///////////////////////////////////////////////////////////////////////
//
// datablock.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 18:04:40 2001
// written: Mon Jan 13 11:01:38 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DATABLOCK_CC_DEFINED
#define DATABLOCK_CC_DEFINED

#include "datablock.h"

#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "util/debug.h"
#include "util/trace.h"

namespace
{
  struct FreeNode
  {
    FreeNode* next;
  };

  FreeNode* fsFreeList = 0;
}

class SharedDataBlock : public DataBlock
{
public:
  SharedDataBlock(int length);
  virtual ~SharedDataBlock();

  virtual bool isUnique() const { return refCount() <= 1; }
};

SharedDataBlock::SharedDataBlock(int length) :
  DataBlock(new double[length], length)
{
  DOTRACE("SharedDataBlock::SharedDataBlock");
  dbgEval(3, this); dbgEvalNL(3, itsData);
}

SharedDataBlock::~SharedDataBlock()
{
  DOTRACE("SharedDataBlock::~SharedDataBlock");
  dbgEval(3, this); dbgEvalNL(3, itsData);
  delete [] itsData;
}

class BorrowedDataBlock : public DataBlock
{
public:
  BorrowedDataBlock(double* borrowedData, unsigned int dataLength) :
    DataBlock(borrowedData, dataLength)
  {}

  virtual ~BorrowedDataBlock()
  { /* don't delete the data, since they are 'borrowed' */ }

  // Since the data are borrowed, we always return false here.
  virtual bool isUnique() const { return false; }
};

class ReferredDataBlock : public DataBlock
{
public:
  ReferredDataBlock(double* borrowedData, unsigned int dataLength) :
    DataBlock(borrowedData, dataLength)
  {}

  virtual ~ReferredDataBlock()
  { /* don't delete the data, since they are 'borrowed' */ }

  virtual bool isUnique() const { return refCount() <= 1; }
};

void* DataBlock::operator new(size_t bytes)
{
DOTRACE("DataBlock::operator new");

  Assert(bytes == sizeof(DataBlock));
  if (fsFreeList == 0)
    return ::operator new(bytes);
  FreeNode* node = fsFreeList;
  fsFreeList = fsFreeList->next;
  return (void*)node;
}

void DataBlock::operator delete(void* space)
{
DOTRACE("DataBlock::operator delete");

  ((FreeNode*)space)->next = fsFreeList;
  fsFreeList = (FreeNode*)space;
}

DataBlock::DataBlock(double* data, unsigned int len) :
  itsRefCount(0),
  itsData(data),
  itsLength(len)
{
DOTRACE("DataBlock::DataBlock");
}

DataBlock::~DataBlock()
{
DOTRACE("DataBlock::~DataBlock");
}

DataBlock* DataBlock::getEmptyDataBlock()
{
DOTRACE("DataBlock::getEmptyDataBlock");
  static DataBlock* empty_rep = 0;
  if (empty_rep == 0)
    {
      empty_rep = new SharedDataBlock(0);
      empty_rep->incrRefCount();
    }

  return empty_rep;
}

DataBlock* DataBlock::makeDataCopy(const double* data, int data_length)
{
DOTRACE("DataBlock::makeDataCopy");
  if (data == 0)
    return getEmptyDataBlock();

  DataBlock* p = new SharedDataBlock(data_length);

  memcpy(p->itsData, data, data_length*sizeof(double));

  return p;
}

DataBlock* DataBlock::makeBlank(int length)
{
DOTRACE("DataBlock::makeBlank");
  if (length <= 0)
    return getEmptyDataBlock();

  DataBlock* p = new SharedDataBlock(length);
  memset(p->itsData, 0, length*sizeof(double));

  return p;
}

DataBlock* DataBlock::makeUninitialized(int length)
{
DOTRACE("DataBlock::makeUninitialized");
  if (length <= 0)
    return getEmptyDataBlock();

  return new SharedDataBlock(length);
}

DataBlock* DataBlock::makeBorrowed(double* data, int data_length)
{
DOTRACE("DataBlock::makeBorrowed");
  return new BorrowedDataBlock(data, data_length);
}

DataBlock* DataBlock::makeReferred(double* data, int data_length)
{
DOTRACE("DataBlock::makeReferred");
  return new ReferredDataBlock(data, data_length);
}

void DataBlock::makeUnique(DataBlock*& rep)
{
  if (rep->isUnique()) return;

  {
    DOTRACE("DataBlock::makeUnique");

    DataBlock* rep_copy = makeDataCopy(rep->itsData, rep->itsLength);

    // do the swap

    rep->decrRefCount();
    rep_copy->incrRefCount();

    rep = rep_copy;

    Postcondition(rep->itsRefCount == 1);
  }
}

static const char vcid_datablock_cc[] = "$Header$";
#endif // !DATABLOCK_CC_DEFINED
