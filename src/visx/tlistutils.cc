///////////////////////////////////////////////////////////////////////
//
// tlistutils.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  4 03:04:32 1999
// written: Wed Aug  8 12:27:26 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTUTILS_CC_DEFINED
#define TLISTUTILS_CC_DEFINED

#include "tlistutils.h"

#include "gtext.h"
#include "position.h"
#include "rect.h"
#include "trial.h"

#include "gwt/canvas.h"

#include "gx/gxseparator.h"

#include "io/reader.h"

#include "util/error.h"
#include "util/objdb.h"
#include "util/ref.h"
#include "util/tostring.h"

#include <cmath>
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>

#include "util/trace.h"
#include "util/debug.h"

Util::UID TlistUtils::createPreview(GWT::Canvas& canvas,
                                    Util::UID* objids,
                                    unsigned int objids_size)
{
DOTRACE("TlistUtils::createPreview");

  Rect<double> world_viewport = canvas.getWorldViewport();

  const double world_width = world_viewport.width();
  const double world_height = world_viewport.height();

  const double window_area = world_width*world_height;
  const double parcel_area = window_area/objids_size;
  const double raw_parcel_side = sqrt(parcel_area);

  const int num_cols = int(world_width/raw_parcel_side) + 1;

  const double parcel_side = world_width/num_cols;

  Ref<GxSeparator> preview_trial(GxSeparator::make());

  int x_step = -1;
  int y_step = 0;

  for (size_t i = 0; i < objids_size; ++i)
    {
      ++x_step;
      if (x_step == num_cols) { x_step = 0; ++y_step; }

      Ref<GrObj> obj(objids[i]);

      obj->setAlignmentMode(GrObj::CENTER_ON_CENTER);
      obj->setBBVisibility(true);
      obj->setScalingMode(GrObj::MAINTAIN_ASPECT_SCALING);
      obj->setMaxDimension(0.8);

      Ref<Gtext> label(Gtext::make());
      label->setText(Util::Convert<Util::UID>::toString(objids[i]));
      label->setAlignmentMode(GrObj::CENTER_ON_CENTER);
      label->setScalingMode(GrObj::MAINTAIN_ASPECT_SCALING);
      label->setHeight(0.1);

      Ref<Position> obj_pos(Position::make());
      double obj_x = -world_width/2.0 + (x_step+0.5)*parcel_side;
      double obj_y = world_height/2.0 - (y_step+0.45)*parcel_side;
      obj_pos->translation.vec().set(obj_x, obj_y, 0.0);
      obj_pos->scaling.vec().set(parcel_side, parcel_side, 1.0);

      Ref<Position> label_pos(Position::make());
      double label_x = obj_x;
      double label_y = obj_y - 0.50*parcel_side;
      label_pos->translation.vec().set(label_x, label_y, 0.0);
      label_pos->scaling.vec().set(parcel_side, parcel_side, 1.0);

      Ref<GxSeparator> obj_pair(GxSeparator::make());
      obj_pair->addChild(obj_pos);
      obj_pair->addChild(obj);

      Ref<GxSeparator> label_pair(GxSeparator::make());
      label_pair->addChild(label_pos);
      label_pair->addChild(label);

      preview_trial->addChild(obj_pair);
      preview_trial->addChild(label_pair);
    }

  return preview_trial.id();
}

void TlistUtils::writeResponses(const char* filename)
{
DOTRACE("TlistUtils::writeResponses");

  STD_IO::ofstream ofs(filename);
  const int wid = 8;

  // We prepend a '%' to the header line so that MATLAB can ignore
  // this line as a comment
  ofs << '%' << setw(wid-1) << "Trial" << setw(wid) << "N"
      << setw(wid) << "Average" << setw(wid) << "msec\n";

  ofs.setf(ios::fixed);
  ofs.precision(2);

  for (ObjDb::CastingIterator<Trial>
         itr = ObjDb::theDb().begin(),
         end = ObjDb::theDb().end();
       itr != end;
       ++itr)
    {
      ofs << setw(wid) << itr->id();
      ofs << setw(wid) << itr->numResponses();
      ofs << setw(wid) << itr->avgResponse();
      ofs << setw(wid) << itr->avgRespTime() << endl;
    }

  if (ofs.fail()) { throw Util::Error("error while writing to file"); }
}

void TlistUtils::writeIncidenceMatrix(const char* filename)
{
DOTRACE("TlistUtils::writeIncidenceMatrix");

  STD_IO::ofstream ofs(filename);

  for (ObjDb::CastingIterator<Trial>
         itr = ObjDb::theDb().begin(),
         end = ObjDb::theDb().end();
       itr != end;
       ++itr)
    {
      // Use this to make sure we don't round down when we should round up.
      double fudge = 0.0001;

      int num_zeros =
        int( (1.0 - itr->avgResponse()) * itr->numResponses() + fudge );

      int num_ones = itr->numResponses() - num_zeros;

      ofs << num_zeros << "  " << num_ones << endl;
    }
}

class MatlabTrialWriter : public IO::Writer {
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

  virtual void writeObject(const char*,
                           Util::WeakRef<const IO::IoObject> obj)
  {
    if (obj.isValid() && dynamic_cast<const GrObj*>(obj.get()) != 0)
      {
        itsOs << obj->id() << ' ';
      }
    else
      {
        obj->writeTo(this);
      }
  }

  virtual void writeOwnedObject(const char* name,
                                Util::Ref<const IO::IoObject> obj)
  {
    writeObject(name, obj);
  }

  virtual void writeBaseClass(const char*,
                              Util::Ref<const IO::IoObject> basePart)
  {
    basePart->writeTo(this);
  }

  virtual void writeRoot(const IO::IoObject* root)
  {
    root->writeTo(this);
  }
};

void TlistUtils::writeMatlab(const char* filename)
{
DOTRACE("TlistUtils::writeMatlab");

  STD_IO::ofstream ofs(filename);

  ofs.setf(ios::fixed);
  ofs.precision(2);

  MatlabTrialWriter writer(ofs);

  for (ObjDb::CastingIterator<Trial>
         itr = ObjDb::theDb().begin(),
         end = ObjDb::theDb().end();
       itr != end;
       ++itr)
    {
      writer.writeRoot(*itr);
      ofs << itr->avgResponse() << '\n';
    }
}

static const char vcid_tlistutils_cc[] = "$Header$";
#endif // !TLISTUTILS_CC_DEFINED
