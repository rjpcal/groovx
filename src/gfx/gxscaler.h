///////////////////////////////////////////////////////////////////////
//
// gxscaler.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 13 13:04:27 2002
// written: Thu Nov 14 17:11:26 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXSCALER_H_DEFINED
#define GXSCALER_H_DEFINED

#include "gfx/gxbin.h"

#include "gx/rect.h"

#include "util/algo.h"

class GxScaler : public GxBin
{
public:
  ///////////////////////////////////////////////////////////////////////
  //
  // Scaling modes
  //
  ///////////////////////////////////////////////////////////////////////

  /** The symbolic constants of type \c ScalingMode provide several ways to
      scale an object with respect to the OpenGL coordinate system. The
      current mode can be get/set with \c getMode() and \c setMode(). The
      default scaling mode is \c NATIVE_SCALING. */
  typedef int Mode;

  /** This is the default scaling mode. No additional scaling is done beyond
      whatever happens in the subclass's \c grRender()
      implementation. */
  static const Mode NATIVE_SCALING           = 1;

  /** In this mode, the native aspect ratio of the object will be
      maintained. Thus, requests to change one of the object's
      dimensions (width or height) will affect the other dimension so
      as to maintain the native aspect ratio. In this mode, \c
      getAspectRatio() should always return 1.0. */
  static const Mode MAINTAIN_ASPECT_SCALING  = 2;

  /** In this mode, the width and height of the object may be set
      independently of each other. */
  static const Mode FREE_SCALING             = 3;


  GxScaler();
  GxScaler(Util::Ref<GxNode> child);

  static GxScaler* make();

  Mode getMode() const { return itsMode; }
  void setMode(Mode new_mode);

  void setWidth(Gfx::Canvas& canvas, double new_width);
  void setHeight(Gfx::Canvas& canvas, double new_height);
  void setMaxDim(Gfx::Canvas& canvas, double new_max_dimension);

  void setAspectRatio(double new_aspect_ratio);

  void setWidthFactor(double f) { itsWidthFactor = f; }
  void setHeightFactor(double f) { itsHeightFactor = f; }

  double scaledWidth(Gfx::Canvas& canvas);
  double scaledHeight(Gfx::Canvas& canvas);
  double scaledMaxDim(Gfx::Canvas& canvas);

  double aspectRatio() const;

  double widthFactor() const { return itsWidthFactor; }
  double heightFactor() const { return itsHeightFactor; }

  virtual void readFrom(IO::Reader* /*reader*/) {};
  virtual void writeTo(IO::Writer* /*writer*/) const {};

  virtual void draw(Gfx::Canvas& canvas) const;

  virtual void getBoundingCube(Gfx::Box<double>& cube,
                               Gfx::Canvas& canvas) const;

private:
  Mode itsMode;

  double itsWidthFactor;
  double itsHeightFactor;
};


static const char vcid_gxscaler_h[] = "$Header$";
#endif // !GXSCALER_H_DEFINED
