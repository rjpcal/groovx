///////////////////////////////////////////////////////////////////////
//
// gmodes.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Aug 10 14:48:32 2001
// written: Fri Jan 18 16:06:53 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GMODES_H_DEFINED
#define GMODES_H_DEFINED

namespace Gmodes
{
  ///////////////////////////////////////////////////////////////////////
  //
  // Rendering and unrendering modes
  //
  ///////////////////////////////////////////////////////////////////////

  /** The symbolic constants of type \c GrObj::RenderMode control the
      way the object is drawn to and undrawn from the screen. The
      current modes can be get/set with \c getRenderMode(), and \c
      setRenderMode(). The default rendering mode is \c GLCOMPILE. **/
  typedef int RenderMode;

  /** In this mode, \c grRender() will be called every time the object
      is drawn or undrawn.  */
  const RenderMode DIRECT_RENDER = 1;

  /** In this mode, \c grRender() is called on a draw request only if
      the object's state has changed since the last draw request. If
      it has not changed, then the object is instead rendered from
      either a cached OpenGL display list. The behavior of \c
      GLCOMPILE should be exactly the same as that of \c
      DIRECT_RENDER, except that drawing should be faster if the
      object's state has not changed since the last draw. **/
  const RenderMode GLCOMPILE = 2;

  /** In this mode, \c grRender() is called on an draw request only if
      the object's state has changed since the last draw request. If
      it has not changed, then the object is instead rendered from a
      cached bitmap using the OpenGL bitmap-rendering API (which may
      be \b slow if the host is not an SGI). This mode may yield a
      speed increase over \c GLCOMPILE, but carries the
      following caveat: the object's size will not respond to OpenGL
      scaling or rotation requests, but will be fixed at the screen
      size at which the object was most recently direct-rendered; note
      however that OpenGL translation requests will still work
      properly. */
  const RenderMode GL_BITMAP_CACHE = 3;

  /** In this mode, \c grRender() is called on an draw request only if
      the object's state has changed since the last draw request. If
      it has not changed, then the object is instead rendered from a
      cached bitmap using the X11 bitmap-rendering API. This mode may
      yield a speed increase over \c GLCOMPILE, but carry the
      following caveats: 1) the object's size will not respond to
      OpenGL scaling or rotation requests, but will be fixed at the
      screen size at which the object was most recently
      direct-rendered; note however that OpenGL translation requests
      will still work properly, and 2) the object will always be
      rendered into the front buffer, regardless of whether OpenGL
      double-buffering is being used. */
  const RenderMode X11_BITMAP_CACHE = 4;



  ///////////////////////////////////////////////////////////////////////
  //
  // Scaling modes
  //
  ///////////////////////////////////////////////////////////////////////

  /** The symbolic constants of type \c ScalingMode provide several
      ways to scale an object with respect to the OpenGL coordinate
      system. The current mode can be get/set with \c getScalingMode()
      and \c setScalineMode(). The default scaling mode is \c
      NATIVE_SCALING. */
  typedef int ScalingMode;

  /** This is the default scaling mode. No additional scaling is done beyond
      whatever happens in the subclass's \c grRender()
      implementation. */
  const ScalingMode NATIVE_SCALING           = 1;

  /** In this mode, the native aspect ratio of the object will be
      maintained. Thus, requests to change one of the object's
      dimensions (width or height) will affect the other dimension so
      as to maintain the native aspect ratio. In this mode, \c
      getAspectRatio() should always return 1.0. */
  const ScalingMode MAINTAIN_ASPECT_SCALING  = 2;

  /** In this mode, the width and height of the object may be set
      independently of each other. */
  const ScalingMode FREE_SCALING             = 3;



  ///////////////////////////////////////////////////////////////////////
  //
  // Alignment modes
  //
  ///////////////////////////////////////////////////////////////////////

  /** The symbolic constants of type \c AlignmentMode provide several
      ways to position an object with respect to the OpenGL coordinate
      system. The current mode can be get/set with \c
      getAlignmentMode() and \c setAlignmentMode(). The default
      alignment mode is \c NATIVE_ALIGNMENT. */
  typedef int AlignmentMode;

  /** This is the default alignment mode. No additional translations
      are performed beyond whatever happens in the subclass's \c
      grRender() implementation. */
  const AlignmentMode NATIVE_ALIGNMENT      = 1;

  /// The center of the object is aligned with OpenGL's origin.
  const AlignmentMode CENTER_ON_CENTER      = 2;

  /// The NorthWest corner of the object is aligned with OpenGL's origin.
  const AlignmentMode NW_ON_CENTER          = 3;

  /// The NorthEast corner of the object is aligned with OpenGL's origin.
  const AlignmentMode NE_ON_CENTER          = 4;

  /// The SouthWest corner of the object is aligned with OpenGL's origin.
  const AlignmentMode SW_ON_CENTER          = 5;

  /// The SouthEast corner of the object is aligned with OpenGL's origin.
  const AlignmentMode SE_ON_CENTER          = 6;

  /// The location of the center of the object may be set arbitrarily.
  const AlignmentMode ARBITRARY_ON_CENTER   = 7;
}

static const char vcid_gmodes_h[] = "$Header$";
#endif // !GMODES_H_DEFINED
