///////////////////////////////////////////////////////////////////////
//
// gmodes.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Aug 10 14:48:32 2001
// written: Wed Nov 13 13:05:14 2002
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
}

static const char vcid_gmodes_h[] = "$Header$";
#endif // !GMODES_H_DEFINED
