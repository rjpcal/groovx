///////////////////////////////////////////////////////////////////////
//
// tclitempkgbase.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Dec 16 15:25:06 1999
// written: Thu Dec 16 15:26:54 1999
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
//
// TclItemPkg class definition
//
///////////////////////////////////////////////////////////////////////

class TclItemPkg : public TclPkg {
public:
  TclItemPkg(Tcl_Interp* interp, const char* name, const char* version,
				 int item_argn=1) :
	 TclPkg(interp, name, version), 
	 itsItemArgn(item_argn) {}

  virtual void* getItemFromId(int id) = 0;

  int itemArgn() const { return itsItemArgn; }
  
protected:
  template <class T>
  void declareGetter(const char* cmd_name, Getter<T>* getter,
							const char* usage = 0)
  { declareGetter_(this, cmd_name, getter, usage); }
  
  template <class T>
  void declareSetter(const char* cmd_name, Setter<T>* setter,
							const char* usage = 0)
  { declareSetter_(this, cmd_name, setter, usage); }

  template <class T>
  void declareAttrib(const char* attrib_name, Attrib<T>* attrib,
							const char* usage = 0)
  { declareAttrib_(this, attrib_name, attrib, usage); }

  void declareAction(const char* action_name, Action* action,
							const char* usage = 0);

private:
  template <class T>
  static void declareGetter_(TclItemPkg*, const char*, Getter<T>*, const char*);

  template <class T>
  static void declareSetter_(TclItemPkg*, const char*, Setter<T>*, const char*);

  template <class T>
  static void declareAttrib_(TclItemPkg*, const char*, Attrib<T>*, const char*);

  void instantiate();

  int itsItemArgn;
};

} // end namespace Tcl

static const char vcid_tclitempkgbase_h[] = "$Header$";
#endif // !TCLITEMPKGBASE_H_DEFINED
