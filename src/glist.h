///////////////////////////////////////////////////////////////////////
// glist.h
// Rob Peters 
// created: Nov-98
// written: Fri Mar 12 12:57:15 1999
static const char vcid_glist_h[] = "$Id$";
//
// This class builds on the functionality provided by ObjList. It
// manages small collections of objid's from an ObjList and allows
// them to be displayed as a group. A current_group member is
// maintained; this is the group that is drawn when drawCurGroup() is
// called. In addition, a global visibility member is maintained; when
// it is zero, no objects will be drawn even if a draw command is
// issued.
///////////////////////////////////////////////////////////////////////

#ifndef GLIST_H_DEFINED
#define GLIST_H_DEFINED

#include <list>

#ifndef IO_H_INCLUDED
#include "io.h"
#endif

class ObjList;

///////////////////////////////////////////////////////////////////////
// Glist class
///////////////////////////////////////////////////////////////////////

class Glist : public virtual IO {
public:
  //////////////
  // creators //
  //////////////

  Glist(int num, ObjList& olist);

  enum GlistType { GLIST_SINGLES, GLIST_PAIRS, GLIST_TRIADS };
  Glist(GlistType type, int num_objs, ObjList& olist);

  Glist(int num_grps, istream& is, ObjList& olist);
  Glist(int num_grps, int shift, int num_objs, istream& is, ObjList& olist);
  ~Glist();

  virtual IOResult serialize(ostream &os, IOFlag flag = NO_FLAGS) const;

  ///////////////
  // accessors //
  ///////////////

  int numGroups() const { return itsNumGroups; }

  typedef list<int> Group;
  const Group& curGroup() const { return itsGroups[itsCurGroup]; }
  const Group& getGroup(int group) const { return itsGroups[group]; }

  //////////////////
  // manipulators //
  //////////////////

  void setCurGroup(int group) { if (group<itsNumGroups) itsCurGroup = group; }
  void addToGroup(int objid, int group);
  void setVisible(int vis) { itsVisibility = vis; } 

  /////////////
  // actions //
  /////////////

  void drawCurGroup() const;
  void show(int group);

private:
  Glist(const Glist&);			  // copy constructor not to be used
  Glist& operator=(const Glist&); // assignment operator not to be used

  const ObjList& itsObjList;	  // ObjList where objid's are stored
  int itsNumGroups;             // the number of groups in the list
  Group *itsGroups;				  // array of lists that hold the objid's
  int itsCurGroup;              // the current group to be drawn
  int itsVisibility;            // global visibility flag
};

#endif // !GLIST_H_DEFINED
