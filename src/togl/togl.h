///////////////////////////////////////////////////////////////////////
//
// togl.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 23 15:36:01 2000
// written: Mon Sep 16 11:04:14 2002
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

#define TOGL_VERSION "1.5"
#define TOGL_MAJOR_VERSION 1
#define TOGL_MINOR_VERSION 5

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
  typedef void (Callback) (Togl* togl);
  typedef void (OverlayCallback) (void* togl);

  // Default callback setup functions
  static void setDefaultClientData(ClientData clientData);
  static void setDefaultCreateFunc(Callback* proc);
  static void setDefaultDisplayFunc(Callback* proc);
  static void setDefaultReshapeFunc(Callback* proc);
  static void setDefaultDestroyFunc(Callback* proc);
  static void setDefaultTimerFunc(Callback* proc);
  static void resetDefaultCallbacks();

  // Change callbacks for existing widget
  void setDisplayFunc(Togl::Callback* proc);
  void setReshapeFunc(Togl::Callback* proc);
  void setDestroyFunc(Togl::Callback* proc);

  // Miscellaneous
  int configure(int objc, Tcl_Obj* const objv[]);
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

  // User client data
  ClientData getClientData() const;
  void setClientData(ClientData clientData);

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
