///////////////////////////////////////////////////////////////////////
//
// grobj.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  1 08:00:00 1998
// written: Wed Nov 13 12:47:26 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJ_H_DEFINED
#define GROBJ_H_DEFINED

#include "gfx/gxnode.h"

#include "io/fields.h"

#include "util/tracer.h"

class GrObjImpl;

class fstring;

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c GrObj is the abstract base class for graphic objects. GrObj*'s may be
 * stored and manipulated in ObjList's. Subclasses derived from GrObj must
 * specify the details of how their objects will be drawn, by overriding
 * the virtual function grRender(), and/or by choosing an appropriate
 * render mode with setRenderMode(). Public clients call draw() to display
 * the object. The caches that mediate the various drawing modes are kept
 * up to date by using the Signal interface. Thus, whenever a manipulator
 * changes a parameter in a derived class, it should also call
 * GrObj::sigNodeChanged.emit() to indicate that an update is needed.
 *
 **/
///////////////////////////////////////////////////////////////////////

class GrObj : public GxNode,
              public FieldContainer
{
public:

  /// This tracer dynamically controls the tracing of GrObj member functions.
  static Util::Tracer tracer;


  //////////////
  // creators //
  //////////////

  /// Default constructor
  GrObj();
  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~GrObj();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  static const FieldMap& classFields();

  ///////////////////////////////////////////////////////////////////////
  //
  // Accessors
  //
  ///////////////////////////////////////////////////////////////////////

  /** Queries whether the bounding box will be visible when the object
      is drawn. */
  bool getBBVisibility() const;

  /** Overridden from GxNode. Extends the cube by the rect returned by
      grGetBoundingBox(), with additional transformations to reflect the
      scaling mode, alignment mode, and pixel border values. Subclasses of
      GrObj should override grGetBoundingBox(), not getBoundingCube(). */
  virtual void getBoundingCube(Gfx::Box<double>& cube,
                               Gfx::Canvas& canvas) const;

  /** Subclasses must override this function to return the bounding
      box in GL coordinates for the object's onscreen image. */
  virtual Gfx::Rect<double> grGetBoundingBox(Gfx::Canvas& canvas) const = 0;

  /// Returns the current scaling mode.
  int getScalingMode() const;

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
  int getAlignmentMode() const;

  /// Returns the x value of the location of the object's center.
  double getCenterX() const;

  /// Returns the x value of the location of the object's center.
  double getCenterY() const;

  /** Returns the thickness (in pixels) of the border around the
      bounding box area. */
  int getPixelBorder() const;

  /// Returns the value of the user-defined category.
  virtual int category() const;

  /// Returns the current rendering mode.
  int getRenderMode() const;


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
  void setScalingMode(int mode);

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
  void setAlignmentMode(int mode);

  /** Change the x value for the location of the object's center to \a
      val. This value is used only if the alignment mode is \c
      ARBITRARY_ON_CENTER. */
  void setCenterX(double val);

  /** Change the y value for the location of the object's center to \a
      val. This value is used only if the alignment mode is \c
      ARBITRARY_ON_CENTER. */
  void setCenterY(double val);

  /** Change the thickness (in pixels) of the border that will be
      added around the bounding box area. */
  void setPixelBorder(int pixels);

  /// Change the value of the user-defined category to \a val.
  virtual void setCategory(int val);

  /** Changes the current rendering mode to \a mode, unless the
      requirements of \a mode (for example, requiring a bounding box)
      are not met, in which case the scaling mode is unchanged. */
  void setRenderMode(int mode);


  ///////////////////////////////////////////////////////////////////////
  //
  // Actions
  //
  ///////////////////////////////////////////////////////////////////////

  /** Saves a bitmap cache of the object to the named file. The file
      will be stored in the directory given to \c
      setBitmapCacheDir(). The filename must refer to a format
      supported by \c Pbm. */
  void saveBitmapCache(Gfx::Canvas& canvas, const char* filename) const;

  /** Recompiles the OpenGL display list or bitmap cache, if there are
      changes pending, otherwise does nothing. */
  void update(Gfx::Canvas& canvas) const;

  /** This function draws the object according to the GrRenderMode
      selected with setRenderMode(). */
  void draw(Gfx::Canvas& canvas) const;

  /** This function must be overridden in derived classes to execute
      the actual drawing commands that render the object. */
  virtual void grRender(Gfx::Canvas& canvas) const = 0;

private:
  const fstring& getCacheFilename() const;
  void setCacheFilename(const fstring& name);

  double getWidthFactor() const;
  void setWidthFactor(double val);

  double getHeightFactor() const;
  void setHeightFactor(double val);

  GrObj(const GrObj&);
  GrObj& operator=(const GrObj&);

  friend class GrObjImpl;
  GrObjImpl* const itsImpl;    // opaque pointer to implementation
};

static const char vcid_grobj_h[] = "$Header$";
#endif // !GROBJ_H_DEFINED
