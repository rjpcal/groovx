///////////////////////////////////////////////////////////////////////
//
// tlisttcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Mar 13 12:38:37 1999
// written: Sun Aug 26 08:53:53 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTTCL_CC_DEFINED
#define TLISTTCL_CC_DEFINED

#include "application.h"
#include "tlistutils.h"
#include "trial.h"

#include "gfx/gxnode.h"
#include "gfx/gxseparator.h"

#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "util/arrays.h"
#include "util/ref.h"

#include <fstream.h>
#include <strstream.h>

#include "util/trace.h"

///////////////////////////////////////////////////////////////////////
//
// Tlist Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

namespace TlistTcl
{
  Util::UID createPreview(Tcl::List objid_list);

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

  Ref<GxSeparator> makeSepPair(Util::Ref<GxNode> obj1, Util::Ref<GxNode> obj2)
  {
    Ref<GxSeparator> sep(GxSeparator::make());
    sep->addChild(obj1);
    sep->addChild(obj2);
    return sep;
  }
}

//---------------------------------------------------------------------
//
// TlistTcl::createPreview --
//
//---------------------------------------------------------------------

Util::UID TlistTcl::createPreview(Tcl::List objid_list)
{
  fixed_block<Util::UID> objids(objid_list.begin<Util::UID>(),
                                objid_list.end<Util::UID>());

  Gfx::Canvas& canvas = Application::theApp().getCanvas();

  return TlistUtils::createPreview(canvas, &objids[0], objids.size());
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

  Ref<GxNode> pos(posid);

  for (Tcl::List::Iterator<Util::UID>
         itr = objids.begin<Util::UID>(),
         end = objids.end<Util::UID>();
       itr != end;
       ++itr)
    {
      Ref<GxNode> obj(*itr);

      Ref<Trial> trial(Trial::make());

      trial->addNode(makeSepPair(pos, obj));
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

  Ref<GxNode> pos1(posid1);
  Ref<GxNode> pos2(posid2);

  for (Tcl::List::Iterator<Util::UID>
         itr1 = objids1.begin<Util::UID>(),
         end1 = objids1.end<Util::UID>();
       itr1 != end1;
       ++itr1)
    {
      Ref<GxSeparator> sep1(makeSepPair(pos1, Ref<GxNode>(*itr1)));

      for (Tcl::List::Iterator<Util::UID>
             itr2 = objids2.begin<Util::UID>(),
             end2 = objids2.end<Util::UID>();
           itr2 != end2;
           ++itr2)
        {
          Ref<GxSeparator> sep2(makeSepPair(pos2, Ref<GxNode>(*itr2)));

          Ref<Trial> trial(Trial::make());

          trial->addNode(makeSepPair(sep1, sep2));
          trial->setType(*itr1 == *itr2);

          result.append(trial.id());
        }
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
  const unsigned int NUM_PERMS = 18;
  static int permutations[NUM_PERMS][3] =
  {
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
    {2, 1, 0}
  };

  fixed_block<Util::UID> objids(objid_list.begin<Util::UID>(),
                                objid_list.end<Util::UID>());

  Util::UID base_triad[3];

  Tcl::List result;

  for (unsigned int i = 0; i < objids.size(); ++i)
    {
      base_triad[0] = objids[i];

      for (unsigned int j = i+1; j < objids.size(); ++j)
        {
          base_triad[1] = objids[j];

          for (unsigned int k = j+1; k < objids.size(); ++k)
            {
              base_triad[2] = objids[k];

              // loops over p,e run through all permutations
              for (unsigned int p = 0; p < NUM_PERMS; ++p)
                {
                  Ref<GxSeparator> sep(GxSeparator::make());

                  sep->addChild(makeSepPair(
                      Ref<GxNode>(base_triad[permutations[p][0]]),
                      Ref<GxNode>(posid1)));

                  sep->addChild(makeSepPair(
                      Ref<GxNode>(base_triad[permutations[p][1]]),
                      Ref<GxNode>(posid2)));

                  sep->addChild(makeSepPair(
                      Ref<GxNode>(base_triad[permutations[p][2]]),
                      Ref<GxNode>(posid3)));

                  Ref<Trial> trial(Trial::make());
                  trial->addNode(sep);
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

      if (ifs.fail())
        throw Util::Error("error reading objid file");

      istrstream ist(line);

      Ref<Trial> trial(Trial::make());
      Ref<GxSeparator> sep(GxSeparator::make());

      int objn = 0;
      int posn = 0;
      while (ist >> objn)
        {
          sep->addChild(makeSepPair(Ref<GxNode>(posids[posn]),
                                    Ref<GxNode>(objids[objn-1])));
          ++posn;
        }

      if (ist.fail() && !ist.eof())
        throw Util::Error("error reading objid file");

      trial->addNode(sep);

      result.append(trial->id());

      ++num_read;
    }

  return result;
}

//--------------------------------------------------------------------
//
// TlistTcl::Tlist_Init --
//
//--------------------------------------------------------------------

extern "C"
int Tlist_Init(Tcl_Interp* interp)
{
DOTRACE("Tlist_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Tlist", "$Revision$");
  pkg->def( "createPreview", "objids", &TlistTcl::createPreview );

  pkg->def( "dealSingles", "objid(s) posid",
            &TlistTcl::dealSingles );
  pkg->def( "dealPairs", "objids1 objids2 posid1 posid2",
            &TlistTcl::dealPairs );
  pkg->def( "dealTriads", "objids posid1 posid2 posid3",
            &TlistTcl::dealTriads );

  pkg->def( "loadObjidFile", "objid_file objids posids",
            &TlistTcl::loadObjidFile );
  pkg->def( "loadObjidFile", "objid_file objids posids num_lines=-1",
            &TlistTcl::loadObjidFileAll );

  pkg->def( "write_responses", "filename", &TlistUtils::writeResponses );
  pkg->def( "writeIncidenceMatrix", "filename", &TlistUtils::writeIncidenceMatrix );
  pkg->def( "writeMatlab", "filename", &TlistUtils::writeMatlab );

  return pkg->initStatus();
}

static const char vcid_tlisttcl_cc[] = "$Header$";
#endif // !TLISTTCL_CC_DEFINED
