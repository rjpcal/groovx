///////////////////////////////////////////////////////////////////////
//
// datablock.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 18:04:10 2001
// written: Mon Jan 13 11:01:38 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DATABLOCK_H_DEFINED
#define DATABLOCK_H_DEFINED

#include <cstddef>

/// Base class for holding ref-counted arrays of floating-point data.
/** Serves as the implementation for higher-level matrix classes, etc. */
class DataBlock
{
private:
  int itsRefCount;


  DataBlock(const DataBlock& other); // not implemented
  DataBlock& operator=(const DataBlock& other); // not implemented

  static DataBlock* getEmptyDataBlock();

  friend class DummyFriend; // to eliminate compiler warning


protected:
  double* const itsData;
  unsigned int const itsLength;

  DataBlock(double* data, unsigned int len);
  virtual ~DataBlock();

  // Class-specific operator new.
  void* operator new(size_t bytes);

  // Class-specific operator delete.
  void operator delete(void* space);

  int refCount() const { return itsRefCount; }

public:
  /** Return a new shared DataBlock whose contents are all set to zero */
  static DataBlock* makeBlank(int length);

  /** Return a new shared DataBlock whose contents are uninitialized (i.e. not
      all set to zero as in makeBlank()) */
  static DataBlock* makeUninitialized(int length);

  static DataBlock* makeDataCopy(const double* data, int data_length);

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


  const double* data()    const { return itsData; }
        double* data_nc()       { return itsData; }

  unsigned int  length()  const { return itsLength; }
};

static const char vcid_datablock_h[] = "$Header$";
#endif // !DATABLOCK_H_DEFINED
