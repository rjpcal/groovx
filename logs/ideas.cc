
class GxLine : public GxNode, public FieldContainer {
public:

  /// Draw the object on \a canvas.
  virtual void draw(GWT::Canvas& canvas) const = 0;

  /// Undraw the object from \a canvas.
  virtual void undraw(GWT::Canvas& canvas) const = 0;

  GxVec3 head;
  GxVec3 tail;
};

class GxRect {
public:
  GxVec3 lowerLeft;
  GxVec3 upperRight;
};

class GxCircle {
public:
  GxVec3 center;
  TField<double> radius;
};

class GxQuadric;

class GxLineStrip;

class GxTriangleStrip;

class GxQuadStrip;

//---------------------------------------------------------------------
//
// GenericItem
//
//---------------------------------------------------------------------

class ItemImpl { virtual ~ItemImpl(); }

template <class C>
class CItemImpl {
public:
  C* itsItem;
};

class Item {
public:
  template <class C>
  C* castTo(C*/*dummy*/)
  {
    CItemImpl<C>* cimpl = dynamic_cast<CItemImpl<C>*>(itsImpl);
    if (cimpl)
      return cimpl->itsItem;
    else
      throw "...";
  }

  ItemImpl* itsImpl;
};

//---------------------------------------------------------------------
//
// MetaClasses
//
//---------------------------------------------------------------------

class Face {
public:

  // The metaclass, like a singleton, is initialized on the first call
  // to this function
  static const MetaClass& getMetaClass();
}

const MetaClass& mc = Face::getMetaClass();

template <class C>
class Constructor {
  C* invoke(const vector<Value*>& args) = 0;
};

class Function {
public:
  Value* invoke(const vector<Value*>& args) = 0;

private:
  const int itsObjcMin;
  const int itsObjcMax;
};

// Arg holds a reference to the real place where the value should be stored
template <class T>
class Arg {
public:
  Arg(T& param, const char* name, T default_val = T()) :
    itsParam(param),
    itsName(name),
    itsDefault(default_val)
    {}
//   static const Arg NONE;
//   bool operator==(const Arg& rhs) { return this == &rhs; }
private:
  T& itsParam;
};

//---------------------------------------------------------------------
//
// Dynamically-typed functions
//
//---------------------------------------------------------------------
#include <iostream.h>
#include <cstring>
#include <typeinfo>

class A {
public:
  int a;
  double b;

  void d(const double& arg) { cout << "arg == " << arg << endl; }

  int f(const char* arg) {
    cout << "arg == " << arg << endl;
    return strlen(arg);
  }
};

class B {
public:
  int b;
};

class MFPtrBase {
public:
  virtual ~MFPtrBase() {}

  virtual const char* describe() = 0;

  template <typename MF>
  static MFPtrBase* make(MF mf);

  class Void_t { };

  static const Void_t VOID;

  template <class C, typename arg1_t>
  void invoke(C* obj, const arg1_t& arg1, Void_t/*dummy*/);

  template <class C, typename arg1_t, typename ret_t>
  ret_t invoke(C* obj, const arg1_t& arg1, ret_t*/*dummy*/);
};

const MFPtrBase::Void_t MFPtrBase::VOID;

template <typename MF>
class MFPtr : public MFPtrBase {
public:

  MFPtr(MF mf) : itsMF(mf) {}

  virtual const char* describe()
  { return typeid(itsMF).name(); }

  MF itsMF;
};

template <class C, typename arg1_t, typename ret_t>
ret_t MFPtrBase::invoke(C* obj, const arg1_t& arg1, ret_t*/*dummy*/)
{
  typedef ret_t (C::* MF) (arg1_t);
  MFPtr<MF>* mfptr = dynamic_cast<MFPtr<MF>*>(this);
  if (mfptr != 0) {
    MF mf = mfptr->itsMF;
    return (obj->*mf)(arg1);
  }
  else {
    typedef ret_t (C::* MF) (const arg1_t&);
    MFPtr<MF>* mfptr = dynamic_cast<MFPtr<MF>*>(this);
    if (mfptr != 0) {
      MF mf = mfptr->itsMF;
      return (obj->*mf)(arg1);
    }
    else {
      throw "cast failed";
    }
  }
}

template <class C, typename arg1_t>
void MFPtrBase::invoke(C* obj, const arg1_t& arg1, MFPtrBase::Void_t/*dummy*/)
{
  typedef void (C::* MF) (arg1_t);
  MFPtr<MF>* mfptr = dynamic_cast<MFPtr<MF>*>(this);
  if (mfptr != 0) {
    MF mf = mfptr->itsMF;
    (obj->*mf)(arg1);
  }
  else {
    typedef void (C::* MF) (const arg1_t&);
    MFPtr<MF>* mfptr = dynamic_cast<MFPtr<MF>*>(this);
    if (mfptr != 0) {
      MF mf = mfptr->itsMF;
      (obj->*mf)(arg1);
    }
    else {
      throw "cast failed";
    }
  }
}

template <typename MF>
MFPtrBase* MFPtrBase::make(MF mf)
{ return new MFPtr<MF>(mf); }


int main() {
  MFPtrBase* p = MFPtrBase::make(&A::d);

  MFPtrBase* q = MFPtrBase::make(&A::f);

  cout << p->describe() << endl;

  A a;

  B b;

  try {
    p->invoke(&a, 1.0, MFPtrBase::VOID);
    const char* str = "Hello, World!";
    int retval = q->invoke(&a, str, (int*)0);
    cerr << "retval == " << retval << endl;
    p->invoke(&b, 1.0, MFPtrBase::VOID);
  }
  catch (const char* msg) {
    cout << msg << endl;
  }

  return 0;
}

template <class T>
class Cached {
public:

  typedef void UpdateFunction(T&);

  Cached(UpdateFunction* funcPtr) :
    itsRep(),
    itsIsCurrent(false),
    itsUpdateFunction(funcPtr)
    {}

  const T& operator()() const { update(); return itsRep; }

  void invalidate() { itsIsCurrent = false; }

  bool isCurrent() const { return itsIsCurrent; }

protected:

  void update()
    {
      if (!itsIsCurrent) itsUpdateFunction(itsRep);
      itsIsCurrent = true;
    }

private:
  mutable T itsRep;
  mutable bool itsIsCurrent;
  UpdateFunction* const itsUpdateFunction;
};

class ExperimentSession {
public:
  ExperimentSession(const char* filename_stem);

  void runSession()
    {
      const char* subject_name = doLoginSubject();
      itsExptDriver.setSubject(subject_name);

      while (doHaveMoreExperiments())
        {
          doNextExperiment();
          moveExptFile();
          moveRespFile();
        }

      doQuit();
    }

  virtual const char* doLoginSubject() = 0;
  virtual bool doHaveMoreExperiments() = 0;
  virtual void doNextExperiment() = 0;

private:
  void moveFile(const string& source, const string& dest);

  void moveExptFile()
  void moveRespFile();

  ExptDriver& itsExptDriver;
  string itsFilenameStem;
};

template <class C, class T>
class PropertyAdapter {
  typedef T (C::* GetterFunc) () const;
  typedef void (C::* SetterFunc) (T);
public:
  PropertyAdapter(T& valRef, GetterFunc getter, SetterFunc setter) :
    itsVal(valRef), itsGetter(getter), itsSetter(setter() {}

  virtual void set(const Value& val);
  virtual const Value& get() const;

private:
  TValue<T> itsVal;
  GetterFunc itsGetter;
  SetterFunc itsSetter;
};

// Manipulator represents a binding between a property pointer, and an
// object. It should be possible to reseat either the property or the
// object (maybe?). Maybe an object should be responsible for returning
// a pointer or reference for a property that is bound to itself.
template <class C>
class Manipulator {
  Property C::* itsProperty;
  C* itsObject;

  void set(const Value& val) { itsObject->set(itsProperty, val); }
  const Value& get() { return itsObject->get(itsProperty); }
};
