///////////////////////////////////////////////////////////////////////
//
// objlisttcl.cc
// Rob Peters
// created: Jan-99
// written: Mon Oct  9 09:22:29 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJLISTTCL_CC_DEFINED
#define OBJLISTTCL_CC_DEFINED

#include "grobj.h"
#include "objlist.h"

#include "io/iolegacy.h"
#include "io/iomgr.h"

#include "tcl/listpkg.h"
#include "tcl/stringifycmd.h"

#include "util/strings.h"

#include <fstream.h>
#include <cctype>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace ObjlistTcl {
  class LoadObjectsCmd;
  class SaveObjectsCmd;
  class LoadMoreCmd;
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

  STD_IO::ifstream ifs(file);
  if (ifs.fail()) { throw Tcl::TclError("unable to open file"); }

  
  ObjList& olist = ObjList::theObjList();

  int num_read = 0;

  ifs >> ws;

  while ( (num_to_read == ALL || num_read < num_to_read)
			 && (ifs.peek() != EOF) ) {

	 // allow for whole-line comments between objects beginning with '#'
	 if (ifs.peek() == '#') {
		ifs.ignore(10000000, '\n');
		continue;
	 }
	 
	 bool reading_typenames = (given_type[0] == 0);

	 fixed_string type;
	 if (reading_typenames) { ifs >> type; }
	 else                   { type = given_type; }

	 IO::IoObject* io = IO::IoMgr::newIO(type.c_str());
	 
	 GrObj* p = dynamic_cast<GrObj*>(io);
	 if (!p) {
		throw IO::InputError("ObjlistTcl::loadObjects");
	 }
	 
	 IO::IOFlag flags = use_bases ? IO::BASES : IO::NO_FLAGS;

	 IO::LegacyReader reader(ifs, flags);
	 reader.readRoot(io);
	 
	 int objid = olist.insert(ObjList::Ptr(p));

	 lappendVal(objid); // add the current objid to the Tcl result

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
					 "objids filename ?use_typename=yes? ?use_bases=yes?", 3, 5)
  {}
protected:
  virtual void invoke() {

	 const char* filename = arg(2).getCstring();

	 bool use_typename = objc() < 4 ? true : arg(3).getBool();
	 bool use_bases    = objc() < 5 ? true : arg(4).getBool();

	 STD_IO::ofstream ofs(filename);
	 if (ofs.fail()) {
		Tcl::TclError err("error opening file: ");
		err.appendMsg(filename);
		throw err;
	 }

	 IO::IOFlag flags = IO::NO_FLAGS;
	 if (use_typename) flags |= IO::TYPENAME;
	 if (use_bases)    flags |= IO::BASES;

	 ObjList& olist = ObjList::theObjList();
	 for (Tcl::ListIterator<int>
			  itr = beginOfArg(1, (int*)0),
			  end = endOfArg(1, (int*)0);
			itr != end;
			++itr)
		{
		  IO::LegacyWriter writer(ofs, flags);
		  writer.writeRoot(olist.getCheckedPtr(*itr).get());
		  ofs << endl;
		}
  }
};

//---------------------------------------------------------------------
//
// LoadMoreCmd --
//
//---------------------------------------------------------------------

class ObjlistTcl::LoadMoreCmd : public Tcl::ASRLoadCmd {
public:
  LoadMoreCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::ASRLoadCmd(interp, cmd_name, "filename", 2), itsSandbox(0) {}

protected:
  virtual IO::IoObject& getIO() { return itsSandbox; }
  virtual const char* getFilename() { return getCstringFromArg(1); }

  virtual void beforeLoadHook()
	 {
		Assert(itsSandbox.count() == 0);
	 }

  virtual void afterLoadHook()
	 {
		ObjList& olist = ObjList::theObjList();

		for (int id = 0; id < itsSandbox.capacity(); ++id)
		  {
			 if (itsSandbox.isValidId(id))
				{
				  PtrList<GrObj>::SharedPtr shptr = itsSandbox.getCheckedPtr(id);
				  itsSandbox.release(id);
				  int newid = olist.insert(shptr);
				  lappendVal(newid);
				}
		  }

		Assert(itsSandbox.count() == 0);
	 }

private:
  PtrList<GrObj> itsSandbox;
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
	 addCommand( new LoadMoreCmd(interp, "ObjList::loadMore") );
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
