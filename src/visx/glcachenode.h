///////////////////////////////////////////////////////////////////////
//
// glcachenode.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Aug 10 16:39:48 2001
// written: Fri Aug 10 17:20:38 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLCACHENODE_H_DEFINED
#define GLCACHENODE_H_DEFINED

#include "gmodes.h"
#include "gnode.h"

// this will handle GLCOMPILE, SWAP_FORE_BACK and CLEAR_BOUNDING_BOX

class GLCacheNode : public Gnode {
public:
  GLCacheNode(shared_ptr<Gnode> child) :
    Gnode(child),
    itsMode(Gmodes::DIRECT_RENDER),
    itsUnMode(Gmodes::CLEAR_BOUNDING_BOX),
    itsDisplayList(0)
  {}

  virtual ~GLCacheNode();

  virtual void gnodeDraw(Gfx::Canvas& canvas) const;

  virtual void gnodeUndraw(Gfx::Canvas& canvas) const;

  virtual Rect<double> gnodeBoundingBox(Gfx::Canvas& canvas) const;

  void invalidate();

  Gmodes::RenderMode getMode() const { return itsMode; }
  void setMode(Gmodes::RenderMode new_mode);

  Gmodes::RenderMode getUnMode() const { return itsUnMode; }
  void setUnMode(Gmodes::RenderMode new_mode);

private:
  Gmodes::RenderMode itsMode;
  Gmodes::RenderMode itsUnMode;

  mutable int itsDisplayList;
};

static const char vcid_glcachenode_h[] = "$Header$";
#endif // !GLCACHENODE_H_DEFINED
