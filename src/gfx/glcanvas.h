///////////////////////////////////////////////////////////////////////
//
// glcanvas.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Dec  6 20:27:48 1999
// written: Fri Feb 18 22:46:54 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLCANVAS_H_DEFINED
#define GLCANVAS_H_DEFINED

#ifndef CANVAS_H_DEFINED
#include "canvas.h"
#endif

class GLCanvas : public Canvas {
public:
  virtual Point<int> getScreenFromWorld(const Point<double>& world_pos) const;
  virtual Point<double> getWorldFromScreen(const Point<int>& screen_pos) const;

  virtual Rect<int> getScreenFromWorld(const Rect<double>& world_pos) const;
  virtual Rect<double> getWorldFromScreen(const Rect<int>& screen_pos) const;

  virtual Rect<int> getScreenViewport() const;
  virtual Rect<double> getWorldViewport() const;


  virtual bool isRgba() const;
  virtual bool isColorIndex() const;
  virtual bool isDoubleBuffered() const;

  /** Swaps the foreground and background colors, in a way that is
      appropriate to color-index/RGBA modes. In order to be able to
      unswap the colors, this function should normally be preceded by
      glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT), so that a
      call to glPopAttrib() will then unswap the colors. */
  virtual void swapForeBack() const;

  virtual void flushOutput() const;

  virtual void clearColorBuffer() const;

  virtual void drawOnFrontBuffer() const;
  virtual void drawOnBackBuffer() const;

  virtual void pushState() const;
  virtual void popState() const;
};

static const char vcid_glcanvas_h[] = "$Header$";
#endif // !GLCANVAS_H_DEFINED
