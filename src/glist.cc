///////////////////////////////////////////////////////////////////////
// glist.cc
// Rob Peters 
// created: Nov-98
// written: Fri Mar 12 11:32:06 1999
static const char vcid[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef GLIST_CC_DEFINED
#define GLIST_CC_DEFINED

#include "glist.h"

#include <iostream.h>
#include <strstream.h>
#include <string>
#include <GL/gl.h>

#include "objlist.h"

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

  ////////////////////////////
  // Local helper functions //
  ////////////////////////////

  Glist::Group* makeSingles(int num_objs, int& num_groups) {
	 num_groups = num_objs;
	 Glist::Group *groups = new Glist::Group[num_groups];
	 for (int i = 0; i < num_groups; i++) {
		groups[i].push_back(i);
	 }
	 return groups;
  }
  
  Glist::Group* makePairs(int num_objs, int& num_groups) {
	 num_groups = num_objs * num_objs;
	 Glist::Group *groups = new Glist::Group[num_groups];
	 int grp = 0;
	 for (int i = 0; i < num_objs; i++) {
		for (int j = 0; j < num_objs; j++) {
		  groups[grp].push_back(i);
		  groups[grp].push_back(j+num_objs);
		  grp++;
		}
	 }
	 return groups;
  }
  
  Glist::Group* makeTriads(int num_objs, int& num_groups) {
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
	 
	 num_groups = 18*(num_objs)*(num_objs-1)*(num_objs-2) / (3*2*1);
	 Glist::Group *groups = new Glist::Group[num_groups];
	 
	 int base_triad[3];
	 
	 int grp = 0;
	 for (int i = 0; i < num_objs-2; i++) {
		base_triad[0] = i;
		for (int j = i+1; j < num_objs; j++) {
		  base_triad[1] = j;
		  for (int k = j+1; k < num_objs; k++) {
			 base_triad[2] = k;
			 for (int p = 0; p < NUM_PERMS; p++) {
				for (int e = 0; e < 3; e++) {
				  groups[grp].push_back(base_triad[permutations[p][e]]
													+ e*num_objs);
				}
				grp++;
			 }
		  }
		}
	 }
	 return groups;
  }

} // end unnamed namespace

///////////////////////////////////////////////////////////////////////
// Glist member functions
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

Glist::Glist(int num, ObjList& olist) :
  itsNumGroups(num),
  itsGroups(new Group[itsNumGroups]),
  itsCurGroup(0), itsVisibility(0), 
  itsObjList(olist) 
{
DOTRACE("Glist::Glist(int num, ObjList& olist)");
}

Glist::Glist(GlistType type, int num_objs, ObjList& olist) : 
  itsCurGroup(0), itsVisibility(0),
  itsObjList(olist)
{
DOTRACE("Glist::Glist(GlistType type, int num_objs, ObjList& olist)");
  switch (type) {
  case GLIST_SINGLES:
	 itsGroups = makeSingles(num_objs, itsNumGroups);
	 break;
  case GLIST_PAIRS:
	 itsGroups = makePairs(num_objs, itsNumGroups);
	 break;
  case GLIST_TRIADS:
	 itsGroups = makeTriads(num_objs, itsNumGroups);
	 break;
  default: // bad situation
	 itsGroups = new Group[1];
	 break;
  } 
}

// the Glist destructor simply frees its dynamically allocated array member
Glist::~Glist() {
DOTRACE("Glist::~Glist");
  delete [] itsGroups;
}

// Input/Output from iostreams: 
//
// file format is as follows-- each line in the stream describes one
// group in the glist; within each line the individual objid's must be
// separated by whitespace

// this constructor assumes that the object's objid does not already
// reflect its position; therefore it adjusts for this
Glist::Glist(int num_grps, int shift, int num_objs, 
				 istream& is, ObjList& olist) : 
  itsNumGroups(num_grps), 
  itsGroups(new Group[itsNumGroups]),
  itsCurGroup(0), itsVisibility(0),
  itsObjList(olist) 
{
DOTRACE("Glist::Glist");
  char line[BUF_SIZE];

  int grp = 0;
  int val;
  int pos;
  while (grp < num_grps && is.getline(line, BUF_SIZE)) {
	 istrstream ist(line);
	 pos = 0;
	 while (ist >> val) {
		itsGroups[grp].push_back( pos*num_objs + val+shift);
		pos++;
	 }
	 if (itsGroups[grp].empty())
		break;
	 grp++;
  }

  itsNumGroups = grp;			  // in case we broke the loop early
}

// this constructor assumes that the objid's already reflect position
Glist::Glist(int num_grps, istream& is, ObjList& olist) : 
  itsNumGroups(num_grps), 
  itsGroups(new Group[itsNumGroups]),
  itsCurGroup(0), itsVisibility(0),
  itsObjList(olist) 
{
DOTRACE("Glist::Glist");
  char line[BUF_SIZE];

  int grp = 0;
  int val;
  while (grp < num_grps && is.getline(line, BUF_SIZE)) {
#ifdef LOCAL_DEBUG
	 DUMP_VAL1(grp);
	 DUMP_VAL2(line);
#endif
	 istrstream ist(line);
	 while (ist >> val) {
#ifdef LOCAL_DEBUG
		DUMP_VAL2(val);
#endif
		itsGroups[grp].push_back(val);
	 }
	 if (itsGroups[grp].empty())
		break;
	 grp++;
  }

  itsNumGroups = grp;			  // in case we broke the loop early
}

IOResult Glist::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Glist::serialize");
  char sep = ' ';
  int i;
  for (i = 0; i < itsNumGroups; i++) {
	 for (Group::iterator ii = itsGroups[i].begin();
			ii != itsGroups[i].end(); ii++) {
		os << *ii << sep;
	 }
	 os << endl;
  }
  return checkStream(os);
}

//////////////////
// manipulators //
//////////////////

// this function adds a new objid to the specified group. If the group
// is already full, the function does nothing, but returns without error.
void Glist::addToGroup(int objid, int group) {
DOTRACE("Glist::addToGroup");
  if ( itsObjList.olIsValidId(objid) ) {
	 itsGroups[group].push_back(objid);
  }
}

/////////////
// actions //
/////////////

// this function draws each object in the current group by calling
// ObjList's olDrawOne memeber function, which scales and translates
// the objects appropriately. If the Glist's global visibility is
// zero, this function does nothing but returns without error.
void Glist::drawCurGroup() const {
DOTRACE("Glist::drawCurGroup");
  if (itsVisibility != 0) {
	 for (Group::iterator ii = itsGroups[itsCurGroup].begin();
			ii != itsGroups[itsCurGroup].end(); ii++) {
		itsObjList.olDrawOne(*ii);
#ifdef LOCAL_DEBUG
		cerr << "drawing " << *ii << endl;
#endif
	 }
  }
}

void Glist::show(int group) {
DOTRACE("Glist::show");
  setCurGroup(group);
  setVisible(1);
  glClear(GL_COLOR_BUFFER_BIT);
  drawCurGroup();
  glFlush();
}

#endif // !GLIST_CC_DEFINED
