///////////////////////////////////////////////////////////////////////
//
// togl.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 23 15:36:01 2000
// written: Mon Sep 16 18:30:22 2002
// $Id$
//
// This is a modified version of the Togl widget by Brian Paul and Ben
// Bederson; see the original copyright notice and log info below.
//
// Win32 support has been removed from this version, and the code has
// been C++-ified.
//
// *** Original Copyright Notice ***
//
// Togl - a Tk OpenGL widget
// Version 1.5
// Copyright (C) 1996-1997  Brian Paul and Ben Bederson
// See the LICENSE file for copyright details.
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGL_H_DEFINED
#define TOGL_H_DEFINED

#include <tcl.h>
#include <tk.h>
#include <X11/Xlib.h>

namespace Gfx
{
  class Canvas;
}

///////////////////////////////////////////////////////////////////////
//
// Togl class definition
//
///////////////////////////////////////////////////////////////////////

class Togl
{
private:
  Togl(const Togl&);
  Togl& operator=(const Togl&);

public:
  Togl(Tcl_Interp* interp, const char* pathname);
  virtual ~Togl();

  // Callbacks
  virtual void displayCallback();
  virtual void reshapeCallback();
  virtual void timerCallback();

  // Miscellaneous

  /// Get info about configuration parameters.
  /** If param is null, get info about all parameters; if param is
      non-null, get info about the parameters that it names. */
  Tcl_Obj* cget(Tcl_Obj* param) const;
  void configure(int objc, Tcl_Obj* const objv[]);
  void makeCurrent() const;
  void requestRedisplay();
  void requestReconfigure();
  void swapBuffers() const;

  // Query functions
  int width() const;
  int height() const;
  bool isRgba() const;
  bool isDoubleBuffered() const;
  unsigned int bitsPerPixel() const;
  bool hasPrivateCmap() const;
  Tcl_Interp* interp() const;
  Tk_Window tkWin() const;
  const char* pathname() const;

  // Color index mode
  unsigned long allocColor(float red, float green, float blue) const;
  void freeColor(unsigned long index) const;
  void setColor(unsigned long index,
                float red, float green, float blue) const;

  // Bitmap fonts
  unsigned int loadBitmapFont(const char* fontname) const;
  unsigned int loadBitmapFonti(int fontnumber) const;
  void unloadBitmapFont(unsigned int fontbase) const;

  // Layers
  enum Layer
    {
      Normal = 1,
      Overlay = 2
    };

  // Overlay functions
  typedef void (OverlayCallback) (void* togl);

  void overlayDisplayFunc(OverlayCallback* proc);
  void useLayer(Layer layer);
  void showOverlay();
  void hideOverlay();
  void requestOverlayRedisplay();
  int existsOverlay() const;
  int getOverlayTransparentValue() const;
  bool isMappedOverlay() const;
  unsigned long allocColorOverlay(float red, float green, float blue) const;
  void freeColorOverlay(unsigned long index) const;

  // Manipulators
  void setWidth(int w);
  void setHeight(int h);

  // X11-only commands
  Display* display() const;
  Screen* screen() const;
  int screenNumber() const;
  Colormap colormap() const;
  Window windowId() const;

  // Canvas
  Gfx::Canvas& getCanvas() const;

  class Impl;
  friend class Impl;

private:
  Impl* const rep;
};

static const char vcid_togl_h[] = "$Header$";
#endif // !TOGL_H_DEFINED
