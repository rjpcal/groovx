///////////////////////////////////////////////////////////////////////
//
// grobj.h
// Rob Peters 
// created: Dec-98
// written: Wed Mar 29 14:07:37 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJ_H_DEFINED
#define GROBJ_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBSERVABLE_H_DEFINED)
#include "observable.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBSERVER_H_DEFINED)
#include "observer.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TRACER_H_DEFINED)
#include "util/tracer.h"
#endif

template <class V> class Point;
template <class V> class Rect;

class Canvas;

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c GrObj is the abstract base class for graphic objects. GrObj*'s
 * may be stored and manipulated in ObjList's. Subclasses derived from
 * GrObj must specify the details of how their objects will be drawn,
 * by overriding the virtual functions grRender() and grUnRender(),
 * and/or by choosing appropriate render/unrender modes with
 * setRenderMode() and setUnRenderMode(). Public clients call draw()
 * or undraw() to display or hide the object. The caches that mediate
 * the various drawing modes are kept up to date by using the
 * Observable interface. Thus, whenever a manipulator changes a
 * parameter in a derived class, it should also call
 * Observable::sendStateChangeMsg() to indicate that an update is
 * needed.
 *
 **/
///////////////////////////////////////////////////////////////////////

class GrObj : public virtual Observable, 
				  public virtual Observer,
				  public virtual IO 
{
public:

  /// This tracer dynamically controls the tracing of GrObj member functions.
  static Util::Tracer tracer;


  ///////////////////////////////////////////////////////////////////////
  //
  // Rendering and unrendering modes
  //
  ///////////////////////////////////////////////////////////////////////

  /** The symbolic constants of type \c GrObjRenderMode control the
	   way the object is drawn to and undrawn from the screen. The
	   current modes can be get/set with \c getRenderMode(), \c
	   setRenderMode(), \c getUnRenderMode(), and \c
	   setUnRenderMode(). The default rendering mode is \c
	   GROBJ_GL_COMPILE, and the default unrendering mode is \c
	   GROBJ_SWAP_FORE_BACK. **/
  typedef int GrObjRenderMode;

  /** In this mode, \c grRender() will be called every time the object
		is drawn, and \c grUnRender() will be called every time the
		object is undrawn.  */
  static const GrObjRenderMode GROBJ_DIRECT_RENDER = 1;

  /** In this mode, \c grRender() is called on a draw request only if
      the object's state has changed since the last draw request. If
      it has not changed, then the object is instead rendered from
      either a cached OpenGL display list. The behavior of \c
      GROBJ_GL_COMPILE should be exactly the same as that of \c
      GROBJ_DIRECT_RENDER, except that drawing should be faster if the
      object's state has not changed since the last draw. **/
  static const GrObjRenderMode GROBJ_GL_COMPILE = 2;

  /** In this mode, \c grRender() is called on an draw request only if
      the object's state has changed since the last draw request. If
      it has not changed, then the object is instead rendered from a
      cached bitmap using the OpenGL bitmap-rendering API (which may
      be \b slow if the host is not an SGI). This mode may yield a
      speed increase over \c GROBJ_GL_COMPILE, but carries the
      following caveat: the object's size will not respond to OpenGL
      scaling or rotation requests, but will be fixed at the screen
      size at which the object was most recently direct-rendered; note
      however that OpenGL translation requests will still work
      properly. This mode requires the object to have a bounding box
      (i.e. \c getBoundingBox() must return \c true). **/
  static const GrObjRenderMode GROBJ_GL_BITMAP_CACHE = 3;

  /** In this mode, \c grRender() is called on an draw request only if
      the object's state has changed since the last draw request. If
      it has not changed, then the object is instead rendered from a
      cached bitmap using the X11 bitmap-rendering API. This mode may
      yield a speed increase over \c GROBJ_GL_COMPILE, but carry the
      following caveats: 1) the object's size will not respond to
      OpenGL scaling or rotation requests, but will be fixed at the
      screen size at which the object was most recently
      direct-rendered; note however that OpenGL translation requests
      will still work properly, and 2) the object will always be
      rendered into the front buffer, regardless of whether OpenGL
      double-buffering is being used. This mode requires the object to
      have a bounding box (i.e. \c getBoundingBox() must return \c
      true). **/
  static const GrObjRenderMode GROBJ_X11_BITMAP_CACHE = 4;

  /** This mode may be used only as an unrendering mode. If selected,
		unrendering will be done by performing a normal render except
		with the foreground and background colors swapped. */
  static const GrObjRenderMode GROBJ_SWAP_FORE_BACK = 5;

  /** This mode may be used only as an unrendering mode. If selected,
      unrendering will be done by clearing to the background color the
      region enclosed by the object's bounding box. This mode requires
      the object to have a bounding box (i.e. \c getBoundingBox() must
      return \c true). */
  static const GrObjRenderMode GROBJ_CLEAR_BOUNDING_BOX = 6;


  ///////////////////////////////////////////////////////////////////////
  //
  // Scaling modes
  //
  ///////////////////////////////////////////////////////////////////////

  /** The symbolic constants of type \c ScalingMode provide several
      ways to scale an object with respect to the OpenGL coordinate
      system. The current mode can be get/set with \c getScalingMode()
      and \c setScalineMode(). The default scaling mode is \c
      NATIVE_SCALING. All other scaling modes require the object to
      have a bounding box (i.e. \c getBoundingBox() must return \c
      true). */
  typedef int ScalingMode;

  /** This is the default scaling mode. No additional scaling is done beyond
      whatever happens in the subclass's \c grRender()
      implementation. */
  static const ScalingMode NATIVE_SCALING           = 1;

  /** In this mode, the native aspect ratio of the object will be
      maintained. Thus, requests to change one of the object's
      dimensions (width or height) will affect the other dimension so
      as to maintain the native aspect ratio. In this mode, \c
      getAspectRatio() should always return 1.0. */
  static const ScalingMode MAINTAIN_ASPECT_SCALING  = 2;

  /** In this mode, the width and height of the object may be set
      independently of each other. */
  static const ScalingMode FREE_SCALING             = 3;


  ///////////////////////////////////////////////////////////////////////
  //
  // Alignment modes
  //
  ///////////////////////////////////////////////////////////////////////

  /** The symbolic constants of type \c AlignmentMode provide several
      ways to position an object with respect to the OpenGL coordinate
      system. The current mode can be get/set with \c
      getAlignmentMode() and \c setAlignmentMode(). The default
      alignment mode is \c NATIVE_ALIGNMENT. All other alignment modes
      require the object to have a bounding box (i.e. \c
      getBoundingBox() must return \c true). */
  typedef int AlignmentMode;

  /** This is the default alignment mode. No additional translations
      are performed beyond whatever happens in the subclass's \c
      grRender() implementation. */
  static const AlignmentMode NATIVE_ALIGNMENT      = 1;

  /// The center of the object is aligned with OpenGL's origin.
  static const AlignmentMode CENTER_ON_CENTER      = 2;

  /// The NorthWest corner of the object is aligned with OpenGL's origin.
  static const AlignmentMode NW_ON_CENTER          = 3;

  /// The NorthEast corner of the object is aligned with OpenGL's origin.
  static const AlignmentMode NE_ON_CENTER          = 4;

  /// The SouthWest corner of the object is aligned with OpenGL's origin.
  static const AlignmentMode SW_ON_CENTER          = 5;

  /// The SouthEast corner of the object is aligned with OpenGL's origin.
  static const AlignmentMode SE_ON_CENTER          = 6;

  /// The location of the center of the object may be set arbitrarily.
  static const AlignmentMode ARBITRARY_ON_CENTER   = 7;

  //////////////
  // creators //
  //////////////

  /// Default constructor
  GrObj(GrObjRenderMode render_mode = GROBJ_GL_COMPILE,
		  GrObjRenderMode unrender_mode = GROBJ_SWAP_FORE_BACK);

  /// Construct from an istream by using deserialize.
  GrObj(istream &is, IOFlag flag);

  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~GrObj();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual unsigned long serialVersionId() const;
  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  ///////////////////////////////////////////////////////////////////////
  //
  // Accessors
  //
  ///////////////////////////////////////////////////////////////////////

  /** Queries whether the bounding box will be visible when the object
      is drawn. */
  bool getBBVisibility() const;

  /** Returns the bounding box given by \c grGetBoundingBox(), with
		additional modifications to reflect the scaling mode, alignment
		mode, and pixel border values. If a bounding box is not
		available, the function returns false and does not modify the
		input reference parameters. */
  bool getBoundingBox(const Canvas& canvas, Rect<double>& bounding_box) const;

protected: 
  /** Subclasses may override this function to fill in the parameters
		with the bounding box in GL coordinates for the object's
		onscreen image. The default version provided by GrObj does not
		modify the input reference parameters. */
  virtual void grGetBoundingBox(Rect<double>& bounding_box,
														 int& border_pixels) const;

  /** This function should be overridden to return true if a bounding
		box has provided, or false if no bounding box is available. The
		default implementation provided by GrObj returns false. */
  virtual bool grHasBoundingBox() const;

public:
  /// Returns the current scaling mode.
  ScalingMode getScalingMode() const;

  /// Returns the object's onscreen width in OpenGL units.
  double getWidth() const;

  /// Returns the object's onscreen height in OpenGL units.
  double getHeight() const;

  /** Returns the object's aspect ratio in OpenGL units. This measures
      how much the object is deformed relative to its native size. For
      both width and height, a scaling factor is computed as the ratio
      of the native dimension to the actual onscreen dimension. The
      aspect ratio is then the ratio of the width scaling factor to
      the height scaling factor. */
  double getAspectRatio() const;

  /// Returns the value of the larger of the object's width and height.
  double getMaxDimension() const;

  /// Returns the current alignment mode.
  AlignmentMode getAlignmentMode() const;

  /// Returns the x value of the location of the object's center.
  double getCenterX() const;

  /// Returns the x value of the location of the object's center.
  double getCenterY() const;

  /// Returns the value of the user-defined category.
  virtual int getCategory() const;

  /// Returns the current rendering mode.
  GrObjRenderMode getRenderMode() const;

  /// Returns the current unrendering mode.
  GrObjRenderMode getUnRenderMode() const;

  ///////////////////////////////////////////////////////////////////////
  //
  // Manipulators 
  //
  ///////////////////////////////////////////////////////////////////////

  /** Changes the bitmap cache directory to \a dirname. This is where
      bitmaps saved with \c saveBitmapCache() will be stored, and also
      where objects will look to restore themselves from previously
      saved bitmap caches. */
  static void setBitmapCacheDir(const char* dirname);

  /// Changes whether the bounding box will be visible when the object is drawn.
  void setBBVisibility(bool visibility);

  /** Changes the current scaling mode to \a mode, unless the
      requirements of \a mode (for example, requiring a bounding box)
      are not met, in which case the scaling mode is unchanged. */
  void setScalingMode(ScalingMode mode);

  /** Changes the current width to \a val, unless the scaling mode is
      \c NATIVE_SCALING. If the scaling mode is \c
      MAINTAIN_ASPECT_SCALING, the height will be adjusted accordingly
      as well. */
  void setWidth(double val);

  /** Changes the current height to \a val, unless the scaling mode is
      \c NATIVE_SCALING. If the scaling mode is \c
      MAINTAIN_ASPECT_SCALING, the width will be adjusted accordingly
      as well. */
  void setHeight(double val);

  /** Changes the aspect ratio to \a val, unless the scaling mode is
      \c NATIVE_SCALING or \c MAINTAIN_ASPECT_SCALING. */
  void setAspectRatio(double val);

  /** Sets the larger of width or height to \a val, unless the scaling
      mode is \c NATIVE_SCALING. If the scaling mode is \c
      MAINTAIN_ASPECT_SCALING, the other dimension will be adjusted
      accordingly as well. */
  void setMaxDimension(double val);

  /** Changes the current alignment mode to \a mode, unless the
      requirements of \a mode (for example, requiring a bounding box)
      are not met, in which case the scaling mode is unchanged. */
  void setAlignmentMode(AlignmentMode mode);

  /** Change the x value for the location of the object's center to \a
      val. This value is used only if the alignment mode is \c
      ARBITRARY_ON_CENTER. */
  void setCenterX(double val);

  /** Change the y value for the location of the object's center to \a
      val. This value is used only if the alignment mode is \c
      ARBITRARY_ON_CENTER. */
  void setCenterY(double val);

  /// Change the value of the user-defined category to \a val.
  virtual void setCategory(int val);
  
  /** Changes the current rendering mode to \a mode, unless the
      requirements of \a mode (for example, requiring a bounding box)
      are not met, in which case the scaling mode is unchanged. */
  void setRenderMode(GrObjRenderMode mode);

  /** Changes the current unrendering mode to \a mode, unless the
      requirements of \a mode (for example, requiring a bounding box)
      are not met, in which case the scaling mode is unchanged. */
  void setUnRenderMode(GrObjRenderMode mode);

  virtual void receiveStateChangeMsg(const Observable* obj);
  virtual void receiveDestroyMsg(const Observable* obj);


  ///////////////////////////////////////////////////////////////////////
  //
  // Actions
  //
  ///////////////////////////////////////////////////////////////////////

  /** Saves a bitmap cache of the object to the named file. The file
      will be stored in the directory given to \c
      setBitmapCacheDir(). The filename must refer to a format
      supported by \c Pbm. */
  void saveBitmapCache(Canvas& canvas, const char* filename) const;

  /** Restores the bitmap cache from the bitmap cache filename that
      was most recently passed to \c saveBitmapCache(). */
  void restoreBitmapCache() const;

  /** Recompiles the OpenGL display list or bitmap cache, if there are
		changes pending, otherwise does nothing. */
  void update(Canvas& canvas) const;

  /** This function draws the object according to the GrRenderMode
		selected with setRenderMode(). */
  void draw(Canvas& canvas) const;

  /** This function undraws the object according to the GrRenderMode
		selected with setUnRenderMode(). */
  void undraw(Canvas& canvas) const;

protected:
  /** This function must be overridden in derived classes to execute
		the actual OpenGL commands that render the object. */
  virtual void grRender(Canvas& canvas) const = 0;

  /** This function will be called if the unrendering mode is set to
		GROBJ_DIRECT_RENDER or to any of the compile or cache modes. The
		default implementation provided by GrObj does nothing. */
  virtual void grUnRender(Canvas& canvas) const;

private:
  GrObj(const GrObj&);
  GrObj& operator=(const GrObj&);

  class Impl;
  friend class Impl;
  Impl* const itsImpl;	  // opaque pointer to implementation
};

static const char vcid_grobj_h[] = "$Header$";
#endif // !GROBJ_H_DEFINED
