///////////////////////////////////////////////////////////////////////
//
// tclitempkg.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 12:33:59 1999
// written: Mon Jul 16 09:03:29 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKG_H_DEFINED
#define TCLITEMPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJFUNCTOR_H_DEFINED)
#include "tcl/objfunctor.h"
#endif

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

  /// Factory function.
  static shared_ptr<Getter<T> > make(Getter_f s)
    { return shared_ptr<Getter<T> >(new CGetter(s)); }

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

  /// Factory function.
  static shared_ptr<Getter<int> > make(Getter_f s)
    { return shared_ptr<Getter<int> >(new CGetter(s)); }

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

  /// Factory function.
  static shared_ptr<Getter<int> > make(Getter_f s)
    { return shared_ptr<Getter<int> >(new CGetter(s)); }

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

  /// Factory function.
  static shared_ptr<Setter<T> > make(Setter_f s)
    { return shared_ptr<Setter<T> >(new CSetter(s)); }

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

  /// Factory function.
  static shared_ptr<Setter<int> > make(Setter_f s)
    { return shared_ptr<Setter<int> >(new CSetter(s)); }

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

  /// Factory function.
  static shared_ptr<Setter<int> > make(Setter_f s)
    { return shared_ptr<Setter<int> >(new CSetter(s)); }

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

namespace Tcl
{

///////////////////////////////////////////////////////////////////////
/**
 *
 * TclItemPkg class definition
 *
 **/
///////////////////////////////////////////////////////////////////////

class TclItemPkg : public ItemFetcher {
public:
  TclItemPkg(Tcl_Interp* interp, const char* name, const char* version);

  virtual ~TclItemPkg();

  virtual void* getItemFromId(int id) = 0;

  virtual void* getItemFromContext(Context& ctx);

protected:
  template <class T>
  void declareGetter(const char* cmd_name, shared_ptr<Getter<T> > getter,
                     const char* usage = 0);

  template <class T>
  void declareSetter(const char* cmd_name, shared_ptr<Setter<T> > setter,
                     const char* usage = 0);

  template <class T>
  void declareAttrib(const char* attrib_name,
                     shared_ptr<Getter<T> > getter,
                     shared_ptr<Setter<T> > setter,
                     const char* usage = 0);

  void declareAction(const char* action_name, shared_ptr<Action> action,
                     const char* usage = 0);

  void addIoCommands();

  static const char* actionUsage(const char* usage);
  static const char* getterUsage(const char* usage);
  static const char* setterUsage(const char* usage);

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
  CTclItemPkg(Tcl_Interp* interp, const char* name, const char* version) :
    TclItemPkg(interp, name, version) {}

  void declareCAction(const char* cmd_name, void (C::* actionFunc) (),
                      const char* usage = 0)
    {
      Tcl::defVec( this, actionFunc,
                   makePkgCmdName(cmd_name), actionUsage(usage) );
    }

  void declareCAction(const char* cmd_name, void (C::* actionFunc) () const,
                      const char* usage = 0)
    {
      Tcl::defVec( this, actionFunc,
                   makePkgCmdName(cmd_name), actionUsage(usage) );
    }

  template <class CC, class T>
  void declareCGetter(const char* cmd_name, T (CC::* getterFunc) () const,
                      const char* usage = 0)
    {
      Tcl::defVec( this, getterFunc,
                   makePkgCmdName(cmd_name), getterUsage(usage) );
    }

  template <class CC, class T>
  void declareCSetter(const char* cmd_name, void (CC::* setterFunc) (T),
                      const char* usage = 0)
    {
      Tcl::defVec( this, setterFunc,
                   makePkgCmdName(cmd_name), setterUsage(usage) );
    }

  template <class CC, class T>
  void declareCAttrib(const char* cmd_name,
                      T (CC::* getterFunc) () const,
                      void (CC::* setterFunc) (T),
                      const char* usage = 0)
    {
      Tcl::defVec( this, getterFunc,
                   makePkgCmdName(cmd_name), getterUsage(usage) );
      Tcl::defVec( this, setterFunc,
                   makePkgCmdName(cmd_name), setterUsage(usage) );
    }

  virtual C* getCItemFromId(int id) = 0;
  virtual void* getItemFromId(int id)
  {
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

  C* getItem(Context& ctx)
  {
    int id = ctx.getIntFromArg(1);
    return itsPkg->getCItemFromId(id);
  }

private:
  TclItemCmd(const TclItemCmd&);
  TclItemCmd& operator=(const TclItemCmd&);

  CTclItemPkg<C>* itsPkg;
};

} // end namespace Tcl

static const char vcid_tclitempkg_h[] = "$Header$";
#endif // !TCLITEMPKG_H_DEFINED
