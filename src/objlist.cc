///////////////////////////////////////////////////////////////////////
// objlist.cc
// Rob Peters
// created: Nov-98
// written: Fri Apr 23 00:51:46 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef OBJLIST_CC_DEFINED
#define OBJLIST_CC_DEFINED

#include "objlist.h"

#include <iostream.h>
#include <string>
#include <typeinfo>
#include <strstream.h>

#include "grobjmgr.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
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

//////////////
// creators //
//////////////

ObjList::ObjList(int size):
  itsFirstVacant(0), 
  itsObjVec(size, NULL) 
{
DOTRACE("ObjList::ObjList");
#ifdef LOCAL_DEBUG
  DUMP_VAL2(itsObjVec.size());
#endif
}

ObjList::~ObjList() {
DOTRACE("ObjList::~ObjList");
  clearObjs();
}

void ObjList::serialize(ostream &os, IOFlag flag) const {
DOTRACE("ObjList::serialize");
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & IO::TYPENAME) { os << typeid(ObjList).name() << sep; }

  // itsObjVec: we will serialize only the non-null GrObj*'s in
  // itsObjVec. In order to correctly deserialize the object later, we
  // must write both the size of itsObjVec (in order to correctly
  // resize later), as well as the number of non-null objects that we
  // serialize (so that deserialize knows when to stop reading).
  os << itsObjVec.size() << sep;
  int num_null = 0;
  for (ObjVec::const_iterator ii = itsObjVec.begin(); 
       ii != itsObjVec.end(); 
       ii++) {
    if (*ii == NULL) num_null++;
  }
  int num_non_null = itsObjVec.size() - num_null;
  os << num_non_null << endl;
  int c = 0;
  for (int i = 0; i < itsObjVec.size(); i++) {
    if (itsObjVec[i] != NULL) {
      os << i << sep;
      // we must serialize the typename since deserialize requires a
      // typename in order to call the virtual constructor newGrobj
      itsObjVec[i]->serialize(os, IO::IOFlag(flag|TYPENAME));
      c++;
    }
  }
  if (c != num_non_null) {
	 throw IoLogicError(typeid(ObjList));
  }
  // itsFirstVacant
  os << itsFirstVacant << endl;
  if (os.fail()) throw OutputError(typeid(ObjList));
}

void ObjList::deserialize(istream &is, IOFlag flag) {
DOTRACE("ObjList::deserialize");
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }
  if (flag & IO::TYPENAME) {
    string name;
    is >> name;
    if (name != typeid(ObjList).name()) { 
		throw InputError(typeid(ObjList));
	 }
  }
  // itsObjVec
  clearObjs();
  int size, num_non_null;
  is >> size >> num_non_null;
  if ( size < 0 || num_non_null < 0 || num_non_null > size ) {
	 throw IoValueError(typeid(ObjList));
  }
  itsObjVec.resize(size, NULL);
  int objid;
  for (int i = 0; i < num_non_null; i++) {
    is >> objid;
	 if (objid < 0 || objid >= size) {
		throw IoValueError(typeid(ObjList));
	 }
    itsObjVec[objid] = GrobjMgr::newGrobj(is, flag);
  }
  // itsFirstVacant
  is >> itsFirstVacant;
  if (itsFirstVacant < 0) {
	 throw IoValueError(typeid(ObjList));
  }
  if (is.fail()) throw InputError(typeid(ObjList));
}

///////////////
// accessors //
///////////////

int ObjList::objCount() const {
DOTRACE("ObjList::objCount"); 
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

void ObjList::getValidObjids(vector<int>& vec) const {
DOTRACE("ObjList::getValidObjids");
  vec.clear();
  for (int i = 0; i < itsObjVec.size(); i++) {
    if (isValidObjid(i)) vec.push_back(i);
  }
}

//////////////////
// manipulators //
//////////////////

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
  itsObjVec[id] = NULL;         // reset the pointer to NULL

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
  itsFirstVacant = 0;
}

static const char vcid_objlist_cc[] = "$Header$";
#endif // !OBJLIST_CC_DEFINED
