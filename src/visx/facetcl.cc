///////////////////////////////////////////////////////////////////////
//
// facetcl.cc
// Rob Peters 
// created: Jan-99
// written: Thu Oct 19 18:47:51 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_CC_DEFINED
#define FACETCL_CC_DEFINED

#include <tcl.h>
#include <fstream.h>
#include <strstream.h>
#include <cstring>
#include <cctype>

#include "cloneface.h"
#include "objlist.h"
#include "face.h"

#include "io/iofactory.h"
#include "io/iolegacy.h"

#include "tcl/listitempkg.h"
#include "tcl/tclcmd.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace FaceTcl {
  class LoadFacesCmd;
  class FacePkg;
}

//---------------------------------------------------------------------
//
// LoadFacesCmd --
//
//---------------------------------------------------------------------

class FaceTcl::LoadFacesCmd : public Tcl::TclCmd {
public:
  LoadFacesCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name,
					 "filename ?num_to_read? ?use_virtual_ctor?", 2, 4, false)
  {}

protected:
  virtual void invoke();
};

void FaceTcl::LoadFacesCmd::invoke() {
DOTRACE("FaceTcl::LoadFacesCmd::invoke");
  const char* file = getCstringFromArg(1);

  int num_to_read = -1;			  // -1 indicates to read to eof
  if (objc() >= 3) { num_to_read = getIntFromArg(2); }

  bool use_virtual_ctor = false;
  if (objc() >= 4) { use_virtual_ctor = getBoolFromArg(3); }

  STD_IO::ifstream ifs(file);
  if (ifs.fail()) { throw Tcl::TclError("unable to open file"); }
  
  ObjList& olist = ObjList::theObjList();

  static const int BUF_SIZE = 200;
  char line[BUF_SIZE];

  int num_read = 0;

  try {
	 while ( (num_to_read < 0 || num_read < num_to_read)
				&& ifs.getline(line, BUF_SIZE)) {
		// allow for whole-line comments beginning with '#'
		if (line[0] == '#') 
		  continue;
		istrstream ist(line);

		Face* p = NULL;

		// Read the Face by one of two methods:
		//
		// 1) if the use_virtual_ctor argument evaluates to true, then
		// always use the virtual constructor from IoMgr
		//
		// 2) if the use_virtual_ctor argument evaluates to false, then
		// use 'smart' algorithm to determine if we need to use the
		// virtual constructor or the regular plain vanilla Face
		// constructor. The 'smart' algorithm just peeks at the first
		// character on each line-- if it is a letter, then it is
		// assumed that it is the beginning of a typename and the
		// virtual constructor is called, and if it is not a letter, it
		// is assumed that it is the beginning of the simple Face
		// format, so the regular Face constructor is called.
		IO::LegacyReader lrdr(ist, IO::BASES);
		IO::IoObject* io = lrdr.readRoot();
		p = dynamic_cast<Face *>(io);
		if (p == 0) {
		  throw IO::InputError("FaceTcl::loadFaces");
		}

		Assert(p != 0);

  		ItemWithId<GrObj> item(p, ItemWithId<GrObj>::INSERT);

  		lappendVal(item.id()); // add the current objid to the Tcl result

		++num_read;
	 }
  }
  catch (ErrorWithMsg& err) {
	 DebugEval(typeid(err).name());
	 DebugEvalNL(err.msg_cstr());
	 throw Tcl::TclError(err.msg_cstr());
  }
}

///////////////////////////////////////////////////////////////////////
//
// FacePkg class definition
//
///////////////////////////////////////////////////////////////////////

class FaceTcl::FacePkg : public Tcl::ListItemPkg<Face, ObjList> {
public:
  FacePkg(Tcl_Interp* interp) :
	 Tcl::ListItemPkg<Face, ObjList>(interp, ObjList::theObjList(), "Face", "2.5")
  {
	 declareAllProperties();

	 addCommand( new LoadFacesCmd(interp, "Face::loadFaces") );
	 
	 Tcl_Eval(interp, "proc face {} { return [Face::Face] }");
  }
};
  

extern "C"
int Face_Init(Tcl_Interp* interp) {
DOTRACE("Face_Init");

  Tcl::TclPkg* pkg = new FaceTcl::FacePkg(interp);

  FactoryRegistrar<IO::IoObject, Face>      :: registerWith(IO::IoFactory::theOne());
  FactoryRegistrar<IO::IoObject, CloneFace> :: registerWith(IO::IoFactory::theOne());

  return pkg->initStatus();
}

static const char vcid_facetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
