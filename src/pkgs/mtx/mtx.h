///////////////////////////////////////////////////////////////////////
//
// keanu.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:23:11 2001
// written: Mon Mar 12 12:27:16 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KEANU_H_DEFINED
#define KEANU_H_DEFINED

#include <cstddef>

class Slice {
  const double* data;
  int stride;
public:
  Slice(const double* d=0, int s=0) : data(d), stride(s) {}

  double operator[](int i) const { return *(data + stride*i); }

  double operator*() const { return *data; }

  void bump() { data += stride; }

  static double dot(const Slice& s1, const Slice& s2,
						  int nelems)
  {
	 double result = 0.0;
	 for (int i = 0; i < nelems; ++i)
		result += s1[i] * s2[i];
	 return result;
  }
};

class DataBlock {
private:
  DataBlock(const DataBlock& other); // not implemented
  DataBlock& operator=(const DataBlock& other); // not implemented

  // Class-specific operator new.
  void* operator new(size_t bytes);

  // Class-specific operator delete.
  void operator delete(void* space);

  DataBlock();
  ~DataBlock();

  static DataBlock* getEmptyDataBlock();

  friend class DummyFriend; // to eliminate compiler warning

public:
  static DataBlock* makeDataCopy(const double* data, int data_length);

  static DataBlock* makeBlank(int length);

  static void makeUnique(DataBlock*& rep);

  void incrRefCount() { ++itsRefCount; }
  void decrRefCount() { if (--itsRefCount <= 0) delete this; }

private:
  int itsRefCount;

public:
  double* itsData;
  unsigned int itsLength;
};


typedef struct mxArray_tag mxArray;

class Mtx {
private:

  Mtx& operator=(const Mtx&); // not allowed

  Mtx(const Mtx& other, int column /* zero-based */) :
	 block_(other.block_),
	 mrows_(other.mrows_),
	 ncols_(1),
	 start_(other.start_ + (other.mrows_*column))
  {
	 block_->incrRefCount();
  }

public:
  Mtx(mxArray* a);

  Mtx(int mrows, int ncols);

  Mtx(const Mtx& other) :
	 block_(other.block_),
	 mrows_(other.mrows_),
	 ncols_(other.ncols_),
	 start_(other.start_)
  {
	 block_->incrRefCount();
  }

  ~Mtx();

  mxArray* makeMxArray() const;

  void print() const;

  int index(int row, int col) const { return row + (col*mrows_); }

  double* address(int row, int col) { return start_ + index(row, col); }

  const double* address(int row, int col) const { return start_ + index(row, col); }

  double& at(int row, int col) { return start_[index(row, col)]; }

  double at(int row, int col) const { return start_[index(row, col)]; }

  double& at(int elem) { return start_[elem]; }

  double at(int elem) const { return start_[elem]; }

  int length() const { return (mrows_ > ncols_) ? mrows_ : ncols_; }

  int nelems() const { return mrows_*ncols_; }

  int mrows() const { return mrows_; }

  int ncols() const { return ncols_; }

  double* data() { return start_; }

  const double* data() const { return start_; }

  Mtx columnSlice(int column) const { return Mtx(*this, column); }

private:

  DataBlock* block_;
  int mrows_;
  int ncols_;
  double* start_;
};

static const char vcid_keanu_h[] = "$Header$";
#endif // !KEANU_H_DEFINED
