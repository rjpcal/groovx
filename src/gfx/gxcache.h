///////////////////////////////////////////////////////////////////////
//
// gxcache.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 13 14:03:23 2002
// written: Wed Nov 13 14:06:38 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXCACHE_H_DEFINED
#define GXCACHE_H_DEFINED

#include "gfx/gxbin.h"

class GxCache : public GxBin
{
public:

  ///////////////////////////////////////////////////////////////////////
  //
  // Rendering modes
  //
  ///////////////////////////////////////////////////////////////////////

  /** These symbolic constants control the way the object is drawn to and
      undrawn from the screen. The current modes can be get/set with \c
      getMode(), and \c setMode(). The default rendering mode
      is \c GLCOMPILE. **/
  typedef int Mode;

  /// In this mode, the object will be rendered freshly each time it is drawn.
  static const Mode DIRECT = 1;

  /** In this mode, the object is rendered freshly on a draw request only
      if the object's state has changed since the last draw request. If it
      has not changed, then the object is instead rendered from either a
      cached OpenGL display list. The behavior of \c GLCOMPILE should be
      exactly the same as that of \c DIRECT, except that drawing should be
      faster if the object's state has not changed since the last draw. */
  static const Mode GLCOMPILE = 2;


  GxCache(Util::SoftRef<GxNode> child);

  virtual ~GxCache();

  virtual void readFrom(IO::Reader* /*reader*/) {};
  virtual void writeTo(IO::Writer* /*writer*/) const {};

  virtual void draw(Gfx::Canvas& canvas) const;

  virtual void getBoundingCube(Gfx::Box<double>& cube,
                               Gfx::Canvas& canvas) const;

  void invalidate();

  Mode getMode() const { return itsMode; }
  void setMode(Mode new_mode);

private:
  Mode itsMode;

  mutable int itsDisplayList;
};

static const char vcid_gxcache_h[] = "$Header$";
#endif // !GXCACHE_H_DEFINED
