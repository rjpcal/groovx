///////////////////////////////////////////////////////////////////////
//
// datablock.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 18:04:10 2001
// written: Fri Apr  6 10:27:20 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DATABLOCK_H_DEFINED
#define DATABLOCK_H_DEFINED

#include <cstddef>

class DataBlock {
private:
  DataBlock(const DataBlock& other); // not implemented
  DataBlock& operator=(const DataBlock& other); // not implemented

  static DataBlock* getEmptyDataBlock();

  friend class DummyFriend; // to eliminate compiler warning

protected:
  DataBlock(double* data, unsigned int len);
  virtual ~DataBlock();

  // Class-specific operator new.
  void* operator new(size_t bytes);

  // Class-specific operator delete.
  void operator delete(void* space);

public:
  static DataBlock* makeDataCopy(const double* data, int data_length);
  static DataBlock* makeBlank(int length);

  // The 'data' are borrowed, but are never considered unique, so
  // attempts to makeUnique() will always duplicate the data.
  static DataBlock* makeBorrowed(double* data, int data_length);

  // The 'data' are borrowed, as in makeBorrowed(), except that the
  // uniqueness is determined by the reference count, so it is
  // possible to write to the data through the DataBlock.
  static DataBlock* makeReferred(double* data, int data_length);

  static void makeUnique(DataBlock*& rep);

  void incrRefCount() { ++itsRefCount; }
  void decrRefCount() { if (--itsRefCount <= 0) delete this; }

  virtual bool isUnique() const = 0;

protected:
  int refCount() const { return itsRefCount; }

private:
  int itsRefCount;

public:
  double* itsData;
  unsigned int itsLength;
};

static const char vcid_datablock_h[] = "$Header$";
#endif // !DATABLOCK_H_DEFINED
