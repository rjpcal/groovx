///////////////////////////////////////////////////////////////////////
//
// tclitempkgbase.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Dec 16 15:25:06 1999
// written: Tue Feb 15 10:29:44 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKGBASE_H_DEFINED
#define TCLITEMPKGBASE_H_DEFINED

#ifndef TCLPKG_H_DEFINED
#include "tclpkg.h"
#endif

struct Tcl_Interp;

///////////////////////////////////////////////////////////////////////
//
// Attrib template class definitions
//
///////////////////////////////////////////////////////////////////////

/** A templated abstract base class that represents getters (const
    member functions that take no arguments and return a value of type
    \c T.) */
template <class T>
class Getter {
public:
  /** Should be overridden by subclasses to cast \a item to the
      appropriate type, then call the appropriate member and return
      its result. */
  virtual T get(void* item) const = 0;
};


/** A templated abstract base class that represents setters (non-const
    member functions that take a single argument of type \c T.*/
template <class T>
class Setter {
public:
  /** Should be overridden by subclasses to cast \a item to the
      appropriate type, then call the appropriate member with \a val
      as the lone argument. */
  virtual void set(void* item, T val) = 0;
};


/** An abstract base class that represent an read-write attribute. */
template <class T>
class Attrib : public virtual Getter<T>, public virtual Setter<T> {};


/** An abstract base class for representing actions (member functions
    with no argument and no return value). */
class Action {
public:
  /** Should be overridden by subclasses to cast \a item to the
      appropriate type, then perform an appropriate action on the result. */
  virtual void action(void* item) = 0;
};

namespace Tcl {

///////////////////////////////////////////////////////////////////////
/**
 *
 * TclItemPkgBase class definition
 *
 **/
///////////////////////////////////////////////////////////////////////

class TclItemPkgBase : public TclPkg {
public:
  TclItemPkgBase(Tcl_Interp* interp, const char* name, const char* version) :
	 TclPkg(interp, name, version) {}

  virtual void* getItemFromId(int id) = 0;
};

} // end namespace Tcl

static const char vcid_tclitempkgbase_h[] = "$Header$";
#endif // !TCLITEMPKGBASE_H_DEFINED
