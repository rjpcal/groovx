///////////////////////////////////////////////////////////////////////
//
// tlisttcl.cc
// Rob Peters
// created: Sat Mar 13 12:38:37 1999
// written: Sat Dec  4 03:35:36 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTTCL_CC_DEFINED
#define TLISTTCL_CC_DEFINED

#include <tcl.h>
#include <vector>

#include "tlist.h"
#include "tclcmd.h"
#include "listpkg.h"
#include "tlistutils.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// Tlist Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

namespace TlistTcl {
  class AddObjectCmd;

  class CreatePreviewCmd;

  class MakeSinglesCmd;
  class MakePairsCmd;
  class MakeTriadsCmd;
  class MakeSummaryTrialCmd;

  class LoadObjidFileCmd;
  class WriteMatlabCmd;
  class WriteIncidenceMatrixCmd;
  class WriteResponsesCmd;

  class TlistPkg;
}

namespace {
  // error messages
  const char* const bad_trial_msg = "invalid trial id";

  // local reference to the global Tlist singleton
  Tlist& theTlist = Tlist::theTlist();
}

///////////////////////////////////////////////////////////////////////
//
// Tlist Tcl package definitions
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
//
// CreatePreviewCmd --
//
//---------------------------------------------------------------------

class TlistTcl::CreatePreviewCmd : public TclCmd {
public:
  CreatePreviewCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "objids pixel_width pixel_height",
			  4, 4, false) {}
protected:
  virtual void invoke() {
	 vector<int> objids;
	 getSequenceFromArg(1, back_inserter(objids), (int*) 0);

	 int pixel_width = arg(2).getInt();
	 int pixel_height = arg(3).getInt();

	 int previewid =
		TlistUtils::createPreview(theTlist, objids, pixel_width, pixel_height);

	 returnInt(previewid);
  }
};

//---------------------------------------------------------------------
//
// AddObjectCmd --
//
//---------------------------------------------------------------------

class TlistTcl::AddObjectCmd : public TclCmd {
public:
  AddObjectCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "trial objid posid", 4, 4) {}
protected:
  virtual void invoke() {
    int trialid = getIntFromArg(1);
	 int objid = getIntFromArg(2);
	 int posid = getIntFromArg(3);

	 TlistUtils::addObject(tlist, trial, objid, posid);
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::MakeSinglesCmd --
//
//--------------------------------------------------------------------

class TlistTcl::MakeSinglesCmd : public TclCmd {
public:
  MakeSinglesCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "posid", 2, 2) {}
protected:
  virtual void invoke() {
	 int posid = getIntFromArg(1);
	 returnInt(TlistUtils::makeSingles(theTlist, posid));
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::MakePairsCmd --
//
//--------------------------------------------------------------------

class TlistTcl::MakePairsCmd : public TclCmd {
public:
  MakePairsCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "posid1 posid2", 3, 3) {}
protected:
  virtual void invoke() {
	 int posid1 = getIntFromArg(1);
	 int posid2 = getIntFromArg(2);

	 returnInt(TlistUtils::makePairs(theTlist, posid1, posid2));
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::MakeTriadsCmd --
//
//--------------------------------------------------------------------

class TlistTcl::MakeTriadsCmd : public TclCmd {
public:
  MakeTriadsCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "posid1 posid2 posid3", 4, 4) {}
protected:
  virtual void invoke() {
	 int posid[3] = { getIntFromArg(1), getIntFromArg(2), getIntFromArg(3) };

	 returnInt(TlistUtils::makeTriads(theTlist, posid));
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::MakeSummaryTrialCmd --
//
//--------------------------------------------------------------------

class TlistTcl::MakeSummaryTrialCmd : public TclCmd {
public:
  MakeSummaryTrialCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "trialid num_cols scale ?xstep? ?ystep?",
			  4, 6, false) {}
protected:
  virtual void invoke() {
	 int trialid = getIntFromArg(1);
	 if (trialid < 0) { throw TclError(bad_trial_msg); }

	 int num_cols = getIntFromArg(2);
	 if (num_cols <= 0) { throw TclError("num_cols must be a positive integer"); }

	 double scale = getDoubleFromArg(3);
	 double xstep = (objc() >= 5) ? getDoubleFromArg(4) : 2.0;
	 double ystep = (objc() >= 6) ? getDoubleFromArg(5) : 3.0;

	 returnInt(TlistUtils::makeSummaryTrial(theTlist,
														 trialid, num_cols, scale,
														 xstep, ystep));
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::LoadObjidFileCmd --
//
//--------------------------------------------------------------------

class TlistTcl::LoadObjidFileCmd : public TclCmd {
public:
  LoadObjidFileCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "objid_file num_lines ?offset?", 3, 4, false) {}
protected:
  virtual void invoke() {
	 const char* objid_file =                 getCstringFromArg(1);
	 int         num_lines  =                 getIntFromArg(2);
	 int         offset     = (objc() >= 4) ? getIntFromArg(3) : 0;

	 try {
		int num_loaded =
		  TlistUtils::readFromObjidsOnly(theTlist, objid_file,
													num_lines, offset);
		returnInt(num_loaded);
	 }
	 catch (IoError& err) {
		throw TclError(err.msg());
	 }
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::write_responsesCmd --
//
//--------------------------------------------------------------------

class TlistTcl::WriteResponsesCmd : public TclCmd {
public:
  WriteResponsesCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "filename", 2, 2) {}
protected:
  virtual void invoke() {
	 const char* filename = getCstringFromArg(1);

	 TlistUtils::writeResponses(theTlist, filename);
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::WriteIncidenceMatrixCmd --
//
//--------------------------------------------------------------------

class TlistTcl::WriteIncidenceMatrixCmd : public TclCmd {
public:
  WriteIncidenceMatrixCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "filename", 2, 2) {}
protected:
  virtual void invoke() {
	 const char* filename = getCstringFromArg(1);	 
	 TlistUtils::writeIncidenceMatrix(theTlist, filename);
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::WriteMatlabCmd --
//
//--------------------------------------------------------------------

class TlistTcl::WriteMatlabCmd : public TclCmd {
public:
  WriteMatlabCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "filename", 2, 2) {}
protected:
  virtual void invoke() {
	 const char* filename = getCstringFromArg(1);
	 TlistUtils::writeMatlab(theTlist, filename);
  }
};

//---------------------------------------------------------------------
//
// TlistTcl::TlistPkg --
//
//---------------------------------------------------------------------

class TlistTcl::TlistPkg : public ListPkg<Tlist> {
public:
  TlistPkg(Tcl_Interp* interp) :
	 ListPkg<Tlist>(interp, Tlist::theTlist(), "Tlist", "3.0")
  {
  DOTRACE("TlistPkg::TlistPkg");
	 addCommand( new AddObjectCmd(interp, "Tlist::addObject") );

	 addCommand( new CreatePreviewCmd(interp, "Tlist::createPreview") );

	 addCommand( new MakeSinglesCmd(interp, "Tlist::makeSingles") );
	 addCommand( new MakePairsCmd(interp, "Tlist::makePairs") );
	 addCommand( new MakeTriadsCmd(interp, "Tlist::makeTriads") );
	 addCommand( new MakeSummaryTrialCmd(interp, "Tlist::makeSummaryTrial") );

	 addCommand( new LoadObjidFileCmd(interp, "Tlist::loadObjidFile") );
	 addCommand( new WriteMatlabCmd(interp, "Tlist::writeMatlab") );
	 addCommand( new WriteIncidenceMatrixCmd(interp,
														  "Tlist::writeIncidenceMatrix") );
	 addCommand( new WriteResponsesCmd(interp, "Tlist::write_responses") );
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::Tlist_Init --
//
//--------------------------------------------------------------------

extern "C" Tcl_PackageInitProc Tlist_Init;

int Tlist_Init(Tcl_Interp* interp) {
DOTRACE("Tlist_Init");

  new TlistTcl::TlistPkg(interp);

  return TCL_OK;
}

static const char vcid_tlisttcl_cc[] = "$Header$";
#endif // !TLISTTCL_CC_DEFINED
