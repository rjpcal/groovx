///////////////////////////////////////////////////////////////////////
//
// tlisttcl.cc
// Rob Peters
// created: Sat Mar 13 12:38:37 1999
// written: Wed Mar 29 22:09:33 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTTCL_CC_DEFINED
#define TLISTTCL_CC_DEFINED

#include "application.h"
#include "experiment.h"
#include "tlist.h"
#include "tlistutils.h"

#include "tcl/listpkg.h"
#include "tcl/tclcmd.h"

#include "util/arrays.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

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

class TlistTcl::CreatePreviewCmd : public Tcl::TclCmd {
public:
  CreatePreviewCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name, "objids pixel_width pixel_height",
			  4, 4, false) {}
protected:
  virtual void invoke() {
	 unsigned int num_ids = getSequenceLengthOfArg(1);
	 fixed_block<int> objids(num_ids);

	 getSequenceFromArg(1, &objids[0], (int*) 0);

	 int pixel_width = arg(2).getInt();
	 int pixel_height = arg(3).getInt();

	 GWT::Canvas* canvas = Application::theApp().getExperiment()->getCanvas();

	 int previewid = TlistUtils::createPreview(theTlist, *canvas,
															 &objids[0], objids.size(),
															 pixel_width, pixel_height);

	 returnInt(previewid);
  }
};

//---------------------------------------------------------------------
//
// AddObjectCmd --
//
//---------------------------------------------------------------------

class TlistTcl::AddObjectCmd : public Tcl::TclCmd {
public:
  AddObjectCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name, "trial objid posid", 4, 4) {}
protected:
  virtual void invoke() {
    int trialid = getIntFromArg(1);
	 int objid = getIntFromArg(2);
	 int posid = getIntFromArg(3);

	 TlistUtils::addObject(theTlist, trialid, objid, posid);
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::MakeSinglesCmd --
//
//--------------------------------------------------------------------

class TlistTcl::MakeSinglesCmd : public Tcl::TclCmd {
public:
  MakeSinglesCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name, "posid", 2, 2) {}
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

class TlistTcl::MakePairsCmd : public Tcl::TclCmd {
public:
  MakePairsCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name, "posid1 posid2", 3, 3) {}
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

class TlistTcl::MakeTriadsCmd : public Tcl::TclCmd {
public:
  MakeTriadsCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name, "posid1 posid2 posid3", 4, 4) {}
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

class TlistTcl::MakeSummaryTrialCmd : public Tcl::TclCmd {
public:
  MakeSummaryTrialCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name, "trialid num_cols scale ?xstep? ?ystep?",
			  4, 6, false) {}
protected:
  virtual void invoke() {
	 int trialid = getIntFromArg(1);
	 if (trialid < 0) { throw Tcl::TclError(bad_trial_msg); }

	 int num_cols = getIntFromArg(2);
	 if (num_cols <= 0) { throw Tcl::TclError("num_cols must be a positive integer"); }

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

class TlistTcl::LoadObjidFileCmd : public Tcl::TclCmd {
public:
  LoadObjidFileCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name, "objid_file num_lines ?offset?", 3, 4, false) {}
protected:
  virtual void invoke() {
	 const char* objid_file =                 getCstringFromArg(1);
	 int         num_lines  =                 getIntFromArg(2);
	 int         offset     = (objc() >= 4) ? getIntFromArg(3) : 0;

	 try {
		int num_loaded =
		  TlistUtils::loadObjidFile(theTlist, objid_file, num_lines, offset);
		returnInt(num_loaded);
	 }
	 catch (IoError& err) {
		throw Tcl::TclError(err.msg_cstr());
	 }
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::write_responsesCmd --
//
//--------------------------------------------------------------------

class TlistTcl::WriteResponsesCmd : public Tcl::TclCmd {
public:
  WriteResponsesCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name, "filename", 2, 2) {}
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

class TlistTcl::WriteIncidenceMatrixCmd : public Tcl::TclCmd {
public:
  WriteIncidenceMatrixCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name, "filename", 2, 2) {}
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

class TlistTcl::WriteMatlabCmd : public Tcl::TclCmd {
public:
  WriteMatlabCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name, "filename", 2, 2) {}
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

class TlistTcl::TlistPkg : public Tcl::IoPtrListPkg {
public:
  TlistPkg(Tcl_Interp* interp) :
	 Tcl::IoPtrListPkg(interp, Tlist::theTlist(), "Tlist", "3.0")
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

extern "C"
int Tlist_Init(Tcl_Interp* interp) {
DOTRACE("Tlist_Init");

  Tcl::TclPkg* pkg = new TlistTcl::TlistPkg(interp);

  return pkg->initStatus();
}

static const char vcid_tlisttcl_cc[] = "$Header$";
#endif // !TLISTTCL_CC_DEFINED
