///////////////////////////////////////////////////////////////////////
//
// tlistutils.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  4 03:04:32 1999
// written: Sat Jun  9 14:24:00 2001
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

#include "util/arrays.h"
#include "util/error.h"
#include "util/iditem.h"
#include "util/objdb.h"

#include <cmath>
#include <iostream.h>
#include <fstream.h>
#include <strstream.h>
#include <iomanip.h>

#include "util/trace.h"
#include "util/debug.h"

int TlistUtils::createPreview(const GWT::Canvas& canvas,
										int* objids, unsigned int objids_size,
										int pixel_width,
										int pixel_height) {
DOTRACE("TlistUtils::createPreview");
  Point<double> world_origin = canvas.getWorldFromScreen( Point<int>(0, 0) );

  double world_origin_x = world_origin.x();
  double world_origin_y = world_origin.y();

  Point<double> world_extent =
	 canvas.getWorldFromScreen( Point<int>(pixel_width, pixel_height) );

  double world_width = world_extent.x();
  double world_height = world_extent.y();

  world_width -= world_origin_x;
  world_height -= world_origin_y;

  fixed_block<Rect<double> > bbxs(objids_size);

  IdItem<Trial> preview_trial(Trial::make());

  double window_area = world_width*world_height;
  double parcel_area = window_area/objids_size;
  double raw_parcel_side = sqrt(parcel_area);
	 
  int num_cols = int(world_width/raw_parcel_side) + 1;

  double parcel_side = world_width/num_cols;

  int x_step = -1;
  int y_step = 0;

  for (size_t i = 0; i < objids_size; ++i) {
	 ++x_step;
	 if (x_step == num_cols) { x_step = 0; ++y_step; }

	 MaybeIdItem<GrObj> obj(objids[i]);
	 bool haveBB = obj->getBoundingBox(canvas, bbxs[i]);

	 if ( !haveBB ) {
		throw ErrorWithMsg("all objects must have bounding boxes");
	 }

	 obj->setAlignmentMode(GrObj::CENTER_ON_CENTER);
	 obj->setBBVisibility(true);
	 obj->setScalingMode(GrObj::MAINTAIN_ASPECT_SCALING);
	 obj->setMaxDimension(0.8);

	 char id_string[32];
	 ostrstream ost(id_string, 31);
	 ost << objids[i] << '\0';

	 IdItem<Gtext> label(Gtext::make());
	 label->setText(id_string);
	 label->setAlignmentMode(GrObj::CENTER_ON_CENTER);
	 label->setScalingMode(GrObj::MAINTAIN_ASPECT_SCALING);
	 label->setHeight(0.1);

	 IdItem<Position> obj_pos(Position::make());
	 double obj_x = -world_width/2.0 + (x_step+0.5)*parcel_side;
	 double obj_y = world_height/2.0 - (y_step+0.45)*parcel_side;
	 obj_pos->translation.vec().set(obj_x, obj_y, 0.0);
	 obj_pos->scaling.vec().set(parcel_side, parcel_side, 1.0);

	 IdItem<Position> label_pos(Position::make());
	 double label_x = obj_x;
	 double label_y = obj_y - 0.50*parcel_side;
	 label_pos->translation.vec().set(label_x, label_y, 0.0);
	 label_pos->scaling.vec().set(parcel_side, parcel_side, 1.0);

	 preview_trial->add(objids[i], obj_pos.id());
	 preview_trial->add(label.id(), label_pos.id());
  }

  return preview_trial.id();
}

void TlistUtils::writeResponses(const char* filename) {
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

  if (ofs.fail()) { throw ErrorWithMsg("error while writing to file"); }
}

void TlistUtils::writeIncidenceMatrix(const char* filename) {
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

void TlistUtils::writeMatlab(const char* filename) {
DOTRACE("TlistUtils::writeMatlab");

  STD_IO::ofstream ofs(filename);
	 
  ofs.setf(ios::fixed);
  ofs.precision(2);

  for (ObjDb::CastingIterator<Trial>
			itr = ObjDb::theDb().begin(),
			end = ObjDb::theDb().end();
		 itr != end;
		 ++itr)
	 {
		itr->writeMatlab(ofs);
	 }
}

static const char vcid_tlistutils_cc[] = "$Header$";
#endif // !TLISTUTILS_CC_DEFINED
