///////////////////////////////////////////////////////////////////////
//
// keanu.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:23:11 2001
// written: Sun Mar 25 17:38:04 2001
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
  int stride;
  const double* stop;

  MtxConstIter(const double* d, int s, int n) :
	 data(d), stride(s), stop(d + s*n) {}

  friend class Slice;
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

  MtxIter(Mtx& m, ptrdiff_t storageOffset, int s, int n);

  friend class Slice;
  friend class Mtx;

public:

  double& operator*() { return *data; }

  MtxIter& operator++() { data += stride; return *this; }

  bool hasMore() const { return data < stop; }
};


///////////////////////////////////////////////////////////////////////
//
// Slice class definition
//
///////////////////////////////////////////////////////////////////////

class Slice {
protected:
  Mtx& itsOwner;
  ptrdiff_t itsOffset;
  int itsStride;
  int itsNelems;

  const double* dataStart() const;
  ptrdiff_t dataOffset(int i) const { return itsStride*i; }
  const double* address(int i) const { return dataStart() + dataOffset(i); }

  ptrdiff_t storageOffset(int i) const { return itsOffset + itsStride*i; }

  friend class Mtx;

  Slice(const Mtx& owner, ptrdiff_t offset, int s, int n);

public:

  // Default copy constructor OK

  //
  // Data access
  //

  double operator[](int i) const { return *(address(i)); }

  int nelems() const { return itsNelems; }

  Slice rightmost(int n) const
  {
	 int first = itsNelems - n;
	 if (first < 0) first = 0;

	 return Slice(itsOwner, storageOffset(first), itsStride, n);
  }

  Slice leftmost(int n) const
  {
	 return Slice(itsOwner, storageOffset(0), itsStride, n);
  }

  //
  // Iteration
  //

  MtxIter beginNC()
    { return MtxIter(itsOwner, storageOffset(0), itsStride, itsNelems); }

  MtxConstIter begin() const
    { return MtxConstIter(dataStart(), itsStride, itsNelems); }

  //
  // Const functions
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


  //
  // Non-const functions
  //

  void apply(double func(double))
  {
	 for (MtxIter i = beginNC(); i.hasMore(); ++i)
		*i = func(*i);
  }

  template <class F>
  void apply(F func)
  {
	 for (MtxIter i = beginNC(); i.hasMore(); ++i)
		*i = func(*i);
  }

  void operator/=(double div) { apply(Div(div)); }

  // This is assignment of value, not reference
  Slice& operator=(const Slice& other);
};



///////////////////////////////////////////////////////////////////////
//
// Mtx class definition
//
///////////////////////////////////////////////////////////////////////


typedef struct mxArray_tag mxArray;

class Mtx {
public:

  //
  // Constructors
  //

  enum StoragePolicy { COPY, BORROW, REFER };

  static const Mtx& emptyMtx();

  Mtx(mxArray* a, StoragePolicy s = COPY) : itsImpl(a, s) {}

  Mtx(double* data, int mrows, int ncols, StoragePolicy s = COPY) :
	 itsImpl(data, mrows, ncols, s) {}

  Mtx(int mrows, int ncols) : itsImpl(mrows, ncols) {}

  Mtx(const Slice& s);

  Mtx(const Mtx& other) : itsImpl(other.itsImpl) {}

  ~Mtx() {}

  Mtx& operator=(const Mtx& other)
  {
	 Mtx temp(other);
	 this->itsImpl.swap(temp.itsImpl);
	 return *this;
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

  Slice row(int r) const
    { return Slice(*this, itsImpl.offsetFromStorage(r,0),
						 itsImpl.rowstride(), itsImpl.ncols()); }

  MtxIter rowIter(int r)
    { return MtxIter(*this, itsImpl.offsetFromStorage(r,0),
							itsImpl.rowstride(), itsImpl.ncols()); }

  MtxConstIter rowIter(int r) const
    { return MtxConstIter(itsImpl.address(r,0),
								  itsImpl.rowstride(), itsImpl.ncols()); }


  Mtx rows(int r, int nr) const;


  Slice column(int c) const
    { return Slice(*this, itsImpl.offsetFromStorage(0,c),
						 itsImpl.colstride(), mrows()); }

  MtxIter colIter(int c)
    { return MtxIter(*this, itsImpl.offsetFromStorage(0,c),
							itsImpl.colstride(), mrows()); }

  MtxConstIter colIter(int c) const
    { return MtxConstIter(itsImpl.address(0,c), itsImpl.colstride(), mrows()); }


  Mtx columns(int c, int nc) const;


  //
  // Functions
  //

  void apply(double func(double))
  {
	 makeUnique();
	 itsImpl.apply(func);
  }

  void setAll(double x)
  {
	 makeUnique();
	 itsImpl.setAll(x);
  }

  // result = vec * mtx;
  static void VMmul_assign(const Slice& vec, const Mtx& mtx,
									Slice& result);

  // this = m1 * m2;
  void assign_MMmul(const Mtx& m1, const Mtx& m2);

private:
  void makeUnique() { itsImpl.makeUnique(); }

  const double* storageStart() const { return itsImpl.storageStart(); }
  double* storageStart() { return itsImpl.storageStart(); }

  friend class MtxIter;

  friend class Slice;

  class MtxImpl {
	 template <class T>
	 static void doswap(T& t1, T& t2)
	 { T t2_ = t2; t2 = t1; t1 = t2_; }

	 void init(double* data, int mrows, int ncols, StoragePolicy s)
	 {
		switch (s) {
		case BORROW:
		  storage_ = DataBlock::makeBorrowed(data, mrows*ncols);
		  break;
		case REFER:
		  storage_ = DataBlock::makeReferred(data, mrows*ncols);
		  break;
		case COPY:
		default:
		  storage_ = DataBlock::makeDataCopy(data, mrows*ncols);
		  break;
		}

		storage_->incrRefCount();

		mrows_ = mrows;
		rowstride_ = mrows;
		ncols_ = ncols;
		offset_ = 0;
	 }

	 MtxImpl& operator=(const MtxImpl& other); // not allowed

  public:
	 void swap(MtxImpl& other)
	 {
		doswap(storage_, other.storage_);
		doswap(mrows_, other.mrows_);
		doswap(rowstride_, other.rowstride_);
		doswap(ncols_, other.ncols_);
		doswap(offset_, other.offset_);
	 }

	 MtxImpl(const MtxImpl& other) :
		storage_(other.storage_),
		mrows_(other.mrows_),
		rowstride_(other.rowstride_),
		ncols_(other.ncols_),
		offset_(other.offset_)
	 {
		storage_->incrRefCount();
	 }

	 MtxImpl(int mrows, int ncols) :
		storage_(DataBlock::makeBlank(mrows*ncols)),
		mrows_(mrows),
		rowstride_(mrows),
		ncols_(ncols),
		offset_(0)
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
	 unsigned int rowgap() const { return rowstride_ - mrows_; }

	 int ncols() const { return ncols_; }
	 int colstride() const { return colstride_; }

	 double at(int i) const { return storage_->itsData[i+offset_]; }
	 double& at(int i) { return storage_->itsData[i+offset_]; }

	 void reshape(int mrows, int ncols);

	 void selectRowRange(int r, int nr);
	 void selectColumnRange(int c, int nc);

	 int offsetFromStart(int row, int col) const
      { return row + (col*rowstride_); }

	 int offsetFromStart(int elem) const
	 {
		return ( (elem/mrows()) /* == # of columns */
					* rowstride_ )
		  + elem%mrows(); /* == # of rows */
	 }

	 ptrdiff_t offsetFromStorage(int row, int col) const
      { return offset_ + offsetFromStart(row, col); }

	 double* address(int row, int col)
      { return storage_->itsData + offset_ + offsetFromStart(row, col); }

	 const double* address(int row, int col) const
      { return storage_->itsData + offset_ + offsetFromStart(row, col); }

#ifdef APPLY_IMPL
#  error macro error
#else
	 // This workaround is required because compilers don't seem to be
	 // able to accept both functors as well as function pointers as
	 // template arguments to a single apply() template
#  define APPLY_IMPL \
 \
		double* p = storage_->itsData + offset_; \
		unsigned int gap = rowgap(); \
 \
		if (gap == 0) \
		  { \
			 double* end = p + nelems(); \
			 for (; p < end; ++p) \
				*p = func(*p); \
		  } \
		else \
		  { \
			 for (int c = 0; c < ncols(); ++c) \
				{ \
				  for (int r = 0; r < mrows(); ++r) \
					 { \
						*p = func(*p); \
						++p; \
					 } \
				  p += gap; \
				} \
		  }

	 template <class F>
	 void apply(F func)
	 {
		APPLY_IMPL
	 }

	 void apply(double func(double))
	 {
		APPLY_IMPL
	 }

#  undef APPLY_IMPL
#endif // APPLY_IMPL

	 struct Setter {
		double v;
		Setter(double v_) : v(v_) {}
		double operator()(double) { return v; }
	 };

	 void setAll(double x) { apply(Setter(x)); }

	 void makeUnique();

	 const double* storageStart() const { return storage_->itsData; }
	 double* storageStart() { return storage_->itsData; }

  private:
	 DataBlock* storage_;
	 int mrows_;
	 int rowstride_;
	 int ncols_;
	 static const int colstride_ = 1;
	 ptrdiff_t offset_;
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


inline const double* Slice::dataStart() const
{ return itsOwner.storageStart() + itsOffset; }

inline Slice::Slice(const Mtx& owner, ptrdiff_t offset, int s, int n) :
  itsOwner(const_cast<Mtx&>(owner)),
  itsOffset(offset),
  itsStride(s),
  itsNelems(n) {}



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
