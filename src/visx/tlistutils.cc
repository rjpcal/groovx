///////////////////////////////////////////////////////////////////////
//
// tlistutils.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Dec  4 03:04:32 1999
// written: Tue Feb  1 17:53:04 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTUTILS_CC_DEFINED
#define TLISTUTILS_CC_DEFINED

#include "tlistutils.h"

#include <fstream.h>
#include <strstream.h>
#include <iomanip.h>
#include <cmath>

#include "canvas.h"
#include "error.h"
#include "gtext.h"
#include "objlist.h"
#include "position.h"
#include "poslist.h"
#include "rect.h"
#include "trial.h"
#include "tlist.h"

#include "trace.h"
#include "debug.h"

namespace {
  const char* const bad_trial_msg = "invalid trial id";
  const char* const bad_objid_msg = "objid out of range";
  const char* const bad_posid_msg = "posid out of range";
}

int TlistUtils::createPreview(Tlist& tlist,
										const Canvas& canvas,
										const vector<int>& objids,
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

  vector<Rect<double> > bbxs(objids.size());

  Trial* preview = new Trial();
  int previewid = tlist.insert(preview);

  double window_area = world_width*world_height;
  double parcel_area = window_area/objids.size();
  double raw_parcel_side = sqrt(parcel_area);
	 
  int num_cols = int(world_width/raw_parcel_side) + 1;

  double parcel_side = world_width/num_cols;

  int x_step = -1;
  int y_step = 0;

  for (size_t i = 0; i < objids.size(); ++i) {
	 ++x_step;
	 if (x_step == num_cols) { x_step = 0; ++y_step; }

	 GrObj* obj = ObjList::theObjList().getCheckedPtr(objids[i]);
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

	 Gtext* label = new Gtext(id_string);
	 int label_objid = ObjList::theObjList().insert(label);
	 label->setAlignmentMode(GrObj::CENTER_ON_CENTER);
	 label->setScalingMode(GrObj::MAINTAIN_ASPECT_SCALING);
	 label->setHeight(0.1);

	 Position* obj_pos = new Position();
	 int obj_posid = PosList::thePosList().insert(obj_pos);
	 double obj_x = -world_width/2.0 + (x_step+0.5)*parcel_side;
	 double obj_y = world_height/2.0 - (y_step+0.45)*parcel_side;
	 obj_pos->setTranslate(obj_x, obj_y, 0.0);
	 obj_pos->setScale(parcel_side, parcel_side, 1.0);

	 Position* label_pos = new Position();
	 int label_posid = PosList::thePosList().insert(label_pos);
	 double label_x = obj_x;
	 double label_y = obj_y - 0.50*parcel_side;
	 label_pos->setTranslate(label_x, label_y, 0.0);
	 label_pos->setScale(parcel_side, parcel_side, 1.0);
		
	 preview->add(objids[i], obj_posid);
	 preview->add(label_objid, label_posid);
  }

  return previewid;
}

int TlistUtils::makeSingles(Tlist& tlist, int posid) {
DOTRACE("TlistUtils::makeSingles");

  if ( !PosList::thePosList().isValidId(posid) )
	 { throw ErrorWithMsg(bad_posid_msg); }

  vector<int> vec;
  const ObjList& olist = ObjList::theObjList();
  olist.getValidIds(vec);

  tlist.clear();

  // This loop runs through all valid objid's and does two things: 
  // 1) it adds the objid and the given posid to the trial
  // 2) it sets the Trial's type to the category of its single GrObj
  for (size_t i=0; i < vec.size(); ++i) {
	 int id = vec[i];
	 if ( !tlist.isValidId(id) ) {
		tlist.insertAt(id, new Trial);
	 }
	 Tlist::Ptr t = tlist.getPtr(id);
	 t->add(id, posid);
	 const GrObj* obj = olist.getCheckedPtr(id);
	 t->setType(obj->getCategory());
  }

  return vec.size();
}

int TlistUtils::makePairs(Tlist& tlist, int posid1, int posid2) {
DOTRACE("TlistUtils::makePairs");
  if ( !PosList::thePosList().isValidId(posid1) ||
		 !PosList::thePosList().isValidId(posid2) )
	 { throw ErrorWithMsg(bad_posid_msg); }
  
  vector<int> vec;
  ObjList::theObjList().getValidIds(vec);
  
  tlist.clear();
  
  // This loop runs through all the trials and does two things:
  // 1) it adds the appropriate objids and the given posids to the trial
  // 2) it sets the Trial's type to zero if the objids are different, and to
  //    one if the objids are the same.
  int trialid = 0;
  for (size_t i = 0; i < vec.size(); ++i) {
	 for (size_t j = 0; j < vec.size(); ++j) {
		if ( !tlist.isValidId(trialid) ) {
		  tlist.insertAt(trialid, new Trial);
		}
		Tlist::Ptr t = tlist.getPtr(trialid);
		t->add(vec[i], posid1);
		t->add(vec[j], posid2);
		t->setType( i == j );
		
		++trialid;
	 }
  }

  // Return the number of trials generated.
  return trialid;
}

int TlistUtils::makeTriads(Tlist& tlist, int posid[]) {
DOTRACE("TlistUtils::makeTriads");
  if ( !PosList::thePosList().isValidId(posid[0]) ||
		 !PosList::thePosList().isValidId(posid[1]) ||
		 !PosList::thePosList().isValidId(posid[2]) ) { 
	 throw ErrorWithMsg(bad_posid_msg);
  }

  DebugEval(posid[0]); DebugEval(posid[1]); DebugEvalNL(posid[3]);

  vector<int> vec;
  ObjList::theObjList().getValidIds(vec);
	 
  tlist.clear();

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
  
  size_t num_objs = vec.size();

  // We need at least three objects in order to make any triads
  if ( num_objs < 3 ) return 0;

  int base_triad[3];
    
  int trial = 0;

  // loops over i,j,k construct all possible base triads
  for (size_t i = 0; i < num_objs-2; ++i) {
	 base_triad[0] = vec[i];
	 for (size_t j = i+1; j < num_objs; ++j) {
		base_triad[1] = vec[j];
		for (size_t k = j+1; k < num_objs; ++k) {
		  base_triad[2] = vec[k];

		  DebugEval(i); DebugEval(j); DebugEval(k); DebugEvalNL(trial);

		  // loops over p,e run through all permutations
		  for (int p = 0; p < NUM_PERMS; ++p) {
			 if ( !tlist.isValidId(trial) ) {
				tlist.insertAt(trial, new Trial);
			 }
			 Tlist::Ptr t = tlist.getPtr(trial);
			 for (int e = 0; e < 3; ++e) {
				t->add(base_triad[permutations[p][e]], posid[e]);
			 }
			 ++trial;
		  } // end p

		} // end k
	 } // end j
  } // end i

  // Return the number of trials generated
  return trial;
}

int TlistUtils::makeSummaryTrial(Tlist& tlist,
											int trialid, int num_cols, double scale,
											double xstep, double ystep) {
DOTRACE("TlistUtils::makeSummaryTrial");
  ObjList& olist = ObjList::theObjList();
  int num_objs = olist.count();
	 
  // Figure number of columns-- (num_objs-1)/num_cols gives one fewer
  // than the number of rows that we need, so add 1 to it
  int num_rows = 1 + (num_objs-1)/num_cols;

  vector<int> objids;
  olist.getValidIds(objids);
	 
  // Coords of upper left corner of viewing area
  const double x0 = scale * (0.0 - xstep*(num_cols-1)/2.0);
  const double y0 = scale * (0.0 + ystep*(num_rows-1)/2.0);

  DebugEval(num_objs); DebugEval(num_cols); DebugEvalNL(num_rows);
  DebugEval(scale); DebugEval(xstep); DebugEval(ystep); 
  DebugEval(x0); DebugEvalNL(y0);

  PosList& plist = PosList::thePosList();
	 
  if ( !tlist.isValidId(trialid) ) {
	 tlist.insertAt(trialid, new Trial);
  }

  Tlist::Ptr t = tlist.getPtr(trialid);

  for (size_t i=0; i < objids.size(); ++i) {
	 int row = i / num_cols;
	 int col = i % num_cols;
	 Position* p = new Position;
	 double xpos = x0+col*xstep*scale;
	 double ypos = y0-row*ystep*scale;
		
	 DebugEval(xpos);
	 DebugEvalNL(ypos);
		
	 p->setTranslate(xpos, ypos, 0.0);
	 p->setScale(scale, scale, 1.0);
	 int posid = plist.insert(p);

	 t->add(objids[i], posid);
  }

  // Return the id of the trial generated.
  return trialid;
}

void TlistUtils::writeResponses(Tlist& tlist, const char* filename) {
DOTRACE("TlistUtils::writeResponses");
  vector<int> trialids;
  tlist.getValidIds(trialids);

  DebugEvalNL(trialids.size());

  ofstream ofs(filename);
  const int wid = 8;

  // We prepend a '%' to the header line so that MATLAB can ignore
  // this line as a comment
  ofs << '%' << setw(wid-1) << "Trial" << setw(wid) << "N" 
		<< setw(wid) << "Average" << setw(wid) << "msec\n";
  
  ofs.setf(ios::fixed);
  ofs.precision(2);
  for (size_t i = 0; i < trialids.size(); ++i) {
	 Tlist::Ptr t = tlist.getPtr(trialids[i]);
	 ofs << setw(wid) << trialids[i];
	 ofs << setw(wid) << t->numResponses();
	 ofs << setw(wid) << t->avgResponse();	 
	 ofs << setw(wid) << t->avgRespTime() << endl;
  }

  if (ofs.fail()) { throw ErrorWithMsg("error while writing to file"); }
}

void TlistUtils::writeIncidenceMatrix(Tlist& tlist, const char* filename) {
DOTRACE("TlistUtils::writeIncidenceMatrix");
  vector<int> trialids;
  tlist.getValidIds(trialids);
	 
  DebugEvalNL(trialids.size());
	 
  ofstream ofs(filename);
	 
  for (size_t i = 0; i < trialids.size(); ++i) {
	 Tlist::Ptr t = tlist.getPtr(trialids[i]);
		
	 // Use this to make sure we don't round down when we should round up.
	 double fudge = 0.0001;
		
	 int num_zeros = int( (1.0 - t.avgResponse()) * t->numResponses() + fudge);
	 int num_ones = t->numResponses() - num_zeros;

	 ofs << num_zeros << "  " << num_ones << endl;
  }
}

int TlistUtils::loadObjidFile(Tlist& tlist, const char* filename,
										int num_lines, int offset) {
DOTRACE("TlistUtils::loadObjidFile");

  ifstream ifs(filename);
  return readFromObjidsOnly(tlist, ifs, num_lines, offset);
}

int TlistUtils::readFromObjidsOnly(Tlist& tlist, istream& is,
											  int num_lines, int offset) {
DOTRACE("TlistUtils::readFromObjidsOnly");
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
	 Trial* t = new Trial;
	 t->readFromObjidsOnly(ist, offset);
    tlist.insert(t);
    ++trial;
  }                          
  if (is.bad()) throw InputError("TlistUtils::readFromObjids");

  // Return the number of trials that were loaded.
  return trial;
}

void TlistUtils::writeMatlab(Tlist& tlist, const char* filename) {
DOTRACE("TlistUtils::writeMatlab");
  vector<int> trialids;
  tlist.getValidIds(trialids);

  DebugEvalNL(trialids.size());

  ofstream ofs(filename);
  const int wid = 8;
	 
  ofs.setf(ios::fixed);
  ofs.precision(2);
  for (size_t i = 0; i < trialids.size(); ++i) {
	 Tlist::Ptr t = tlist.getPtr(trialids[i]);
	 Trial::ObjGrp objs = t->objs();
	 
	 for (size_t j = 0; j < objs.size(); ++j) {
		ofs << objs[j].objid << ' ';
	 }
		
	 ofs << t->avgResponse() << endl;
  }
}

void TlistUtils::addObject(Tlist& tlist, int trialid, int objid, int posid) {
DOTRACE("TlistUtils::addObject");
  if ( trialid < 0 ) { throw ErrorWithMsg(bad_trial_msg); }

  if ( !ObjList::theObjList().isValidId(objid) )
	 { throw ErrorWithMsg(bad_objid_msg); }

  if ( !PosList::thePosList().isValidId(posid) )
	 { throw ErrorWithMsg(bad_posid_msg); }

  if ( !tlist.isValidId(trialid) ) {
	 tlist.insertAt(trialid, new Trial);
  }

  tlist.getPtr(trialid)->add(objid, posid);
}

static const char vcid_tlistutils_cc[] = "$Header$";
#endif // !TLISTUTILS_CC_DEFINED
