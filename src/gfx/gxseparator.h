///////////////////////////////////////////////////////////////////////
//
// gxseparator.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Nov  2 11:20:15 2000
// written: Thu Nov  2 13:51:24 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXSEPARATOR_H_DEFINED
#define GXSEPARATOR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GXNODE_H_DEFINED)
#include "gx/gxnode.h"
#endif

template <class T> class IdItem;

class GxSeparator : public GxNode {
protected:
  GxSeparator();

public:
  static GxSeparator* make();

  virtual ~GxSeparator();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  typedef int ChildId;

  ChildId addChild(int ioId);
  void insertChild(int ioId, ChildId at_index);
  void removeChildId(ChildId index);
  void removeChildItem(int ioId);

  int numChildren() const;
  IdItem<GxNode> getChild(ChildId index) const;

  typedef const IdItem<GxNode>* ChildItr;

  ChildItr beginChildren() const;
  ChildItr endChildren() const;

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
