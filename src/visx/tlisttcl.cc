///////////////////////////////////////////////////////////////////////
//
// tlisttcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Mar 13 12:38:37 1999
// written: Wed Nov 13 13:01:18 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTTCL_CC_DEFINED
#define TLISTTCL_CC_DEFINED

#include "gfx/canvas.h"
#include "gfx/gxaligner.h"
#include "gfx/gxnode.h"
#include "gfx/gxseparator.h"

#include "grsh/grsh.h"

#include "gx/rect.h"

#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "util/arrays.h"
#include "util/error.h"
#include "util/ref.h"

#include "visx/gmodes.h"
#include "visx/gtext.h"
#include "visx/position.h"
#include "visx/grobj.h"
#include "visx/tlistutils.h"
#include "visx/trial.h"

#include <cmath>
#include <fstream>
#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

#include "util/trace.h"

namespace
{
  Util::UID doCreatePreview(Gfx::Canvas& canvas,
                            Util::UID* objids,
                            unsigned int objids_size,
                            int num_cols_hint = -1,
                            bool text_labels = true)
  {
    DOTRACE("<tlisttcl.cc>::doCreatePreview");

    Gfx::Rect<double> world_viewport = canvas.getWorldViewport();

    const double world_width = world_viewport.width();
    const double world_height = world_viewport.height();

    const double window_area = world_width*world_height;
    const double parcel_area = window_area/objids_size;
    const double raw_parcel_side = sqrt(parcel_area);

    const int num_cols = num_cols_hint > 0
      ? num_cols_hint : int(world_width/raw_parcel_side) + 1;

    const double parcel_side = world_width/num_cols;

    Ref<GxSeparator> preview_trial(GxSeparator::make());

    int x_step = -1;
    int y_step = 0;

    for (size_t i = 0; i < objids_size; ++i)
      {
        ++x_step;
        if (x_step == num_cols) { x_step = 0; ++y_step; }

        Ref<GrObj> obj(objids[i]);

        obj->setAlignmentMode(GxAligner::CENTER_ON_CENTER);
        obj->setBBVisibility(true);
        obj->setScalingMode(Gmodes::MAINTAIN_ASPECT_SCALING);
        obj->setMaxDimension(0.8);

        Ref<Position> obj_pos(Position::make());
        double obj_x = -world_width/2.0 + (x_step+0.5)*parcel_side;
        double obj_y = world_height/2.0 - (y_step+0.45)*parcel_side;
        obj_pos->translation.vec().set(obj_x, obj_y, 0.0);
        obj_pos->scaling.vec().set(parcel_side, parcel_side, 1.0);

        Ref<GxSeparator> obj_pair(GxSeparator::make());
        obj_pair->addChild(obj_pos);
        obj_pair->addChild(obj);

        preview_trial->addChild(obj_pair);

        if (text_labels)
          {
            Ref<Gtext> label(Gtext::make());
            label->setText(fstring(objids[i]));
            label->setAlignmentMode(GxAligner::CENTER_ON_CENTER);
            label->setScalingMode(Gmodes::MAINTAIN_ASPECT_SCALING);
            label->setHeight(0.1);

            Ref<Position> label_pos(Position::make());
            double label_x = obj_x;
            double label_y = obj_y - 0.50*parcel_side;
            label_pos->translation.vec().set(label_x, label_y, 0.0);
            label_pos->scaling.vec().set(parcel_side, parcel_side, 1.0);

            Ref<GxSeparator> label_pair(GxSeparator::make());
            label_pair->addChild(label_pos);
            label_pair->addChild(label);

            preview_trial->addChild(label_pair);
          }
      }

    return preview_trial.id();
  }
}

///////////////////////////////////////////////////////////////////////
//
// Tlist Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

namespace TlistTcl
{
  Util::UID createPreview(Tcl::List objid_list,
                          int num_cols_hit, bool use_text_labels);

  Tcl::List dealSingles(Tcl::List objids, Util::UID posid);

  Tcl::List dealPairs(Tcl::List objids1, Tcl::List objids2,
                      Util::UID posid1, Util::UID posid2);

  Tcl::List dealTriads(Tcl::List objids, Util::UID posid1,
                       Util::UID posid2, Util::UID posid3);

  Tcl::List loadObjidFile(const char* objid_file, Tcl::List objids,
                          Tcl::List posids, int num_lines);

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

Util::UID TlistTcl::createPreview(Tcl::List objid_list,
                                  int num_cols_hint = -1,
                                  bool text_labels = true)
{
  fixed_block<Util::UID> objids(objid_list.begin<Util::UID>(),
                                objid_list.end<Util::UID>());

  return doCreatePreview(Grsh::canvas(), &objids[0], objids.size(),
                         num_cols_hint, text_labels);
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

  if (ifs.fail())
    throw Util::Error(fstring("couldn't open '", objid_file,
                              "' for reading"));

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

#ifdef HAVE_SSTREAM
      std::istringstream ist(line);
#else
      istrstream ist(line);
#endif

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
  pkg->def( "createPreview", "objids num_cols_hint use_text_labels",
            &TlistTcl::createPreview );
  pkg->def( "createPreview", "objids",
            Util::bindLast(Util::bindLast(&TlistTcl::createPreview, true), -1) );

  pkg->def( "dealSingles", "objid(s) posid",
            &TlistTcl::dealSingles );
  pkg->def( "dealPairs", "objids1 objids2 posid1 posid2",
            &TlistTcl::dealPairs );
  pkg->def( "dealTriads", "objids posid1 posid2 posid3",
            &TlistTcl::dealTriads );

  pkg->def( "loadObjidFile", "objid_file objids posids num_lines=-1",
            &TlistTcl::loadObjidFile );
  pkg->def( "loadObjidFile", "objid_file objids posids",
            Util::bindLast(&TlistTcl::loadObjidFile, -1) );

  pkg->def( "write_responses", "filename", &TlistUtils::writeResponses );
  pkg->def( "writeIncidenceMatrix", "filename", &TlistUtils::writeIncidenceMatrix );
  pkg->def( "writeMatlab", "filename", &TlistUtils::writeMatlab );

  return pkg->initStatus();
}

static const char vcid_tlisttcl_cc[] = "$Header$";
#endif // !TLISTTCL_CC_DEFINED
