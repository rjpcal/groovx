///////////////////////////////////////////////////////////////////////
//
// tlisttcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Mar 13 12:38:37 1999
// written: Wed Jul 11 14:16:43 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTTCL_CC_DEFINED
#define TLISTTCL_CC_DEFINED

#include "application.h"
#include "tlistutils.h"
#include "trial.h"

#include "gx/gxnode.h"
#include "gx/gxseparator.h"

#include "tcl/tclcmd.h"
#include "tcl/tclpkg.h"

#include "util/arrays.h"
#include "util/ref.h"

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
    fixed_block<int> objids(beginOfArg(1, (int*)0), endOfArg(1, (int*)0));

    int pixel_width = getIntFromArg(2);
    int pixel_height = getIntFromArg(3);

    GWT::Canvas& canvas = Application::theApp().getCanvas();

    int previewid = TlistUtils::createPreview(canvas,
                                              &objids[0], objids.size(),
                                              pixel_width, pixel_height);

    returnVal(previewid);
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

    Tcl::List result;

    for (Tcl::List::Iterator<int>
           itr = beginOfArg(1, (int*)0),
           end = endOfArg(1, (int*)0);
         itr != end;
         ++itr)
      {
        Ref<Trial> trial(Trial::make());

        trial->add(*itr, posid);

        Ref<GxNode> obj(*itr);
        trial->setType(obj->category());

        result.append(trial.id());
      }

    returnVal(result);
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

    Tcl::List result;

    for (Tcl::List::Iterator<int>
           itr1 = beginOfArg(1, (int*)0),
           end1 = endOfArg(1, (int*)0);
         itr1 != end1;
         ++itr1)
      for (Tcl::List::Iterator<int>
             itr2 = beginOfArg(2, (int*)0),
             end2 = endOfArg(2, (int*)0);
           itr2 != end2;
           ++itr2)
        {
          Ref<Trial> trial(Trial::make());

          trial->add(*itr1, posid1);
          trial->add(*itr2, posid2);
          trial->setType(*itr1 == *itr2);

          result.append(trial.id());
        }

    returnVal(result);
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

    Tcl::List::Iterator<int>
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

    Tcl::List result;

    for (unsigned int i = 0; i < objids.size(); ++i) {
      base_triad[0] = objids[i];

      for (unsigned int j = i+1; j < objids.size(); ++j) {
        base_triad[1] = objids[j];

        for (unsigned int k = j+1; k < objids.size(); ++k) {
          base_triad[2] = objids[k];

          // loops over p,e run through all permutations
          for (int p = 0; p < NUM_PERMS; ++p) {
            Ref<Trial> trial(Trial::make());
            for (int e = 0; e < 3; ++e) {
              trial->add(base_triad[permutations[p][e]], posids[e]);
            }
            result.append(trial.id());
          } // end p
        } // end itr3
      } // end itr2
    } // end itr1

    returnVal(result);
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

    fixed_block<int> objids(beginOfArg(2, (int*)0), endOfArg(2, (int*)0));

    fixed_block<int> posids(beginOfArg(3, (int*)0), endOfArg(3, (int*)0));

    STD_IO::ifstream ifs(objid_file);

    const int BUF_SIZE = 200;
    char line[BUF_SIZE];

    Tcl::List result;

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

        Ref<Trial> trial(Trial::make());
        Ref<GxSeparator> sep(GxSeparator::make());

        int objn = 0;
        int posn = 0;
        while (ist >> objn)
          {
            Ref<GxSeparator> innersep(GxSeparator::make());
            innersep->addChild(posids[posn]);
            innersep->addChild(objids[objn-1]);
            sep->addChild(innersep->id());
            ++posn;
          }

        if (ist.fail() && !ist.eof())
          throw IO::InputError("Tlist::loadObjidFile");

        trial->addNode(sep->id());

        result.append(trial->id());

        ++num_read;
      }

    returnVal(result);
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
