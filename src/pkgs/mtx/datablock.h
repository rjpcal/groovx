///////////////////////////////////////////////////////////////////////
//
// datablock.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Mar 12 18:04:10 2001
// written: Tue Apr  1 17:56:27 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
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

protected:
  double* const itsData;          ///< Pointer to the actual storage.
  unsigned int const itsLength;   ///< Allocated length of the storage.

  /// Construct with a given data array and length.
  /** No copy is made of the data array here; the DataBlock object will
      point directly to the given array. */
  DataBlock(double* data, unsigned int len);

  /// Virtual destructor.
  virtual ~DataBlock();

  /// Class-specific operator new.
  void* operator new(size_t bytes);

  /// Class-specific operator delete.
  void operator delete(void* space);

  /// Get the current reference count.
  int refCount() const { return itsRefCount; }

public:
  /// Return a new shared DataBlock whose contents are all set to zero.
  static DataBlock* makeBlank(int length);

  /// Return a new shared DataBlock whose contents are uninitialized
  /** I.e. contents not zero-set as in makeBlank(). */
  static DataBlock* makeUninitialized(int length);

  /// Make a copy of the given data array.
  static DataBlock* makeDataCopy(const double* data, int data_length);

  /// The 'data' are borrowed, but are never considered unique.
  /** Therefore, attempts to makeUnique() will always duplicate the data. */
  static DataBlock* makeBorrowed(double* data, int data_length);

  /// The 'data' are borrowed, as in makeBorrowed(), but...
  /** Uniqueness is determined by the reference count, so it is possible to
      write to the data through the DataBlock. */
  static DataBlock* makeReferred(double* data, int data_length);

  /// Make the given DataBlock have a unique copy of its data, copying if needed.
  static void makeUnique(DataBlock*& rep);

  /// Increment the reference count.
  void incrRefCount() { ++itsRefCount; }

  /// Decrement the reference count.
  void decrRefCount() { if (--itsRefCount <= 0) delete this; }

  /// Query if the data block is unique.
  virtual bool isUnique() const = 0;


  /// Get a pointer to const data.
  const double* data()    const { return itsData; }

  /// Get a pointer to non-const data.
        double* data_nc()       { return itsData; }

  /// Get the length of the data array.
  unsigned int  length()  const { return itsLength; }
};

static const char vcid_datablock_h[] = "$Header$";
#endif // !DATABLOCK_H_DEFINED
