///////////////////////////////////////////////////////////////////////
//
// gxseparator.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov  2 11:20:15 2000
// written: Fri May 11 20:51:40 2001
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

template <class T> class IdItem;

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

  ChildId addChild(IO::UID ioUid);
  void insertChild(IO::UID ioUid, ChildId at_index);
  void removeChildId(ChildId index);
  void removeChildUid(IO::UID ioUid);

  int numChildren() const;
  IdItem<GxNode> getChild(ChildId index) const;

  typedef const IdItem<GxNode>* ConstChildItr;

  ConstChildItr beginChildren() const;
  ConstChildItr endChildren() const;

  typedef IdItem<GxNode>* ChildItr;

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
