///////////////////////////////////////////////////////////////////////
//
// tlistutils.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sat Dec  4 03:04:32 1999
// commit: $Id$
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

#ifndef TLISTUTILS_CC_DEFINED
#define TLISTUTILS_CC_DEFINED

#include "visx/tlistutils.h"

#include "gfx/canvas.h"
#include "gfx/gxaligner.h"
#include "gfx/gxscaler.h"
#include "gfx/gxseparator.h"
#include "gfx/gxshapekit.h"
#include "gfx/gxtext.h"
#include "gfx/gxtransform.h"

#include "gx/rect.h"

#include "io/writer.h"

#include "tcl/tcllistobj.h"

#include "util/arrays.h"
#include "util/cstrstream.h"
#include "util/error.h"
#include "util/iter.h"
#include "util/objdb.h"
#include "util/ref.h"

#include "visx/trial.h"

#include <fstream>
#include <iomanip>

#include "util/trace.h"

using Util::Ref;

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
            label->setText(fstring(objids[i]));
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

  fstring gsubst(const fstring& in, char match, const char* replace)
  {
    fstring result;
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
DOTRACE("TlistUtils::writeResponses");

  STD_IO::ofstream ofs(filename);
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

  for (ObjDb::CastingIterator<Trial> itr(ObjDb::theDb().objects());
       itr.isValid();
       ++itr)
    {
      ofs << std::setw(wid) << itr->id() << " ";
      ofs << std::setw(wid) << itr->numResponses() << " ";
      ofs << std::setw(wid) << itr->avgResponse() << " ";
      ofs << std::setw(wid) << itr->avgRespTime() << " ";
      ofs << "% " << gsubst(itr->vxInfo(), '\n', "\\n") << std::endl;
    }

  if (ofs.fail())
    throw Util::Error("error while writing to file", SRC_POS);
}

void TlistUtils::writeIncidenceMatrix(const char* filename)
{
DOTRACE("TlistUtils::writeIncidenceMatrix");

  STD_IO::ofstream ofs(filename);

  for (ObjDb::CastingIterator<Trial> itr(ObjDb::theDb().objects());
       itr.isValid();
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
  STD_IO::ostream& itsOs;
public:
  MatlabTrialWriter(STD_IO::ostream& os) : itsOs(os) {}

  virtual void writeChar(const char*, char) {}
  virtual void writeInt(const char*, int) {}
  virtual void writeBool(const char*, bool) {}
  virtual void writeDouble(const char*, double) {}
  virtual void writeValueObj(const char*, const Value&) {}
  virtual void writeCstring(const char*, const char*) {}

  virtual void writeRawData(const char*,
                            const unsigned char*,
                            unsigned int) {}

  virtual void writeObject(const char*,
                           Util::SoftRef<const IO::IoObject> obj)
  {
    DOTRACE("MatlabTrialWriter::writeObject");
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
                                Util::Ref<const IO::IoObject> obj)
  {
    DOTRACE("MatlabTrialWriter::writeOwnedObject");
    writeObject(name, obj);
  }

  virtual void writeBaseClass(const char*,
                              Util::Ref<const IO::IoObject> basePart)
  {
    DOTRACE("MatlabTrialWriter::writeBaseClass");
    basePart->writeTo(*this);
  }

  virtual void writeRoot(const IO::IoObject* root)
  {
    DOTRACE("MatlabTrialWriter::writeRoot");
    root->writeTo(*this);
  }
};

void TlistUtils::writeMatlab(const char* filename)
{
DOTRACE("TlistUtils::writeMatlab");

  STD_IO::ofstream ofs(filename);

  ofs.setf(std::ios::fixed);
  ofs.precision(2);

  MatlabTrialWriter writer(ofs);

  for (ObjDb::CastingIterator<Trial> itr(ObjDb::theDb().objects());
       itr.isValid();
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

Util::UID TlistUtils::createPreview(Tcl::List objid_list,
                                    int num_cols_hint = -1,
                                    bool text_labels = true)
{
  fixed_block<Util::UID> objids(objid_list.begin<Util::UID>(),
                                objid_list.end<Util::UID>());

  return doCreatePreview(Gfx::Canvas::current(), &objids[0], objids.size(),
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

Tcl::List TlistUtils::dealSingles(Tcl::List objids, Util::UID posid)
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
// TlistUtils::DealPairsCmd --
//
//--------------------------------------------------------------------

Tcl::List TlistUtils::dealPairs(Tcl::List objids1, Tcl::List objids2,
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
// TlistUtils::dealTriads --
//
//--------------------------------------------------------------------

Tcl::List TlistUtils::dealTriads(Tcl::List objid_list, Util::UID posid1,
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

  fixed_block<Util::UID> objids(objid_list.begin<Util::UID>(),
                                objid_list.end<Util::UID>());

  fixed_block<Util::UID> posids(posid_list.begin<Util::UID>(),
                                posid_list.end<Util::UID>());

  STD_IO::ifstream ifs(objid_file);

  if (ifs.fail())
    throw Util::Error(fstring("couldn't open '", objid_file,
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
        throw Util::Error("error reading objid file", SRC_POS);

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
        throw Util::Error("error reading objid file", SRC_POS);

      trial->addNode(sep);

      result.append(trial->id());

      ++num_read;
    }

  return result;
}

static const char vcid_tlistutils_cc[] = "$Header$";
#endif // !TLISTUTILS_CC_DEFINED
