///////////////////////////////////////////////////////////////////////
// grobj.h
// Rob Peters 
// created: Dec-98
// written: Fri Mar 12 12:56:57 1999
static const char vcid_grobj_h[] = "$Id$";
//
// This is the abstract base class for graphic objects that are to be
// held and manipulated in ObjList's and Glist's. Classes derived from
// this base class can specify the details of how their objects will
// be drawn. The pure virtual function that must be overridden in
// derived classes is the grRecompile() function. This is the function
// that should generate an up-to-date OpenGL display list that, when
// called, will properly display the object; the id for this display
// list is then stored in itsDisplayList and itsIsCurrent is set to
// true. The function that should actually be called to display the
// object is grAction(). This function is declared virtual and so may
// be overridden, but the default version, which simply recompiles the
// display list (if necessary) and then calls the display list, should
// be adequate in most situations.
///////////////////////////////////////////////////////////////////////

#ifndef GROBJ_H_DEFINED
#define GROBJ_H_DEFINED

#ifndef IO_H_DEFINED
#include "io.h"
#endif

///////////////////////////////////////////////////////////////////////
// GrObj abstract class definition
///////////////////////////////////////////////////////////////////////

class GrObj : public virtual IO {
public:
  GrObj (int categ = -1);
  GrObj (istream &is);
  virtual ~GrObj ();

  // write the object's state from/to an output/input stream
  virtual IOResult serialize(ostream &os, IOFlag flag = NO_FLAGS) const;
  virtual IOResult deserialize(istream &is, IOFlag flag = NO_FLAGS);

  // must be overridden in the derived classes
  virtual void grRecompile() const = 0;

  // may be overridden, but defaults to simply calling the display list
  virtual void grAction() const;

  virtual int getCategory() const = 0;
  virtual void setCategory(int val) {}

protected:
  bool grIsCurrent() const { return itsIsCurrent; }
  void grPostRecompile() const { itsIsCurrent = false; }
  void grPostCompiled() const { itsIsCurrent = true; }

  void grNewList() const;
  int grDisplayList() const { return itsDisplayList; }

private:
  mutable bool itsIsCurrent;	  // true if displaylist is current
  mutable int itsDisplayList;	  // OpenGL display list that draws the object
};

#endif // !GROBJ_H_DEFINED
