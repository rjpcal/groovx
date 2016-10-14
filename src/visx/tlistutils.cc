/** @file visx/tlistutils.cc auxiliary functions for Trial objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat Dec  4 03:04:32 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#include "tcl/list.h"

#include "rutz/arrays.h"
#include "rutz/cstrstream.h"
#include "rutz/error.h"
#include "rutz/iter.h"
#include "rutz/sfmt.h"

#include "visx/trial.h"

#include <fstream>
#include <iomanip>
#include <vector>

#include "rutz/trace.h"

using nub::ref;

namespace
{
  nub::uid doCreatePreview(const geom::rect<double>& world_viewport,
                           nub::uid* objids,
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

    ref<GxSeparator> preview_trial(GxSeparator::make());

    int x_step = -1;
    int y_step = 0;

    for (size_t i = 0; i < objids_size; ++i)
      {
        ++x_step;
        if (x_step == num_cols) { x_step = 0; ++y_step; }

        ref<GxShapeKit> obj(objids[i]);

        obj->setAlignmentMode(GxAligner::CENTER_ON_CENTER);
        obj->setBBVisibility(true);
        obj->setScalingMode(GxScaler::MAINTAIN_ASPECT_SCALING);
        obj->setMaxDimension(0.8);

        ref<GxTransform> obj_pos(GxTransform::make());
        double obj_x = -world_width/2.0 + (x_step+0.5)*parcel_side;
        double obj_y = world_height/2.0 - (y_step+0.45)*parcel_side;
        obj_pos->translation.vec().set(obj_x, obj_y, 0.0);
        obj_pos->scaling.vec().set(parcel_side, parcel_side, 1.0);

        ref<GxSeparator> obj_pair(GxSeparator::make());
        obj_pair->addChild(obj_pos);
        obj_pair->addChild(obj);

        preview_trial->addChild(obj_pair);

        if (text_labels)
          {
            ref<GxText> label(GxText::make());
            label->setText(rutz::sfmt("%lu", objids[i]));
            label->setAlignmentMode(GxAligner::CENTER_ON_CENTER);
            label->setScalingMode(GxScaler::MAINTAIN_ASPECT_SCALING);
            label->setHeight(0.1);

            ref<GxTransform> label_pos(GxTransform::make());
            double label_x = obj_x;
            double label_y = obj_y - 0.50*parcel_side;
            label_pos->translation.vec().set(label_x, label_y, 0.0);
            label_pos->scaling.vec().set(parcel_side, parcel_side, 1.0);

            ref<GxSeparator> label_pair(GxSeparator::make());
            label_pair->addChild(label_pos);
            label_pair->addChild(label);

            preview_trial->addChild(label_pair);
          }
      }

    return preview_trial.id();
  }

  ref<GxSeparator> makeSepPair(ref<GxNode> obj1, ref<GxNode> obj2)
  {
    ref<GxSeparator> sep(GxSeparator::make());
    sep->addChild(obj1);
    sep->addChild(obj2);
    return sep;
  }

  rutz::fstring gsubst(const rutz::fstring& in,
                       const char match,
                       const char* const replace)
  {
    std::vector<char> result;
    result.reserve(in.length());

    for (const char* p = in.c_str(); *p != '\0'; ++p)
      {
        if (*p == match)
          {
            for (const char* q = replace; *q != '\0'; ++q)
              result.push_back(*q);
          }
        else
          {
            result.push_back(*p);
          }
      }

    result.push_back('\0');

    return rutz::fstring(&result[0]);
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

  for (nub::objectdb::casting_iterator<Trial> itr(nub::objectdb::instance().objects());
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

  for (nub::objectdb::casting_iterator<Trial> itr(nub::objectdb::instance().objects());
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

class MatlabTrialWriter : public io::writer
{
private:
  std::ostream& itsOs;
public:
  MatlabTrialWriter(std::ostream& os) : itsOs(os) {}

  virtual void write_char(const char*, char) {}
  virtual void write_int(const char*, int) {}
  virtual void write_bool(const char*, bool) {}
  virtual void write_double(const char*, double) {}
  virtual void write_value_obj(const char*, const rutz::value&) {}
  virtual void write_cstring(const char*, const char*) {}

  virtual void write_byte_array(const char*,
                            const unsigned char*,
                            unsigned int) {}

  virtual void write_object(const char*,
                           nub::soft_ref<const io::serializable> obj)
  {
    GVX_TRACE("MatlabTrialWriter::write_object");
    if (!obj.is_valid())
      return;
    if (dynamic_cast<const GxShapeKit*>(obj.get()) != 0)
      {
        itsOs << obj->id() << ' ';
      }
    else
      {
        obj->write_to(*this);
      }
  }

  virtual void write_owned_object(const char* name,
                                nub::ref<const io::serializable> obj)
  {
    GVX_TRACE("MatlabTrialWriter::write_owned_object");
    write_object(name, obj);
  }

  virtual void write_base_class(const char*,
                                nub::ref<const io::serializable> base_part)
  {
    GVX_TRACE("MatlabTrialWriter::write_base_class");
    base_part->write_to(*this);
  }

  virtual void write_root(const io::serializable* root)
  {
    GVX_TRACE("MatlabTrialWriter::write_root");
    root->write_to(*this);
  }
};

void TlistUtils::writeMatlab(const char* filename)
{
GVX_TRACE("TlistUtils::writeMatlab");

  std::ofstream ofs(filename);

  ofs.setf(std::ios::fixed);
  ofs.precision(2);

  MatlabTrialWriter writer(ofs);

  for (nub::objectdb::casting_iterator<Trial> itr(nub::objectdb::instance().objects());
       itr.is_valid();
       ++itr)
    {
      writer.write_root(*itr);
      ofs << itr->avgResponse() << '\n';
    }
}

//---------------------------------------------------------------------
//
// TlistUtils::createPreview --
//
//---------------------------------------------------------------------

nub::uid TlistUtils::createPreview(tcl::list objid_list,
                                   const geom::rect<double>& world_viewport,
                                   int num_cols_hint = -1,
                                   bool text_labels = true)
{
  rutz::fixed_block<nub::uid> objids(objid_list.begin<nub::uid>(),
                                     objid_list.end<nub::uid>());

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

tcl::list TlistUtils::dealSingles(tcl::list objids, nub::uid posid)
{
  tcl::list result;

  ref<GxNode> pos(posid);

  for (tcl::list::iterator<nub::uid>
         itr = objids.begin<nub::uid>(),
         end = objids.end<nub::uid>();
       itr != end;
       ++itr)
    {
      ref<GxNode> obj(*itr);

      ref<Trial> trial(Trial::make());

      trial->addNode(makeSepPair(pos, obj));

      nub::soft_ref<GxShapeKit> sk(*itr);
      if (sk.is_valid())
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

tcl::list TlistUtils::dealPairs(tcl::list objids1, tcl::list objids2,
                                nub::uid posid1, nub::uid posid2)
{
  tcl::list result;

  ref<GxNode> pos1(posid1);
  ref<GxNode> pos2(posid2);

  for (tcl::list::iterator<nub::uid>
         itr1 = objids1.begin<nub::uid>(),
         end1 = objids1.end<nub::uid>();
       itr1 != end1;
       ++itr1)
    {
      ref<GxSeparator> sep1(makeSepPair(pos1, ref<GxNode>(*itr1)));

      for (tcl::list::iterator<nub::uid>
             itr2 = objids2.begin<nub::uid>(),
             end2 = objids2.end<nub::uid>();
           itr2 != end2;
           ++itr2)
        {
          ref<GxSeparator> sep2(makeSepPair(pos2, ref<GxNode>(*itr2)));

          ref<Trial> trial(Trial::make());

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

tcl::list TlistUtils::dealTriads(tcl::list objid_list, nub::uid posid1,
                                 nub::uid posid2, nub::uid posid3)
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

  rutz::fixed_block<nub::uid> objids(objid_list.begin<nub::uid>(),
                                     objid_list.end<nub::uid>());

  nub::uid base_triad[3];

  tcl::list result;

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
                  ref<GxSeparator> sep(GxSeparator::make());

                  sep->addChild(makeSepPair(
                      ref<GxNode>(base_triad[permutations[p][0]]),
                      ref<GxNode>(posid1)));

                  sep->addChild(makeSepPair(
                      ref<GxNode>(base_triad[permutations[p][1]]),
                      ref<GxNode>(posid2)));

                  sep->addChild(makeSepPair(
                      ref<GxNode>(base_triad[permutations[p][2]]),
                      ref<GxNode>(posid3)));

                  ref<Trial> trial(Trial::make());
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

tcl::list TlistUtils::loadObjidFile(const char* objid_file,
                                    tcl::list objid_list,
                                    tcl::list posid_list, int num_lines)
{
  // Determine whether we will read to the end of the input stream, or
  // whether we will read only num_lines lines from the stream.
  bool read_to_eof = (num_lines < 0);

  rutz::fixed_block<nub::uid> objids(objid_list.begin<nub::uid>(),
                                     objid_list.end<nub::uid>());

  rutz::fixed_block<nub::uid> posids(posid_list.begin<nub::uid>(),
                                     posid_list.end<nub::uid>());

  std::ifstream ifs(objid_file);

  if (ifs.fail())
    throw rutz::error(rutz::sfmt("couldn't open '%s' for reading",
                                 objid_file), SRC_POS);

  const int BUF_SIZE = 200;
  char line[BUF_SIZE];

  tcl::list result;

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

      ref<Trial> trial(Trial::make());
      ref<GxSeparator> sep(GxSeparator::make());

      int objn = 0;
      int posn = 0;
      while (ist >> objn)
        {
          sep->addChild(makeSepPair(ref<GxNode>(posids[posn]),
                                    ref<GxNode>(objids[objn-1])));
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

#endif // !GROOVX_VISX_TLISTUTILS_CC_UTC20050626084015_DEFINED
