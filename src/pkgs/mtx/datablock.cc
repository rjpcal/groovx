///////////////////////////////////////////////////////////////////////
//
// datablock.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 18:04:40 2001
// written: Thu Mar 15 14:57:20 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DATABLOCK_CC_DEFINED
#define DATABLOCK_CC_DEFINED

#include "datablock.h"

#include <cstddef>
#include <cstdlib>
#include <cstring>

#define LOCAL_ASSERT
#include "debug.h"
#define NO_TRACE
#include "trace.h"

namespace {
  struct FreeNode {
    FreeNode* next;
  };

  FreeNode* fsFreeList = 0;
}

class SharedDataBlock : public DataBlock {
public:
  SharedDataBlock(int length) :
    DataBlock(new double[length], length)
  {}

  virtual ~SharedDataBlock() { delete [] itsData; }
};

class BorrowedDataBlock : public DataBlock {
public:
  BorrowedDataBlock(double* borrowedData, unsigned int dataLength) :
    DataBlock(borrowedData, dataLength)
  {}

  virtual ~BorrowedDataBlock()
  { /* don't delete the data, since they are 'borrowed' */ }
};

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

DataBlock::DataBlock(double* data, unsigned int len) :
  itsRefCount(0),
  itsData(data),
  itsLength(len)
{
DOTRACE("DataBlock::DataBlock");
}

DataBlock::~DataBlock() {}

DataBlock* DataBlock::getEmptyDataBlock() {
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

DataBlock* DataBlock::makeBlank(int length) {
DOTRACE("DataBlock::makeBlank");
  if (length <= 0)
    return getEmptyDataBlock();

  DataBlock* p = new SharedDataBlock(length);
  memset(p->itsData, 0, length*sizeof(double));

  return p;
}

DataBlock* DataBlock::makeBorrowed(double* data, int data_length) {
  DataBlock* p = new BorrowedDataBlock(data, data_length);
  return p;
}

void DataBlock::makeUnique(DataBlock*& rep) {
DOTRACE("DataBlock::makeUnique");
  if (rep->itsRefCount <= 1) return;

  DataBlock* rep_copy = makeDataCopy(rep->itsData, rep->itsLength);

  // do the swap

  rep->decrRefCount();
  rep_copy->incrRefCount();

  rep = rep_copy;

  Postcondition(rep->itsRefCount == 1);
}

static const char vcid_datablock_cc[] = "$Header$";
#endif // !DATABLOCK_CC_DEFINED
