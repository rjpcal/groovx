///////////////////////////////////////////////////////////////////////
//
// id.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu May 27 18:29:58 1999
// written: Tue Oct 12 10:44:33 1999
// $Id$
//
// This file defines a template iterator/object-id class. These are
// currently intended to provide access to singleton object
// repositories. For this purpose, two template specializations have
// been instantiated: one to access GrObj's through the ObjList
// singleton, and one to access Position's through the singleton
// PosList. Finally, two classes ObjId and PosId have been defined
// which publicly inherit from these templates. These classes provide
// a more reasonable identifier name than the full template name, and
// allow for easy forward declarations of these classes.
//
///////////////////////////////////////////////////////////////////////

#ifndef ID_H_DEFINED
#define ID_H_DEFINED

class istream;
class ostream;

///////////////////////////////////////////////////////////////////////
//
// Id class definition
//
///////////////////////////////////////////////////////////////////////

template <class T, class C>
class Id {
public:
  Id(int val = 0) : itsIndex(val) {}
  Id(const Id& rhs) : itsIndex(rhs.itsIndex) {}
  const Id& operator=(const Id& rhs) 
    { itsIndex = rhs.itsIndex; return *this; }
  ~Id() {}

  int toInt() const { return itsIndex; }

  Id& operator++() { ++itsIndex; return *this; }
  Id& operator--() { --itsIndex; return *this; }

  Id operator+(int val) { return Id(itsIndex+val); }
  Id operator-(int val) { return Id(itsIndex-val); }

  bool operator<(const Id& rhs) { return ( itsIndex < rhs.itsIndex ); }
  bool operator>(const Id& rhs) { return ( itsIndex > rhs.itsIndex ); }

  // Check if id is valid index into its container
  bool operator!() const;

  // Unchecked access into its container
  T* get() const;

  template <class T1, class C1> 
  friend istream& operator>>(istream& is, Id<T1, C1>& id);

  template <class T2, class C2>
  friend ostream& operator<<(ostream& os, const Id<T2, C2>& id);

private:
  int itsIndex;
};


///////////////////////////////////////////////////////////////////////
//
// ObjId and PosId definitions
//
// These definitions are basically typedef-- PosId and ObjId should
// have functionality identical to the templates from which they
// derive. However, if these are declared as typedefs, then it is not
// possible to use forward definitions of the classes.
//
///////////////////////////////////////////////////////////////////////

class GrObj;
class ObjList;

class ObjId : public Id<GrObj, ObjList> {
private:
  typedef Id<GrObj, ObjList> Base;
public:
  ObjId(int val = 0) : Base(val) {}
  ObjId(const Base& rhs) : Base(rhs) {}
  const ObjId& operator=(const ObjId& rhs)
    { Base::operator=(rhs); return *this; }
  ~ObjId() {}
};

class Position;
class PosList;

class PosId : public Id<Position, PosList> {
private:
  typedef Id<Position, PosList> Base;
public:
  PosId(int val = 0) : Base(val) {}
  PosId(const Base& rhs) : Base(rhs) {}
  const PosId& operator=(const PosId& rhs) 
    {	Base::operator=(rhs); return *this; }
  ~PosId() {}
};

static const char vcid_id_h[] = "$Header$";
#endif // !ID_H_DEFINED
