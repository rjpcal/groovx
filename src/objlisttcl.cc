///////////////////////////////////////////////////////////////////////
//
// objlisttcl.cc
// Rob Peters
// created: Jan-99
// written: Tue Oct  5 14:28:25 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJLISTTCL_CC_DEFINED
#define OBJLISTTCL_CC_DEFINED

#include <fstream.h>
#include <cctype>
#include <string>

#include "iomgr.h"
#include "objlist.h"
#include "listpkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace ObjlistTcl {
  class LoadObjectsCmd;
  class ObjListPkg;  

  void eatWhitespace(istream& is) {
	 while ( isspace(is.peek()) ) {
		is.get();
	 }
  }
}

//---------------------------------------------------------------------
//
// LoadFacesCmd --
//
//---------------------------------------------------------------------

class ObjlistTcl::LoadObjectsCmd : public TclCmd {
public:
  LoadObjectsCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name,
			  "filename ?num_to_read? ?typename?", 2, 4, false)
  {}

protected:
  virtual void invoke();
};

void ObjlistTcl::LoadObjectsCmd::invoke() {
  const char* file = getCstringFromArg(1);

  int num_to_read = -1;			  // -1 indicates to read to eof
  if (objc() >= 3) { num_to_read = getIntFromArg(2); }

  const char* given_type = 0;
  if (objc() >= 4) { given_type = getCstringFromArg(3); }

  ifstream ifs(file);
  if (ifs.fail()) { throw TclError("unable to open file"); }

  
  ObjList& olist = ObjList::theObjList();

  string dummy;

  int num_read = 0;
  vector<int> ids;

  eatWhitespace(ifs);

  while ( (num_to_read < 0 || num_read < num_to_read)
			 && (ifs.peek() != EOF) ) {

	 // allow for whole-line comments between objects beginning with '#'
	 if (ifs.peek() == '#') {
		getline(ifs, dummy, '\n');
		continue;
	 }
	 
	 string type;
	 if (given_type == 0) { ifs >> type; } 
	 else                 { type = given_type; }

	 IO* io = IoMgr::newIO(type.c_str());
	 
	 GrObj* p = dynamic_cast<GrObj*>(io);
	 if (!p) {
		throw InputError("ObjlistTcl::loadObjects");
	 }
	 
	 io->deserialize(ifs, IO::NO_FLAGS);
	 
	 int objid = olist.insert(p);
	 ids.push_back(objid);
	 ++num_read;

	 eatWhitespace(ifs);
  }
  
  // Return the ids of all the faces created
  returnSequence(ids.begin(), ids.end());
}

//---------------------------------------------------------------------
//
// ObjListPkg class definition
//
//---------------------------------------------------------------------

class ObjlistTcl::ObjListPkg : public ListPkg<ObjList> {
public:
  ObjListPkg(Tcl_Interp* interp) :
	 ListPkg<ObjList>(interp, ObjList::theObjList(), "ObjList", "3.0")
  {
	 addCommand( new LoadObjectsCmd(interp, "ObjList::loadObjects") );
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
