///////////////////////////////////////////////////////////////////////
//
// keanu.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:23:11 2001
// written: Tue Apr 10 10:09:04 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KEANU_H_DEFINED
#define KEANU_H_DEFINED

#include "datablock.h"
#include "num.h"

namespace RC // Range checking
{
  void less(const void* x, const void* lim, const char* f, int ln);
  void leq(const void* x, const void* lim, const char* f, int ln);
  void inHalfOpen(const void* x, const void* llim, const void* ulim,
						const char* f, int ln);

  void less(int x, int lim, const char* f, int ln);
  void leq(int x, int lim, const char* f, int ln);
  void inHalfOpen(int x, int llim, int ulim, const char* f, int ln);
}

#ifdef RANGE_CHECK
// Range check
#  define RC_less(x,lim) RC::less((x),(lim),__FILE__,__LINE__)
#  define RC_leq(x,lim) RC::leq((x),(lim),__FILE__,__LINE__)
#  define RC_inHalfOpen(x,llim,ulim) RC::inHalfOpen((x),(llim),(ulim),__FILE__,__LINE__)

// Range check, and return the checked value
#  define RCR_less(x,lim) (RC::less((x),(lim),__FILE__,__LINE__), x)
#  define RCR_leq(x,lim) (RC::leq((x),(lim),__FILE__,__LINE__), x)
#  define RCR_inHalfOpen(x,llim,ulim) (RC::inHalfOpen((x),(llim),(ulim),__FILE__,__LINE__), x)

#else // !RANGE_CHECK

#  define RC_less(x,lim)
#  define RC_leq(x,lim)
#  define RC_inHalfOpen(x,llim,ulim)

#  define RCR_less(x,lim) (x)
#  define RCR_leq(x,lim) (x)
#  define RCR_inHalfOpen(x,llim,ulim) (x)
#endif

class Mtx;

///////////////////////////////////////////////////////////////////////
//
// ElemProxy class definition
//
///////////////////////////////////////////////////////////////////////


class ElemProxy {
  Mtx& m;
  int i;

  double value() const;
  double& uniqueRef();

public:
  ElemProxy(Mtx& m_, int i_) : m(m_), i(i_) {}

  double& operator=(const ElemProxy& other);
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


class MtxIter {
  double* data;
  int stride;
  double* stop;

  MtxIter(Mtx& m, ptrdiff_t storageOffset, int s, int n);

  friend class MtxConstIter;
  friend class Slice;
  friend class Mtx;

public:

  double& operator*() { RC_less(data, stop); return *data; }

  MtxIter& operator++()
  {
	 data += stride;
	 return *this;
  }

  bool hasMore() const { return data < stop; }
};

class MtxConstIter {
  const double* data;
  int stride;
  const double* stop;

  MtxConstIter(const double* d, int s, int n) :
	 data(d), stride(s), stop(d + s*n) {}

  friend class Slice;
  friend class Mtx;

public:
  MtxConstIter(const MtxIter& other) :
	 data(other.data), stride(other.stride), stop(other.stop) {}

  double operator*() const { RC_less(data, stop); return *data; }

  MtxConstIter& operator++() { data += stride; return *this; }

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

  double operator[](int i) const
  {
	 RC_inHalfOpen(i, 0, itsNelems);
	 return *(address(i));
  }

  int nelems() const { return itsNelems; }

  Slice rightmost(int n) const
  {
	 RC_inHalfOpen(n, 0, itsNelems);

	 int first = itsNelems - n;
	 if (first < 0) first = 0;

	 return Slice(itsOwner, storageOffset(first), itsStride, n);
  }

  Slice leftmost(int n) const
  {
	 RC_inHalfOpen(n, 0, itsNelems);

	 return Slice(itsOwner, storageOffset(0), itsStride, n);
  }

  void print() const;

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


  bool operator==(const Slice& other) const
  {
	 if (itsNelems != other.itsNelems) return false;

	 for (MtxConstIter a = this->begin(), b = other.begin();
			a.hasMore();
			++a, ++b)
		if (*a != *b) return false;

	 return true;
  }

  bool operator!=(const Slice& other) const
  { return !(operator==(other)); }

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
  Slice& operator=(const Mtx& other);
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
  // Constructors + Conversion
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

  // This will destroy any data in the process of changing the size of
  // the Mtx to the specified dimensions; its only advantage over just
  // declaring a new Mtx is that it will avoid a deallocate/allocate
  // cycle if the new dimensions are the same as the current dimensions.
  void resize(int mrowsNew, int ncolsNew);

  static Mtx extractStructField(mxArray* structArray, const char* fieldName,
										  int indexIntoArray = 0);

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

  void swapColumns(int c1, int c2);

  //
  // Functions
  //

  void apply(double func(double))
  {
	 makeUnique();
	 itsImpl.apply(func);
  }

  template <class F>
  void apply(F func)
  {
	 makeUnique();
	 itsImpl.apply(func);
  }

  void setAll(double x)
  {
	 makeUnique();
	 itsImpl.setAll(x);
  }

  Mtx& operator+=(double x) { apply(Add(x)); return *this; }
  Mtx& operator-=(double x) { apply(Sub(x)); return *this; }
  Mtx& operator*=(double fac) { apply(Mul(fac)); return *this; }
  Mtx& operator/=(double div) { apply(Div(div)); return *this; }

  bool operator==(const Mtx& other) const
  {
	 if ( (mrows() != other.mrows()) || (ncols() != other.ncols()) )
		return false;
	 for (int c = 0; c < ncols(); ++c)
		if ( column(c) != other.column(c) ) return false;
	 return true;
  }

  bool operator!=(const Mtx& other) const
  { return !(operator==(other)); }

  // result = vec * mtx;
  static void VMmul_assign(const Slice& vec, const Mtx& mtx,
									Slice& result);

  // this = m1 * m2;
  void assign_MMmul(const Mtx& m1, const Mtx& m2);

  void makeUnique() { itsImpl.makeUnique(); }

private:
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
		  datablock_ = DataBlock::makeBorrowed(data, mrows*ncols);
		  break;
		case REFER:
		  datablock_ = DataBlock::makeReferred(data, mrows*ncols);
		  break;
		case COPY:
		default:
		  datablock_ = DataBlock::makeDataCopy(data, mrows*ncols);
		  break;
		}

		datablock_->incrRefCount();

		mrows_ = mrows;
		rowstride_ = mrows;
		ncols_ = ncols;
		offset_ = 0;
	 }

	 MtxImpl& operator=(const MtxImpl& other); // not allowed

  public:
	 void swap(MtxImpl& other)
	 {
		doswap(datablock_, other.datablock_);
		doswap(mrows_, other.mrows_);
		doswap(rowstride_, other.rowstride_);
		doswap(ncols_, other.ncols_);
		doswap(offset_, other.offset_);
	 }

	 MtxImpl(const MtxImpl& other) :
		datablock_(other.datablock_),
		mrows_(other.mrows_),
		rowstride_(other.rowstride_),
		ncols_(other.ncols_),
		offset_(other.offset_)
	 {
		datablock_->incrRefCount();
	 }

	 MtxImpl(int mrows, int ncols) :
		datablock_(DataBlock::makeBlank(mrows*ncols)),
		mrows_(mrows),
		rowstride_(mrows),
		ncols_(ncols),
		offset_(0)
	 {
		datablock_->incrRefCount();
	 }

	 MtxImpl(double* data, int mrows, int ncols, StoragePolicy s = COPY)
    { init(data, mrows, ncols, s); }

	 MtxImpl(mxArray* a, StoragePolicy s);

	 ~MtxImpl() { datablock_->decrRefCount(); }

	 int length() const { return (mrows_ > ncols_) ? mrows_ : ncols_; }
	 int nelems() const { return mrows_*ncols_; }

	 int mrows() const { return mrows_; }
	 int rowstride() const { return rowstride_; }

	 int ncols() const { return ncols_; }
	 int colstride() const { return colstride_; }

	 double at(int i) const
	 {
		RC_less(i+offset_, storageLength());
		return datablock_->itsData[i+offset_];
	 }

	 double& at(int i)
	 {
		RC_less(i+offset_, storageLength());
		return datablock_->itsData[i+offset_];
	 }

	 void reshape(int mrows, int ncols);

	 void selectRowRange(int r, int nr);
	 void selectColumnRange(int c, int nc);

	 int offsetFromStart(int row, int col) const
	 {
		RC_inHalfOpen(row, 0, mrows_);
		RC_inHalfOpen(col, 0, ncols_);
		return row + (col*rowstride_);
	 }

	 int offsetFromStart(int elem) const
	   { return offsetFromStart(elem%mrows(), elem/mrows()); }

	 ptrdiff_t offsetFromStorage(int row, int col) const
	   { return RCR_less(offset_ + offsetFromStart(row, col), storageLength()); }

	 double* address(int row, int col)
      { return datablock_->itsData + offsetFromStorage(row, col); }

	 const double* address(int row, int col) const
      { return datablock_->itsData + offsetFromStorage(row, col); }

#ifdef APPLY_IMPL
#  error macro error
#else
	 // This workaround is required because compilers don't seem to be
	 // able to accept both functors as well as function pointers as
	 // template arguments to a single apply() template
#  define APPLY_IMPL \
 \
		double* p = datablock_->itsData + offset_; \
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

	 const double* storageStart() const { return datablock_->itsData; }
	 double* storageStart() { return datablock_->itsData; }

  private:
	 int storageLength() const { return datablock_->itsLength; }
	 unsigned int rowgap() const { return rowstride_ - mrows_; }

	 DataBlock* datablock_;
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

inline double ElemProxy::value() const
{ return m.itsImpl.at(i); }

inline double& ElemProxy::uniqueRef()
{ m.makeUnique(); return m.itsImpl.at(i); }

inline double& ElemProxy::operator=(double d)
{ return (uniqueRef() = d); }

inline double& ElemProxy::operator=(const ElemProxy& other)
{ return (uniqueRef() = other.value()); }

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
