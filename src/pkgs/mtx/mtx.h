///////////////////////////////////////////////////////////////////////
//
// keanu.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:23:11 2001
// written: Wed Mar 14 17:15:35 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KEANU_H_DEFINED
#define KEANU_H_DEFINED

#include "datablock.h"
#include "num.h"

class Mtx;



///////////////////////////////////////////////////////////////////////
//
// ElemProxy class definition
//
///////////////////////////////////////////////////////////////////////


class ElemProxy {
  Mtx& m;
  int i;

  double& uniqueRef();

public:
  ElemProxy(Mtx& m_, int i_) : m(m_), i(i_) {}

  double& operator=(double d);
  double& operator+=(double d);
  double& operator-=(double d);
  double& operator*=(double d);
  double& operator/=(double d);

  operator double();
};


///////////////////////////////////////////////////////////////////////
//
// ConstSlice class definition
//
// We need separate classes for const slice references and non-const
// slice references. This is because the non-const should not
// increment the ref count of the underlying storage, allowing writes
// to pass through to the originating matrix.
//
///////////////////////////////////////////////////////////////////////


class ConstSlice {
private:
  ConstSlice& operator=(const ConstSlice& other); // not implemented

protected:
  DataBlock*& itsStorage;
  ptrdiff_t itsOffset;
  int itsStride;
  int itsNelems;

  const double* data() const;

  const double* address(int i) const { return data() + itsStride*i; }

  friend class Mtx;

  ConstSlice(DataBlock* const& storage, const double* d, int s, int n);

public:

  // Forms a dummy empty slice
  ConstSlice();

  ConstSlice(const ConstSlice& other) :
	 itsStorage(other.itsStorage),
	 itsOffset(other.itsOffset),
	 itsStride(other.itsStride),
	 itsNelems(other.itsNelems)
  {}

  //
  // Data access
  //

  double operator[](int i) const { return *(address(i)); }

  int nelems() const { return itsNelems; }

  ConstSlice rightmost(int n) const
  {
	 int first = itsNelems - n;
	 if (first < 0) first = 0;

	 return ConstSlice(itsStorage, address(first), itsStride, n);
  }

  ConstSlice leftmost(int n) const
  {
	 return ConstSlice(itsStorage, data(), itsStride, n);
  }

  //
  // Iteration
  //

  class ConstIterator {
	 const double* data;
	 const double* stop;
	 int stride;

	 ConstIterator(const double* d, int s, int n) :
		data(d), stride(s), stop(d + s*n) {}

	 friend class ConstSlice;
	 friend class Mtx;

  public:

	 double operator*() const { return *data; }

	 ConstIterator& operator++() { data += stride; return *this; }

	 bool hasMore() const { return data < stop; }
  };

  ConstIterator begin() const
    { return ConstIterator(data(), itsStride, itsNelems); }


  //
  // Functions
  //

  double sum() const
  {
	 double s = 0.0;
	 for (ConstIterator i = begin(); i.hasMore(); ++i)
		s += *i;
	 return s;
  }

  double mean() const
    { return sum()/itsNelems; }

  static double dot(ConstSlice::ConstIterator s1, ConstSlice::ConstIterator s2)
  {
	 double result = 0.0;

	 for (; s1.hasMore(); ++s1, ++s2)
		result += (*s1) * (*s2);

	 return result;
  }
};


///////////////////////////////////////////////////////////////////////
//
// Slice class definition
//
///////////////////////////////////////////////////////////////////////


class Slice : public ConstSlice {
  friend class Mtx;

  double* data();
  double* address(int i) { return data() + itsStride*i; }

  Slice(DataBlock*& m, double* d, int s, int n) :
	 ConstSlice(m,d,s,n) {}

public:

  double& operator[](int i) { return *(address(i)); }

  Slice rightmost(int n)
  {
	 int first = itsNelems - n;
	 if (first < 0) first = 0;

	 return Slice(itsStorage, address(first), itsStride, n);
  }

  Slice leftmost(int n)
  {
	 return Slice(itsStorage, data(), itsStride, n);
  }

  Slice& operator=(const ConstSlice& other);

  //
  // Functions
  //

  template <class F>
  void apply(F func)
  {
	 for (Iterator i = begin(); i.hasMore(); ++i)
		*i = func(*i);
  }

  void operator/=(double div) { apply(Div(div)); }

  //
  // Iteration
  //

  class Iterator {
	 double* data;
	 int stride;
	 double* stop;

	 Iterator(double* d, int s, int n) : data(d), stride(s), stop(d + s*n) {}

	 friend class Slice;
	 friend class Mtx;

  public:

	 double& operator*() { return *data; }

	 Iterator& operator++() { data += stride; return *this; }

	 bool hasMore() const { return data < stop; }
  };

  Iterator begin()
    { return Iterator(data(), itsStride, itsNelems); }
};




///////////////////////////////////////////////////////////////////////
//
// Mtx class definition
//
///////////////////////////////////////////////////////////////////////


typedef struct mxArray_tag mxArray;

class Mtx {
public:
  enum StoragePolicy { COPY, BORROW };

private:
  template <class T>
  static void doswap(T& t1, T& t2)
  { T t2_ = t2; t2 = t1; t1 = t2_; }


  void swap(Mtx& other)
  {
	 doswap(storage_, other.storage_);
	 doswap(mrows_, other.mrows_);
	 doswap(rowstride_, other.rowstride_);
	 doswap(ncols_, other.ncols_);
	 doswap(start_, other.start_);
  }

  // Sets up an appropriate DataBlock and increments its reference count
  void initialize(double* data, int mrows, int ncols, StoragePolicy s = COPY);

public:

  //
  // Constructors
  //

  static const Mtx& emptyMtx();

  Mtx(mxArray* a, StoragePolicy s = COPY);

  Mtx(double* data, int mrows, int ncols, StoragePolicy s = COPY)
    { initialize(data, mrows, ncols, s); }

  Mtx(int mrows, int ncols);

  Mtx(const ConstSlice& s);

  Mtx(const Mtx& other) :
	 storage_(other.storage_),
	 mrows_(other.mrows_),
	 rowstride_(other.rowstride_),
	 ncols_(other.ncols_),
	 start_(other.start_)
  {
	 storage_->incrRefCount();
  }

  ~Mtx();

  Mtx& operator=(const Mtx& other)
  {
	 Mtx temp(other);
	 this->swap(temp);
  }

  mxArray* makeMxArray() const;


  //
  // I/O
  //

  void print() const;


  //
  // Data access
  //

  friend class ElemProxy;

  ElemProxy at(int row, int col) { return ElemProxy(*this, offsetFromStart(row, col)); }

  double at(int row, int col) const { return start_[offsetFromStart(row, col)]; }

  ElemProxy at(int elem) { return ElemProxy(*this, offsetFromStart(elem)); }

  double at(int elem) const { return start_[offsetFromStart(elem)]; }

  void reshape(int mrows, int ncols);

  int length() const { return (mrows_ > ncols_) ? mrows_ : ncols_; }

  int nelems() const { return mrows_*ncols_; }

  int mrows() const { return mrows_; }

  int ncols() const { return ncols_; }

  //
  // Slices, submatrices
  //

  Slice row(int r)
    { return Slice(storage_, address(r,0), rowstride_, ncols_); }

  ConstSlice row(int r) const
    { return ConstSlice(storage_, address(r,0), rowstride_, ncols_); }

  Slice::Iterator rowIter(int r)
    { return Slice::Iterator(address(r,0), rowstride_, ncols_); }

  ConstSlice::ConstIterator rowIter(int r) const
    { return ConstSlice::ConstIterator(address(r,0), rowstride_, ncols_); }


  Mtx rows(int r, int nr) const;


  Slice column(int c)
    { return Slice(storage_, address(0,c), colstride_, mrows_); }

  ConstSlice column(int c) const
    { return ConstSlice(storage_, address(0,c), colstride_, mrows_); }

  Slice::Iterator colIter(int c)
    { return Slice::Iterator(address(0,c), colstride_, mrows_); }

  ConstSlice::ConstIterator colIter(int c) const
    { return ConstSlice::ConstIterator(address(0,c), colstride_, mrows_); }



  //
  // Functions
  //

  void apply(double func(double))
    {
		double* p = start_;
		int gap = mrows_ - rowstride_;

		if (gap == 0)
		  {
			 double* end = p + nelems();
			 for (; p < end; ++p)
				*p = func(*p);
		  }
		else
		  {
			 for (int c = 0; c < ncols_; ++c)
				{
				  for (int r = 0; r < mrows_; ++r)
					 {
						*p = func(*p);
						++p;
					 }
				  p += gap;
				}
		  }
    }

  // result = vec * mtx;
  static void VMmul_assign(const ConstSlice& vec, const Mtx& mtx,
									Slice& result);

  // this = m1 * m2;
  void assign_MMmul(const Mtx& m1, const Mtx& m2);

private:
  void makeUnique();

  ptrdiff_t offsetFromStorage(int row, int col) const
    { return (start_ - storage_->itsData) + offsetFromStart(row, col); }

  int offsetFromStart(int row, int col) const
    { return row + (col*rowstride_); }

  int offsetFromStart(int elem) const
  {
	 return ( elem/mrows() /* == # of columns */
				 * rowstride_ )
		+ elem%mrows(); /* == # of rows */
  }

  double* address(int row, int col)
    { return start_ + offsetFromStart(row, col); }

  const double* address(int row, int col) const
    { return start_ + offsetFromStart(row, col); }

  friend class ConstSlice;
  friend class Slice;

  DataBlock* storage_;
  int mrows_;
  int rowstride_;
  int ncols_;
  static const int colstride_ = 1;
  double* start_;
};

///////////////////////////////////////////////////////////////////////
//
// Inline member function definitions
//
///////////////////////////////////////////////////////////////////////


inline double& ElemProxy::uniqueRef()
{ m.makeUnique(); return m.start_[i]; }

inline double& ElemProxy::operator=(double d)
{ return (uniqueRef() = d); }

inline double& ElemProxy::operator+=(double d)
{ return (uniqueRef() += d); }

inline double& ElemProxy::operator-=(double d)
{ return (uniqueRef() -= d); }

inline double& ElemProxy::operator*=(double d)
{ return (uniqueRef() *= d); }

inline double& ElemProxy::operator/=(double d)
{ return (uniqueRef() /= d); }

inline ElemProxy::operator double() { return m.start_[i]; }


inline const double* ConstSlice::data() const
{ return itsStorage->itsData + itsOffset; }

inline ConstSlice::ConstSlice(DataBlock* const& storage,
										const double* d, int s, int n) :
  itsStorage(const_cast<DataBlock*&>(storage)),
  itsOffset(d - storage->itsData),
  itsStride(s),
  itsNelems(n) {}

inline double* Slice::data() { return itsStorage->itsData + itsOffset; }

static const char vcid_keanu_h[] = "$Header$";
#endif // !KEANU_H_DEFINED
