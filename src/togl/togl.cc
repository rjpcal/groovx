///////////////////////////////////////////////////////////////////////
//
// togl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 23 13:11:59 2000
// written: Tue Sep 17 17:53:11 2002
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

#ifndef TOGL_CC_DEFINED
#define TOGL_CC_DEFINED

#include "togl/togl.h"

#include "gfx/canvas.h"

#include "gx/rgbacolor.h"

#include "tcl/tclsafeinterp.h"

#include "togl/glutil.h"
#include "togl/glxopts.h"
#include "togl/glxwrapper.h"
#include "togl/x11util.h"

#include "util/error.h"
#include "util/pointers.h"
#include "util/ref.h"
#include "util/strings.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include <tcl.h>
#include <tk.h>
#ifndef _TKPORT
#  define _TKPORT  // This eliminates need to include a bunch of Tk baggage
#endif
#include <tkInt.h>

#ifdef HAVE_LIMITS
#  include <limits>
#else
#  include <climits>
#endif

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// ToglOpts definition (plain-old-data struct)
//
///////////////////////////////////////////////////////////////////////

struct ToglOpts
{
  void toDefaults()
  {
    time = 0;
    privateCmapFlag = 0;
    glx.toDefaults();
  }

  int time;
  int privateCmapFlag;
  GlxOpts glx;
};

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace
{
  // Defaults
  const char*  DEFAULT_TIME      = "0";

  // Tk option database machinery for Togl:

  const int TOGL_GLX_OPTION     = 1 << 0;

  Tk_OptionTable toglOptionTable = 0;

  Tk_OptionSpec optionSpecs[] =
  {
    {TK_OPTION_BOOLEAN, (char*)"-rgba", (char*)"rgba", (char*)"Rgba",
#ifndef NO_RGBA
     (char*)"true",
#else
     (char*)"false",
#endif
     -1, Tk_Offset(ToglOpts, glx.rgbaFlag), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_INT, (char*)"-redsize", (char*)"redsize", (char*)"RedSize",
     (char*)"1", -1, Tk_Offset(ToglOpts, glx.rgbaRed), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_INT, (char*)"-greensize", (char*)"greensize", (char*)"GreenSize",
     (char*)"1", -1, Tk_Offset(ToglOpts, glx.rgbaGreen), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_INT, (char*)"-bluesize", (char*)"bluesize", (char*)"BlueSize",
     (char*)"1", -1, Tk_Offset(ToglOpts, glx.rgbaBlue), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_INT, (char*)"-colorindexsize", (char*)"colorindexsize", (char*)"ColorIndexSize",
     (char*)"8", -1, Tk_Offset(ToglOpts, glx.colorIndexSize), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_BOOLEAN, (char*)"-double", (char*)"double", (char*)"Double",
#ifndef NO_DOUBLE_BUFFER
     (char*)"true",
#else
     (char*)"false",
#endif
     -1, Tk_Offset(ToglOpts, glx.doubleFlag), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_BOOLEAN, (char*)"-depth", (char*)"depth", (char*)"Depth",
     (char*)"true", -1, Tk_Offset(ToglOpts, glx.depthFlag), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_INT, (char*)"-depthsize", (char*)"depthsize", (char*)"DepthSize",
     (char*)"8", -1, Tk_Offset(ToglOpts, glx.depthSize), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_BOOLEAN, (char*)"-accum", (char*)"accum", (char*)"Accum",
     (char*)"false", -1, Tk_Offset(ToglOpts, glx.accumFlag), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_INT, (char*)"-accumredsize", (char*)"accumredsize", (char*)"AccumRedSize",
     (char*)"1", -1, Tk_Offset(ToglOpts, glx.accumRed), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_INT, (char*)"-accumgreensize", (char*)"accumgreensize", (char*)"AccumGreenSize",
     (char*)"1", -1, Tk_Offset(ToglOpts, glx.accumGreen), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_INT, (char*)"-accumbluesize", (char*)"accumbluesize", (char*)"AccumBlueSize",
     (char*)"1", -1, Tk_Offset(ToglOpts, glx.accumBlue), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_INT, (char*)"-accumalphasize", (char*)"accumalphasize", (char*)"AccumAlphaSize",
     (char*)"1", -1, Tk_Offset(ToglOpts, glx.accumAlpha), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_BOOLEAN, (char*)"-alpha", (char*)"alpha", (char*)"Alpha",
     (char*)"false", -1, Tk_Offset(ToglOpts, glx.alphaFlag), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_INT, (char*)"-alphasize", (char*)"alphasize", (char*)"AlphaSize",
     (char*)"1", -1, Tk_Offset(ToglOpts, glx.alphaSize), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_BOOLEAN, (char*)"-stencil", (char*)"stencil", (char*)"Stencil",
     (char*)"false", -1, Tk_Offset(ToglOpts, glx.stencilFlag), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_INT, (char*)"-stencilsize", (char*)"stencilsize", (char*)"StencilSize",
     (char*)"1", -1, Tk_Offset(ToglOpts, glx.stencilSize), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_INT, (char*)"-auxbuffers", (char*)"auxbuffers", (char*)"AuxBuffers",
     (char*)"0", -1, Tk_Offset(ToglOpts, glx.auxNumber), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_BOOLEAN, (char*)"-indirect", (char*)"indirect", (char*)"Indirect",
     (char*)"false", -1, Tk_Offset(ToglOpts, glx.indirect), 0, NULL, TOGL_GLX_OPTION},

    {TK_OPTION_BOOLEAN, (char*)"-privatecmap", (char*)"privateCmap", (char*)"PrivateCmap",
     (char*)"false", -1, Tk_Offset(ToglOpts, privateCmapFlag), 0, NULL, 0},

    {TK_OPTION_INT, (char*)"-time", (char*)"time", (char*)"Time",
     (char*)DEFAULT_TIME, -1, Tk_Offset(ToglOpts, time), 0, NULL, 0},

    {TK_OPTION_END, (char*) 0, (char*) 0, (char *) 0,
     (char*) 0, 0, 0, 0, NULL, 0}
  };
}

///////////////////////////////////////////////////////////////////////
//
// Togl::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class Togl::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Togl* itsOwner;
  const Tk_Window itsTkWin;
  shared_ptr<ToglOpts> itsOpts;
  shared_ptr<GlxWrapper> itsGlx;

  int itsFontListBase;

  Tcl_TimerToken itsTimerToken;

  Impl(Togl* owner, Tcl_Interp* interp);
  ~Impl() throw();

  Tcl_Obj* cget(Tcl_Obj* param) const;
  void configure(int objc, Tcl_Obj* const objv[]);

  // All callbacks cast to/from Togl::Impl*, _NOT_ Togl* !!!
  static void cTimerCallback(ClientData clientData) throw();

  static Window cClassCreateProc(Tk_Window tkwin,
                                 Window parent,
                                 ClientData clientData);

  void swapBuffers() const;

  Togl::Color queryColor(unsigned int color_index) const;

  void loadFontList(GLuint newListBase);

  Gfx::Canvas& canvas() const { return itsGlx->canvas(); }

private:
  void makeWindowExist(); // throws a Util::Error on failure
};


//---------------------------------------------------------------------
//
// Togl::Impl::Impl
//
//     * Creates a new window
//     * Creates an 'Togl' data structure
//     * Creates an event handler for this window
//     * Creates a command that handles this object
//     * Configures this Togl for the given arguments
//
//---------------------------------------------------------------------

Togl::Impl::Impl(Togl* owner, Tcl_Interp* interp) :
  itsOwner(owner),
  itsTkWin(owner->tkWin()),
  itsOpts(new ToglOpts),
  itsGlx(0),

  itsFontListBase(0),

  itsTimerToken(0)
{
DOTRACE("Togl::Impl::Impl");

  //
  // Configure the widget
  //

  Tk_SetClass(itsTkWin, (char*)"Togl");

  //
  // Set up options
  //

  if (toglOptionTable == 0)
    {
      toglOptionTable = Tk_CreateOptionTable(interp, optionSpecs);
    }

  itsOpts->toDefaults();

  if (Tk_InitOptions(interp, reinterpret_cast<char*>(itsOpts.get()),
                     toglOptionTable, itsTkWin) == TCL_ERROR)
    {
      throw Util::Error(fstring("Togl couldn't initialize options:\n",
                                Tcl_GetStringResult(interp)));
    }

  //
  // Get the window mapped onscreen
  //

  Tk_GeometryRequest(itsTkWin, itsOwner->width(), itsOwner->height());

  makeWindowExist();

  //
  // Set up handlers
  //

  if (itsOpts->time > 0)
    {
      itsTimerToken =
        Tcl_CreateTimerHandler(itsOpts->time, &cTimerCallback,
                               static_cast<ClientData>(this));
    }

  //
  // Set up canvas
  //

  if (itsOpts->glx.rgbaFlag)
    {
      itsGlx->canvas().setColor(Gfx::RgbaColor(0.0, 0.0, 0.0, 1.0));
      itsGlx->canvas().setClearColor(Gfx::RgbaColor(1.0, 1.0, 1.0, 1.0));
    }
  else
    {
      // FIXME use XBlackPixel(), XWhitePixel() here?
      itsGlx->canvas().setColorIndex(0);
      itsGlx->canvas().setClearColorIndex(1);
    }
}

Togl::Impl::~Impl() throw()
{
DOTRACE("Togl::Impl::~Impl");

  Assert(itsTkWin != 0);

  Tcl_DeleteTimerHandler(itsTimerToken);

  Tk_FreeConfigOptions(reinterpret_cast<char*>(itsOpts.get()),
                       toglOptionTable, itsTkWin);
}

//---------------------------------------------------------------------
//
// Process a configure read-request
//
//---------------------------------------------------------------------

Tcl_Obj* Togl::Impl::cget(Tcl_Obj* param) const
{
DOTRACE("Togl::Impl::cget");

  Tcl_Obj* objResult =
    Tk_GetOptionInfo(itsOwner->interp(),
                     reinterpret_cast<char*>(itsOpts.get()),
                     toglOptionTable,
                     param, itsTkWin);

  if (objResult == 0)
    throw Util::Error("couldn't get configuration parameters");

  return objResult;
}

//---------------------------------------------------------------------
//
// Process a configure write-request
//
//---------------------------------------------------------------------

void Togl::Impl::configure(int objc, Tcl_Obj* const objv[])
{
DOTRACE("Togl::Impl::configure");

  int mask = 0;

  if (Tk_SetOptions(itsOwner->interp(),
                    reinterpret_cast<char*>(itsOpts.get()),
                    toglOptionTable, objc, objv, itsTkWin,
                    (Tk_SavedOptions*) 0, &mask)
      == TCL_ERROR)
    {
      throw Util::Error("error while setting togl widget options");
    }

  // If any GLX options changed, then we have to recreate the window
  // and GLX context
  if (mask & TOGL_GLX_OPTION)
    makeWindowExist();
}

void Togl::Impl::cTimerCallback(ClientData clientData) throw()
{
DOTRACE("Togl::Impl::cTimerCallback");
  Impl* rep = static_cast<Impl*>(clientData);
  Tcl_Preserve(clientData);

  try
    {
      rep->itsOwner->timerCallback();
      rep->itsTimerToken =
        Tcl_CreateTimerHandler(rep->itsOpts->time, cTimerCallback,
                               static_cast<ClientData>(rep));
    }
  catch (...)
    {
      Tcl::Interp(rep->itsOwner->interp()).handleLiveException("cTimerCallback", true);
    }

  Tcl_Release(clientData);
}

void Togl::Impl::swapBuffers() const
{
DOTRACE("Togl::Impl::swapBuffers");
  if (itsOpts->glx.doubleFlag)
    {
      glXSwapBuffers(Tk_Display(itsTkWin), Tk_WindowId(itsTkWin));
    }
  else
    {
      glFlush();
    }
}

Togl::Color Togl::Impl::queryColor(unsigned int color_index) const
{
  XColor col;

  col.pixel = color_index;
  XQueryColor(Tk_Display(itsTkWin), Tk_Colormap(itsTkWin), &col);

  Togl::Color color;

  color.pixel = (unsigned int)col.pixel;
#ifdef HAVE_LIMITS
  const unsigned short usmax = std::numeric_limits<unsigned short>::max();
#else
  const unsigned short usmax = USHRT_MAX;
#endif

  color.red   = double(col.red)   / usmax;
  color.green = double(col.green) / usmax;
  color.blue  = double(col.blue)  / usmax;

  return color;
}

void Togl::Impl::loadFontList(GLuint newListBase)
{
DOTRACE("Togl::Impl::loadFontList");
  // Check if font loading succeeded...
  if (newListBase == 0)
    {
      throw Util::Error("unable to load font");
    }

  // ... otherwise unload the current font
  if (itsFontListBase > 0)
    {
      GLUtil::unloadBitmapFont(itsFontListBase);
    }

  // ... and point to the new font
  itsFontListBase = newListBase;
}

Window Togl::Impl::cClassCreateProc(Tk_Window tkwin,
                                    Window parent,
                                    ClientData clientData)
{
  Togl::Impl* rep = static_cast<Togl::Impl*>(clientData);

  Display* dpy = Tk_Display(tkwin);

  int dummy;
  if (!glXQueryExtension(dpy, &dummy, &dummy))
    {
      throw Util::Error("Togl: X server has no OpenGL GLX extension");
    }

  rep->itsGlx.reset(GlxWrapper::make(dpy, rep->itsOpts->glx));

  XVisualInfo* visInfo = rep->itsGlx->visInfo();

  Colormap cmap = X11Util::findColormap(dpy, visInfo,
                                        rep->itsOpts->privateCmapFlag);

  // Make sure Tk knows to switch to the new colormap when the cursor is over
  // this window when running in color index mode.
  Tk_SetWindowVisual(tkwin, visInfo->visual, visInfo->depth, cmap);

#define ALL_EVENTS_MASK \
KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask| \
EnterWindowMask|LeaveWindowMask|PointerMotionMask|ExposureMask|   \
VisibilityChangeMask|FocusChangeMask|PropertyChangeMask|ColormapChangeMask

  XSetWindowAttributes atts;

  atts.colormap = cmap;
  atts.border_pixel = 0;
  atts.event_mask = ALL_EVENTS_MASK;

  Window win = XCreateWindow(dpy,
                             parent,
                             0, 0,
                             rep->itsOwner->width(),
                             rep->itsOwner->height(),
                             0, visInfo->depth,
                             InputOutput, visInfo->visual,
                             CWBorderPixel | CWColormap | CWEventMask,
                             &atts);

  Tk_ChangeWindowAttributes(tkwin,
                            CWBorderPixel | CWColormap | CWEventMask,
                            &atts);

  XSelectInput(dpy, win, ALL_EVENTS_MASK);

  if (!rep->itsOpts->glx.rgbaFlag)
    {
      X11Util::hackInstallColormap(dpy, Tk_WindowId(tkwin), cmap);
    }

  // Bind the context to the window and make it the current context
  rep->itsGlx->makeCurrent(win);

  // Check for a single/double buffering snafu
  if (rep->itsOpts->glx.doubleFlag == 0 && rep->itsGlx->isDoubleBuffered())
    {
      // We requested single buffering but had to accept a double buffered
      // visual.  Set the GL draw buffer to be the front buffer to
      // simulate single buffering.
      glDrawBuffer(GL_FRONT);
    }

  return win;
}

Tk_ClassProcs toglProcs =
  {
    sizeof(Tk_ClassProcs),
    (Tk_ClassWorldChangedProc*) 0,
    Togl::Impl::cClassCreateProc,
    (Tk_ClassModalProc*) 0,
  };

void Togl::Impl::makeWindowExist()
{
DOTRACE("Togl::Impl::makeWindowExist");

  TkWindow* winPtr = reinterpret_cast<TkWindow*>(itsTkWin);

  if (winPtr->window != None)
    {
      XDestroyWindow(winPtr->display, winPtr->window);
      winPtr->window = 0;
    }

  Tk_SetClassProcs(itsTkWin, &toglProcs, static_cast<ClientData>(this));

  Tk_MakeWindowExist(itsTkWin);

  Tk_MapWindow(itsTkWin);
}

///////////////////////////////////////////////////////////////////////
//
// Togl member function definitions
//
///////////////////////////////////////////////////////////////////////

Togl::Togl(Tcl_Interp* interp, const char* pathname) :
  Tcl::TkWidget(interp, pathname),
  rep(new Impl(this, interp))
{
DOTRACE("Togl::Togl");

  makeCurrent();
}

Togl::~Togl()
{
DOTRACE("Togl::~Togl");
  delete rep;
}

void Togl::displayCallback()
{
DOTRACE("Togl::displayCallback");

  rep->itsGlx->makeCurrent(Tk_WindowId(rep->itsTkWin));
  fullRender();
}

void Togl::timerCallback()
{
DOTRACE("Togl::timerCallback");
}

Tcl_Obj* Togl::cget(Tcl_Obj* param) const
  { return rep->cget(param); }

void Togl::configure(int objc, Tcl_Obj* const objv[])
  { rep->configure(objc, objv); }

void Togl::makeCurrent() const          { rep->itsGlx->makeCurrent(Tk_WindowId(rep->itsTkWin)); }
void Togl::swapBuffers()                { rep->swapBuffers(); }
bool Togl::isRgba() const               { return rep->itsOpts->glx.rgbaFlag; }
bool Togl::isDoubleBuffered() const     { return rep->itsOpts->glx.doubleFlag; }
unsigned int Togl::bitsPerPixel() const { return rep->itsGlx->visInfo()->depth; }
bool Togl::hasPrivateCmap() const       { return rep->itsOpts->privateCmapFlag; }

Togl::Color Togl::queryColor(unsigned int color_index) const
  { return rep->queryColor(color_index); }

void Togl::loadDefaultFont() const
{
  loadBitmapFont(0);
}

void Togl::loadBitmapFont(const char* fontname) const
{
DOTRACE("Togl::loadBitmapFont");
  rep->loadFontList(GLUtil::loadBitmapFont(Tk_Display(rep->itsTkWin), fontname));
}

void Togl::loadBitmapFonti(int fontnumber) const
{
DOTRACE("Togl::loadBitmapFonti");
  rep->loadFontList(GLUtil::loadBitmapFont(Tk_Display(rep->itsTkWin),
                                           GLUtil::NamedFont(fontnumber)));
}

Gfx::Canvas& Togl::getCanvas() const
{
DOTRACE("Togl::getCanvas");
  makeCurrent();
  return rep->itsGlx->canvas();
}

static const char vcid_togl_cc[] = "$Header$";
#endif // !TOGL_CC_DEFINED
