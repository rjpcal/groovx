///////////////////////////////////////////////////////////////////////
//
// gxnode.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Nov  1 18:26:45 2000
// written: Wed Nov  1 18:31:58 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXNODE_H_DEFINED
#define GXNODE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

namespace GWT { class Canvas; }

class GxNode : public virtual IO::IoObject {
protected:
  /// Default constructor.
  GxNode();

public:
  /// Virtual destructor.
  virtual ~GxNode();

  /// Get the value of the user-defined category. Default returns -1.
  virtual int getCategory() const;

  /// Draw the object on \a canvas.
  virtual void draw(GWT::Canvas& canvas) const = 0;

  /// Undraw the object from \a canvas.
  virtual void undraw(GWT::Canvas& canvas) const = 0;
};

static const char vcid_gxnode_h[] = "$Header$";
#endif // !GXNODE_H_DEFINED
