///////////////////////////////////////////////////////////////////////
//
// objlisttcl.cc
// Rob Peters
// created: Jan-99
// written: Fri Oct 27 18:39:40 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJLISTTCL_CC_DEFINED
#define OBJLISTTCL_CC_DEFINED

#include "grobj.h"
#include "ioptrlist.h"

#include "io/iolegacy.h"

#include "tcl/listpkg.h"
#include "tcl/stringifycmd.h"

#include <fstream.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

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
					 "filename ?num_to_read=-1?", 2, 3, false)
  {}

protected:
  virtual void invoke();
};

void ObjlistTcl::LoadObjectsCmd::invoke() {
DOTRACE("ObjlistTcl::LoadObjectsCmd::invoke");
  static const int ALL = -1; // indicates to read all objects until eof

  const char* file        =                             getCstringFromArg(1);
  int         num_to_read =      (objc() < 3) ? ALL   : getIntFromArg(2);

  STD_IO::ifstream ifs(file);
  if (ifs.fail()) { throw Tcl::TclError("unable to open file"); }
  
  IoPtrList& olist = IoPtrList::theList();

  int num_read = 0;

  ifs >> ws;

  while ( (num_to_read == ALL || num_read < num_to_read)
			 && (ifs.peek() != EOF) ) {

	 // allow for whole-line comments between objects beginning with '#'
	 if (ifs.peek() == '#') {
		ifs.ignore(10000000, '\n');
		continue;
	 }

	 IO::LegacyReader reader(ifs);
	 IO::IoObject* io = reader.readRoot(0);

	 GrObj* p = dynamic_cast<GrObj*>(io);
	 if (!p) {
		throw IO::InputError("ObjlistTcl::loadObjects");
	 }

	 IdItem<GrObj> obj(p, IdItem<GrObj>::Insert());

	 lappendVal(obj.id()); // add the current objid to the Tcl result

	 ++num_read;

	 ifs >> ws;
  }
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
					 "objids filename ?use_bases=yes?", 3, 4)
  {}
protected:
  virtual void invoke() {

	 const char* filename = arg(2).getCstring();

	 bool use_bases    = objc() < 4 ? true : arg(3).getBool();

	 STD_IO::ofstream ofs(filename);
	 if (ofs.fail()) {
		Tcl::TclError err("error opening file: ");
		err.appendMsg(filename);
		throw err;
	 }

	 IoPtrList& olist = IoPtrList::theList();
	 for (Tcl::ListIterator<int>
			  itr = beginOfArg(1, (int*)0),
			  end = endOfArg(1, (int*)0);
			itr != end;
			++itr)
		{
		  IO::LegacyWriter writer(ofs, use_bases);
		  IdItem<GrObj> item(*itr);
		  writer.writeRoot(item.get());
		  ofs << endl;
		}
  }
};

//---------------------------------------------------------------------
//
// ObjListPkg class definition
//
//---------------------------------------------------------------------

class ObjlistTcl::ObjListPkg : public Tcl::PtrListPkg<GrObj> {
public:
  ObjListPkg(Tcl_Interp* interp) :
	 Tcl::PtrListPkg<GrObj>(interp, IoPtrList::theList(), "ObjList", "3.0")
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

extern "C"
int Objlist_Init(Tcl_Interp* interp) {
DOTRACE("Objlist_Init");

  Tcl::TclPkg* pkg = new ObjlistTcl::ObjListPkg(interp);

  return pkg->initStatus();
}

static const char vcid_objlisttcl_cc[] = "$Header$";
#endif // !OBJLISTTCL_CC_DEFINED
