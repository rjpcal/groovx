///////////////////////////////////////////////////////////////////////
//
// gxnode.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov  1 18:26:45 2000
// written: Fri May 11 20:21:36 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXNODE_H_DEFINED
#define GXNODE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

namespace GWT { class Canvas; }

///////////////////////////////////////////////////////////////////////
/**
 *
 * GxNode is the base class for all graphic objects.
 *
 **/
///////////////////////////////////////////////////////////////////////

class GxNode : public virtual IO::IoObject {
private:
  GxNode(const GxNode&);
  GxNode& operator=(const GxNode&);

protected:
  /// Default constructor.
  GxNode();

public:
  /// Virtual destructor.
  virtual ~GxNode();

  /// Get the value of the user-defined category. Default returns -1.
  virtual int category() const;

  /** Returns true if \a other is contained within this node in the
      scene graph. The default implementation (for leaf nodes) returns
      true only if this == other. For composite nodes, the function
      will check recursively is \a other is contained within the
      composite structure. This function is used to avoid generated
      cycles in the scene graph. */
  virtual bool contains(GxNode* other) const;

  /// Draw the object on \a canvas.
  virtual void draw(GWT::Canvas& canvas) const = 0;

  /// Undraw the object from \a canvas.
  virtual void undraw(GWT::Canvas& canvas) const = 0;
};

static const char vcid_gxnode_h[] = "$Header$";
#endif // !GXNODE_H_DEFINED
