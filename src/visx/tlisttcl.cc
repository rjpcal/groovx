///////////////////////////////////////////////////////////////////////
//
// tlisttcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Mar 13 12:38:37 1999
// written: Fri Jun  1 18:11:46 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTTCL_CC_DEFINED
#define TLISTTCL_CC_DEFINED

#include "application.h"
#include "experiment.h"
#include "tlistutils.h"
#include "trial.h"

#include "gx/gxnode.h"
#include "gx/gxseparator.h"

#include "io/iditem.h"

#include "tcl/tclcmd.h"
#include "tcl/tclpkg.h"

#include "util/arrays.h"

#include <fstream.h>
#include <strstream.h>

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

  class DealSinglesCmd;
  class DealPairsCmd;
  class DealTriadsCmd;

  class LoadObjidFileCmd;
  class WriteMatlabCmd;
  class WriteIncidenceMatrixCmd;
  class WriteResponsesCmd;

  class TlistPkg;
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

	 int previewid = TlistUtils::createPreview(*canvas,
															 &objids[0], objids.size(),
															 pixel_width, pixel_height);

	 returnInt(previewid);
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::DealSinglesCmd --
//
// Make new Trial's so that there is one Trial for each valid object
// in the ObjList. Returns the ids of the trials that were created.
//
//--------------------------------------------------------------------

class TlistTcl::DealSinglesCmd : public Tcl::TclCmd {
public:
  DealSinglesCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name, "objid(s) posid", 3, 3) {}
protected:
  virtual void invoke() {
	 int posid = getIntFromArg(2);

	 for (Tcl::ListIterator<int>
			  itr = beginOfArg(1, (int*)0),
			  end = endOfArg(1, (int*)0);
			itr != end;
			++itr)
		{
		  IdItem<Trial> trial(Trial::make());

		  trial->add(*itr, posid);

		  IdItem<GxNode> obj(*itr);
		  trial->setType(obj->category());

		  lappendVal(trial.id());
		}
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::DealPairsCmd --
//
//--------------------------------------------------------------------

class TlistTcl::DealPairsCmd : public Tcl::TclCmd {
public:
  DealPairsCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name, "objids1 objids2 posid1 posid2", 5, 5) {}
protected:
  virtual void invoke() {
	 int posid1 = getIntFromArg(3);
	 int posid2 = getIntFromArg(4);

	 for (Tcl::ListIterator<int>
			  itr1 = beginOfArg(1, (int*)0),
			  end1 = endOfArg(1, (int*)0);
			itr1 != end1;
			++itr1)
		for (Tcl::ListIterator<int>
				 itr2 = beginOfArg(2, (int*)0),
				 end2 = endOfArg(2, (int*)0);
			  itr2 != end2;
			  ++itr2)
		  {
			 IdItem<Trial> trial(Trial::make());

			 trial->add(*itr1, posid1);
			 trial->add(*itr2, posid2);
			 trial->setType(*itr1 == *itr2);

			 lappendVal(trial.id());
		  }
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::DealTriadsCmd --
//
//--------------------------------------------------------------------

class TlistTcl::DealTriadsCmd : public Tcl::TclCmd {
public:
  DealTriadsCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name,
					 "objids posid1 posid2 posid3", 5, 5) {}
protected:
  virtual void invoke() {
	 int posids[3] = { getIntFromArg(2), getIntFromArg(3), getIntFromArg(4) };

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

	 Tcl::ListIterator<int>
		itr = beginOfArg(1, (int*)0),
		end = endOfArg(1, (int*)0);

	 int id_count = end - itr;

	 fixed_block<int> objids(id_count);

	 {for (unsigned int i = 0; i < objids.size(); ++i)
		{
		  objids[i] = *itr;
		  ++itr;
		}
	 }

	 int base_triad[3];

	 for (unsigned int i = 0; i < objids.size(); ++i) {
		base_triad[0] = objids[i];

		for (unsigned int j = i+1; j < objids.size(); ++j) {
		  base_triad[1] = objids[j];

		  for (unsigned int k = j+1; k < objids.size(); ++k) {
			 base_triad[2] = objids[k];

			 // loops over p,e run through all permutations
			 for (int p = 0; p < NUM_PERMS; ++p) {
				IdItem<Trial> trial(Trial::make());
				for (int e = 0; e < 3; ++e) {
				  trial->add(base_triad[permutations[p][e]], posids[e]);
				}
				lappendVal(trial.id());
			 } // end p
		  } // end itr3
		} // end itr2
	 } // end itr1

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
	 Tcl::TclCmd(interp, cmd_name, "objid_file objids posids ?num_lines=-1?",
					 4, 5, false) {}
protected:
  virtual void invoke() {
  DOTRACE("TlistTcl::LoadObjidFileCmd::invoke");

	 const char* objid_file =                 getCstringFromArg(1);
	 int         num_lines  = (objc() >= 5) ? getIntFromArg(4) : -1;

	 // Determine whether we will read to the end of the input stream, or
	 // whether we will read only num_lines lines from the stream.
	 bool read_to_eof = (num_lines < 0);

	 int num_objids = getSequenceLengthOfArg(2);
	 fixed_block<int> objids(num_objids);
	 getSequenceFromArg(2, objids.begin(), (int*)0);

	 int num_posids = getSequenceLengthOfArg(3);
	 fixed_block<int> posids(num_posids);
	 getSequenceFromArg(3, posids.begin(), (int*)0);

	 STD_IO::ifstream ifs(objid_file);

	 const int BUF_SIZE = 200;
	 char line[BUF_SIZE];

	 int num_read = 0;
	 while ( (read_to_eof || num_read < num_lines) 
				&& ifs.getline(line, BUF_SIZE) )
		{
		  // Allow for whole-line comments beginning with '#'. If '#' is
		  // seen, skip this line and continue with the next line. The trial
		  // count is unaffected.
		  if (line[0] == '#')
			 continue;

		  if (ifs.fail()) throw IO::InputError("Tlist::loadObjidFile");

		  istrstream ist(line);

		  IdItem<Trial> trial(Trial::make());
		  IdItem<GxSeparator> sep(GxSeparator::make());

		  int objn = 0;
		  int posn = 0;
		  while (ist >> objn)
			 {
				IdItem<GxSeparator> innersep(GxSeparator::make());
				innersep->addChild(posids[posn]);
				innersep->addChild(objids[objn-1]);
				sep->addChild(innersep->id());
				++posn;
			 }

		  if (ist.fail() && !ist.eof())
			 throw IO::InputError("Tlist::loadObjidFile");

		  trial->addNode(sep->id());

		  lappendVal(trial->id());

		  ++num_read;
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

	 TlistUtils::writeResponses(filename);
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
	 TlistUtils::writeIncidenceMatrix(filename);
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
	 TlistUtils::writeMatlab(filename);
  }
};

//---------------------------------------------------------------------
//
// TlistTcl::TlistPkg --
//
//---------------------------------------------------------------------

class TlistTcl::TlistPkg : public Tcl::TclPkg {
public:
  TlistPkg(Tcl_Interp* interp) :
	 Tcl::TclPkg(interp, "Tlist", "$Revision$")
  {
	 addCommand( new CreatePreviewCmd(interp, "Tlist::createPreview") );

	 addCommand( new DealSinglesCmd(interp, "Tlist::dealSingles") );
	 addCommand( new DealPairsCmd(interp, "Tlist::dealPairs") );
	 addCommand( new DealTriadsCmd(interp, "Tlist::dealTriads") );

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
