///////////////////////////////////////////////////////////////////////
// objlist.cc
// Rob Peters
// created: Nov-98
// written: Fri Mar 12 17:25:29 1999
static const char vcid_objlist_cc[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef OBJLIST_CC_DEFINED
#define OBJLIST_CC_DEFINED

#include "objlist.h"

#include <iostream.h>
#include <strstream.h>

#include "randutils.h"
#include "grobjmgr.h"
#include "scaleable.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

#ifndef NULL
#define NULL 0L
#endif

///////////////////////////////////////////////////////////////////////
// File scope declarations
///////////////////////////////////////////////////////////////////////

namespace {
  const int BUF_SIZE = 200;
  const int RESIZE_CHUNK = 20;
}

///////////////////////////////////////////////////////////////////////
// ObjList member functions
///////////////////////////////////////////////////////////////////////

ObjList::ObjList(int size):
  itsFirstVacant(0), 
  itsObjVec(size, NULL) 
{
DOTRACE("ObjList::ObjList");
#ifdef LOCAL_DEBUG
  DUMP_VAL2(itsObjVec.size());
#endif
}

ObjList::ObjList(istream &is, const char* type, 
					  int num_objs, int grp_size, double obj_spacing, 
					  double t_jitter, double r_jitter) : 
  itsFirstVacant(0),
  itsObjVec(num_objs*grp_size, NULL)
{
DOTRACE("ObjList::ObjList");
  char **line_array = new char*[num_objs];
  
  for (int ln = 0; ln < num_objs; ln++) {
	 line_array[ln] = new char[BUF_SIZE];
	 is.getline(line_array[ln], BUF_SIZE);
  }
  
  double x_pos = -1*(grp_size-1)*obj_spacing/2.0;
  
  for (int i = 0; i < grp_size; i++) {
	 readRangeObjs(type, line_array, i*num_objs, num_objs);
	 scaleRangeObjs(i*num_objs, num_objs, x_pos, 0.0, 1.0, 1.0, 
						 t_jitter, r_jitter);
	 x_pos += obj_spacing;
  }
  
  for (int j = 0; j < num_objs; j++) {
	 delete [] line_array[j];
  }
  delete [] line_array;
}

ObjList::~ObjList() {
DOTRACE("ObjList::~ObjList");
  clearObjs();
}

IOResult ObjList::writeRangeObjs(ostream &os, int first, int count) const {
DOTRACE("ObjList::writeRangeObjs");
  int id = first;
  for (int i = 0; i < count; i++) {
	 itsObjVec[id+i]->serialize(os);
  }
  return checkStream(os);
}

IOResult ObjList::readRangeObjs(istream &is, int first, int count) {
DOTRACE("ObjList::readRangeObjs");
  for (int i = 0; i < count; i++) {
	 addObjAt(first+i, GrobjMgr::newGrobj(is));
  }
  return checkStream(is);
}

IOResult ObjList::readRangeObjs(const char* type, istream &is, 
										  int first, int count) {
DOTRACE("ObjList::readRangeObjs");
  for (int i = 0; i < count; i++) {
	 addObjAt(first+i, GrobjMgr::newGrobj(type, is));
  }
  return checkStream(is);
}

void ObjList::readRangeObjs(char **line_array, int first, int count) {
DOTRACE("ObjList::readRangeObjs");
  for (int i = 0; i < count; i++) {
	 istrstream ist(line_array[i]);
	 addObjAt(first+i, GrobjMgr::newGrobj(ist));
  }
}

void ObjList::readRangeObjs(const char* type, char **line_array, 
									 int first, int count) {
DOTRACE("ObjList::readRangeObjs");
  for (int i = 0; i < count; i++) {
	 istrstream ist(line_array[i]);
	 addObjAt(first+i, GrobjMgr::newGrobj(type, ist));
  }
}

void ObjList::scaleRangeObjs(int first, int count,
									  double t_x, double t_y, double s_x, double s_y,
									  double t_jitter, double r_jitter) {
  Scaleable *p = NULL;
  for (int i = 0; i < count; i++) {
	 if ( (p = dynamic_cast<Scaleable *>(itsObjVec[first+i])) != NULL ) {
		p->setTranslate(t_x, t_y+randDoubleRange(-t_jitter, t_jitter), 0.0);
		p->setScale(s_x, s_y, 0.0);
		p->setAngle(randDoubleRange(-r_jitter, r_jitter));
	 }
#ifdef LOCAL_DEBUG
	 else {
		cerr << "dynamic_cast<Scaleable *> failed.\n";
	 }
#endif
  }
}

int ObjList::nobjs() const {
DOTRACE("ObjList::nobjs"); 
  int count=0;
  for (ObjVec::const_iterator ii = itsObjVec.begin(); 
		 ii != itsObjVec.end(); ii++) {
	 if (*ii != NULL) count++;
  }
  return count;
}

bool ObjList::isValidObjid(int id) const {
DOTRACE("ObjList::isValidObjid");
#ifdef LOCAL_DEBUG
  DUMP_VAL1(id);
  DUMP_VAL1(id>=0);
  DUMP_VAL2(itsObjVec.size());
  DUMP_VAL2(itsObjVec[id]);
  DUMP_VAL2(id<itsObjVec.size());
  DUMP_VAL2(itsObjVec[id] != NULL);
#endif
  return ( id >= 0 && id < itsObjVec.size() && itsObjVec[id] != NULL); 
}

int ObjList::addObj(GrObj *obj) {
DOTRACE("ObjList::addObj");
  int new_site = itsFirstVacant;
  addObjAt(new_site, obj);
  return new_site;              // return the id of the inserted GrObj
}

void ObjList::addObjAt(int id, GrObj *obj) {
DOTRACE("ObjList::addObjAt");
  if (id < 0) return;

  if (id >= itsObjVec.size()) {
	 itsObjVec.resize(id+RESIZE_CHUNK, NULL);
  }

  delete itsObjVec[id];
  itsObjVec[id] = obj;

  // make sure itsFirstVacant is up-to-date
  while ( (itsObjVec[itsFirstVacant] != NULL) &&
			 (++itsFirstVacant < itsObjVec.size()) );
}

// this member function delete's the GrObj pointed to by itsObjVec[i]
void ObjList::removeObj(int id) {
DOTRACE("ObjList::removeObj");
  if (!isValidObjid(id)) return;

  delete itsObjVec[id];
  itsObjVec[id] = NULL;			  // reset the pointer to NULL

  // reset itsFirstVacant in case i would now be the first vacant
  if (itsFirstVacant > id) itsFirstVacant = id;
}

// delete all GrObj's currently pointed to in itsObjVec by repeatedly
// calling ObjList::removeObj
void ObjList::clearObjs() {
DOTRACE("ObjList::clearObjs()");
  for (ObjVec::iterator ii = itsObjVec.begin(); ii != itsObjVec.end(); ii++) {
	 delete *ii;
	 *ii = NULL;
  }
}

#endif // !OBJLIST_CC_DEFINED
