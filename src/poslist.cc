///////////////////////////////////////////////////////////////////////
// poslist.cc
// Rob Peters
// created: Fri Mar 12 17:21:29 1999
// written: Tue Mar 16 19:34:42 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef POSLIST_CC_DEFINED
#define POSLIST_CC_DEFINED

#include "poslist.h"

#include <iostream.h>
#include <strstream.h>
#include <typeinfo>
#include <string>

#include "randutils.h"

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
// PosList member functions
///////////////////////////////////////////////////////////////////////

PosList::PosList(int size):
  itsFirstVacant(0), 
  itsPosVec(size, NULL) 
{
DOTRACE("PosList::PosList");
#ifdef LOCAL_DEBUG
  DUMP_VAL2(itsPosVec.size());
#endif
}

PosList::~PosList() {
DOTRACE("PosList::~PosList");
  clearPos();
}

IOResult PosList::serialize(ostream &os, IOFlag flag) const {
DOTRACE("PosList::serialize");
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & IO::TYPENAME) { os << typeid(PosList).name() << sep; }

  // itsPosVec: we will serialize only the non-null Position*'s in
  // itsPosVec. In order to correctly deserialize the object later, we
  // must write both the size of itsPosVec (in order to correctly
  // resize later), as well as the number of non-null objects that we
  // serialize (so that deserialize knows when to stop reading).
  os << itsPosVec.size() << sep;
  int num_null = 0;
  for (PosVec::const_iterator ii = itsPosVec.begin(); 
       ii != itsPosVec.end(); 
       ii++) {
    if (*ii == NULL) num_null++;
  }
  int num_non_null = itsPosVec.size() - num_null;
  os << num_non_null << endl;
  int c = 0;
  for (int i = 0; i < itsPosVec.size(); i++) {
    if (itsPosVec[i] != NULL) {
      os << i << sep;
      itsPosVec[i]->serialize(os, flag);
      c++;
    }
  }
  Assert(c==num_non_null);
  // itsFirstVacant
  os << itsFirstVacant << endl;
  return checkStream(os);
}

IOResult PosList::deserialize(istream &is, IOFlag flag) {
DOTRACE("PosList::deserialize");
#ifdef LOCAL_DEBUG
  DUMP_VAL2(flag);
#endif
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }
  if (flag & IO::TYPENAME) {
    string name;
    is >> name;
#ifdef LOCAL_DEBUG
    DUMP_VAL2(name);
#endif
    if (name != string(typeid(PosList).name())) { return IO_ERROR; }
  }
  // itsPosVec
  clearPos();
  int size, num_non_null;
  is >> size >> num_non_null;
#ifdef LOCAL_DEBUG
  DUMP_VAL1(size);
  DUMP_VAL2(num_non_null);
#endif
  Assert( num_non_null <= size );
  itsPosVec.resize(size, NULL);
  int posid;
  for (int i = 0; i < num_non_null; i++) {
    is >> posid;
#ifdef LOCAL_DEBUG
    DUMP_VAL2(posid);
#endif
    itsPosVec[posid] = new Position(is, flag);
  }
  // itsFirstVacant
  is >> itsFirstVacant;
  return checkStream(is);
}

int PosList::npos() const {
DOTRACE("PosList::npos"); 
  int count=0;
  for (PosVec::const_iterator ii = itsPosVec.begin(); 
       ii != itsPosVec.end(); ii++) {
    if (*ii != NULL) count++;
  }
  return count;
}

bool PosList::isValidPosid(int id) const {
DOTRACE("PosList::isValidPosid");
#ifdef LOCAL_DEBUG
  DUMP_VAL1(id);
  DUMP_VAL1(id>=0);
  DUMP_VAL2(itsPosVec.size());
  DUMP_VAL2(itsPosVec[id]);
  DUMP_VAL2(id<itsPosVec.size());
  DUMP_VAL2(itsPosVec[id] != NULL);
#endif
  return ( id >= 0 && id < itsPosVec.size() && itsPosVec[id] != NULL); 
}

Position* PosList::getPos(int posid) const {
DOTRACE("PosList::getPos"); 
#ifdef LOCAL_DEBUG
  DUMP_VAL1(posid);
  DUMP_VAL2((void *) itsPosVec[posid]);
#endif
  return itsPosVec[posid]; 
}

int PosList::addPos(Position *pos) {
DOTRACE("PosList::addPos");
  int new_site = itsFirstVacant;
  addPosAt(new_site, pos);
  return new_site;              // return the id of the inserted Position
}

void PosList::addPosAt(int id, Position *pos) {
DOTRACE("PosList::addPosAt");
  if (id < 0) return;

  if (id >= itsPosVec.size()) {
    itsPosVec.resize(id+RESIZE_CHUNK, NULL);
  }

  delete itsPosVec[id];
  itsPosVec[id] = pos;

  // make sure itsFirstVacant is up-to-date
  while ( (itsPosVec[itsFirstVacant] != NULL) &&
          (++itsFirstVacant < itsPosVec.size()) );
}

// this member function delete's the Position pointed to by itsPosVec[i]
void PosList::removePos(int id) {
DOTRACE("PosList::removePos");
  if (!isValidPosid(id)) return;

  delete itsPosVec[id];
  itsPosVec[id] = NULL;         // reset the pointer to NULL

  // reset itsFirstVacant in case i would now be the first vacant
  if (itsFirstVacant > id) itsFirstVacant = id;
}

// delete all Position's currently pointed to in itsPosVec by repeatedly
// calling PosList::removePos
void PosList::clearPos() {
DOTRACE("PosList::clearPos()");
  for (PosVec::iterator ii = itsPosVec.begin(); ii != itsPosVec.end(); ii++) {
    delete *ii;
    *ii = NULL;
  }
}

static const char vcid_poslist_cc[] = "$Header$";
#endif // !POSLIST_CC_DEFINED
