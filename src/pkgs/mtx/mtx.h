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

typedef struct mxArray_tag mxArray;

class Mtx {
private:

  enum StorageType { BORROWED, OWNED };

  Mtx& operator=(const Mtx&); // not allowed

  Mtx(const Mtx& other, int column /* zero-based */) :
	 mrows_(other.mrows_),
	 ncols_(1),
	 data_(other.data_ + (other.mrows_*column)),
	 storage_(BORROWED)
  {}

public:
  Mtx(mxArray* a);

  Mtx(const Mtx& other) :
	 mrows_(other.mrows_),
	 ncols_(other.ncols_),
	 data_(0),
	 storage_(other.storage_)
  {
	 switch(storage_) {
	 case BORROWED:
		data_ = other.data_;
		break;
	 case OWNED:
		data_ = new double[nelems()];
		for (int i = 0; i < nelems(); ++i)
		  data_[i] = other.data_[i];
		break;
	 }
  }

  ~Mtx()
  {
	 switch(storage_) {
	 case OWNED:
		delete [] data_;
		break;
	 case BORROWED:
		break;
	 }
  }

  void print() const;

  int index(int row, int col) const { return row + (col*mrows_); }

  double* address(int row, int col) { return data_ + index(row, col); }

  const double* address(int row, int col) const { return data_ + index(row, col); }

  double& at(int row, int col) { return data_[index(row, col)]; }

  double at(int row, int col) const { return data_[index(row, col)]; }

  double& at(int elem) { return data_[elem]; }

  double at(int elem) const { return data_[elem]; }

  int length() const { return (mrows_ > ncols_) ? mrows_ : ncols_; }

  int nelems() const { return mrows_*ncols_; }

  int mrows() const { return mrows_; }

  int ncols() const { return ncols_; }

  double* data() { return data_; }

  const double* data() const { return data_; }

  Mtx columnSlice(int column) const { return Mtx(*this, column); }

private:
  int mrows_;
  int ncols_;
  double* data_;
  StorageType storage_;
};

static const char vcid_keanu_h[] = "$Header$";
#endif // !KEANU_H_DEFINED
