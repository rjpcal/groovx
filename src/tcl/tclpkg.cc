///////////////////////////////////////////////////////////////////////
//
// tclitempkg.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 12:33:54 1999
// written: Thu Jun 17 18:32:38 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKG_CC_DEFINED
#define TCLITEMPKG_CC_DEFINED

#include "tclitempkg.h"

#include "attribcmd.h"

///////////////////////////////////////////////////////////////////////
//
// PkgAttribCmd class definition
//
///////////////////////////////////////////////////////////////////////

class PkgAttribCmd : public AttribCmd {
public:
  PkgAttribCmd(TclItemPkg* pkg, const char* cmd_name) :
	 AttribCmd(pkg->interp(), cmd_name, "item_id ?new_value?", 2, 3),
	 itsPkg(pkg),
	 itsItem(0) {}

protected:
  virtual void get() = 0;
  virtual void set() = 0;

  TclItemPkg* pkg() { return itsPkg; }
  void* item() { return itsItem; }
  
private:
  virtual void invoke() {
	 int id = getIntFromArg(1);
	 itsItem = itsPkg->getItemFromId(id);

	 AttribCmd::invoke();
  }

  TclItemPkg* itsPkg;
  void* itsItem;
};

///////////////////////////////////////////////////////////////////////
//
// Subclasses of PkgAttribCmd
//
///////////////////////////////////////////////////////////////////////

template <class T>
class TAttribCmd : public PkgAttribCmd {
public:
  TAttribCmd(TclItemPkg* pkg, const char* cmd_name, Attrib<T>* attrib) :
	 PkgAttribCmd(pkg, cmd_name),
	 itsAttrib(attrib) {}

protected:
  virtual void get() {
	 returnVal(itsAttrib->get(item()));
  }
  virtual void set() {
	 T val;
	 getValFromArg(2, val);
	 itsAttrib->set(item(), val);
	 returnVoid();
  }

private:
  auto_ptr< Attrib<T> > itsAttrib;
};

// We need a specialization of TAttribCmd to handle C++ string's. This
// is because the Attrib itself should deal with 'const string&' for
// efficiency reasons, yet in the set() function we cannot obtain the
// value by passing a reference to getValFromArg() as for the other
// types, since we can't have a reference to a reference.
template <>
class TAttribCmd<const string&> : public PkgAttribCmd {
public:
  TAttribCmd(TclItemPkg* pkg, const char* cmd_name, 
				 Attrib<const string&>* attrib) :
	 PkgAttribCmd(pkg, cmd_name),
	 itsAttrib(attrib) {}

protected:
  virtual void get() {
	 returnVal(itsAttrib->get(item()));
  }
  virtual void set() {
	 string val = getStringFromArg(2); // <-- the crucial difference
	 itsAttrib->set(item(), val);
	 returnVoid();
  }

private:
  auto_ptr< Attrib<const string&> > itsAttrib;
};

///////////////////////////////////////////////////////////////////////
//
// TclItemPkg member definitions
//
///////////////////////////////////////////////////////////////////////

void TclItemPkg::declareIntAttrib(const char* attrib_name, 
											 Attrib<int>* attrib) {
  addCommand( new TAttribCmd<int>(this, makePkgCmdName(attrib_name), attrib) );
}

void TclItemPkg::declareBoolAttrib(const char* attrib_name, 
											  Attrib<bool>* attrib) {
  addCommand ( new TAttribCmd<bool>(this, makePkgCmdName(attrib_name), attrib) );
}

void TclItemPkg::declareDoubleAttrib(const char* attrib_name, 
												 Attrib<double>* attrib) {
  addCommand( new TAttribCmd<double>(this, makePkgCmdName(attrib_name), attrib) );
}

void TclItemPkg::declareCstringAttrib(const char* attrib_name, 
												 Attrib<const char*>* attrib) {
  addCommand( new TAttribCmd<const char*>(this, makePkgCmdName(attrib_name), attrib) );
}

void TclItemPkg::declareStringAttrib(const char* attrib_name, 
												 Attrib<const string&>* attrib) {
  addCommand( new TAttribCmd<const string&>(
								  this, makePkgCmdName(attrib_name), attrib) );
}

static const char vcid_tclitempkg_cc[] = "$Header$";
#endif // !TCLITEMPKG_CC_DEFINED
