///////////////////////////////////////////////////////////////////////
// stlutils.h
// Rob Peters
// created: Thu May 13 14:08:52 1999
// written: Thu May 13 15:30:31 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef STLUTILS_H_DEFINED
#define STLUTILS_H_DEFINED

template<class T>
class AddTo {
  T& val;
public:
  AddTo(T& i) : val(i) {}
  void operator() (const T& i) { val += i; }
};

template<class T>
T* DeletePtr(void* p) {
  delete static_cast<T *>(p);
  return 0;
}

static const char vcid_stlutils_h[] = "$Header$";
#endif // !STLUTILS_H_DEFINED
