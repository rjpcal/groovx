///////////////////////////////////////////////////////////////////////
//
// tlistutils.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sat Dec  4 03:04:32 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_VISX_TLISTUTILS_CC_UTC20050626084015_DEFINED
#define GROOVX_VISX_TLISTUTILS_CC_UTC20050626084015_DEFINED

#include "tlistutils.h"

#include "geom/rect.h"

#include "gfx/gxaligner.h"
#include "gfx/gxscaler.h"
#include "gfx/gxseparator.h"
#include "gfx/gxshapekit.h"
#include "gfx/gxtext.h"
#include "gfx/gxtransform.h"

#include "io/writer.h"

#include "nub/objdb.h"
#include "nub/ref.h"

#include "tcl/tcllistobj.h"

#include "rutz/arrays.h"
#include "rutz/cstrstream.h"
#include "rutz/error.h"
#include "rutz/iter.h"

#include "visx/trial.h"

#include <fstream>
#include <iomanip>

#include "rutz/trace.h"

using Nub::Ref;

namespace
{
  Nub::UID doCreatePreview(const geom::rect<double>& world_viewport,
                           Nub::UID* objids,
                           unsigned int objids_size,
                           int num_cols_hint = -1,
                           bool text_labels = true)
  {
    GVX_TRACE("<tlisttcl.cc>::doCreatePreview");

    if (world_viewport.is_void())
      throw rutz::error("invalid void rect", SRC_POS);

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

        Ref<GxShapeKit> obj(objids[i]);

        obj->setAlignmentMode(GxAligner::CENTER_ON_CENTER);
        obj->setBBVisibility(true);
        obj->setScalingMode(GxScaler::MAINTAIN_ASPECT_SCALING);
        obj->setMaxDimension(0.8);

        Ref<GxTransform> obj_pos(GxTransform::make());
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
            Ref<GxText> label(GxText::make());
            label->setText(rutz::fstring(objids[i]));
            label->setAlignmentMode(GxAligner::CENTER_ON_CENTER);
            label->setScalingMode(GxScaler::MAINTAIN_ASPECT_SCALING);
            label->setHeight(0.1);

            Ref<GxTransform> label_pos(GxTransform::make());
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

  Ref<GxSeparator> makeSepPair(Ref<GxNode> obj1, Ref<GxNode> obj2)
  {
    Ref<GxSeparator> sep(GxSeparator::make());
    sep->addChild(obj1);
    sep->addChild(obj2);
    return sep;
  }

  rutz::fstring gsubst(const rutz::fstring& in,
                       char match, const char* replace)
  {
    rutz::fstring result;
    for (const char* p = in.c_str(); *p != '\0'; ++p)
      {
        if (*p == match)
          {
            result.append(replace);
          }
        else
          {
            result.append(*p);
          }
      }
    return result;
  }
}

void TlistUtils::writeResponses(const char* filename)
{
GVX_TRACE("TlistUtils::writeResponses");

  std::ofstream ofs(filename);
  const int wid = 8;

  // We prepend a '%' to the header line so that MATLAB can ignore
  // this line as a comment
  ofs << '%' << std::setw(wid-1) << "Trial" << " "
      << std::setw(wid) << "N" << " "
      << std::setw(wid) << "Average" << " "
      << std::setw(wid) << "msec" << " "
      << "  description\n";

  ofs.setf(std::ios::fixed);
  ofs.precision(2);

  for (Nub::ObjDb::CastingIterator<Trial> itr(Nub::ObjDb::theDb().objects());
       itr.is_valid();
       ++itr)
    {
      ofs << std::setw(wid) << itr->id() << " ";
      ofs << std::setw(wid) << itr->numResponses() << " ";
      ofs << std::setw(wid) << itr->avgResponse() << " ";
      ofs << std::setw(wid) << itr->avgRespTime() << " ";
      ofs << "% " << gsubst(itr->vxInfo(), '\n', "\\n") << std::endl;
    }

  if (ofs.fail())
    throw rutz::error("error while writing to file", SRC_POS);
}

void TlistUtils::writeIncidenceMatrix(const char* filename)
{
GVX_TRACE("TlistUtils::writeIncidenceMatrix");

  std::ofstream ofs(filename);

  for (Nub::ObjDb::CastingIterator<Trial> itr(Nub::ObjDb::theDb().objects());
       itr.is_valid();
       ++itr)
    {
      // Use this to make sure we don't round down when we should round up.
      double fudge = 0.0001;

      int num_zeros =
        int( (1.0 - itr->avgResponse()) * itr->numResponses() + fudge );

      int num_ones = itr->numResponses() - num_zeros;

      ofs << num_zeros << "  " << num_ones << std::endl;
    }
}

class MatlabTrialWriter : public IO::Writer
{
private:
  std::ostream& itsOs;
public:
  MatlabTrialWriter(std::ostream& os) : itsOs(os) {}

  virtual void writeChar(const char*, char) {}
  virtual void writeInt(const char*, int) {}
  virtual void writeBool(const char*, bool) {}
  virtual void writeDouble(const char*, double) {}
  virtual void writeValueObj(const char*, const rutz::value&) {}
  virtual void writeCstring(const char*, const char*) {}

  virtual void writeRawData(const char*,
                            const unsigned char*,
                            unsigned int) {}

  virtual void writeObject(const char*,
                           Nub::SoftRef<const IO::IoObject> obj)
  {
    GVX_TRACE("MatlabTrialWriter::writeObject");
    if (!obj.isValid())
      return;
    if (dynamic_cast<const GxShapeKit*>(obj.get()) != 0)
      {
        itsOs << obj->id() << ' ';
      }
    else
      {
        obj->writeTo(*this);
      }
  }

  virtual void writeOwnedObject(const char* name,
                                Nub::Ref<const IO::IoObject> obj)
  {
    GVX_TRACE("MatlabTrialWriter::writeOwnedObject");
    writeObject(name, obj);
  }

  virtual void writeBaseClass(const char*,
                              Nub::Ref<const IO::IoObject> basePart)
  {
    GVX_TRACE("MatlabTrialWriter::writeBaseClass");
    basePart->writeTo(*this);
  }

  virtual void writeRoot(const IO::IoObject* root)
  {
    GVX_TRACE("MatlabTrialWriter::writeRoot");
    root->writeTo(*this);
  }
};

void TlistUtils::writeMatlab(const char* filename)
{
GVX_TRACE("TlistUtils::writeMatlab");

  std::ofstream ofs(filename);

  ofs.setf(std::ios::fixed);
  ofs.precision(2);

  MatlabTrialWriter writer(ofs);

  for (Nub::ObjDb::CastingIterator<Trial> itr(Nub::ObjDb::theDb().objects());
       itr.is_valid();
       ++itr)
    {
      writer.writeRoot(*itr);
      ofs << itr->avgResponse() << '\n';
    }
}

//---------------------------------------------------------------------
//
// TlistUtils::createPreview --
//
//---------------------------------------------------------------------

Nub::UID TlistUtils::createPreview(Tcl::List objid_list,
                                   const geom::rect<double>& world_viewport,
                                   int num_cols_hint = -1,
                                   bool text_labels = true)
{
  rutz::fixed_block<Nub::UID> objids(objid_list.begin<Nub::UID>(),
                                     objid_list.end<Nub::UID>());

  return doCreatePreview(world_viewport,
                         &objids[0], objids.size(),
                         num_cols_hint, text_labels);
}

//--------------------------------------------------------------------
//
// TlistUtils::dealSingles --
//
// Make new Trial's so that there is one Trial for each valid object
// in the ObjList. Returns the ids of the trials that were created.
//
//--------------------------------------------------------------------

Tcl::List TlistUtils::dealSingles(Tcl::List objids, Nub::UID posid)
{
  Tcl::List result;

  Ref<GxNode> pos(posid);

  for (Tcl::List::Iterator<Nub::UID>
         itr = objids.begin<Nub::UID>(),
         end = objids.end<Nub::UID>();
       itr != end;
       ++itr)
    {
      Ref<GxNode> obj(*itr);

      Ref<Trial> trial(Trial::make());

      trial->addNode(makeSepPair(pos, obj));

      Nub::SoftRef<GxShapeKit> sk(*itr);
      if (sk.isValid())
        trial->setType(sk->category());

      result.append(trial.id());
    }
  return result;
}

//--------------------------------------------------------------------
//
// TlistUtils::DealPairsCmd --
//
//--------------------------------------------------------------------

Tcl::List TlistUtils::dealPairs(Tcl::List objids1, Tcl::List objids2,
                                Nub::UID posid1, Nub::UID posid2)
{
  Tcl::List result;

  Ref<GxNode> pos1(posid1);
  Ref<GxNode> pos2(posid2);

  for (Tcl::List::Iterator<Nub::UID>
         itr1 = objids1.begin<Nub::UID>(),
         end1 = objids1.end<Nub::UID>();
       itr1 != end1;
       ++itr1)
    {
      Ref<GxSeparator> sep1(makeSepPair(pos1, Ref<GxNode>(*itr1)));

      for (Tcl::List::Iterator<Nub::UID>
             itr2 = objids2.begin<Nub::UID>(),
             end2 = objids2.end<Nub::UID>();
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
// TlistUtils::dealTriads --
//
//--------------------------------------------------------------------

Tcl::List TlistUtils::dealTriads(Tcl::List objid_list, Nub::UID posid1,
                                 Nub::UID posid2, Nub::UID posid3)
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

  rutz::fixed_block<Nub::UID> objids(objid_list.begin<Nub::UID>(),
                                     objid_list.end<Nub::UID>());

  Nub::UID base_triad[3];

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
// TlistUtils::loadObjidFile --
//
//--------------------------------------------------------------------

Tcl::List TlistUtils::loadObjidFile(const char* objid_file,
                                    Tcl::List objid_list,
                                    Tcl::List posid_list, int num_lines)
{
  // Determine whether we will read to the end of the input stream, or
  // whether we will read only num_lines lines from the stream.
  bool read_to_eof = (num_lines < 0);

  rutz::fixed_block<Nub::UID> objids(objid_list.begin<Nub::UID>(),
                                     objid_list.end<Nub::UID>());

  rutz::fixed_block<Nub::UID> posids(posid_list.begin<Nub::UID>(),
                                     posid_list.end<Nub::UID>());

  std::ifstream ifs(objid_file);

  if (ifs.fail())
    throw rutz::error(rutz::fstring("couldn't open '", objid_file,
                                    "' for reading"), SRC_POS);

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
        throw rutz::error("error reading objid file", SRC_POS);

      rutz::icstrstream ist(line);

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
        throw rutz::error("error reading objid file", SRC_POS);

      trial->addNode(sep);

      result.append(trial->id());

      ++num_read;
    }

  return result;
}

static const char vcid_groovx_visx_tlistutils_cc_utc20050626084015[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_TLISTUTILS_CC_UTC20050626084015_DEFINED
