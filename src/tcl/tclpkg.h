///////////////////////////////////////////////////////////////////////
//
// tclitempkg.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 12:33:59 1999
// written: Thu Jul 12 13:10:09 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKG_H_DEFINED
#define TCLITEMPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLITEMPKGBASE_H_DEFINED)
#include "tcl/tclitempkgbase.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLCMD_H_DEFINED)
#include "tcl/tclcmd.h"
#endif

namespace IO { class IoObject; }

namespace Util
{
  template <class T> class Ref;
  template <class T> class WeakRef;
};

///////////////////////////////////////////////////////////////////////
//
// Getter/Setter/Action template functor implentations
//
///////////////////////////////////////////////////////////////////////

/**
 *
 * \c CGetter implements the \c Getter interface by calling a member
 * function of the template class \c C.
 *
 **/
template <class C, class T>
class CGetter : public Getter<T> {
public:
  /// The type of member function pointer that will be called in \c get().
  typedef T (C::* Getter_f) () const;

  /// Construct with a member function pointer.
  CGetter(Getter_f getter) : itsGetter_f(getter) {}

  /// Casts \a item to type \c C* and calls the stored member function.
  virtual T get(void *item) const {
    const C* p = static_cast<C*>(item);
    return (p->*itsGetter_f)();
  }

private:
  CGetter(const CGetter&);
  CGetter& operator=(const CGetter&);

  Getter_f itsGetter_f;
};

// Partial specialization of CGetter for T == Util::Ref<T>
template <class C, class T>
class CGetter<C, Util::Ref<T> > : public Getter<int> {
public:
  /// The type of member function pointer that will be called in \c get().
  typedef Util::Ref<T> (C::* Getter_f) () const;

  /// Construct with a member function pointer.
  CGetter<C, Util::Ref<T> >(Getter_f getter) : itsGetter_f(getter) {}

  /// Casts \a item to type \c C* and calls the stored member function.
  virtual int get(void *item) const {
    const C* p = static_cast<C*>(item);
    return (p->*itsGetter_f)()->id();
  }

private:
  CGetter<C, Util::Ref<T> >(const CGetter<C, Util::Ref<T> >&);
  CGetter<C, Util::Ref<T> >& operator=(const CGetter<C, Util::Ref<T> >&);

  Getter_f itsGetter_f;
};

// Partial specialization of CGetter for T == Util::WeakRef<T>
template <class C, class T>
class CGetter<C, Util::WeakRef<T> > : public Getter<int> {
public:
  /// The type of member function pointer that will be called in \c get().
  typedef Util::WeakRef<T> (C::* Getter_f) () const;

  /// Construct with a member function pointer.
  CGetter<C, Util::WeakRef<T> >(Getter_f getter) : itsGetter_f(getter) {}

  /// Casts \a item to type \c C* and calls the stored member function.
  virtual int get(void *item) const {
    const C* p = static_cast<C*>(item);
    return (p->*itsGetter_f)().id();
  }

private:
  CGetter<C, Util::WeakRef<T> >(const CGetter<C, Util::WeakRef<T> >&);
  CGetter<C, Util::WeakRef<T> >& operator=(const CGetter<C, Util::WeakRef<T> >&);

  Getter_f itsGetter_f;
};

/**
 *
 * \c CSetter implements the \c Setter interface by calling a member
 * function of the template class \c C.
 *
 **/
template <class C, class T>
class CSetter : public Setter<T> {
public:
  /// The type of member function pointer that will be called in \c set().
  typedef void (C::* Setter_f) (T);

  /// Construct with a member function pointer.
  CSetter(Setter_f setter) : itsSetter_f(setter) {}

  /// Casts \a item to type \c C* and calls the stored member function.
  virtual void set(void* item, T val) {
    C* p = static_cast<C*>(item);
    (p->*itsSetter_f)(val);
  }

private:
  CSetter(const CSetter&);
  CSetter& operator=(const CSetter&);

  Setter_f itsSetter_f;
};


// Specialization of CSetter for T == Util::Ref<T>
template <class C, class T>
class CSetter<C, Util::Ref<T> > : public Setter<int> {
public:
  /// The type of member function pointer that will be called in \c set().
  typedef void (C::* Setter_f) (Util::Ref<T>);

  /// Construct with a member function pointer.
  CSetter<C, Util::Ref<T> >(Setter_f setter) : itsSetter_f(setter) {}

  /// Casts \a item to type \c C* and calls the stored member function.
  virtual void set(void* item, int val) {
    C* p = static_cast<C*>(item);
    (p->*itsSetter_f)(Util::Ref<T>(val));
  }

private:
  CSetter<C, Util::Ref<T> >(const CSetter<C, Util::Ref<T> >&);
  CSetter<C, Util::Ref<T> >& operator=(const CSetter<C, Util::Ref<T> >&);

  Setter_f itsSetter_f;
};

// Specialization of CSetter for T == Util::WeakRef<T>
template <class C, class T>
class CSetter<C, Util::WeakRef<T> > : public Setter<int> {
public:
  /// The type of member function pointer that will be called in \c set().
  typedef void (C::* Setter_f) (Util::WeakRef<T>);

  /// Construct with a member function pointer.
  CSetter<C, Util::WeakRef<T> >(Setter_f setter) : itsSetter_f(setter) {}

  /// Casts \a item to type \c C* and calls the stored member function.
  virtual void set(void* item, int val) {
    C* p = static_cast<C*>(item);
    (p->*itsSetter_f)(Util::WeakRef<T>(val));
  }

private:
  CSetter<C, Util::WeakRef<T> >(const CSetter<C, Util::WeakRef<T> >&);
  CSetter<C, Util::WeakRef<T> >& operator=(const CSetter<C, Util::WeakRef<T> >&);

  Setter_f itsSetter_f;
};

/**
 *
 * \c CAction implements the \c Action interface by calling a member
 * function of the template class \c C.
 *
 **/
template <class C>
class CAction: public Action {
public:
  /// The type of member function pointer that will be called in \c action().
  typedef void (C::* Action_f) ();

  /// Construct with a member function pointer.
  CAction(Action_f action_f) : itsAction_f(action_f) {}

  /// Casts \a item to type \c C* and calls the stored member function.
  virtual void action(void* item) {
    C* p = static_cast<C*>(item);
    (p->*itsAction_f)();
  }

private:
  CAction(const CAction&);
  CAction& operator=(const CAction&);

  Action_f itsAction_f;
};

/**
 *
 * \c CConstAction implements the \c Action interface by calling a
 * const member function of the template class \c C.
 *
 **/
template <class C>
class CConstAction : public Action {
public:
  /// The type of member function pointer that will be called in \c action().
  typedef void (C::* Action_f) () const;

  /// Construct with a member function pointer.
  CConstAction(Action_f action_f) : itsAction_f(action_f) {}

  /// Casts \a item to type \c const \c C* and calls the stored member function.
  virtual void action(void* item) {
    const C* p = static_cast<C*>(item);
    (p->*itsAction_f)();
  }

private:
  Action_f itsAction_f;
};


///////////////////////////////////////////////////////////////////////
//
// Begin Tcl namespace ...
//
///////////////////////////////////////////////////////////////////////

namespace Tcl {

///////////////////////////////////////////////////////////////////////
/**
 *
 * IoFetcher class definition
 *
 * This class exists to provide an interface for retrieving
 * IO::IoObject's from integer id's.
 *
 **/
///////////////////////////////////////////////////////////////////////

class IoFetcher {
protected:
  IoFetcher();

public:
  virtual ~IoFetcher();

  virtual IO::IoObject& getIoFromId(int id) = 0;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * TclItemPkg class definition
 *
 **/
///////////////////////////////////////////////////////////////////////

class TclItemPkg : public TclItemPkgBase {
public:
  TclItemPkg(Tcl_Interp* interp, const char* name, const char* version,
             int item_argn=1);

  virtual ~TclItemPkg();

  virtual void* getItemFromId(int id) = 0;

protected:
  template <class T>
  void declareGetter(const char* cmd_name, Getter<T>* getter,
                     const char* usage = 0);

  template <class T>
  void declareSetter(const char* cmd_name, Setter<T>* setter,
                     const char* usage = 0);

  template <class T>
  void declareAttrib(const char* attrib_name,
                     Getter<T>* getter, Setter<T>* setter,
                     const char* usage = 0);

  void declareAction(const char* action_name, Action* action,
                     const char* usage = 0);

  void addIoCommands(IoFetcher* fetcher);

private:
  void instantiate();
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * CTclItemPkg class definition
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class C>
class CTclItemPkg : public TclItemPkg {
public:
  CTclItemPkg(Tcl_Interp* interp, const char* name, const char* version,
              int item_argn=1) :
    TclItemPkg(interp, name, version, item_argn) {}

  void declareCAction(const char* cmd_name, void (C::* actionFunc) (),
                      const char* usage = 0)
    {
      declareAction(cmd_name, new CAction<C>(actionFunc), usage);
    }

  void declareCAction(const char* cmd_name, void (C::* actionFunc) () const,
                      const char* usage = 0)
    {
      declareAction(cmd_name, new CConstAction<C>(actionFunc), usage);
    }

  template <class CC, class T>
  void declareCGetter(const char* cmd_name, T (CC::* getterFunc) () const,
                      const char* usage = 0)
    {
      declareGetter(cmd_name, new CGetter<C,T>(getterFunc), usage);
    }

  template <class CC, class T>
  void declareCSetter(const char* cmd_name, void (CC::* setterFunc) (T),
                      const char* usage = 0)
    {
      declareSetter(cmd_name, new CSetter<C,T>(setterFunc), usage);
    }

  template <class CC, class T>
  void declareCAttrib(const char* cmd_name,
                      T (CC::* getterFunc) () const,
                      void (CC::* setterFunc) (T),
                      const char* usage = 0)
    {
      declareAttrib(cmd_name,
                    new CGetter<C,T>(getterFunc), new CSetter<C,T>(setterFunc),
                    usage);
    }

  virtual C* getCItemFromId(int id) = 0;
  virtual void* getItemFromId(int id) {
    return static_cast<void*>(getCItemFromId(id));
  }
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * TclItemCmd class definition
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class C>
class TclItemCmd : public TclCmd {
public:
  TclItemCmd(CTclItemPkg<C>* pkg, const char* cmd_name, const char* usage,
             int objc_min=0, int objc_max=-1, bool exact_objc=false) :
    TclCmd(pkg->interp(), cmd_name, usage, objc_min, objc_max, exact_objc),
    itsPkg(pkg) {}
protected:
  virtual void invoke(Context& ctx) = 0;

  C* getItem(Context& ctx) {
    int id = itsPkg->itemArgn() ? ctx.getIntFromArg(itsPkg->itemArgn()) : -1;
    return itsPkg->getCItemFromId(id);
  }

  int afterItemArg(int n) { return itsPkg->itemArgn() + n; }

private:
  TclItemCmd(const TclItemCmd&);
  TclItemCmd& operator=(const TclItemCmd&);

  CTclItemPkg<C>* itsPkg;
};

} // end namespace Tcl

static const char vcid_tclitempkg_h[] = "$Header$";
#endif // !TCLITEMPKG_H_DEFINED
