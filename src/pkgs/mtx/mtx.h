///////////////////////////////////////////////////////////////////////
//
// mtx.h
//
// Copyright (c) 2001-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:23:11 2001
// written: Mon Mar  4 12:57:35 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MTX_H_DEFINED
#define MTX_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(DATABLOCK_H_DEFINED)
#include "datablock.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(NUM_H_DEFINED)
#include "num.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ITERATOR_DEFINED)
#include <iterator>
#define ITERATOR_DEFINED
#endif

class fstring;

namespace RC // Range checking
{
  void geq(const void* x, const void* lim, const char* f, int ln);
  void less(const void* x, const void* lim, const char* f, int ln);
  void leq(const void* x, const void* lim, const char* f, int ln);
  void inHalfOpen(const void* x, const void* llim, const void* ulim,
                  const char* f, int ln);

  void geq(int x, int lim, const char* f, int ln);
  void less(int x, int lim, const char* f, int ln);
  void leq(int x, int lim, const char* f, int ln);
  void inHalfOpen(int x, int llim, int ulim, const char* f, int ln);
}

#ifdef RANGE_CHECK
// Range check
#  define RC_geq(x,lim) RC::geq((x),(lim),__FILE__,__LINE__)
#  define RC_less(x,lim) RC::less((x),(lim),__FILE__,__LINE__)
#  define RC_leq(x,lim) RC::leq((x),(lim),__FILE__,__LINE__)
#  define RC_inHalfOpen(x,llim,ulim) RC::inHalfOpen((x),(llim),(ulim),__FILE__,__LINE__)

// Range check, and return the checked value
#  define RCR_geq(x,lim) (RC::geq((x),(lim),__FILE__,__LINE__), x)
#  define RCR_less(x,lim) (RC::less((x),(lim),__FILE__,__LINE__), x)
#  define RCR_leq(x,lim) (RC::leq((x),(lim),__FILE__,__LINE__), x)
#  define RCR_inHalfOpen(x,llim,ulim) (RC::inHalfOpen((x),(llim),(ulim),__FILE__,__LINE__), x)

#else // !RANGE_CHECK

#  define RC_geq(x,lim)
#  define RC_less(x,lim)
#  define RC_leq(x,lim)
#  define RC_inHalfOpen(x,llim,ulim)

#  define RCR_geq(x,lim) (x)
#  define RCR_less(x,lim) (x)
#  define RCR_leq(x,lim) (x)
#  define RCR_inHalfOpen(x,llim,ulim) (x)
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * Range class.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Range
{
private:
  const int itsFirst;
  const int itsCount;

public:
  Range(int first, int count) : itsFirst(first), itsCount(count) {}

  int begin() const { return itsFirst; }
  int end() const { return itsFirst+itsCount; }
  int count() const { return itsCount; }
};

class RowRange : public Range
{
public:
  RowRange(int first, int count) : Range(first, count) {}
};

class ColRange : public Range
{
public:
  ColRange(int first, int count) : Range(first, count) {}
};

inline Range range(int begin, int end) { return Range(begin, end-begin); }
inline Range range_n(int begin, int count) { return Range(begin, count); }

inline RowRange row_range(int begin, int end) { return RowRange(begin, end-begin); }
inline RowRange row_range_n(int begin, int count) { return RowRange(begin, count); }

inline ColRange col_range(int begin, int end) { return ColRange(begin, end-begin); }
inline ColRange col_range_n(int begin, int count) { return ColRange(begin, count); }



class Mtx;


///////////////////////////////////////////////////////////////////////
//
// Mtx iterators
//
///////////////////////////////////////////////////////////////////////

template <class T>
class MtxIterBase
{
protected:

  MtxIterBase(T* d, int str, int n) :
    data(d), stride(str), stop(data+str*n) {}

public:
  MtxIterBase(const MtxIterBase& other) :
    data(other.data), stride(other.stride), stop(other.stop) {}

  template <class U>
  MtxIterBase(const MtxIterBase<U>& other) :
    data(other.data), stride(other.stride), stop(other.stop) {}

  typedef std::random_access_iterator_tag iterator_category;

  typedef T            value_type;
  typedef ptrdiff_t    difference_type;
  typedef T*           pointer;
  typedef T&           reference;

  MtxIterBase end() const { MtxIterBase e(*this); e.data = stop; return e; }

  bool hasMore() const { return data < stop; }

  // Dereference

  reference operator*() const { RC_less(data, stop); return *data; }

  // Comparison

  bool operator==(const MtxIterBase& other) const { return data == other.data; }

  bool operator!=(const MtxIterBase& other) const { return data != other.data; }

  bool operator<(const MtxIterBase& other) const { return data < other.data; }

  difference_type operator-(const MtxIterBase& other) const
    { return (data - other.data) / stride; }

  // Increment/Decrement

  MtxIterBase& operator++() { data += stride; return *this; }
  MtxIterBase& operator--() { data -= stride; return *this; }

  MtxIterBase operator++(int) { MtxIterBase cpy(*this); data += stride; return cpy; }
  MtxIterBase operator--(int) { MtxIterBase cpy(*this); data -= stride; return cpy; }

  MtxIterBase& operator+=(int x) { data += x*stride; return *this; }
  MtxIterBase& operator-=(int x) { data -= x*stride; return *this; }

  MtxIterBase operator+(int x) const { MtxIterBase res(*this); res += x; return res; }
  MtxIterBase operator-(int x) const { MtxIterBase res(*this); res -= x; return res; }

private:

  template <class U> friend class MtxIterBase;
  friend class Slice;
  friend class Mtx;

  T* data;
  int stride;
  T* stop;
};

typedef MtxIterBase<double> MtxIter;
typedef MtxIterBase<const double> MtxConstIter;


///////////////////////////////////////////////////////////////////////
//
// Slice class definition
//
///////////////////////////////////////////////////////////////////////

class Slice
{
protected:
  Mtx& itsOwner;
  ptrdiff_t itsOffset;
  int itsStride;
  int itsNelems;

  inline const double* dataStart() const;
  inline double* dataStart_nc();
  ptrdiff_t dataOffset(int i) const { return itsStride*i; }
  const double* address(int i) const { return dataStart() + dataOffset(i); }

  ptrdiff_t storageOffset(int i) const { return itsOffset + itsStride*i; }

  friend class Mtx;

  inline Slice(const Mtx& owner, ptrdiff_t offset, int s, int n);

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

  Slice operator()(const Range& rng) const;

  void print() const;

  //
  // Iteration
  //

  MtxIter beginNC()
    { return MtxIter(dataStart_nc(), itsStride, itsNelems); }

  MtxIter endNC()
    { return beginNC().end(); }

  MtxConstIter begin() const
    { return MtxConstIter(dataStart(), itsStride, itsNelems); }

  MtxConstIter end() const
    { return begin().end(); }

  //
  // Const functions
  //

  double sum() const;
  double min() const;
  double max() const;

  double mean() const { return sum()/itsNelems; }


  // Returns an index matrix so that this->reorder(Mtx) will put the
  // Slice in sorted order
  Mtx getSortOrder() const;

  bool operator==(const Slice& other) const;

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

  void sort();

  // Reorders the Slice by applying *this[i] = *this[index[i]]
  void reorder(const Mtx& index);

  void operator+=(double val) { apply(Add(val)); }
  void operator-=(double val) { apply(Sub(val)); }

  void operator*=(double factor) { apply(Mul(factor)); }
  void operator/=(double div) { apply(Div(div)); }

  Slice& operator+=(const Slice& other);
  Slice& operator-=(const Slice& other);

  // This is assignment of value, not reference
  Slice& operator=(const Slice& other);
  Slice& operator=(const Mtx& other);
};



typedef struct mxArray_tag mxArray;

struct WithPolicies
{
  enum InitPolicy { ZEROS, NO_INIT };
  enum StoragePolicy { COPY, BORROW, REFER };
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * MtxStorage class.
 *
 **/
///////////////////////////////////////////////////////////////////////

class MtxStorage
{
public:
  MtxStorage() : mrows_(0), rowstride_(0), ncols_(0), offset_(0) {}

  MtxStorage(int mrows, int ncols) :
    mrows_(mrows),
    rowstride_(mrows),
    ncols_(ncols),
    offset_(0)
  {}

  MtxStorage(const MtxStorage& other) :
    mrows_(other.mrows_),
    rowstride_(other.rowstride_),
    ncols_(other.ncols_),
    offset_(other.offset_)
  {}

  int length() const { return (mrows_ > ncols_) ? mrows_ : ncols_; }
  int nelems() const { return mrows_*ncols_; }

  int mrows() const { return mrows_; }
  int rowstride() const { return rowstride_; }

  int ncols() const { return ncols_; }
  int colstride() const { return colstride_; }

  unsigned int rowgap() const { return rowstride_ - mrows_; }

  int offsetFromStart(int row, int col) const
  {
    RC_inHalfOpen(row, 0, mrows_);
    RC_inHalfOpen(col, 0, ncols_);
    return row + (col*rowstride_);
  }

  int offsetFromStart(int elem) const
  { return offsetFromStart(elem%mrows(), elem/mrows()); }

protected:
  int mrows_;
  int rowstride_;
  int ncols_;
  static const int colstride_ = 1;
  ptrdiff_t offset_;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * MtxImpl class.
 *
 **/
///////////////////////////////////////////////////////////////////////

class MtxImpl : public MtxStorage, public WithPolicies
{
private:
  void init(double* data, int mrows, int ncols, StoragePolicy s);

  MtxImpl& operator=(const MtxImpl& other); // not allowed

public:
  void swap(MtxImpl& other);

  MtxImpl(const MtxImpl& other);

  MtxImpl(int mrows, int ncols, InitPolicy p);

  MtxImpl(double* data, int mrows, int ncols, StoragePolicy s = COPY) :
    MtxStorage()
  { init(data, mrows, ncols, s); }

  MtxImpl(mxArray* a, StoragePolicy s);

  /** With a const mxArray*, only BORROW or COPY are allowed as storage
      policies, in order to preserve const-correctness. */
  MtxImpl(const mxArray* a, StoragePolicy s);

  ~MtxImpl();

  const double& at(int i) const
  {
    RC_less(i+offset_, storageLength());
    return storage()[i+offset_];
  }

  double& at_nc(int i)
  {
    RC_less(i+offset_, storageLength());
    return storage_nc()[i+offset_];
  }

  void reshape(int mrows, int ncols);

  void selectRows(const RowRange& rng);
  void selectCols(const ColRange& rng);

  ptrdiff_t offsetFromStorage(int row, int col) const
  { return RCR_leq(offset_ + offsetFromStart(row, col), storageLength()); }

  double* address_nc(int row, int col)
  { return storage_nc() + offsetFromStorage(row, col); }

  const double* address(int row, int col) const
  { return storage() + offsetFromStorage(row, col); }

#ifdef APPLY_IMPL
#  error macro error
#else
  // This workaround is required because compilers don't seem to be
  // able to accept both functors as well as function pointers as
  // template arguments to a single apply() template
#  define APPLY_IMPL \
 \
      double* p = storage_nc() + offset_; \
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
  void applyFF(F func)
  {
    APPLY_IMPL;
  }

  void apply(double func(double))
  {
    APPLY_IMPL;
  }

#  undef APPLY_IMPL
#endif // APPLY_IMPL

  void makeUnique() { DataBlock::makeUnique(datablock_); }

  const double* storage() const { return datablock_->data(); }
  double* storage_nc() { makeUnique(); return datablock_->data_nc(); }

  int storageLength() const { return datablock_->length(); }

private:
  DataBlock* datablock_;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * Mtx class definition
 *
 **/
///////////////////////////////////////////////////////////////////////

class Mtx : public WithPolicies
{
public:

  //
  // Constructors + Conversion
  //

  static const Mtx& emptyMtx();

  Mtx(mxArray* a, StoragePolicy s = COPY) : itsImpl(a, s) {}

  /** With a const mxArray*, only BORROW or COPY are allowed as storage
      policies, in order to preserve const-correctness. */
  Mtx(const mxArray* a, StoragePolicy s = COPY) : itsImpl(a, s) {}

  Mtx(double* data, int mrows, int ncols, StoragePolicy s = COPY) :
    itsImpl(data, mrows, ncols, s) {}

  Mtx(int mrows, int ncols, InitPolicy p = ZEROS) :
    itsImpl(mrows, ncols, p) {}

  Mtx(const Slice& s);

  Mtx(const Mtx& other) : itsImpl(other.itsImpl) {}

  virtual ~Mtx();

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

  mxArray* makeMxArray() const;


  //
  // I/O
  //

  void print() const;

  // This version will print the given name before printing the matrix contents
  void print(const char* mtxName) const;

  //
  // Iteration
  //

  template <class M, class T>
  class iter_base
  {
    M* mtx;
    int elem;

  public:
    iter_base(M* m, int e) : mtx(m), elem(e) {}

    typedef std::random_access_iterator_tag iterator_category;

    typedef T            value_type;
    typedef ptrdiff_t    difference_type;
    typedef T*           pointer;
    typedef T&           reference;

    iter_base end() const { return iter_base(mtx, mtx->nelems()); }

    bool hasMore() const { return elem < mtx->nelems(); }

    reference operator*() const { return mtx->at(elem); }

    // Comparison

    bool operator==(const iter_base& other) const { return elem == other.elem; }

    bool operator!=(const iter_base& other) const { return elem != other.elem; }

    bool operator<(const iter_base& other) const { return elem < other.elem; }

    difference_type operator-(const iter_base& other) const
      { return elem - other.elem; }

    // Increment/Decrement

    iter_base& operator++() { ++elem; return *this; }
    iter_base& operator--() { --elem; return *this; }

    iter_base operator++(int) { return iter_base(mtx, elem++); }
    iter_base operator--(int) { return iter_base(mtx, elem--); }

    iter_base& operator+=(int x) { elem += x; return *this; }
    iter_base& operator-=(int x) { elem -= x; return *this; }

    iter_base operator+(int x) const { return iter_base(mtx, elem+x); }
    iter_base operator-(int x) const { return iter_base(mtx, elem-x); }
  };

  typedef iter_base<Mtx, double> iterator;
  typedef iter_base<const Mtx, const double> const_iterator;

  iterator begin_nc() { return iterator(this, 0); }
  iterator end_nc() { return iterator(this, nelems()); }

  const_iterator begin() const { return const_iterator(this, 0); }
  const_iterator end() const { return const_iterator(this, nelems()); }


  template <class T>
  class colmaj_iter_base
  {
    int itsRowgap;
    int itsRowstride;
    T* itsPtr;
    T* itsCurrentEnd;

  public:
    colmaj_iter_base(int rg, int rs, T* ptr) :
      itsRowgap(rg),
      itsRowstride(rs),
      itsPtr(ptr),
      itsCurrentEnd(itsPtr+(itsRowstride-itsRowgap))
    {}

    T& operator*() const { return *itsPtr; }

    colmaj_iter_base& operator++()
    {
      if (++itsPtr == itsCurrentEnd)
        {
          itsPtr += itsRowgap;
          itsCurrentEnd += itsRowstride;
        }
      return *this;
    }

    bool operator==(const colmaj_iter_base& other) const
    { return itsPtr == other.itsPtr; }

    bool operator!=(const colmaj_iter_base& other) const
    { return itsPtr != other.itsPtr; }
  };

  typedef colmaj_iter_base<double> colmaj_iter;
  typedef colmaj_iter_base<const double> const_colmaj_iter;

  colmaj_iter colmaj_begin_nc()
  { return colmaj_iter(itsImpl.rowgap(), itsImpl.rowstride(),
                       itsImpl.address_nc(0,0)); }

  colmaj_iter colmaj_end_nc()
  { return colmaj_iter(itsImpl.rowgap(), itsImpl.rowstride(),
                       itsImpl.address_nc(0,ncols())); }

  const_colmaj_iter colmaj_begin() const
  { return const_colmaj_iter(itsImpl.rowgap(), itsImpl.rowstride(),
                             itsImpl.address(0,0)); }

  const_colmaj_iter colmaj_end() const
  { return const_colmaj_iter(itsImpl.rowgap(), itsImpl.rowstride(),
                             itsImpl.address(0,ncols())); }


  template <class T>
  class rowmaj_iter_base
  {
    int itsStride;
    T* itsCurrentStart;
    T* itsPtr;
    T* itsCurrentEnd;

  public:
    rowmaj_iter_base(int s, int ncols, T* ptr) :
      itsStride(s),
      itsCurrentStart(ptr),
      itsPtr(ptr),
      itsCurrentEnd(itsPtr+(ncols*s))
    {}

    T& operator*() const { return *itsPtr; }

    rowmaj_iter_base& operator++()
    {
      itsPtr += itsStride;
      if (itsPtr == itsCurrentEnd)
        {
          ++itsCurrentStart;
          ++itsCurrentEnd;
          itsPtr = itsCurrentStart;
        }
      return *this;
    }

    bool operator==(const rowmaj_iter_base& other) const
    { return itsPtr == other.itsPtr; }

    bool operator!=(const rowmaj_iter_base& other) const
    { return itsPtr != other.itsPtr; }
  };

  typedef rowmaj_iter_base<double> rowmaj_iter;
  typedef rowmaj_iter_base<const double> const_rowmaj_iter;

  rowmaj_iter rowmaj_begin_nc()
  { return rowmaj_iter(itsImpl.rowstride(), ncols(),
                       itsImpl.address_nc(0,0)); }

  rowmaj_iter rowmaj_end_nc()
  { return rowmaj_iter(itsImpl.rowstride(), ncols(),
                       itsImpl.address_nc(mrows(),0)); }

  const_rowmaj_iter rowmaj_begin() const
  { return const_rowmaj_iter(itsImpl.rowstride(), ncols(),
                             itsImpl.address(0,0)); }

  const_rowmaj_iter rowmaj_end() const
  { return const_rowmaj_iter(itsImpl.rowstride(), ncols(),
                             itsImpl.address(mrows(),0)); }

  //
  // Data access
  //

  double& at(int row, int col)
    { return itsImpl.at_nc(itsImpl.offsetFromStart(row, col)); }

  const double& at(int row, int col) const
    { return itsImpl.at(itsImpl.offsetFromStart(row, col)); }

  double& at(int elem)
    { return itsImpl.at_nc(itsImpl.offsetFromStart(elem)); }

  const double& at(int elem) const
    { return itsImpl.at(itsImpl.offsetFromStart(elem)); }

  void reshape(int mrows, int ncols) { itsImpl.reshape(mrows, ncols); }

  int length() const { return itsImpl.length(); }

  int nelems() const { return itsImpl.nelems(); }

  int mrows() const { return itsImpl.mrows(); }

  int ncols() const { return itsImpl.ncols(); }

  bool sameSize(const Mtx& x) const
  { return (mrows() == x.mrows()) && (ncols() == x.ncols()); }

  //
  // Slices, submatrices
  //

  Slice row(int r) const
    { return Slice(*this, itsImpl.offsetFromStorage(r,0),
                   itsImpl.rowstride(), itsImpl.ncols()); }

  MtxIter rowIter(int r)
    { return MtxIter(itsImpl.address_nc(r,0),
                     itsImpl.rowstride(), itsImpl.ncols()); }

  MtxConstIter rowIter(int r) const
    { return MtxConstIter(itsImpl.address(r,0),
                          itsImpl.rowstride(), itsImpl.ncols()); }

  Mtx operator()(const RowRange& rng) const;

  Mtx asRow() const
    { Mtx result(*this); result.reshape(1, nelems()); return result; }

  void reorderRows(const Mtx& index);



  Slice column(int c) const
    { return Slice(*this, itsImpl.offsetFromStorage(0,c),
                   itsImpl.colstride(), mrows()); }

  MtxIter columnIter(int c)
    { return MtxIter(itsImpl.address_nc(0,c),
                     itsImpl.colstride(), mrows()); }

  MtxConstIter columnIter(int c) const
    { return MtxConstIter(itsImpl.address(0,c),
                          itsImpl.colstride(), mrows()); }

  Mtx operator()(const ColRange& rng) const;

  Mtx asColumn() const
    { Mtx result(*this); result.reshape(nelems(), 1); return result; }

  void reorderColumns(const Mtx& index);

  void swapColumns(int c1, int c2);


  //
  // Functions
  //

  Mtx meanRow() const;

  Mtx meanColumn() const;

  const_iterator find_min() const;
  const_iterator find_max() const;

  double min() const;
  double max() const;

  double sum() const;

  double mean() const { return sum() / nelems(); }

  void apply(double func(double)) { itsImpl.apply(func); }

  template <class F>
  void applyF(F func) { itsImpl.applyFF(func); }

  struct Setter
  {
    double v;
    Setter(double v_) : v(v_) {}
    double operator()(double) { return v; }
  };

  void setAll(double x) { applyF(Setter(x)); }

  void clear(double x = 0.0) { applyF(Setter(x)); }

  Mtx& operator+=(double x) { applyF(Add(x)); return *this; }
  Mtx& operator-=(double x) { applyF(Sub(x)); return *this; }
  Mtx& operator*=(double fac) { applyF(Mul(fac)); return *this; }
  Mtx& operator/=(double div) { applyF(Div(div)); return *this; }

  Mtx& operator+=(const Mtx& other);
  Mtx& operator-=(const Mtx& other);

  bool operator==(const Mtx& other) const;

  bool operator!=(const Mtx& other) const
  { return !(operator==(other)); }

  // result = vec * mtx;
  static void VMmul_assign(const Slice& vec, const Mtx& mtx,
                           Slice& result);

  // this = m1 * m2;
  void assign_MMmul(const Mtx& m1, const Mtx& m2);

  void makeUnique() { itsImpl.makeUnique(); }

private:
  const double* storage() const { return itsImpl.storage(); }
  double* storage_nc() { return itsImpl.storage_nc(); }

  friend class MtxIterBase<double>;
  friend class MtxIterBase<const double>;

  friend class Slice;

  MtxImpl itsImpl;
};

///////////////////////////////////////////////////////////////////////
//
// Inline member function definitions
//
///////////////////////////////////////////////////////////////////////

inline const double* Slice::dataStart() const
{ return itsOwner.storage() + itsOffset; }

inline double* Slice::dataStart_nc()
{ return itsOwner.storage_nc() + itsOffset; }

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


inline double innerProduct(MtxConstIter s1, MtxConstIter s2)
{
  double result = 0.0;

  for (; s1.hasMore(); ++s1, ++s2)
    result += (*s1) * (*s2);

  return result;
}

Mtx operator+(const Mtx& m, double x);
Mtx operator-(const Mtx& m, double x);
Mtx operator*(const Mtx& m, double x);
Mtx operator/(const Mtx& m, double x);

Mtx operator+(const Mtx& m1, const Mtx& m2);
Mtx operator-(const Mtx& m1, const Mtx& m2);

// Simple element-by-element multiplication (i.e. NOT matrix multiplication)
Mtx arr_mul(const Mtx& m1, const Mtx& m2);

// Simple element-by-element division (i.e. NOT matrix division)
Mtx arr_div(const Mtx& m1, const Mtx& m2);

Mtx min(const Mtx& m1, const Mtx& m2);
Mtx max(const Mtx& m1, const Mtx& m2);

static const char vcid_mtx_h[] = "$Header$";
#endif // !MTX_H_DEFINED
