///////////////////////////////////////////////////////////////////////
//
// grobj.h
// Rob Peters 
// created: Dec-98
// written: Wed Jun 30 10:23:05 1999
// $Id$
//
// This is the abstract base class for graphic objects. GrObj*'s may
// be stored and manipulated in ObjList's. Classes derived from this
// base class must specify the details of how their objects will be
// drawn. The pure virtual function that must be overridden in derived
// classes is the grRecompile() function. This is the function that
// should generate an up-to-date OpenGL display list that, when
// called, will properly display the object. The function that should
// actually be called to display the object is draw(). This
// function is declared virtual and so may be overridden, but the
// default version, which simply recompiles the display list (if
// necessary) and then calls the display list, should be adequate in
// most situations.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJ_H_DEFINED
#define GROBJ_H_DEFINED

#ifndef IO_H_DEFINED
#include "io.h"
#endif

#ifndef OBSERVABLE_H_DEFINED
#include "observable.h"
#endif

#ifndef OBSERVER_H_DEFINED
#include "observer.h"
#endif

///////////////////////////////////////////////////////////////////////
// GrObj abstract class definition
///////////////////////////////////////////////////////////////////////

class GrObj : public virtual Observable, 
				  public virtual Observer,
				  public virtual IO 
{
public:
  //////////////
  // creators //
  //////////////

  GrObj ();
  // Default constructor.

  GrObj (istream &is, IOFlag flag);
  // Construct from an istream by using deserialize.

  virtual ~GrObj ();
  // Frees the OpenGL display list.

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  // These functions write the object's state from/to an output/input
  // stream. Both functions are defined, but are no-ops for GrObj.

  virtual int charCount() const = 0;

  ///////////////
  // accessors //
  ///////////////

  virtual int getCategory() const = 0;
  // getCategory and setCategory are used to manipulate some
  // user-defined categories that can be used to classify different
  // GrObj objects.

  //////////////////
  // manipulators //
  //////////////////

  virtual void setCategory(int val) = 0;
  
  virtual void receiveStateChangeMsg(const Observable* obj);
  virtual void receiveDestroyMsg(const Observable* obj);


  /////////////
  // actions //
  /////////////

  virtual void update() const;
  // Recompiles the OpenGL display list, if necessary, otherwise does
  // nothing.

  virtual void draw() const;
  // This function may be overridden, but a default version is
  // provided in GrObj that simply calls the display list that was
  // compiled in grRecompile. If overridden, this function should
  // render the object that is represented by the GrObj.

  virtual void undraw() const;
  // This function should arrange for the GrObj to undraw itself from
  // the screen. The default implementation is to do the same thing as
  // draw(), with the requirement that the caller swap the foreground
  // and background colors first.

protected:
  virtual void grRecompile() const = 0;
  // This function must be overridden in the derived classes, and
  // should arrange for an appropriate display list to be compiled
  // that can render the object.

  bool grIsCurrent() const { return itsIsCurrent; }
  void grPostUpdated() const;
  // These functions manipulate whether the GrObj is considered
  // current: the GrObj is current if its OpenGL display list is in
  // correspondence with all of its parameters. Thus, whenever a
  // manipulator changes a parameter in a derived class, it should
  // also call sendStateChangeMsg to indicate that the display list
  // must be recompiled. And when grRecompile has finished compiling
  // the display list, it should call grPostUpdated to indicate that
  // the GrObj is ready for display.

  void grNewList() const;
  // This function is to be used from grRecompile in derived classes to 
  // get rid of old display list, allocate a new display list, and
  // check that the allocation actually succeeded.

  int grDisplayList() const { return itsDisplayList; }
  // This function returns the id of the OpenGL display list that
  // should be compiled in grRecompile.

private:
  mutable bool itsIsCurrent;    // true if displaylist is current
  mutable int itsDisplayList;   // OpenGL display list that draws the object
};

static const char vcid_grobj_h[] = "$Header$";
#endif // !GROBJ_H_DEFINED
