///////////////////////////////////////////////////////////////////////
//
// objlisttcl.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Jan-99
// written: Thu Dec  7 18:44:13 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJLISTTCL_CC_DEFINED
#define OBJLISTTCL_CC_DEFINED

#include "grobj.h"

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

  template <class ReaderType, class Inserter>
  void readBatch(STD_IO::istream& is, int num_to_read,
					  Inserter result_inserter)
	 {
		const int ALL = -1; // indicates to read all objects until eof

		int num_read = 0;

		is >> ws;

		while ( (num_to_read == ALL || num_read < num_to_read)
				  && (is.peek() != EOF) ) {

		  // allow for whole-line comments between objects beginning with '#'
		  if (is.peek() == '#') {
			 is.ignore(10000000, '\n');
			 continue;
		  }

		  ReaderType reader(is);
		  IO::IoObject* io = reader.readRoot(0);

		  IdItem<IO::IoObject> obj(io, IdItem<IO::IoObject>::Insert());

		  *result_inserter = obj.id();
		  ++result_inserter;

		  ++num_read;

		  is >> ws;
		}
	 }

  template <class WriterType, class Iterator>
  void writeBatch(WriterType& writer, Iterator obj_itr, Iterator end)
	 {
		while (obj_itr != end)
		  {
			 IdItem<IO::IoObject> item(*obj_itr);
			 writer.writeRoot(item.get());
//  			 os << endl;

			 ++obj_itr;
		  }
	 }
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

  readBatch<IO::LegacyReader>(ifs, num_to_read, resultAppender((int*)0));
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

	 IO::LegacyWriter writer(ofs, use_bases);
	 writer.usePrettyPrint(false);
	 writeBatch(writer, beginOfArg(1, (int*)0), endOfArg(1, (int*)0));
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
	 Tcl::PtrListPkg<GrObj>(interp, "ObjList", "$Revision$")
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
