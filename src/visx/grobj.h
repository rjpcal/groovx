///////////////////////////////////////////////////////////////////////
//
// grobj.h
// Rob Peters 
// created: Dec-98
// written: Fri Sep 17 09:29:51 1999
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

class GrObjImpl;

///////////////////////////////////////////////////////////////////////
// GrObj abstract class definition
///////////////////////////////////////////////////////////////////////

class GrObj : public virtual Observable, 
				  public virtual Observer,
				  public virtual IO 
{
public:

  /////////////////////////
  // flags and constants //
  /////////////////////////
  typedef int GrObjRenderMode;

  static const GrObjRenderMode GROBJ_DIRECT_RENDER = 1;
  // In this mode, grRender() will be called every time the object is
  // drawn.

  static const GrObjRenderMode GROBJ_GL_COMPILE = 2;
  static const GrObjRenderMode GROBJ_GL_BITMAP_CACHE = 3;
  static const GrObjRenderMode GROBJ_X11_BITMAP_CACHE = 4;
  // In these mode, grRender() is called on an draw request only if
  // the object's state has changed since the last draw request. If it
  // has not changed, then the object is instead rendered from either
  // 1) a cached OpenGL display list, 2) a cached OpenGL bitmap, or 3)
  // a cached X11 bitmap. The GROBJ_GL_COMPILE will function exactly
  // the same as GROBJ_DIRECT_RENDER, except that drawing should be
  // faster when the object's state is unchanged. The bitmap cache
  // modes may also yield a speed increase, but carry the following
  // caveats: 1) the object's size will not respond to OpenGL scaling
  // or rotation requests, but will be fixed at the screen size at
  // which the object was most recently direct-rendered; note that
  // OpenGL translation requests will still work properly, and 2) in
  // the X11 bitmap mode, the object will always be rendered into the
  // front buffer, regardless of whether OpenGL double-buffering is
  // being used.



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

  GrObjRenderMode getRenderMode() const;

  bool getUsingCompile() const;

  virtual int getCategory() const = 0;
  // getCategory and setCategory are used to manipulate some
  // user-defined categories that can be used to classify different
  // GrObj objects.

  virtual bool getBoundingBox(double& left, double& top,
										double& right, double& bottom);
  // Subclasses may override this function to fill in the parameters
  // with the bounding box in GL coordinates for the object's onscreen
  // image. The function returns true if a bounding box has provided,
  // or false if no bounding box is available. The default
  // implementation provided by GrObj returns false.


  //////////////////
  // manipulators //
  //////////////////

  void setRenderMode(GrObjRenderMode mode);

  void setUsingCompile(bool val);

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
  virtual void grRender() const = 0;
  // This function must be overridden in derived classes to execute
  // the actual OpenGL commands that render the object.

  virtual void grRecompile() const;
  // This function should arrange for an up-to-date GL display list to
  // be compiled that will draw the object. This function may be
  // overridden in the derived classes, but it defaults to simply
  // creating a new display list, then compiling it with the effects
  // of grRender().

  bool grIsCurrent() const;
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
  mutable bool itsUsingCompile; // true if GrObj is to be compiled before draw
  mutable int itsDisplayList;   // OpenGL display list that draws the object

  GrObjImpl* const itsImpl;	  // opaque pointer to implementation
};

static const char vcid_grobj_h[] = "$Header$";
#endif // !GROBJ_H_DEFINED
