///////////////////////////////////////////////////////////////////////
// objlist.h 
// Rob Peters
// Created: Nov-98
// written: Mon Mar  8 20:25:49 1999
//
// The ObjList class implements a list of GrObj pointers. The
// interface allows for management of a collection of GrObj's (i.e.,
// adding and removing objects), and for rendering individual GrObj's.
///////////////////////////////////////////////////////////////////////

#ifndef OBJLIST_H_DEFINED
#define OBJLIST_H_DEFINED

#ifndef VECTOR_INCLUDED
#include <vector>
#define VECTOR_INCLUDED
#endif

#ifndef IO_H_INCLUDED
#include "io.h"
#endif
#ifndef GROBJ_H_INCLUDED
#include "grobj.h"
#endif

///////////////////////////////////////////////////////////////////////
// ObjList class
///////////////////////////////////////////////////////////////////////

class ObjList {
public:
  //////////////
  // creators //
  //////////////

  ObjList (int size);
  // construct a list of 'size' GrObj*'s, initialized to NULL

  ObjList (istream &is, const char* type, int num_objs, int grp_size, 
			  double obj_spacing, double t_jitter, double r_jitter);
  // constructs multiple sets of objects from an istream containing
  // descriptions of GrObj's suitable for passing to the virtual
  // constructor GrobjMgr::newGrobj. The istream is read in one pass
  // to an array of c-strings, which in turn is read 'grp_size' # of
  // times to create the obj's in each of 'grp_size' positions


  ~ObjList ();

  IOResult writeRangeObjs(ostream &os, int first, int count) const;
  // calls serialize(os) on a range of GrObj's from the ObjList,
  // starting with 'first', and writing 'count' objects total

  IOResult readRangeObjs(istream &is, int first, int count);
  IOResult readRangeObjs(const char* type, istream &is, int first, int count);
  // constructs 'count' GrObj's into the ObjList, starting with first,
  // by calling a virtual constructor GrobjMgr::newGrobj. When the
  // string argument 'type' is provided, it is assumed that the
  // istream describes objects all of that type, and contains no
  // internal type strings

  void readRangeObjs(char **line_array, int first, int count);
  void readRangeObjs(const char* type, char **line_array, int first, int count);
  // constructs 'count' GrObj's into the ObjList, starting with first,
  // by calling a virtual constructor GrobjMgr::newGrobj. Each line of
  // the char** array is first used to initialize a strstream, which
  // is then passed to the virtual constructor. If the string argument
  // 'type' is provided, it is assumed that the istream describes
  // objects all of that type, and contains no internal type strings

  ///////////////
  // accessors //
  ///////////////

  int olNObjs() const;
  // returns the number of (both filled and unfilled) sites in the ObjList

  bool olIsValidId(int id) const;
  // returns true if 'id' is a valid index into a non-NULL GrObj* in
  // the ObjList, given its current size

  GrObj* olObj (int id) const { return itsObjVec[id]; }
  GrObj* operator[] (int id) const { return olObj(id); }
  // both functions return the GrObj* at the index given by 'id'.
  // There is no range-checking--this must be done by the client with
  // olIsValidId().

  //////////////////
  // manipulators //
  //////////////////

  int insert(GrObj *obj);
  // add obj at the next available location, and return the index
  // where it was inserted. If necessary, the list will be expanded to
  // make room for the obj

  void insertAt(int id, GrObj *obj);
  // add obj at index 'id', destroying any GrObj that previously
  // occupied that location. The list will be expanded if 'id' exceeds
  // the size of the list. If id is < 0, the function returns without
  // effect.

  void remove(int id);
  // delete the GrObj at index 'i', and reset the GrObj* to NULL

  void olClearList();
  // delete all GrObj's held by the list, and reset all GrObj*'s to NULL

  void scaleRangeObjs(int first, int count,
							 double t_x, double t_y, double s_x, double s_y,
							 double t_jitter, double r_jitter);

  /////////////
  // actions //
  /////////////

  void olDrawOne(int id) const { olObj(id)->grAction(); }
  // call the action function for the GrObj at site 'id'

private:
  ObjList(const ObjList&);		  // copy constructor not to be used
  ObjList& operator=(const ObjList&); // assignment operator not to be used

  typedef vector<GrObj *> ObjVec;
  int itsFirstVacant;			  // smallest index of a vacant array site
  ObjVec itsObjVec;				  // associative array from objid's to GrObj*'s
};

#endif // !OBJLIST_H_DEFINED
