///////////////////////////////////////////////////////////////////////
//
// glcachenode.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Aug 10 16:39:48 2001
// written: Wed Nov 13 12:35:45 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLCACHENODE_H_DEFINED
#define GLCACHENODE_H_DEFINED

#include "gfx/gxbin.h"

#include "visx/gmodes.h"

class GLCacheNode : public GxBin
{
public:
  GLCacheNode(Util::SoftRef<GxNode> child) :
    GxBin(child),
    itsMode(Gmodes::DIRECT_RENDER),
    itsDisplayList(0)
  {}

  virtual ~GLCacheNode();

  virtual void readFrom(IO::Reader* /*reader*/) {};
  virtual void writeTo(IO::Writer* /*writer*/) const {};

  virtual void draw(Gfx::Canvas& canvas) const;

  virtual void getBoundingCube(Gfx::Box<double>& cube,
                               Gfx::Canvas& canvas) const;

  void invalidate();

  Gmodes::RenderMode getMode() const { return itsMode; }
  void setMode(Gmodes::RenderMode new_mode);

private:
  Gmodes::RenderMode itsMode;

  mutable int itsDisplayList;
};

static const char vcid_glcachenode_h[] = "$Header$";
#endif // !GLCACHENODE_H_DEFINED
