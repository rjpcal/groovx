///////////////////////////////////////////////////////////////////////
//
// tclitempkgbase.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Dec 16 15:25:06 1999
// written: Thu Dec 16 15:37:00 1999
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

template <class T>
class Getter {
public:
  virtual T get(void* item) const = 0;
};

template <class T>
class Setter {
public:
  virtual void set(void* item, T val) = 0;
};

template <class T>
class Attrib : public virtual Getter<T>, public virtual Setter<T> {};

class Action {
public:
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
