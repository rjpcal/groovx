///////////////////////////////////////////////////////////////////////
//
// tlistutils.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Dec  4 03:04:32 1999
// written: Wed Oct 25 13:59:10 2000
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
#include "tlist.h"

#include "gwt/canvas.h"

#include "util/arrays.h"
#include "util/error.h"

#include <iostream.h>
#include <fstream.h>
#include <strstream.h>
#include <iomanip.h>
#include <cmath>

#include "util/trace.h"
#include "util/debug.h"

namespace {
  const char* const bad_trial_msg = "invalid trial id";
  const char* const bad_objid_msg = "objid out of range";
  const char* const bad_posid_msg = "posid out of range";
}

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

  Trial* preview = Trial::make();
  ItemWithId<TrialBase> preview_trial(preview, ItemWithId<TrialBase>::INSERT);

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

	 NullableItemWithId<GrObj> obj(objids[i]);
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

	 Gtext* label = Gtext::make();
	 label->setText(id_string);
	 ItemWithId<GrObj> label_obj(label, ItemWithId<GrObj>::INSERT);
	 label->setAlignmentMode(GrObj::CENTER_ON_CENTER);
	 label->setScalingMode(GrObj::MAINTAIN_ASPECT_SCALING);
	 label->setHeight(0.1);

	 ItemWithId<Position> obj_pos(Position::make(), ItemWithId<Position>::INSERT);
	 double obj_x = -world_width/2.0 + (x_step+0.5)*parcel_side;
	 double obj_y = world_height/2.0 - (y_step+0.45)*parcel_side;
	 obj_pos->setTranslate(obj_x, obj_y, 0.0);
	 obj_pos->setScale(parcel_side, parcel_side, 1.0);

	 ItemWithId<Position> label_pos(Position::make(), ItemWithId<Position>::INSERT);
	 double label_x = obj_x;
	 double label_y = obj_y - 0.50*parcel_side;
	 label_pos->setTranslate(label_x, label_y, 0.0);
	 label_pos->setScale(parcel_side, parcel_side, 1.0);

	 preview->add(objids[i], obj_pos.id());
	 preview->add(label_obj.id(), label_pos.id());
  }

  return preview_trial.id();
}

void TlistUtils::writeResponses(const char* filename) {
DOTRACE("TlistUtils::writeResponses");

  Tlist& tlist = Tlist::theTlist(); 

  STD_IO::ofstream ofs(filename);
  const int wid = 8;

  // We prepend a '%' to the header line so that MATLAB can ignore
  // this line as a comment
  ofs << '%' << setw(wid-1) << "Trial" << setw(wid) << "N" 
		<< setw(wid) << "Average" << setw(wid) << "msec\n";
  
  ofs.setf(ios::fixed);
  ofs.precision(2);
  for (Tlist::IdIterator
			itr = tlist.beginIds(),
			end = tlist.endIds();
		 itr != end;
		 ++itr)
	 {
		Trial* t = dynamic_cast<Trial*>(itr.getObject());

		if ( t )
		  {
			 ofs << setw(wid) << *itr;
			 ofs << setw(wid) << t->numResponses();
			 ofs << setw(wid) << t->avgResponse();	 
			 ofs << setw(wid) << t->avgRespTime() << endl;
		  }
	 }

  if (ofs.fail()) { throw ErrorWithMsg("error while writing to file"); }
}

void TlistUtils::writeIncidenceMatrix(const char* filename) {
DOTRACE("TlistUtils::writeIncidenceMatrix");

  Tlist& tlist = Tlist::theTlist(); 

  STD_IO::ofstream ofs(filename);

  for (Tlist::IdIterator
			itr = tlist.beginIds(),
			end = tlist.endIds();
		 itr != end;
		 ++itr)
	 {
		Trial* t = dynamic_cast<Trial*>(itr.getObject());

		if ( t )
		  {
			 // Use this to make sure we don't round down when we should round up.
			 double fudge = 0.0001;

			 int num_zeros = int( (1.0 - t->avgResponse()) * t->numResponses() + fudge);
			 int num_ones = t->numResponses() - num_zeros;

			 ofs << num_zeros << "  " << num_ones << endl;
		  }
	 }
}

int TlistUtils::loadObjidFile(const char* filename,
										int num_lines, int offset) {
DOTRACE("TlistUtils::loadObjidFile");

  STD_IO::ifstream ifs(filename);
  return readFromObjidsOnly(ifs, num_lines, offset);
}

int TlistUtils::readFromObjidsOnly(STD_IO::istream& is,
											  int num_lines, int offset) {
DOTRACE("TlistUtils::readFromObjidsOnly");

  Tlist& tlist = Tlist::theTlist(); 

  // FIXME this will not really do what we want anymore -- need to
  // return a list of the trial ids that are created
  // Remove all trials and resize itsTrials to 0
  tlist.clear();

  // Determine whether we will read to the end of the input stream, or
  // whether we will read only num_lines lines from the stream,
  // according to the convention set in the header file.
  bool read_to_eof = (num_lines < 0);

  const int BUF_SIZE = 200;
  char line[BUF_SIZE];

  int trial = 0;
  while ( (read_to_eof || trial < num_lines) 
          && is.getline(line, BUF_SIZE) ) {
	 // Allow for whole-line comments beginning with '#'. If '#' is
	 // seen, skip this line and continue with the next line. The trial
	 // count is unaffected.
	 if (line[0] == '#')
		continue;
	 istrstream ist(line);
	 Trial* t = Trial::make();
	 t->readFromObjidsOnly(ist, offset);
	 ItemWithId<TrialBase> trialinsert(t, ItemWithId<TrialBase>::INSERT);
    ++trial;
  }
  if (is.bad()) throw IO::InputError("TlistUtils::readFromObjids");

  // Return the number of trials that were loaded.
  return trial;
}

void TlistUtils::writeMatlab(const char* filename) {
DOTRACE("TlistUtils::writeMatlab");

  Tlist& tlist = Tlist::theTlist(); 

  STD_IO::ofstream ofs(filename);
	 
  ofs.setf(ios::fixed);
  ofs.precision(2);

  for (Tlist::IdIterator
			itr = tlist.beginIds(),
			end = tlist.endIds();
		 itr != end;
		 ++itr)
	 {
		Trial* t = dynamic_cast<Trial*>(itr.getObject());

		if ( t )
		  {
			 for (Trial::GrObjItr ii = t->beginGrObjs(), end = t->endGrObjs();
					ii != end;
					++ii)
				{
				  ofs << ii->id() << ' ';
				}

			 ofs << t->avgResponse() << endl;
		  }
	 }
}

static const char vcid_tlistutils_cc[] = "$Header$";
#endif // !TLISTUTILS_CC_DEFINED
