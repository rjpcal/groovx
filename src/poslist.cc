///////////////////////////////////////////////////////////////////////
// poslist.cc
// Rob Peters
// created: Fri Mar 12 17:21:29 1999
// written: Fri Mar 12 17:26:30 1999
static const char vcid_poslist_cc[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef POSLIST_CC_DEFINED
#define POSLIST_CC_DEFINED

#include "poslist.h"

#include <iostream.h>
#include <strstream.h>

#include "randutils.h"

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
  itsPosVec[id] = NULL;			  // reset the pointer to NULL

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

#endif // !POSLIST_CC_DEFINED
