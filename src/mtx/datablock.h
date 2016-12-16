/** @file pkgs/mtx/datablock.h auxiliary storage-management class for
    mtx */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Mar 12 18:04:10 2001
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_PKGS_MTX_DATABLOCK_H_UTC20050626084022_DEFINED
#define GROOVX_PKGS_MTX_DATABLOCK_H_UTC20050626084022_DEFINED

#include <cstddef>


//  #######################################################
//  =======================================================
//  initialization type tags:

struct init_policy_zeros {};
struct init_policy_no_init {};
struct storage_policy_copy {};
struct storage_policy_borrow {};
struct storage_policy_refer {};

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
  size_t const m_length;          ///< Allocated length of the storage.

  /// Construct with a given data array and length.
  /** No copy is made of the data array here; the data_block object will
      point directly to the given array. */
  data_block(double* data, size_t len);

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
  static data_block* make_zeros(size_t length);

  /// Return a new shared data_block whose contents are uninitialized
  /** I.e. contents not zero-set as in make_zeros(). */
  static data_block* make_uninitialized(size_t length);

  /// Make a copy of the given data array.
  static data_block* make_data_copy(const double* data, size_t data_length);

  /// The 'data' are borrowed, but are never considered unique.
  /** Therefore, attempts to make_unique() will always duplicate the
      data. */
  static data_block* make_borrowed(double* data, size_t data_length);

  /// The 'data' are borrowed, as in make_borrowed(), but...
  /** Uniqueness is determined by the reference count, so it is
      possible to write to the data through the data_block. */
  static data_block* make_referred(double* data, size_t data_length);

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
        size_t  length()  const { return m_length; }
};


//  #######################################################
//  =======================================================
/// data_holder enforces the ref-counting semantics of data_block

class data_holder
{
public:
  /// Construct with copy of a data array, dimensions.
  data_holder(const double* data, size_t mrows, size_t ncols, storage_policy_copy);

  /// Construct with borrow of a data array, dimensions.
  data_holder(double* data, size_t mrows, size_t ncols, storage_policy_borrow);

  /// Construct with reference to a data array, dimensions.
  data_holder(double* data, size_t mrows, size_t ncols, storage_policy_refer);

  /// Construct full of zeros with given dimensions.
  data_holder(size_t mrows, size_t ncols, init_policy_zeros);

  /// Construct with unintialized values with given dimensions.
  data_holder(size_t mrows, size_t ncols, init_policy_no_init);

  /// Generic construction from a data_block.
  data_holder(data_block* d);

  /// Copy constructor.
  data_holder(const data_holder& other);

  /// Move constructor.
  data_holder(data_holder&& other) noexcept;

  /// Destructor.
  ~data_holder();

  /// Swap contents with another data_holder.
  void swap(data_holder& other);

  /// Make a unique copy of our data block if needed.
  void make_unique() { data_block::make_unique(m_data); }

  /// Get a pointer to const underlying data.
  const double* storage() const { return m_data->data(); }

  /// Get a pointer to non-const underlying data.
  double* storage_nc() { make_unique(); return m_data->data_nc(); }

  /// Get the allocated length of underlying data array.
  size_t storage_length() const { return m_data->length(); }

private:
  data_holder& operator=(const data_holder&); // not allowed

  data_block* m_data;
};


//  #######################################################
//  =======================================================
/// data_ref_holder acts like a reference to a data_holder

class data_ref_holder
{
public:
  /// Construct with a pointee.
  data_ref_holder(data_holder* ref) : m_ref(ref) {}

  /// Copy constructor.
  data_ref_holder(const data_ref_holder& other) : m_ref(other.m_ref) {}

  /// Assignment not allowed
  data_ref_holder& operator=(const data_ref_holder&) = delete;

  /// Destructor.
  ~data_ref_holder() {}

  /// Swap contents with another data_ref_holder.
  void swap(data_ref_holder& other);

  /// Get a pointer to const underlying data.
  const double* storage() const { return m_ref->storage(); }

  /// Get a pointer to non-const underlying data.
  double* storage_nc() { return m_ref->storage_nc(); }

  /// Get the allocated length of underlying data array.
  size_t storage_length() const { return m_ref->storage_length(); }

private:

  data_holder* m_ref;
};


#endif // !GROOVX_PKGS_MTX_DATABLOCK_H_UTC20050626084022_DEFINED
