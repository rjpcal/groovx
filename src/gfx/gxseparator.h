///////////////////////////////////////////////////////////////////////
//
// gxseparator.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov  2 11:20:15 2000
// written: Mon Aug  6 11:09:28 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXSEPARATOR_H_DEFINED
#define GXSEPARATOR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GXNODE_H_DEFINED)
#include "gx/gxnode.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IODECLS_H_DEFINED)
#include "io/iodecls.h"
#endif

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

class GxSeparator : public GxNode {
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

  typedef const Util::Ref<GxNode>* ConstChildItr;

  ConstChildItr beginChildren() const;
  ConstChildItr endChildren() const;

  typedef Util::Ref<GxNode>* ChildItr;

  ChildItr beginChildren();
  ChildItr endChildren();

  virtual bool contains(GxNode* other) const;

  /// Draw all of the contained objects on \a canvas.
  virtual void draw(GWT::Canvas& canvas) const;

  /// Undraw all of the contained objects from \a canvas.
  virtual void undraw(GWT::Canvas& canvas) const;

private:
  GxSeparator(const GxSeparator&);
  GxSeparator& operator=(const GxSeparator&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_gxseparator_h[] = "$Header$";
#endif // !GXSEPARATOR_H_DEFINED
