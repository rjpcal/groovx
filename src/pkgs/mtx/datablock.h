///////////////////////////////////////////////////////////////////////
//
// datablock.h
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Mar 12 18:04:10 2001
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef DATABLOCK_H_DEFINED
#define DATABLOCK_H_DEFINED

#include <cstddef>

/// Base class for holding ref-counted arrays of floating-point data.
/** Serves as the implementation for higher-level matrix classes, etc. */
class data_block
{
private:
  int m_refcount;

  data_block(const data_block& other); // not implemented
  data_block& operator=(const data_block& other); // not implemented

  static data_block* get_empty_data_block();

protected:
  double* const m_storage;        ///< Pointer to the actual storage.
  unsigned int const m_length;    ///< Allocated length of the storage.

  /// Construct with a given data array and length.
  /** No copy is made of the data array here; the data_block object will
      point directly to the given array. */
  data_block(double* data, unsigned int len);

  /// Virtual destructor.
  virtual ~data_block();

  /// Class-specific operator new.
  void* operator new(size_t bytes);

  /// Class-specific operator delete.
  void operator delete(void* space);

  /// Get the current reference count.
  int refcount() const { return m_refcount; }

public:
  /// Return a new shared data_block whose contents are all set to zero.
  static data_block* make_zeros(int length);

  /// Return a new shared data_block whose contents are uninitialized
  /** I.e. contents not zero-set as in make_zeros(). */
  static data_block* make_uninitialized(int length);

  /// Make a copy of the given data array.
  static data_block* make_data_copy(const double* data, int data_length);

  /// The 'data' are borrowed, but are never considered unique.
  /** Therefore, attempts to make_unique() will always duplicate the
      data. */
  static data_block* make_borrowed(double* data, int data_length);

  /// The 'data' are borrowed, as in make_borrowed(), but...
  /** Uniqueness is determined by the reference count, so it is
      possible to write to the data through the data_block. */
  static data_block* make_referred(double* data, int data_length);

  /// Make the given data_block have a unique copy of its data, copying if needed.
  static void make_unique(data_block*& rep);

  /// Increment the reference count.
  void incr_refcount() { ++m_refcount; }

  /// Decrement the reference count.
  void decr_refcount() { if (--m_refcount <= 0) delete this; }

  /// Query if the data block is unique.
  virtual bool is_unique() const = 0;


  /// Get a pointer to const data.
  const double* data()    const { return m_storage; }

  /// Get a pointer to non-const data.
        double* data_nc()       { return m_storage; }

  /// Get the length of the data array.
  unsigned int  length()  const { return m_length; }
};

static const char vcid_datablock_h[] = "$Header$";
#endif // !DATABLOCK_H_DEFINED
