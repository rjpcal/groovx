///////////////////////////////////////////////////////////////////////
//
// objlisttcl.cc
// Rob Peters
// created: Jan-99
// written: Thu Oct 14 15:56:28 1999
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

  IO::eatWhitespace(ifs);

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

class ObjlistTcl::SaveObjectsCmd : public TclCmd {
public:
  SaveObjectsCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "objids filename", 3, 3)
  {}
protected:
  virtual void invoke() {
	 vector<int> objids;
	 getSequenceFromArg(1, back_inserter(objids), (int*) 0);

	 const char* filename = arg(2).getCstring();

	 ofstream ofs(filename);
	 if (ofs.fail()) {
		throw TclError(string("error opening file: ") + filename);
	 }

	 for (int i = 0; i < objids.size(); ++i) {
		ObjList::theObjList().getCheckedPtr(objids[i])->
		  serialize(ofs, IO::TYPENAME | IO::BASES);
		ofs << endl;
	 }
  }
};

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
