///////////////////////////////////////////////////////////////////////
//
// glxoverlay.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Aug  5 17:08:56 2002
// written: Mon Aug  5 17:13:16 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLXOVERLAY_H_DEFINED
#define GLXOVERLAY_H_DEFINED

#include <X11/Xlib.h>

class GlxWrapper;

class GlxOverlay
{
public:
  GlxOverlay(Display* dpy, Window parent,
             bool direct, GlxWrapper* share,
             int width, int height);

  ~GlxOverlay();

  typedef void (Callback) (void* data);

  void setDisplayProc(Callback* proc, void* data);

  void render();

  void reconfigure(int width, int height);

  void makeCurrent();

  void show();

  void hide();

  void requestRedisplay();

  int transparentPixel() const { return itsTransparentPixel; }
  bool isMapped() const { return isItMapped; }

  unsigned long allocColor(float red, float green, float blue);

  void freeColor(unsigned long pixel);

  Window windowId() const { return itsWindow; }

private:
  Display* itsDisplay;
  GlxWrapper* itsGlx;
  Window itsWindow;
  bool itsUpdatePending;
  Colormap itsCmap;
  int itsTransparentPixel;
  bool isItMapped;
  Callback* itsDisplayProc;
  void* itsData;
};

static const char vcid_glxoverlay_h[] = "$Header$";
#endif // !GLXOVERLAY_H_DEFINED
