///////////////////////////////////////////////////////////////////////
//
// grobj.h
// Rob Peters 
// created: Dec-98
// written: Wed Nov 10 12:41:51 1999
// $Id$
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
/**
 *
 * This is the abstract base class for graphic objects. GrObj*'s may
 * be stored and manipulated in ObjList's. Subclasses derived from
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
 * @memo Abstract base class for graphic objects.
 **/

class GrObj : public virtual Observable, 
				  public virtual Observer,
				  public virtual IO 
{
public:

  /**@name   Rendering modes
	*
	* These symbolic constants can be used to get/set the rendering or
	* unrendering modes with get/setRenderMode() or
	* get/setUnRednerMode(). The default rendering mode is
	* GROBJ_GL_COMPILE, and the default unrendering mode is
	* GROBJ_SWAP_FORE_BACK.
	*
	**/
  //@{
  ///
  typedef int GrObjRenderMode;

  /** In this mode, grRender() will be called every time the object is
		drawn, and grUnRender() will be called every time the object is
		undrawn.  */
  static const GrObjRenderMode GROBJ_DIRECT_RENDER = 1;

  /**@name   Caching rendering modes
	*
   * In these modes, grRender() is called on an draw request only if
   * the object's state has changed since the last draw request. If it
   * has not changed, then the object is instead rendered from either
   * 1) a cached OpenGL display list, 2) a cached OpenGL bitmap, or 3)
   * a cached X11 bitmap. The GROBJ_GL_COMPILE will function exactly
   * the same as GROBJ_DIRECT_RENDER, except that drawing should be
   * faster when the object's state is unchanged. The bitmap cache
   * modes may also yield a speed increase, but carry the following
   * caveats: 1) the object's size will not respond to OpenGL scaling
   * or rotation requests, but will be fixed at the screen size at
   * which the object was most recently direct-rendered; note that
   * OpenGL translation requests will still work properly, and 2) in
   * the X11 bitmap mode, the object will always be rendered into the
   * front buffer, regardless of whether OpenGL double-buffering is
   * being used.
	**/
  //@{
  ///
  static const GrObjRenderMode GROBJ_GL_COMPILE = 2;
  ///
  static const GrObjRenderMode GROBJ_GL_BITMAP_CACHE = 3;
  ///
  static const GrObjRenderMode GROBJ_X11_BITMAP_CACHE = 4;
  //@}

  /** This mode may be used only as an unrendering mode. If selected,
		unrendering will be done by performing a normal render except
		with the foreground and background colors swapped. */
  static const GrObjRenderMode GROBJ_SWAP_FORE_BACK = 5;

  /** This mode may be used only as an unrendering mode. If selected,
		unrendering will be done by clearing to the background color the
		region enclosed by the object's bounding box. */
  static const GrObjRenderMode GROBJ_CLEAR_BOUNDING_BOX = 6;

  //@}


  /**@name  Scaling modes   */
  //@{

  ///
  typedef int ScalingMode;

  /// This is the default mode
  static const int NATIVE_SCALING           = 1;
  ///
  static const int MAINTAIN_ASPECT_SCALING  = 2;
  ///
  static const int FREE_SCALING             = 3;

  //@}

  /**@name  Alignment modes   */
  //@{

  ///
  typedef int AlignmentMode;

  ///  
  static const int NATIVE_ALIGNMENT      = 1; // default
  ///  
  static const int CENTER_ON_CENTER      = 2;
  ///  
  static const int NW_ON_CENTER          = 3;
  ///  
  static const int NE_ON_CENTER          = 4;
  ///  
  static const int SW_ON_CENTER          = 5;
  ///  
  static const int SE_ON_CENTER          = 6;
  ///  
  static const int ARBITRARY_ON_CENTER   = 7;
  //@}

  //////////////
  // creators //
  //////////////

  /// Default constructor
  GrObj(GrObjRenderMode render_mode = GROBJ_GL_COMPILE,
		  GrObjRenderMode unrender_mode = GROBJ_SWAP_FORE_BACK);

  /// Construct from an istream by using deserialize.
  GrObj(istream &is, IOFlag flag);

  ///
  virtual ~GrObj();

  ///
  virtual void serialize(ostream &os, IOFlag flag) const;
  ///
  virtual void deserialize(istream &is, IOFlag flag);

  ///
  virtual int charCount() const;

  ///
  virtual void readFrom(Reader* reader);
  ///
  virtual void writeTo(Writer* writer) const;

  ///////////////
  // accessors //
  ///////////////

  ///
  bool getBBVisibility() const;

  /** This will return the bounding box given by grGetBoundingBox(),
		except that those values will be modified to reflect internal
		scaling, translation, and pixel border values. */
  bool getBoundingBox(double& left, double& top,
							 double& right, double& bottom) const;

  /** Subclasses may override this function to fill in the parameters
		with the bounding box in GL coordinates for the object's onscreen
		image. The function returns true if a bounding box has provided,
		or false if no bounding box is available. The default
		implementation provided by GrObj returns false. */
  protected: virtual bool grGetBoundingBox(double& left, double& top,
														 double& right, double& bottom,
														 int& border_pixels) const;

public:
  ///
  ScalingMode getScalingMode() const;
  ///
  double getWidth() const;
  ///
  double getHeight() const;
  ///
  double getAspectRatio() const;
  ///
  double getMaxDimension() const;

  ///
  AlignmentMode getAlignmentMode() const;
  ///
  double getCenterX() const;
  ///
  double getCenterY() const;

  /** getCategory and setCategory are used to manipulate some
		user-defined categories that can be used to classify different
		GrObj objects. */
  virtual int getCategory() const;

  ///
  GrObjRenderMode getRenderMode() const;
  ///
  GrObjRenderMode getUnRenderMode() const;

  /**@name Screen-World coordinate conversion
	*
   * These two functions convert between world coordinates (OpenGL
   * object coordinates) and screen/window coordinates. Normally the
   * OpenGL state (modelview matrix, projection matrix, and viewport)
   * is re-queried each time, but if it is known that the OpenGL state
   * has not changed between calls to these functions, then some speed
   * can be gained by passing recalculate_state as false.
	*/
  //@{
  ///
  static void getScreenFromWorld(double world_x, double world_y,
											int& screen_x, int& screen_y,
											bool recalculate_state = true);
  ///
  static void getWorldFromScreen(int screen_x, int screen_y,
											double& world_x, double& world_y,
											bool recalculate_state = true);
  //@}

  //////////////////
  // manipulators //
  //////////////////

  ///
  void setBBVisibility(bool visibility);

  ///
  void setScalingMode(ScalingMode val);
  ///
  void setWidth(double val);
  ///
  void setHeight(double val);
  ///
  void setAspectRatio(double val);
  ///
  void setMaxDimension(double val);

  ///
  void setAlignmentMode(AlignmentMode val);
  ///
  void setCenterX(double val);
  ///
  void setCenterY(double val);

  ///
  virtual void setCategory(int val);
  
  ///
  void setRenderMode(GrObjRenderMode mode);
  ///
  void setUnRenderMode(GrObjRenderMode mode);

  ///
  virtual void receiveStateChangeMsg(const Observable* obj);
  ///
  virtual void receiveDestroyMsg(const Observable* obj);


  /////////////
  // actions //
  /////////////

  /** Swaps the foreground and background colors, in a way that is
      appropriate to color-index/RGBA modes. In order to be able to
      unswap the colors, this function should normally be preceded by
      glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT), so that a
      call to glPopAttrib() will then unswap the colors. */
  static void swapForeBack();

  /** Recompiles the OpenGL display list or bitmap cache, if there are
		changes pending, otherwise does nothing. */
  void update() const;

  /** This function draws the object according to the GrRenderMode
		selected with setRenderMode(). */
  void draw() const;

  /** This function undraws the object according to the GrRenderMode
		selected with setUnRenderMode(). */
  void undraw() const;

protected:
  ///
  void grDrawBoundingBox() const;

  /** This function must be overridden in derived classes to execute
		the actual OpenGL commands that render the object. */
  virtual void grRender() const = 0;

  /** This function will be called if the unrendering mode is set to
		GROBJ_DIRECT_RENDER or to any of the compile or cache modes. The
		default implementation provided by GrObj does nothing. */
  virtual void grUnRender() const;

  /** This function queries whether the GrObj's cached representation
		is in correspondence with all of its parameters. */
  bool grIsCurrent() const;

  /** This function returns the id of the OpenGL display list that
		should be compiled in grRecompile. */
  int grDisplayList() const;

private:
  friend class GrObjImpl;
  GrObjImpl* const itsImpl;	  // opaque pointer to implementation
};

static const char vcid_grobj_h[] = "$Header$";
#endif // !GROBJ_H_DEFINED
