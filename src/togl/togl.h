///////////////////////////////////////////////////////////////////////
//
// togl.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 23 15:36:01 2000
// written: Tue Sep 17 17:14:02 2002
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

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TKWIDGET_H_DEFINED)
#include "visx/tkwidget.h"
#endif

struct Tcl_Interp;
struct Tcl_Obj;

typedef struct Tk_Window_ *Tk_Window;

namespace Gfx
{
  class Canvas;
}

///////////////////////////////////////////////////////////////////////
//
// Togl class definition
//
///////////////////////////////////////////////////////////////////////

class Togl : public Tcl::TkWidget
{
private:
  Togl(const Togl&);
  Togl& operator=(const Togl&);

public:
  Togl(Tcl_Interp* interp, const char* pathname);
  virtual ~Togl();

  virtual bool isNotShareable() const { return true; }

  /// Overridden from GWT::Widget.
  virtual Gfx::Canvas& getCanvas() const;

  /// Overridden from Tcl::TkWidget.
  virtual void displayCallback();

  virtual void timerCallback();

  // Miscellaneous

  /// Get info about configuration parameters.
  /** If param is null, get info about all parameters; if param is
      non-null, get info about the parameters that it names. */
  Tcl_Obj* cget(Tcl_Obj* param) const;
  void configure(int objc, Tcl_Obj* const objv[]);
  void makeCurrent() const;

  /// Overridden from GWT::Widget.
  virtual void swapBuffers();

  // Query functions
  bool isRgba() const;
  bool isDoubleBuffered() const;
  unsigned int bitsPerPixel() const;
  bool hasPrivateCmap() const;

  // Color index mode

  /// Simple RGB color struct for use with Togl.
  struct Color
  {
    Color(unsigned int p=0, double r=0.0, double g=0.0, double b=0.0):
      pixel(p), red(r), green(g), blue(b) {}
    unsigned int pixel;
    double red, green, blue;
  };

  Color queryColor(unsigned int color_index) const;

  // Bitmap fonts
  void loadDefaultFont() const;
  void loadBitmapFont(const char* fontname) const;
  void loadBitmapFonti(int fontnumber) const;

  class Impl;
  friend class Impl;

private:
  Impl* const rep;
};

static const char vcid_togl_h[] = "$Header$";
#endif // !TOGL_H_DEFINED
