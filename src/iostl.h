///////////////////////////////////////////////////////////////////////
// iostl.h
// Rob Peters
// created: Sat Mar 13 15:20:43 1999
// written: Thu Jun 24 18:10:46 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef IOSTL_H_DEFINED
#define IOSTL_H_DEFINED

#ifndef IO_H_DEFINED
#include "io.h"
#endif

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

// This template class serves as a wrapper of builtin types to provide
// them with the IO interface.
template<class T, char sep=' '>
class IoWrapper : public virtual IO {
public:
  IoWrapper(T val=T()) : itsVal(val) {}

  virtual void serialize(ostream& os, IOFlag) const {
	 os << itsVal << sep;
  }
  virtual void deserialize(istream& is, IOFlag) {
	 is >> itsVal;
  }
  virtual int charCount() const { return gCharCount<T>(itsVal); }

  T& operator() () { return itsVal; }
  const T& operator() () const { return itsVal; }

private:
  T itsVal;
};

// Specialization of IoWrapper for vectors of basic types. Here it is
// assumed that class T supports operator >> and operator << for input
// and output.
template<class T, char sep>
class IoWrapper<vector<T> > : public virtual IO {
public:
  IoWrapper<vector<T> >() : itsVec() {}

  virtual void serialize(ostream& os, IOFlag) const {
	 os << itsVec.size() << sep << sep;
	 for (int i = 0; i < itsVec.size(); ++i) {
		os << itsVec[i] << sep;
	 }
	 if (os.fail()) throw OutputError("IoWrapper<vector<T> >");
  }

  virtual void deserialize(istream& is, IOFlag) {
	 int size;
	 is >> size;
	 itsVec.resize(size, 0);
	 for (int i = 0; i < size; ++i) {
		is >> itsVec[i];
	 }
	 if (is.fail()) throw InputError("IoWrapper<vector<T> >");
  }

  virtual int charCount() const {

	 // For some reason, the aCC compiler crashes on a direct
	 // initialization of count with the template parameter explicitly
	 // specified like this:
	 //
	 //	 int count = ::charCount<int>(itsVec.size()) + 1; // crashes
	 // 
	 // However, either removing the explicit template specification or
	 // putting the declaration and assignment of 'count' in separate
	 // statements avoids the bug.
	 //
	 // Due to a bug in the aCC compiler, 'count' cannot be directly
	 // initialized with ::
	 //
	 //	 int count = ::charCount(itsVec.size()) + 1; // OK

	 int count=0;
	 count = gCharCount<int>(itsVec.size()) + 1;

	 for (int i = 0; i < itsVec.size(); ++i) {
		count += gCharCount<T>(itsVec[i]);
		++count; // for separator
	 }
	 count += 5; // fudge factor
	 return count;
  }

  vector<T>& operator() () { return itsVec; }
  const vector<T>& operator() () const { return itsVec; }

private:
  vector<T> itsVec;
};

// Specialization of IoWrapper for vectors of pointers. The objects
// pointed to must be derived from IO.
template<class T, char sep>
class IoWrapper<vector<T *> > : public virtual IO {
public:
  IoWrapper<vector<T *> >() : itsVec() {}

  virtual void serialize(ostream& os, IOFlag flag) const;

  virtual void deserialize(istream& is, IOFlag flag);

  virtual int charCount() const;

  vector<T *>& operator() () { return itsVec; }
  const vector<T *>& operator() () const { return itsVec; }

private:
  vector<T *> itsVec;
};

void serializeVecInt(ostream &os, const vector<int>& vec);
void deserializeVecInt(istream &is, vector<int>& vec);
int charCountVecInt(const vector<int>& vec);

static const char vcid_iostl_h[] = "$Header$";
#endif // !IOSTL_H_DEFINED
