///////////////////////////////////////////////////////////////////////
//
// facetcl.cc
// Rob Peters 
// created: Jan-99
// written: Mon Jun 28 18:44:31 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_CC_DEFINED
#define FACETCL_CC_DEFINED

#include "facetcl.h"

#include <tcl.h>
#include <fstream.h>
#include <strstream.h>
#include <cstring>
#include <cctype>
#include <vector>

#include "iomgr.h"
#include "errmsg.h"
#include "objlist.h"
#include "objlisttcl.h"
#include "face.h"
#include "id.h"
#include "tclitempkg.h"
#include "tclcmd.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace FaceTcl {
  class FaceCmd;
  class LoadFacesCmd;
  class FacePkg;
}

//---------------------------------------------------------------------
//
// FaceCmd --
//
//---------------------------------------------------------------------

class FaceTcl::FaceCmd : public TclCmd {
public:
  FaceCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
	 double eh=0.6, es=0.4, nl=0.4, mh=-0.8;
	 Face* p = new Face(eh, es, nl, mh);

	 ObjId objid = ObjList::theObjList().addObj(p);
	 returnInt(objid.toInt());
  }
};

//---------------------------------------------------------------------
//
// LoadFacesCmd --
//
//---------------------------------------------------------------------

class FaceTcl::LoadFacesCmd : public TclCmd {
public:
  LoadFacesCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name,
			  "filename ?num_to_read? ?use_virtual_ctor?", 2, 4, false)
  {}

protected:
  virtual void invoke();
};

void FaceTcl::LoadFacesCmd::invoke() {
  const char* file = getCstringFromArg(1);

  int num_to_read = -1;			  // -1 indicates to read to eof
  if (objc() >= 3) { num_to_read = getIntFromArg(2); }

  bool use_virtual_ctor = false;
  if (objc() >= 4) { use_virtual_ctor = getBoolFromArg(3); }

  ifstream ifs(file);
  if (ifs.fail()) { throw TclError("unable to open file"); }
  
  ObjList& olist = ObjList::theObjList();

  static const int BUF_SIZE = 200;
  char line[BUF_SIZE];

  int num_read = 0;
  vector<int> ids;
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
		char c = char(ist.peek());
		bool saw_alpha = bool(isalpha(c));
		if (use_virtual_ctor | saw_alpha) {
		  IO* io = IoMgr::newIO(ist, IO::BASES & IO::TYPENAME);
		  p = dynamic_cast<Face *>(io);
		  if (!p) {
			 throw InputError("FaceTcl::loadFaces");
		  }
		}
		else {
		  p = new Face(ist, IO::NO_FLAGS);
		}
		ObjId objid = olist.addObj(p);
		ids.push_back(objid.toInt());
		++num_read;
	 }
  }
  catch (IoError& err) {
	 throw TclError(err.msg());
  }

  // Return the ids of all the faces created
  returnSequence(ids.begin(), ids.end());
}

///////////////////////////////////////////////////////////////////////
//
// FacePkg class definition
//
///////////////////////////////////////////////////////////////////////

class FaceTcl::FacePkg : public CTclIoItemPkg<Face> {
public:
  FacePkg(Tcl_Interp* interp) :
	 CTclIoItemPkg<Face>(interp, "Face", "2.5")
  {
	 addCommand ( new FaceCmd(interp, "Face::face") );

	 declareAttrib("mouthHgt", new CAttrib<Face, double>(&Face::getMouthHgt,
																		  &Face::setMouthHgt));
	 declareAttrib("noseLen", new CAttrib<Face, double>(&Face::getNoseLen,
																		 &Face::setNoseLen));
	 declareAttrib("eyeDist", new CAttrib<Face, double>(&Face::getEyeDist,
																		&Face::setEyeDist));
	 declareAttrib("eyeHgt", new CAttrib<Face, double>(&Face::getEyeHgt,
																		&Face::setEyeHgt));

	 addCommand( new LoadFacesCmd(interp, "Face::loadFaces") );
	 
	 Tcl_Eval(interp, 
				 "namespace eval Face {\n"
				 "  namespace export face\n"
				 "}");
	 Tcl_Eval(interp,
				 "namespace import Face::face\n");
	 
  }

  virtual Face* getCItemFromId(int id) {
	 ObjId objid(id);
	 if ( !objid ) {
		throw TclError("objid out of range");
	 }
	 Face* p = dynamic_cast<Face *>(objid.get());
	 if ( p == NULL ) {
		throw TclError("object not of correct type");
	 }
	 return p;
  }

  virtual IO& getIoFromId(int id) {
	 return dynamic_cast<IO&>( *(getCItemFromId(id)) );
  }
};
  
int Face_Init(Tcl_Interp* interp) {
DOTRACE("Face_Init");

  new FaceTcl::FacePkg(interp);

  return TCL_OK;
}

static const char vcid_facetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
