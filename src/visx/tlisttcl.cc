///////////////////////////////////////////////////////////////////////
//
// tlisttcl.cc
// Rob Peters
// created: Sat Mar 13 12:38:37 1999
// written: Sat Dec  4 01:19:14 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTTCL_CC_DEFINED
#define TLISTTCL_CC_DEFINED

#include "tlisttcl.h"

#include <tcl.h>
#include <strstream.h>
#include <fstream.h>
#include <iomanip.h>
#include <vector>
#include <cmath>

#include "tlist.h"
#include "trial.h"
#include "grobj.h"
#include "gtext.h"
#include "position.h"
#include "objlist.h"
#include "poslist.h"
#include "objlisttcl.h"
#include "poslisttcl.h"
#include "tclcmd.h"
#include "listpkg.h"
#include "rect.h"

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
  using ObjlistTcl::bad_objid_msg;
  using PoslistTcl::bad_posid_msg;

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

	 double world_width, world_height, world_origin_x, world_origin_y;

	 GrObj::getWorldFromScreen(0, 0, world_origin_x, world_origin_y);
	 GrObj::getWorldFromScreen(pixel_width, pixel_height,
										world_width, world_height, false);

	 world_width -= world_origin_x;
	 world_height -= world_origin_y;

	 vector<Rect<double> > bbxs(objids.size());

	 Trial* preview = new Trial();
	 int previewid = Tlist::theTlist().insert(preview);

	 double window_area = world_width*world_height;
	 double parcel_area = window_area/objids.size();
	 double raw_parcel_side = sqrt(parcel_area);
	 
	 int num_cols = int(world_width/raw_parcel_side) + 1;

	 double parcel_side = world_width/num_cols;

	 int x_step = -1;
	 int y_step = 0;

	 for (int i = 0; i < objids.size(); ++i) {
		++x_step;
		if (x_step == num_cols) { x_step = 0; ++y_step; }

		GrObj* obj = ObjList::theObjList().getCheckedPtr(objids[i]);
		bool haveBB = obj->getBoundingBox(bbxs[i]);

		if ( !haveBB ) {
		  throw TclError("all objects must have bounding boxes");
		}

		obj->setAlignmentMode(GrObj::CENTER_ON_CENTER);
		obj->setBBVisibility(true);
		obj->setScalingMode(GrObj::MAINTAIN_ASPECT_SCALING);
		obj->setMaxDimension(0.8);

		char id_string[32];
		ostrstream ost(id_string, 31);
		ost << objids[i] << '\0';

		Gtext* label = new Gtext(id_string);
		int label_objid = ObjList::theObjList().insert(label);
		label->setAlignmentMode(GrObj::CENTER_ON_CENTER);
		label->setScalingMode(GrObj::MAINTAIN_ASPECT_SCALING);
		label->setHeight(0.1);

		Position* obj_pos = new Position();
		int obj_posid = PosList::thePosList().insert(obj_pos);
		double obj_x = -world_width/2.0 + (x_step+0.5)*parcel_side;
		double obj_y = world_height/2.0 - (y_step+0.45)*parcel_side;
		obj_pos->setTranslate(obj_x, obj_y, 0.0);
		obj_pos->setScale(parcel_side, parcel_side, 1.0);

		Position* label_pos = new Position();
		int label_posid = PosList::thePosList().insert(label_pos);
		double label_x = obj_x;
		double label_y = obj_y - 0.50*parcel_side;
		label_pos->setTranslate(label_x, label_y, 0.0);
		label_pos->setScale(parcel_side, parcel_side, 1.0);
		
		preview->add(objids[i], obj_posid);
		preview->add(label_objid, label_posid);
	 }

	 returnInt(previewid);
  }
};

//---------------------------------------------------------------------
//
// AddObjectCmd --
//
// This function adds an object/position pair (specified by their id#'s
// in the ObjList and PosList) to a specified trial in a Tlist.
//
//---------------------------------------------------------------------

class TlistTcl::AddObjectCmd : public TclCmd {
public:
  AddObjectCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "trial objid posid", 4, 4) {}
protected:
  virtual void invoke() {
  DOTRACE("AddObjectCmd::invoke");
    int trialid = getIntFromArg(1);
	 if ( trialid < 0 ) { throw TclError(bad_trial_msg); }

	 int objid = getIntFromArg(2);
	 if ( !ObjList::theObjList().isValidId(objid) )
		{ throw TclError(bad_objid_msg); }

	 int posid = getIntFromArg(3);
	 if ( !PosList::thePosList().isValidId(posid) )
		{ throw TclError(bad_posid_msg); }

	 if ( !theTlist.isValidId(trialid) ) {
		theTlist.insertAt(trialid, new Trial);
	 }

	 theTlist.getPtr(trialid)->add(objid, posid);
	 returnVoid();
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::MakeSinglesCmd --
//
// Clear all trials currently in the Tlist, then make new Trial's so
// that there is one Trial for each valid object in the ObjList. In
// addition, the trialid for each generated Trial will be the same as
// the objid of the object it refers to. Returns the number of trials
// that were created.
//
//--------------------------------------------------------------------

class TlistTcl::MakeSinglesCmd : public TclCmd {
public:
  MakeSinglesCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "posid", 2, 2) {}
protected:
  virtual void invoke() {
	 int posid = getIntFromArg(1);
	 if ( !PosList::thePosList().isValidId(posid) )
		{ throw TclError(bad_posid_msg); }

	 vector<int> vec;
	 const ObjList& olist = ObjList::theObjList();
	 olist.getValidIds(vec);

	 theTlist.clear();
	 
	 // This loop runs through all valid objid's and does two things: 
	 // 1) it adds the objid and the given posid to the trial
	 // 2) it sets the Trial's type to the category of its single GrObj
	 for (int i=0; i < vec.size(); ++i) {
		int id = vec[i];
		if ( !theTlist.isValidId(id) ) {
		  theTlist.insertAt(id, new Trial);
		}
		Trial* t = theTlist.getPtr(id);
		t->add(id, posid);
		const GrObj* obj = olist.getCheckedPtr(id);
		t->setType(obj->getCategory());
	 }

	 returnInt(vec.size());
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::MakePairsCmd --
//
// Clear all trials currently in the Tlist, then make new Trial's so
// that there is one Trial for each ordered pair of valid objects in
// the ObjList. Returns the number of trials created.
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

	 if ( !PosList::thePosList().isValidId(posid1) ||
			!PosList::thePosList().isValidId(posid2) )
		{ throw TclError(bad_posid_msg); }

	 vector<int> vec;
	 ObjList::theObjList().getValidIds(vec);
	 
	 theTlist.clear();
	 
	 // This loop runs through all the trials and does two things:
	 // 1) it adds the appropriate objids and the given posids to the trial
	 // 2) it sets the Trial's type to zero if the objids are different, and to
	 //    one if the objids are the same.
	 int trialid = 0;
	 for (int i = 0; i < vec.size(); ++i) {
		for (int j = 0; j < vec.size(); ++j) {
		  if ( !theTlist.isValidId(trialid) ) {
			 theTlist.insertAt(trialid, new Trial);
		  }
		  Trial* t = theTlist.getPtr(trialid);
		  t->add(vec[i], posid1);
		  t->add(vec[j], posid2);
		  t->setType( i == j );
		  
		  ++trialid;
		}
	 }

	 // Return the number of trials generated.
	 returnInt(trialid);
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::MakeTriadsCmd --
//
// Clear all trials currently in the Tlist, then make new Trial's such
// that there is one Trial for each ordered triad of valid objects in
// the ObjList. Returns the number of trials created.
//
//--------------------------------------------------------------------

class TlistTcl::MakeTriadsCmd : public TclCmd {
public:
  MakeTriadsCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "posid1 posid2 posid3", 4, 4) {}
protected:
  virtual void invoke() {
	 int posid[3] = { getIntFromArg(1), getIntFromArg(2), getIntFromArg(3) };	 

	 if ( !PosList::thePosList().isValidId(posid[0]) ||
			!PosList::thePosList().isValidId(posid[1]) ||
			!PosList::thePosList().isValidId(posid[2]) ) { 
		throw TclError(bad_posid_msg);
	 }

	 DebugEval(posid[0]); DebugEval(posid[1]); DebugEvalNL(posid[3]);

	 vector<int> vec;
	 ObjList::theObjList().getValidIds(vec);
	 
	 theTlist.clear();

	 const int NUM_PERMS = 18;
	 static int permutations[NUM_PERMS][3] = { 
		{0, 0, 1},
		{0, 0, 2},
		{1, 1, 0},
		{1, 1, 2},
		{2, 2, 0},
		{2, 2, 1},
		{0, 1, 1},
		{0, 2, 2},
		{1, 0, 0},
		{2, 0, 0},
		{2, 1, 1},
		{1, 2, 2},
		{0, 1, 2},
		{0, 2, 1},
		{1, 0, 2},
		{1, 2, 0},
		{2, 0, 1},
		{2, 1, 0} };
  
	 int num_objs = vec.size();
	 int base_triad[3];
    
	 int trial = 0;

	 // loops over i,j,k construct all possible base triads
	 for (int i = 0; i < num_objs-2; ++i) {
		base_triad[0] = vec[i];
		for (int j = i+1; j < num_objs; ++j) {
		  base_triad[1] = vec[j];
		  for (int k = j+1; k < num_objs; ++k) {
			 base_triad[2] = vec[k];

			 DebugEval(i); DebugEval(j); DebugEval(k); DebugEvalNL(trial);

			 // loops over p,e run through all permutations
			 for (int p = 0; p < NUM_PERMS; ++p) {
				if ( !theTlist.isValidId(trial) ) {
				  theTlist.insertAt(trial, new Trial);
				}
				Trial* t = theTlist.getPtr(trial);
				for (int e = 0; e < 3; ++e) {
				  t->add(base_triad[e], posid[e]);
				}
				++trial;
			 } // end p

		  } // end k
		} // end j
	 } // end i

	 // Return the number of trials generated
	 returnInt(trial);
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::MakeSummaryTrialCmd --
//
// Make a trial that contains all the objects in the ObjList on a
// grid. New Position's will be created as necessary. Return the id of
// the trial that was created.
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

	 ObjList& olist = ObjList::theObjList();
	 int num_objs = olist.count();
	 
	 // Figure number of columns-- (num_objs-1)/num_cols gives one fewer
	 // than the number of rows that we need, so add 1 to it
	 int num_rows = 1 + (num_objs-1)/num_cols;

	 vector<int> objids;
	 olist.getValidIds(objids);
	 
	 // Coords of upper left corner of viewing area
	 const double x0 = scale * (0.0 - xstep*(num_cols-1)/2.0);
	 const double y0 = scale * (0.0 + ystep*(num_rows-1)/2.0);

	 DebugEval(num_objs); DebugEval(num_cols); DebugEvalNL(num_rows);
	 DebugEval(scale); DebugEval(xstep); DebugEval(ystep); 
	 DebugEval(x0); DebugEvalNL(y0);

	 PosList& plist = PosList::thePosList();
	 
	 if ( !theTlist.isValidId(trialid) ) {
		theTlist.insertAt(trialid, new Trial);
	 }

	 Trial* t = theTlist.getPtr(trialid);

	 for (int i=0; i < objids.size(); ++i) {
		int row = i / num_cols;
		int col = i % num_cols;
		Position* p = new Position;
		double xpos = x0+col*xstep*scale;
		double ypos = y0-row*ystep*scale;
		
		DebugEval(xpos);
		DebugEvalNL(ypos);
		
		p->setTranslate(xpos, ypos, 0.0);
		p->setScale(scale, scale, 1.0);
		int posid = plist.insert(p);

		t->add(objids[i], posid);
	 }

	 // Return the id of the trial generated.
	 returnInt(trialid);
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::LoadObjidFileCmd --
//
// This procedure initializes the Tlist from objid_file, which must be
// the name of a file containing one list of objid's per
// line. num_lines specifies how many lines to read, or, if set to -1,
// the entire file will be read. offset is an optional parameter
// which, if provided, will be added to each objid before the objid is
// inserted into the Tlist. Any Trial's that were previously in the
// Tlist are erased before the the file is read. Returns the number of
// trials that were loaded from the file.
//
//--------------------------------------------------------------------

class TlistTcl::LoadObjidFileCmd : public TclCmd {
public:
  LoadObjidFileCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "objid_file num_lines ?offset?", 3, 4, false) {}
protected:
  virtual void invoke() {
	 const char* objid_file = getCstringFromArg(1);

	 int num_lines = getIntFromArg(2);

	 int offset = (objc() >= 4) ? getIntFromArg(3) : 0;

	 ifstream ifs(objid_file);
	 int num_loaded = -1;
	 try {
		num_loaded = theTlist.readFromObjidsOnly(ifs, num_lines, offset);
	 }
	 catch (IoError& err) {
		throw TclError(err.msg());
	 }

	 returnInt(num_loaded);
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::write_responsesCmd --
//
// A Tcl command to write a file containing a summary of the responses
// to each Trial in the Tlist. The work is delegated to
// write_responsesProc.
//
//--------------------------------------------------------------------

class TlistTcl::WriteResponsesCmd : public TclCmd {
public:
  WriteResponsesCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "filename", 2, 2) {}
protected:
  virtual void invoke() {
	 const char* filename = getCstringFromArg(1);

	 writeResponsesProc(filename);
	 returnVoid();
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::writeResponsesProc --
//
// Write a file containing a summary of the responses to each Trial in
// the Tlist.
//
//--------------------------------------------------------------------

void TlistTcl::writeResponsesProc(const char* filename) {
DOTRACE("TlistTcl::write_responsesProc");
  vector<int> trialids;
  theTlist.getValidIds(trialids);

  DebugEvalNL(trialids.size());

  ofstream ofs(filename);
  const int wid = 8;

  // We prepend a '%' to the header line so that MATLAB can ignore
  // this line as a comment
  ofs << '%' << setw(wid-1) << "Trial" << setw(wid) << "N" 
		<< setw(wid) << "Average" << setw(wid) << "msec\n";
  
  ofs.setf(ios::fixed);
  ofs.precision(2);
  for (int i = 0; i < trialids.size(); ++i) {
	 const Trial& t = *(theTlist.getPtr(trialids[i]));
	 ofs << setw(wid) << trialids[i];
	 ofs << setw(wid) << t.numResponses();
	 ofs << setw(wid) << t.avgResponse();	 
	 ofs << setw(wid) << t.avgRespTime() << endl;
  }

  if (ofs.fail()) { throw TclError("error while writing to file"); }
}

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
	 
	 vector<int> trialids;
	 theTlist.getValidIds(trialids);
	 
	 DebugEvalNL(trialids.size());
	 
	 ofstream ofs(filename);
	 
	 for (int i = 0; i < trialids.size(); ++i) {
		const Trial& t = *(theTlist.getPtr(trialids[i]));
		
		// Use this to make sure we don't round down when we should round up.
		double fudge = 0.0001;
		
		int num_zeros = int( (1.0 - t.avgResponse())*t.numResponses() + fudge);
		int num_ones = t.numResponses() - num_zeros;

		ofs << num_zeros << "  " << num_ones << endl;
	 }
	 returnVoid();
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::WriteMatlabCmd --
//
// Write a matlab-readable file with one line per Trial, where each
// line contains a list of the objid's in that Trial followed by the
// average response given to that Trial.
//
//--------------------------------------------------------------------

class TlistTcl::WriteMatlabCmd : public TclCmd {
public:
  WriteMatlabCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "filename", 2, 2) {}
protected:
  virtual void invoke() {
	 const char* filename = getCstringFromArg(1);

	 vector<int> trialids;
	 theTlist.getValidIds(trialids);

	 DebugEvalNL(trialids.size());

	 ofstream ofs(filename);
	 const int wid = 8;
	 
	 ofs.setf(ios::fixed);
	 ofs.precision(2);
	 for (int i = 0; i < trialids.size(); ++i) {
		const Trial& t = *(theTlist.getPtr(trialids[i]));
		Trial::ObjGrp objs = t.objs();
	 
		for (int j = 0; j < objs.size(); ++j) {
		  ofs << objs[j].objid << ' ';
		}
		
		ofs << t.avgResponse() << endl;
	 }

	 returnVoid();
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
