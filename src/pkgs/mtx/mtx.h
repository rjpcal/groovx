///////////////////////////////////////////////////////////////////////
//
// keanu.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:23:11 2001
// written: Wed Mar 14 18:54:37 2001
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
// Mtx iterators
//
///////////////////////////////////////////////////////////////////////

class MtxConstIter {
  const double* data;
  const double* stop;
  int stride;

  MtxConstIter(const double* d, int s, int n) :
	 data(d), stride(s), stop(d + s*n) {}

  friend class ConstSlice;
  friend class Mtx;

public:

  double operator*() const { return *data; }

  MtxConstIter& operator++() { data += stride; return *this; }

  bool hasMore() const { return data < stop; }
};


class MtxIter {
  double* data;
  int stride;
  double* stop;

  MtxIter(double* d, int s, int n) : data(d), stride(s), stop(d + s*n) {}

  friend class Slice;
  friend class Mtx;

public:

  double& operator*() { return *data; }

  MtxIter& operator++() { data += stride; return *this; }

  bool hasMore() const { return data < stop; }
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

  MtxConstIter begin() const
    { return MtxConstIter(data(), itsStride, itsNelems); }


  //
  // Functions
  //

  double sum() const
  {
	 double s = 0.0;
	 for (MtxConstIter i = begin(); i.hasMore(); ++i)
		s += *i;
	 return s;
  }

  double mean() const
    { return sum()/itsNelems; }
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
	 for (MtxIter i = begin(); i.hasMore(); ++i)
		*i = func(*i);
  }

  void operator/=(double div) { apply(Div(div)); }

  //
  // Iteration
  //

  MtxIter begin()
    { return MtxIter(data(), itsStride, itsNelems); }
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

public:

  //
  // Constructors
  //

  static const Mtx& emptyMtx();

  Mtx(mxArray* a, StoragePolicy s = COPY) : itsImpl(a, s) {}

  Mtx(double* data, int mrows, int ncols, StoragePolicy s = COPY) :
	 itsImpl(data, mrows, ncols, s) {}

  Mtx(int mrows, int ncols) : itsImpl(mrows, ncols) {}

  Mtx(const ConstSlice& s);

  Mtx(const Mtx& other) : itsImpl(other.itsImpl) {}

  ~Mtx() {}

  Mtx& operator=(const Mtx& other)
  {
	 Mtx temp(other);
	 this->itsImpl.swap(temp.itsImpl);
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

  ElemProxy at(int row, int col)
    { return ElemProxy(*this, itsImpl.offsetFromStart(row, col)); }

  double at(int row, int col) const
    { return itsImpl.at(itsImpl.offsetFromStart(row, col)); }

  ElemProxy at(int elem)
    { return ElemProxy(*this, itsImpl.offsetFromStart(elem)); }

  double at(int elem) const
    { return itsImpl.at(itsImpl.offsetFromStart(elem)); }

  void reshape(int mrows, int ncols) { itsImpl.reshape(mrows, ncols); }

  int length() const { return itsImpl.length(); }

  int nelems() const { return itsImpl.nelems(); }

  int mrows() const { return itsImpl.mrows(); }

  int ncols() const { return itsImpl.ncols(); }

  //
  // Slices, submatrices
  //

  Slice row(int r)
    { return Slice(itsImpl.storage_, itsImpl.address(r,0),
						 itsImpl.rowstride(), itsImpl.ncols()); }

  ConstSlice row(int r) const
    { return ConstSlice(itsImpl.storage_, itsImpl.address(r,0),
								itsImpl.rowstride(), itsImpl.ncols()); }

  MtxIter rowIter(int r)
    { return MtxIter(itsImpl.address(r,0),
							itsImpl.rowstride(), itsImpl.ncols()); }

  MtxConstIter rowIter(int r) const
    { return MtxConstIter(itsImpl.address(r,0),
								  itsImpl.rowstride(), itsImpl.ncols()); }


  Mtx rows(int r, int nr) const;


  Slice column(int c)
    { return Slice(itsImpl.storage_, itsImpl.address(0,c),
						 itsImpl.colstride(), mrows()); }

  ConstSlice column(int c) const
    { return ConstSlice(itsImpl.storage_, itsImpl.address(0,c),
								itsImpl.colstride(), mrows()); }

  MtxIter colIter(int c)
    { return MtxIter(itsImpl.address(0,c), itsImpl.colstride(), mrows()); }

  MtxConstIter colIter(int c) const
    { return MtxConstIter(itsImpl.address(0,c), itsImpl.colstride(), mrows()); }



  //
  // Functions
  //

  void apply(double func(double))
    { itsImpl.apply(func); }

  // result = vec * mtx;
  static void VMmul_assign(const ConstSlice& vec, const Mtx& mtx,
									Slice& result);

  // this = m1 * m2;
  void assign_MMmul(const Mtx& m1, const Mtx& m2);

private:
  void makeUnique() { itsImpl.makeUnique(); }

  friend class ConstSlice;
  friend class Slice;

  class MtxImpl {
	 template <class T>
	 static void doswap(T& t1, T& t2)
	 { T t2_ = t2; t2 = t1; t1 = t2_; }

	 void init(double* data, int mrows, int ncols, StoragePolicy s)
	 {
		if (s == COPY)
		  storage_ = DataBlock::makeDataCopy(data, mrows*ncols);
		else
		  storage_ = DataBlock::makeBorrowed(data, mrows*ncols);

		storage_->incrRefCount();

		mrows_ = mrows;
		rowstride_ = mrows;
		ncols_ = ncols;
		start_ = storage_->itsData;
	 }

	 MtxImpl& operator=(const MtxImpl& other); // not allowed

  public:
	 void swap(MtxImpl& other)
	 {
		doswap(storage_, other.storage_);
		doswap(mrows_, other.mrows_);
		doswap(rowstride_, other.rowstride_);
		doswap(ncols_, other.ncols_);
		doswap(start_, other.start_);
	 }

	 MtxImpl(const MtxImpl& other) :
		storage_(other.storage_),
		mrows_(other.mrows_),
		rowstride_(other.rowstride_),
		ncols_(other.ncols_),
		start_(other.start_)
	 {
		storage_->incrRefCount();
	 }

	 MtxImpl(int mrows, int ncols) :
		storage_(DataBlock::makeBlank(mrows*ncols)),
		mrows_(mrows),
		rowstride_(mrows),
		ncols_(ncols),
		start_(storage_->itsData)
	 {
		storage_->incrRefCount();
	 }

	 MtxImpl(double* data, int mrows, int ncols, StoragePolicy s = COPY)
    { init(data, mrows, ncols, s); }

	 MtxImpl(mxArray* a, StoragePolicy s);

	 ~MtxImpl() { storage_->decrRefCount(); }

	 int length() const { return (mrows_ > ncols_) ? mrows_ : ncols_; }
	 int nelems() const { return mrows_*ncols_; }

	 int mrows() const { return mrows_; }
	 int rowstride() const { return rowstride_; }
	 int rowgap() const { return mrows_ - rowstride_; }

	 int ncols() const { return ncols_; }
	 int colstride() const { return colstride_; }

	 double at(int i) const { return start_[i]; }
	 double& at(int i) { return start_[i]; }

	 void reshape(int mrows, int ncols);

	 void selectRowRange(int r, int nr);

	 int offsetFromStart(int row, int col) const
      { return row + (col*rowstride_); }

	 int offsetFromStart(int elem) const
	 {
		return ( (elem/mrows()) /* == # of columns */
					* rowstride_ )
		  + elem%mrows(); /* == # of rows */
	 }

	 ptrdiff_t offsetFromStorage(int row, int col) const
      { return (start_ - storage_->itsData) + offsetFromStart(row, col); }

	 double* address(int row, int col)
      { return start_ + offsetFromStart(row, col); }

	 const double* address(int row, int col) const
      { return start_ + offsetFromStart(row, col); }

	 void apply(double func(double))
    {
		double* p = start_;
		int gap = rowgap();

		if (gap == 0)
		  {
			 double* end = p + nelems();
			 for (; p < end; ++p)
				*p = func(*p);
		  }
		else
		  {
			 for (int c = 0; c < ncols(); ++c)
				{
				  for (int r = 0; r < mrows(); ++r)
					 {
						*p = func(*p);
						++p;
					 }
				  p += gap;
				}
		  }
    }

	 void makeUnique()
	 {
		if ( !storage_->isUnique() )
		  {
			 ptrdiff_t offset = start_ - storage_->itsData;
			 DataBlock::makeUnique(storage_);
			 start_ = storage_->itsData + offset;
		  }
	 }

	 DataBlock* storage_;
  private:
	 int mrows_;
	 int rowstride_;
	 int ncols_;
	 static const int colstride_ = 1;
	 double* start_;
  };

  MtxImpl itsImpl;
};

///////////////////////////////////////////////////////////////////////
//
// Inline member function definitions
//
///////////////////////////////////////////////////////////////////////


inline double& ElemProxy::uniqueRef()
{ m.makeUnique(); return m.itsImpl.at(i); }

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

inline ElemProxy::operator double() { return m.itsImpl.at(i); }


inline const double* ConstSlice::data() const
{ return itsStorage->itsData + itsOffset; }

inline ConstSlice::ConstSlice(DataBlock* const& storage,
										const double* d, int s, int n) :
  itsStorage(const_cast<DataBlock*&>(storage)),
  itsOffset(d - storage->itsData),
  itsStride(s),
  itsNelems(n) {}

inline double* Slice::data() { return itsStorage->itsData + itsOffset; }



///////////////////////////////////////////////////////////////////////
//
// Inline free functions
//
///////////////////////////////////////////////////////////////////////


static double innerProduct(MtxConstIter s1, MtxConstIter s2)
{
  double result = 0.0;

  for (; s1.hasMore(); ++s1, ++s2)
	 result += (*s1) * (*s2);

  return result;
}

static const char vcid_keanu_h[] = "$Header$";
#endif // !KEANU_H_DEFINED
