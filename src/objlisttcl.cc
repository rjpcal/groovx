///////////////////////////////////////////////////////////////////////
//
// objlisttcl.cc
// Rob Peters
// created: Jan-99
// written: Tue Feb  1 18:07:04 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJLISTTCL_CC_DEFINED
#define OBJLISTTCL_CC_DEFINED

#include <fstream.h>
#include <cctype>
#include <string>

#include "grobj.h"
#include "iomgr.h"
#include "objlist.h"
#include "listpkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace ObjlistTcl {
  class LoadObjectsCmd;
  class SaveObjectsCmd;
  class ObjListPkg;  
}

//---------------------------------------------------------------------
//
// LoadObjectsCmd --
//
//---------------------------------------------------------------------

class ObjlistTcl::LoadObjectsCmd : public Tcl::TclCmd {
public:
  LoadObjectsCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name,
					 "filename ?num_to_read=-1? ?typename=\"\"? ?use_bases=no?",
					 2, 5, false)
  {}

protected:
  virtual void invoke();
};

void ObjlistTcl::LoadObjectsCmd::invoke() {
  static const int ALL = -1; // indicates to read all objects until eof

  const char* file        =                             getCstringFromArg(1);
  int         num_to_read =      (objc() < 3) ? ALL   : getIntFromArg(2);
  const char* given_type  =      (objc() < 4) ?  ""   : getCstringFromArg(3);
  bool        use_bases   = bool((objc() < 5) ? false : getBoolFromArg(4));

  ifstream ifs(file);
  if (ifs.fail()) { throw Tcl::TclError("unable to open file"); }

  
  ObjList& olist = ObjList::theObjList();

  string dummy;

  int num_read = 0;
  vector<int> ids;

  IO::eatWhitespace(ifs);

  while ( (num_to_read == ALL || num_read < num_to_read)
			 && (ifs.peek() != EOF) ) {

	 // allow for whole-line comments between objects beginning with '#'
	 if (ifs.peek() == '#') {
		getline(ifs, dummy, '\n');
		continue;
	 }
	 
	 bool reading_typenames = (given_type[0] == 0);

	 string type;
	 if (reading_typenames) { ifs >> type; }
	 else                   { type = given_type; }

	 IO* io = IoMgr::newIO(type.c_str());
	 
	 GrObj* p = dynamic_cast<GrObj*>(io);
	 if (!p) {
		throw InputError("ObjlistTcl::loadObjects");
	 }
	 
	 IO::IOFlag flags = use_bases ? IO::BASES : IO::NO_FLAGS;

	 io->deserialize(ifs, flags);
	 
	 int objid = olist.insert(ObjList::Ptr(p));
	 ids.push_back(objid);
	 ++num_read;

	 IO::eatWhitespace(ifs);
  }
  
  // Return the ids of all the faces created
  returnSequence(ids.begin(), ids.end());
}

//---------------------------------------------------------------------
//
// SaveObjectsCmd --
//
//---------------------------------------------------------------------

class ObjlistTcl::SaveObjectsCmd : public Tcl::TclCmd {
public:
  SaveObjectsCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name,
					 "objids filename ?use_typename=yes? ?use_bases=yes?", 3, 5)
  {}
protected:
  virtual void invoke() {
	 vector<int> objids;
	 getSequenceFromArg(1, back_inserter(objids), (int*) 0);

	 const char* filename = arg(2).getCstring();

	 bool use_typename = arg(3).getBool();
	 bool use_bases    = arg(4).getBool();

	 ofstream ofs(filename);
	 if (ofs.fail()) {
		throw Tcl::TclError(string("error opening file: ") + filename);
	 }

	 IO::IOFlag flags = IO::NO_FLAGS;
	 if (use_typename) flags |= IO::TYPENAME;
	 if (use_bases)    flags |= IO::BASES;

	 for (size_t i = 0; i < objids.size(); ++i) {
		ObjList::theObjList().getCheckedPtr(objids[i])->
		  serialize(ofs, flags);
		ofs << endl;
	 }
  }
};

//---------------------------------------------------------------------
//
// ObjListPkg class definition
//
//---------------------------------------------------------------------

class ObjlistTcl::ObjListPkg : public Tcl::IoPtrListPkg {
public:
  ObjListPkg(Tcl_Interp* interp) :
	 Tcl::IoPtrListPkg(interp, ObjList::theObjList(), "ObjList", "3.0")
  {
	 addCommand( new LoadObjectsCmd(interp, "ObjList::loadObjects") );
	 addCommand( new SaveObjectsCmd(interp, "ObjList::saveObjects") );
  }
};

//---------------------------------------------------------------------
//
// ObjlistTcl::Objlist_Init --
//
//---------------------------------------------------------------------

extern "C" Tcl_PackageInitProc Objlist_Init;

int Objlist_Init(Tcl_Interp* interp) {
DOTRACE("Objlist_Init");

  new ObjlistTcl::ObjListPkg(interp);

  return TCL_OK;
}

static const char vcid_objlisttcl_cc[] = "$Header$";
#endif // !OBJLISTTCL_CC_DEFINED
