///////////////////////////////////////////////////////////////////////
//
// mtx.h
//
// Copyright (c) 2001-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:23:11 2001
// written: Wed Sep 25 18:56:25 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MTX_H_DEFINED
#define MTX_H_DEFINED

#include "datablock.h"

#include "num.h"

#include <iterator>

class fstring;

namespace RC // Range checking
{
  void geq(const void* x, const void* lim, const char* f, int ln);
  void less(const void* x, const void* lim, const char* f, int ln);
  void leq(const void* x, const void* lim, const char* f, int ln);
  void inHalfOpen(const void* x, const void* llim, const void* ulim,
                  const char* f, int ln);
  void inFullOpen(const void* x, const void* llim, const void* ulim,
                  const char* f, int ln);

  void geq(int x, int lim, const char* f, int ln);
  void less(int x, int lim, const char* f, int ln);
  void leq(int x, int lim, const char* f, int ln);
  void inHalfOpen(int x, int llim, int ulim, const char* f, int ln);
  void inFullOpen(int x, int llim, int ulim, const char* f, int ln);
}

#ifdef RANGE_CHECK
// Range check
#  define RC_geq(x,lim) RC::geq((x),(lim),__FILE__,__LINE__)
#  define RC_less(x,lim) RC::less((x),(lim),__FILE__,__LINE__)
#  define RC_leq(x,lim) RC::leq((x),(lim),__FILE__,__LINE__)
#  define RC_inHalfOpen(x,llim,ulim) RC::inHalfOpen((x),(llim),(ulim),__FILE__,__LINE__)
#  define RC_inFullOpen(x,llim,ulim) RC::inFullOpen((x),(llim),(ulim),__FILE__,__LINE__)

// Range check, and return the checked value
#  define RCR_geq(x,lim) (RC::geq((x),(lim),__FILE__,__LINE__), x)
#  define RCR_less(x,lim) (RC::less((x),(lim),__FILE__,__LINE__), x)
#  define RCR_leq(x,lim) (RC::leq((x),(lim),__FILE__,__LINE__), x)
#  define RCR_inHalfOpen(x,llim,ulim) (RC::inHalfOpen((x),(llim),(ulim),__FILE__,__LINE__), x)
#  define RCR_inFullOpen(x,llim,ulim) (RC::inFullOpen((x),(llim),(ulim),__FILE__,__LINE__), x)

#else // !RANGE_CHECK

#  define RC_geq(x,lim)
#  define RC_less(x,lim)
#  define RC_leq(x,lim)
#  define RC_inHalfOpen(x,llim,ulim)
#  define RC_inFullOpen(x,llim,ulim)

#  define RCR_geq(x,lim) (x)
#  define RCR_less(x,lim) (x)
#  define RCR_leq(x,lim) (x)
#  define RCR_inHalfOpen(x,llim,ulim) (x)
#  define RCR_inFullOpen(x,llim,ulim) (x)
#endif

// ####################################################################

/// Range class represents a half-open range of indices.

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

/// A range class for row ranges only.
class RowRange : public Range
{
public:
  RowRange(int first, int count) : Range(first, count) {}
};

/// A range class for column ranges only.
class ColRange : public Range
{
public:
  ColRange(int first, int count) : Range(first, count) {}
};

inline Range range(int i) { return Range(i, 1); }
inline Range range(int begin, int end) { return Range(begin, end-begin); }
inline Range range_n(int begin, int count) { return Range(begin, count); }

inline RowRange row_range(int r) { return RowRange(r, 1); }
inline RowRange row_range(int begin, int end) { return RowRange(begin, end-begin); }
inline RowRange row_range_n(int begin, int count) { return RowRange(begin, count); }

inline ColRange col_range(int c) { return ColRange(c, 1); }
inline ColRange col_range(int begin, int end) { return ColRange(begin, end-begin); }
inline ColRange col_range_n(int begin, int count) { return ColRange(begin, count); }



class Mtx;


///////////////////////////////////////////////////////////////////////
//
// Mtx iterators
//
///////////////////////////////////////////////////////////////////////


/// Generic stride-based array iterator.
template <class T>
class MtxIterBase
{
protected:
  MtxIterBase(T* d, int str, int n) :
    data(d), stride(str), stop(data+str*n) {}

public:
  typedef std::random_access_iterator_tag iterator_category;

  MtxIterBase(const MtxIterBase& other) :
    data(other.data), stride(other.stride), stop(other.stop) {}

  template <class U>
  explicit MtxIterBase(const MtxIterBase<U>& other) :
    data(other.data), stride(other.stride), stop(other.stop) {}

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
  friend class Slice;
  friend class Mtx;

  template <class U> friend class MtxIterBase;

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

/// Aggregation of initialization and storage policy enums.
struct WithPolicies
{
  enum InitPolicy { ZEROS, NO_INIT };
  enum StoragePolicy { COPY, BORROW, REFER };
};

// ####################################################################

/// MtxShape class holds number-of-rows/number-of-columns info.

class MtxShape
{
public:
  MtxShape(int mr, int nc) : mrows_(mr), ncols_(nc) {}

  int mrows() const { return mrows_; }
  int ncols() const { return ncols_; }

  int length() const { return (mrows_ > ncols_) ? mrows_ : ncols_; }

  int nelems() const { return mrows_*ncols_; }

private:
  int mrows_;
  int ncols_;
};

// ####################################################################

/// MtxSpecs class holds shape plus storage offset and rowstride info.

class MtxSpecs
{
public:
  MtxSpecs() : shape_(0, 0), rowstride_(0), offset_(0) {}

  MtxSpecs(const MtxShape& shape) :
    shape_(shape),
    rowstride_(shape.mrows()),
    offset_(0)
  {}

  MtxSpecs(int mrows, int ncols) :
    shape_(mrows, ncols),
    rowstride_(mrows),
    offset_(0)
  {}

  MtxSpecs(const MtxSpecs& other) :
    shape_(other.shape_),
    rowstride_(other.rowstride_),
    offset_(other.offset_)
  {}

  void swap(MtxSpecs& other);

  MtxSpecs as_shape(const MtxShape& s) const;

  MtxSpecs as_shape(int mr, int nc) const { return as_shape(MtxShape(mr,nc)); }

  void selectRows(const RowRange& rng);
  void selectCols(const ColRange& rng);

  MtxSpecs subRows(const RowRange& rng) const
  {
    MtxSpecs result(*this); result.selectRows(rng); return result;
  }

  MtxSpecs subCols(const ColRange& rng) const
  {
    MtxSpecs result(*this); result.selectCols(rng); return result;
  }

  const MtxShape& shape() const { return shape_; }

  ptrdiff_t offset() const { return offset_; }

  int length() const { return shape_.length(); }
  int nelems() const { return mrows()*ncols(); }

  int mrows() const { return shape_.mrows(); }
  int rowstride() const { return rowstride_; }

  int ncols() const { return shape_.ncols(); }
  int colstride() const { return colstride_; }

  unsigned int rowgap() const { return rowstride_ - mrows(); }

  int offsetFromStart(int row, int col) const
  {
    // Strictly speaking, the only valid offsets are those that pass
    // RC_inHalfOpen(), but in order to allow "one-past-the-end" indexing
    // for iterators etc., we use the more permissive RC_inFullOpen()
    // instead:
    RC_inFullOpen(row, 0, mrows());
    RC_inFullOpen(col, 0, ncols());

    return row + (col*rowstride_);
  }

  int offsetFromStart(int elem) const
  { return offsetFromStart(elem%mrows(), elem/mrows()); }

  int offsetFromStorage(int row, int col) const
  { return offset_ + offsetFromStart(row, col); }

  int offsetFromStorage(int elem) const
  { return offset_ + offsetFromStart(elem); }

private:
  MtxShape shape_;
  int rowstride_;
  static const int colstride_ = 1;
  ptrdiff_t offset_;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * DataHolder class.
 *
 **/
///////////////////////////////////////////////////////////////////////

class DataHolder : public WithPolicies
{
public:
  DataHolder(double* data, int mrows, int ncols, StoragePolicy s);

  DataHolder(int mrows, int ncols, InitPolicy p);

  DataHolder(mxArray* a, StoragePolicy s);

  /** With a const mxArray*, only BORROW or COPY are allowed as storage
      policies, in order to preserve const-correctness. */
  DataHolder(const mxArray* a, StoragePolicy s);

  DataHolder(const DataHolder& other);

  ~DataHolder();

  void swap(DataHolder& other);

  void makeUnique() { DataBlock::makeUnique(datablock_); }

  const double* storage() const { return datablock_->data(); }
  double* storage_nc() { makeUnique(); return datablock_->data_nc(); }

  int storageLength() const { return datablock_->length(); }

private:
  DataHolder& operator=(const DataHolder&); // not allowed

  DataBlock* datablock_;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * DataHolderRef class.
 *
 **/
///////////////////////////////////////////////////////////////////////

class DataHolderRef : public WithPolicies
{
public:
  DataHolderRef(DataHolder* ref) : ref_(ref) {}

  DataHolderRef(const DataHolderRef& other) : ref_(other.ref_) {}

  ~DataHolderRef() {}

  void swap(DataHolderRef& other);

  const double* storage() const { return ref_->storage(); }
  double* storage_nc() { return ref_->storage_nc(); }

  int storageLength() const { return ref_->storageLength(); }

private:
  DataHolderRef& operator=(const DataHolderRef&); // not allowed

  DataHolder* ref_;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * MtxBase class.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class Data>
class MtxBase : public MtxSpecs, public WithPolicies
{
private:
  MtxBase& operator=(const MtxBase& other); // not allowed

protected:
  Data data_;

  void swap(MtxBase& other);

  MtxBase(const MtxBase& other);

  MtxBase(int mrows, int ncols, const Data& data);

  MtxBase(const MtxSpecs& specs, const Data& data);

  ~MtxBase();

  const double& at(int i) const
  {
    RC_less(i+offset(), data_.storageLength());
    return data_.storage()[i+offset()];
  }

  double& at_nc(int i)
  {
    RC_less(i+offset(), data_.storageLength());
    return data_.storage_nc()[i+offset()];
  }

  ptrdiff_t offsetFromStorage(int r, int c) const
  { return RCR_leq(MtxSpecs::offsetFromStorage(r, c), data_.storageLength()); }

  double* address_nc(int row, int col)
  { return data_.storage_nc() + offsetFromStorage(row, col); }

  const double* address(int row, int col) const
  { return data_.storage() + offsetFromStorage(row, col); }

  // Does the same thing as address_nc(), but doesn't range-check, since
  // this result is assumed to be some kind of "one-past-the-end" offset.
  ptrdiff_t end_offsetFromStorage(int r, int c) const
  { return MtxSpecs::offsetFromStorage(r, c); }

  // Does the same thing as address_nc(), but doesn't range-check, since
  // this result is assumed to be some kind of "one-past-the-end" address.
  double* end_address_nc(int row, int col)
  { return data_.storage_nc() + end_offsetFromStorage(row, col); }

  // Does the same thing as address(), but doesn't range-check, since
  // this result is assumed to be some kind of "one-past-the-end" address.
  const double* end_address(int row, int col) const
  { return data_.storage() + end_offsetFromStorage(row, col); }

  const double* storage() const { return data_.storage(); }
  double* storage_nc() { return data_.storage_nc(); }

public:

  const MtxShape& shape() const { return specs().shape(); }
  const MtxSpecs& specs() const { return *this; }

#ifdef APPLY_IMPL
#  error macro error
#endif // APPLY_IMPL

  // This workaround is required because compilers don't seem to be
  // able to accept both functors as well as function pointers as
  // template arguments to a single apply() template
#define APPLY_IMPL \
 \
      double* p = data_.storage_nc() + offset(); \
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
  void applyF(F func)
  {
    APPLY_IMPL;
  }

  void apply(double func(double))
  {
    APPLY_IMPL;
  }

#undef APPLY_IMPL

  //
  // Iterators
  //

  /// Index-based iterator for Mtx class.
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

  friend class iter_base<MtxBase, double>;
  friend class iter_base<const MtxBase, const double>;

  typedef iter_base<MtxBase, double> iterator;
  typedef iter_base<const MtxBase, const double> const_iterator;

  iterator begin_nc() { return iterator(this, 0); }
  iterator end_nc() { return iterator(this, nelems()); }

  const_iterator begin() const { return const_iterator(this, 0); }
  const_iterator end() const { return const_iterator(this, nelems()); }


  /// Column-major iterator for Mtx class.
  template <class T>
  class colmaj_iter_base
  {
    int itsRowgap;
    int itsRowstride;
    T* itsPtr;
    T* itsCurrentEnd;

  public:

    typedef std::forward_iterator_tag iterator_category;

    typedef T            value_type;
    typedef ptrdiff_t    difference_type;
    typedef T*           pointer;
    typedef T&           reference;

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
  { return colmaj_iter(rowgap(), rowstride(), address_nc(0,0)); }

  colmaj_iter colmaj_end_nc()
  { return colmaj_iter(rowgap(), rowstride(), end_address_nc(0,ncols())); }

  const_colmaj_iter colmaj_begin() const
  { return const_colmaj_iter(rowgap(), rowstride(), address(0,0)); }

  const_colmaj_iter colmaj_end() const
  { return const_colmaj_iter(rowgap(), rowstride(), end_address(0,ncols())); }


  /// Row-major iterator for Mtx class.
  template <class T>
  class rowmaj_iter_base
  {
    int itsStride;
    T* itsCurrentStart;
    T* itsPtr;
    T* itsCurrentEnd;

  public:

    typedef std::forward_iterator_tag iterator_category;

    typedef T            value_type;
    typedef ptrdiff_t    difference_type;
    typedef T*           pointer;
    typedef T&           reference;

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
  { return rowmaj_iter(rowstride(), ncols(), address_nc(0,0)); }

  rowmaj_iter rowmaj_end_nc()
  { return rowmaj_iter(rowstride(), ncols(), address_nc(mrows(),0)); }

  const_rowmaj_iter rowmaj_begin() const
  { return const_rowmaj_iter(rowstride(), ncols(), address(0,0)); }

  const_rowmaj_iter rowmaj_end() const
  { return const_rowmaj_iter(rowstride(), ncols(), address(mrows(),0)); }


  //
  // Functions
  //

  /// Functor for "set all elements to x" operation.
  struct Setter
  {
    double v;
    Setter(double v_) : v(v_) {}
    double operator()(double) { return v; }
  };

  void setAll(double x) { applyF(Setter(x)); }

  void clear(double x = 0.0) { applyF(Setter(x)); }
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * SubMtxRef class definition
 *
 **/
///////////////////////////////////////////////////////////////////////

class SubMtxRef : public MtxBase<DataHolderRef>
{
public:
  typedef MtxBase<DataHolderRef> Base;

  SubMtxRef(const MtxSpecs& specs, DataHolder& ref) :
    Base(specs, DataHolderRef(&ref))
  {}

  SubMtxRef& operator=(const SubMtxRef& other);
  SubMtxRef& operator=(const Mtx& other);
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * Mtx class definition
 *
 **/
///////////////////////////////////////////////////////////////////////

class Mtx : public MtxBase<DataHolder>
{
public:

  typedef MtxBase<DataHolder> Base;

  //
  // Constructors + Conversion
  //

  static const Mtx& emptyMtx();

  Mtx(const MtxSpecs& specs, const DataHolder& data) :
    Base(specs, data)
  {}

  Mtx(mxArray* a, StoragePolicy s = COPY);

  /** With a const mxArray*, only BORROW or COPY are allowed as storage
      policies, in order to preserve const-correctness. */
  Mtx(const mxArray* a, StoragePolicy s = COPY);

  Mtx(double* data, int mrows, int ncols, StoragePolicy s = COPY);

  Mtx(const MtxShape& s, InitPolicy p = ZEROS);

  Mtx(int mrows, int ncols, InitPolicy p = ZEROS);

  Mtx(const Slice& s);

  Mtx(const Mtx& other) : Base(other) {}

  virtual ~Mtx();

  Mtx& operator=(const Mtx& other)
  {
    Mtx temp(other);
    Base::swap(temp);
    return *this;
  }

  // This will destroy any data in the process of changing the size of
  // the Mtx to the specified dimensions; its only advantage over just
  // declaring a new Mtx is that it will avoid a deallocate/allocate
  // cycle if the new dimensions are the same as the current dimensions.
  void resize(int mrowsNew, int ncolsNew);

  /** Makes sure that the data are in contiguous storage; if called on a
      submatrix, contig() will make a new matrix of the proper size and copy
      the elements there; otherwise, it will just return the current
      matrix. */
  Mtx contig() const;

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


  //
  // Data access
  //

  double& at(int row, int col)
    { return Base::at_nc(offsetFromStart(row, col)); }

  const double& at(int row, int col) const
    { return Base::at(offsetFromStart(row, col)); }

  double& at(int elem)
    { return Base::at_nc(offsetFromStart(elem)); }

  const double& at(int elem) const
    { return Base::at(offsetFromStart(elem)); }

  bool sameSize(const Mtx& x) const
  { return (mrows() == x.mrows()) && (ncols() == x.ncols()); }

  //
  // Slices, submatrices
  //

  SubMtxRef sub(const RowRange& rng)
  {
    return SubMtxRef(this->specs().subRows(rng), this->data_);
  }

  SubMtxRef sub(const ColRange& rng)
  {
    return SubMtxRef(this->specs().subCols(rng), this->data_);
  }

  SubMtxRef sub(const RowRange& rr, const ColRange& cc)
  {
    return SubMtxRef(this->specs().subRows(rr).subCols(cc), this->data_);
  }


  Mtx sub(const RowRange& rng) const
  {
    return Mtx(this->specs().subRows(rng), this->data_);
  }

  Mtx sub(const ColRange& rng) const
  {
    return Mtx(this->specs().subCols(rng), this->data_);
  }

  Mtx sub(const RowRange& rr, const ColRange& cc) const
  {
    return Mtx(this->specs().subRows(rr).subCols(cc), this->data_);
  }


  Mtx operator()(const RowRange& rng) const { return sub(rng); }

  Mtx operator()(const ColRange& rng) const { return sub(rng); }

  Mtx operator()(const RowRange& rr, const ColRange& cc) const
  { return sub(rr, cc); }


  Mtx as_shape(const MtxShape& s) const
  { return Mtx(this->specs().as_shape(s), this->data_); }

  Mtx as_shape(int mr, int nc) const
  { return Mtx(this->specs().as_shape(mr, nc), this->data_); }

  Slice row(int r) const
    { return Slice(*this, offsetFromStorage(r,0), rowstride(), ncols()); }

  MtxIter rowIter(int r)
    { return MtxIter(address_nc(r,0), rowstride(), ncols()); }

  MtxConstIter rowIter(int r) const
    { return MtxConstIter(address(r,0), rowstride(), ncols()); }

  Mtx asRow() const { return as_shape(1, nelems()); }

  void reorderRows(const Mtx& index);



  Slice column(int c) const
    { return Slice(*this, offsetFromStorage(0,c), colstride(), mrows()); }

  MtxIter columnIter(int c)
    { return MtxIter(address_nc(0,c), colstride(), mrows()); }

  MtxConstIter columnIter(int c) const
    { return MtxConstIter(address(0,c), colstride(), mrows()); }

  Mtx asColumn() const { return as_shape(nelems(), 1); }

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

  void makeUnique() { Base::data_.makeUnique(); }

private:
  friend class Slice;
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
