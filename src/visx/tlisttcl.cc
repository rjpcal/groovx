///////////////////////////////////////////////////////////////////////
//
// tlisttcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Mar 13 12:38:37 1999
// written: Wed Jul 18 11:27:35 2001
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

#include "tcl/tcllistobj.h"
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

namespace TlistTcl
{
  Util::UID createPreview(Tcl::List objid_list,
                          int pixel_width, int pixel_height);

  Tcl::List dealSingles(Tcl::List objids, Util::UID posid);

  Tcl::List dealPairs(Tcl::List objids1, Tcl::List objids2,
                      Util::UID posid1, Util::UID posid2);

  Tcl::List dealTriads(Tcl::List objids, Util::UID posid1,
                       Util::UID posid2, Util::UID posid3);

  Tcl::List loadObjidFile(const char* objid_file, Tcl::List objids,
                          Tcl::List posids, int num_lines);

  Tcl::List loadObjidFileAll(const char* objid_file, Tcl::List objids,
                             Tcl::List posids)
  {
    return loadObjidFile(objid_file, objids, posids, -1);
  }

  class TlistPkg;
}

///////////////////////////////////////////////////////////////////////
//
// Tlist Tcl package definitions
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
//
// TlistTcl::createPreview --
//
//---------------------------------------------------------------------

Util::UID TlistTcl::createPreview(Tcl::List objid_list,
                                  int pixel_width, int pixel_height)
{
  fixed_block<int> objids(objid_list.begin<int>(),
                          objid_list.end<int>());

  GWT::Canvas& canvas = Application::theApp().getCanvas();

  return TlistUtils::createPreview(canvas, &objids[0], objids.size(),
                                   pixel_width, pixel_height);
}

//--------------------------------------------------------------------
//
// TlistTcl::dealSingles --
//
// Make new Trial's so that there is one Trial for each valid object
// in the ObjList. Returns the ids of the trials that were created.
//
//--------------------------------------------------------------------

Tcl::List TlistTcl::dealSingles(Tcl::List objids, Util::UID posid)
{
  Tcl::List result;

  for (Tcl::List::Iterator<int>
         itr = objids.begin<int>(),
         end = objids.end<int>();
       itr != end;
       ++itr)
    {
      Ref<Trial> trial(Trial::make());

      trial->add(*itr, posid);

      Ref<GxNode> obj(*itr);
      trial->setType(obj->category());

      result.append(trial.id());
    }
  return result;
}

//--------------------------------------------------------------------
//
// TlistTcl::DealPairsCmd --
//
//--------------------------------------------------------------------

Tcl::List TlistTcl::dealPairs(Tcl::List objids1, Tcl::List objids2,
                              Util::UID posid1, Util::UID posid2)
{
  Tcl::List result;

  for (Tcl::List::Iterator<Util::UID>
         itr1 = objids1.begin<Util::UID>(),
         end1 = objids1.end<Util::UID>();
       itr1 != end1;
       ++itr1)
    for (Tcl::List::Iterator<Util::UID>
           itr2 = objids2.begin<Util::UID>(),
           end2 = objids2.end<Util::UID>();
         itr2 != end2;
       ++itr2)
      {
        Ref<Trial> trial(Trial::make());

        trial->add(*itr1, posid1);
        trial->add(*itr2, posid2);
        trial->setType(*itr1 == *itr2);

        result.append(trial.id());
      }

  return result;
}

//--------------------------------------------------------------------
//
// TlistTcl::dealTriads --
//
//--------------------------------------------------------------------

Tcl::List TlistTcl::dealTriads(Tcl::List objid_list, Util::UID posid1,
                               Util::UID posid2, Util::UID posid3)
{
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

  fixed_block<int> objids(objid_list.begin<Util::UID>(),
                          objid_list.end<Util::UID>());

  Util::UID base_triad[3];

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
            trial->add(base_triad[permutations[p][0]], posid1);
            trial->add(base_triad[permutations[p][1]], posid2);
            trial->add(base_triad[permutations[p][2]], posid3);
          result.append(trial.id());
        } // end p
      } // end itr3
    } // end itr2
  } // end itr1

  return result;
}

//--------------------------------------------------------------------
//
// TlistTcl::loadObjidFile --
//
//--------------------------------------------------------------------

Tcl::List TlistTcl::loadObjidFile(const char* objid_file,
                                  Tcl::List objid_list,
                                  Tcl::List posid_list, int num_lines)
{
  // Determine whether we will read to the end of the input stream, or
  // whether we will read only num_lines lines from the stream.
  bool read_to_eof = (num_lines < 0);

  fixed_block<Util::UID> objids(objid_list.begin<Util::UID>(),
                                objid_list.end<Util::UID>());

  fixed_block<Util::UID> posids(posid_list.begin<Util::UID>(),
                                posid_list.end<Util::UID>());

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

  return result;
}

//---------------------------------------------------------------------
//
// TlistTcl::TlistPkg --
//
//---------------------------------------------------------------------

class TlistTcl::TlistPkg : public Tcl::Pkg {
public:
  TlistPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "Tlist", "$Revision$")
  {
    def( &TlistTcl::createPreview,
         "createPreview", "objids pixel_width pixel_height" );

    def( &TlistTcl::dealSingles,
         "dealSingles", "objid(s) posid" );
    def( &TlistTcl::dealPairs,
         "dealPairs", "objids1 objids2 posid1 posid2" );
    def( &TlistTcl::dealTriads,
         "dealTriads", "objids posid1 posid2 posid3" );

    def( &TlistTcl::loadObjidFile,
         "loadObjidFile", "objid_file objids posids" );
    def( &TlistTcl::loadObjidFileAll,
         "loadObjidFile", "objid_file objids posids num_lines=-1" );

    def( &TlistUtils::writeResponses,
         "write_responses", "filename" );
    def( &TlistUtils::writeIncidenceMatrix,
         "writeIncidenceMatrix", "filename" );
    def( &TlistUtils::writeMatlab,
         "writeMatlab", "filename" );
  }
};

//--------------------------------------------------------------------
//
// TlistTcl::Tlist_Init --
//
//--------------------------------------------------------------------

extern "C"
int Tlist_Init(Tcl_Interp* interp)
{
DOTRACE("Tlist_Init");

  Tcl::Pkg* pkg = new TlistTcl::TlistPkg(interp);

  return pkg->initStatus();
}

static const char vcid_tlisttcl_cc[] = "$Header$";
#endif // !TLISTTCL_CC_DEFINED
