///////////////////////////////////////////////////////////////////////
//
// gxseparator.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov  2 11:20:15 2000
// written: Mon Jan 13 11:04:47 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXSEPARATOR_H_DEFINED
#define GXSEPARATOR_H_DEFINED

#include "gfx/gxnode.h"

namespace Util
{
  template <class T> class Ref;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * GxSeparator is a subclass of GxNode that holds a list of children,
 * and can be used to implement arbitrary tree structured hierarchies
 * of GxNode's. GxSeparator saves the OpenGL state before drawing or
 * undrawing its children, and restores the state when it is done
 * handling its children.
 *
 **/
///////////////////////////////////////////////////////////////////////

class GxSeparator : public GxNode
{
protected:
  GxSeparator();

public:
  static GxSeparator* make();

  virtual ~GxSeparator();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  typedef unsigned int ChildId;

  ChildId addChild(Util::Ref<GxNode> item);
  void insertChild(Util::Ref<GxNode> item, ChildId at_index);
  void removeChildAt(ChildId index);
  void removeChild(Util::Ref<GxNode> item);

  unsigned int numChildren() const;
  Util::Ref<GxNode> getChild(ChildId index) const;

  Util::FwdIter<Util::Ref<GxNode> > children() const;

  /** Returns an iterator to all the children recursively contained in
      this object. */
  virtual Util::FwdIter<const Util::Ref<GxNode> > deepChildren();

  virtual bool contains(GxNode* other) const;

  /// Apply all of the contained objects' transformations to the cube.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  /// Query whether debug-mode drawing is turned on.
  bool getDebugMode() const;

  /// Turn debug-mode drawing on or off.
  void setDebugMode(bool b);

  /// Draw all of the contained objects on \a canvas.
  virtual void draw(Gfx::Canvas& canvas) const;

private:
  GxSeparator(const GxSeparator&);
  GxSeparator& operator=(const GxSeparator&);

  class Impl;
  Impl* const rep;
};

static const char vcid_gxseparator_h[] = "$Header$";
#endif // !GXSEPARATOR_H_DEFINED
