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

#include "datablock.h"

class Mtx;

class ConstSlice {
private:
  ConstSlice& operator=(const ConstSlice& other); // not implemented

protected:
  double* itsData;
  int itsStride;
  int itsNelems;

  const double* address(int i) const { return itsData + itsStride*i; }

  friend class Mtx;

public:
  ConstSlice(const double* d, int s, int n) :
	 itsData(const_cast<double*>(d)), itsStride(s), itsNelems(n) {}

  ConstSlice() : itsData(0), itsStride(0), itsNelems(0) {}

  ConstSlice(const ConstSlice& other) :
	 itsData(other.itsData),
	 itsStride(other.itsStride),
	 itsNelems(other.itsNelems)
  {}

  void rebind(const ConstSlice& other)
  {
	 itsData = other.itsData;
	 itsStride = other.itsStride;
	 itsNelems = other.itsNelems;
  }

  double operator[](int i) const { return *(address(i)); }

  const double* data() const { return itsData; }

  int nelems() const { return itsNelems; }

  ConstSlice rightmost(int n) const
  {
	 int first = itsNelems - n;
	 if (first < 0) first = 0;

	 return ConstSlice(address(first), itsStride, n);
  }

  ConstSlice leftmost(int n) const
  {
	 return ConstSlice(itsData, itsStride, n);
  }

  static double dot(const ConstSlice& s1, const ConstSlice& s2)
  {
	 double result = 0.0;
	 const int n = s1.itsNelems > s2.itsNelems ? s1.itsNelems : s2.itsNelems;
	 for (int i = 0; i < n; ++i)
		result += s1[i] * s2[i];
	 return result;
  }

  class ConstIterator {
	 const double* data;
	 int stride;

	 ConstIterator(const double* d, int s) : data(d), stride(s) {}

	 friend class ConstSlice;

  public:

	 double operator*() const { return *data; }

	 ConstIterator& operator++() { data += stride; return *this; }

	 bool operator==(const ConstIterator& other) const
	   { return data == other.data; }

	 bool operator!=(const ConstIterator& other) const
	   { return data != other.data; }
  };

  ConstIterator begin() const
    { return ConstIterator(itsData, itsStride); }
  ConstIterator end() const
    { return ConstIterator(address(itsNelems), itsStride); }
};

class Slice : public ConstSlice {
  friend class Mtx;

  double* address(int i) { return itsData + itsStride*i; }

public:
  Slice(double* d, int s, int n) :
	 ConstSlice(d,s,n) {}

  Slice() : ConstSlice() {}

  double& operator[](int i) { return *(address(i)); }

  Slice& operator=(const Slice& other);

  Slice rightmost(int n)
  {
	 int first = itsNelems - n;
	 if (first < 0) first = 0;

	 return Slice(address(first), itsStride, n);
  }

  Slice leftmost(int n)
  {
	 return Slice(itsData, itsStride, n);
  }
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
  enum StoragePolicy { COPY, BORROW };

private:
  // Sets up an appropriate DataBlock and increments its reference count
  void initialize(double* data, int mrows, int ncols, StoragePolicy s = COPY);

public:
  Mtx(mxArray* a, StoragePolicy s = COPY);

  Mtx(double* data, int mrows, int ncols, StoragePolicy s = COPY)
  { initialize(data, mrows, ncols, s); }

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

  double& at(int row, int col) { return start_[index(row, col)]; }

  double at(int row, int col) const { return start_[index(row, col)]; }

  double& at(int elem) { return start_[elem]; }

  double at(int elem) const { return start_[elem]; }

  int length() const { return (mrows_ > ncols_) ? mrows_ : ncols_; }

  int nelems() const { return mrows_*ncols_; }

  int mrows() const { return mrows_; }

  int ncols() const { return ncols_; }

  Mtx columnSlice(int column) const { return Mtx(*this, column); }

  Slice rowSlice(int row)
    { return Slice(address(row,0), mrows_, ncols_); }

  ConstSlice rowSlice(int row) const
    { return ConstSlice(address(row,0), mrows_, ncols_); }

  Slice asSlice()
    { return Slice(start_, 1, nelems()); }

private:
  int index(int row, int col) const { return row + (col*mrows_); }
  double* address(int row, int col) { return start_ + index(row, col); }
  const double* address(int row, int col) const { return start_ + index(row, col); }

  DataBlock* block_;
  int mrows_;
  int ncols_;
  double* start_;
};


static const char vcid_keanu_h[] = "$Header$";
#endif // !KEANU_H_DEFINED
