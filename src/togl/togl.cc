///////////////////////////////////////////////////////////////////////
//
// togl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 23 13:11:59 2000
// written: Tue Sep 17 12:17:22 2002
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
#include "togl/tkutil.h"
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
    width = 400;
    height = 400;
    time = 0;
    privateCmapFlag = 0;
    glx.toDefaults();
  }

  int width;
  int height;
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
  Tcl_Interp* itsInterp;
  const Tk_Window itsTkWin;
  Display* itsDisplay;
  shared_ptr<ToglOpts> itsOpts;
  shared_ptr<GlxWrapper> itsGlx;

  int itsFontListBase;

  bool itsUpdatePending;

  Tcl_TimerToken itsTimerToken;

  Impl(Togl* owner, Tcl_Interp* interp);
  ~Impl() throw();

  Tcl_Obj* cget(Tcl_Obj* param) const;
  void configure(int objc, Tcl_Obj* const objv[]);

  // All callbacks cast to/from Togl::Impl*, _NOT_ Togl* !!!
  static void cEventCallback(ClientData clientData, XEvent* eventPtr) throw();
  static void cTimerCallback(ClientData clientData) throw();
  static void cRenderCallback(ClientData clientData) throw();

  void requestRedisplay();
  void requestReconfigure();
  void swapBuffers() const;

  void ensureSharedColormap(const char* what) const;

  unsigned long allocColor(float red, float green, float blue) const;
  void freeColor(unsigned long pixel) const;
  void setColor(unsigned long index, float red, float green, float blue) const;
  void setColor(const Togl::Color& color) const;
  Togl::Color queryColor(unsigned int color_index) const;
  void queryColor(unsigned int color_index, Togl::Color& color) const;

  void loadFontList(GLuint newListBase);

  Window windowId() const { return Tk_WindowId(itsTkWin); }

  Gfx::Canvas& canvas() const { return itsGlx->canvas(); }

private:
  void eventProc(XEvent* eventPtr);
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
  itsInterp(interp),
  itsTkWin(owner->tkWin()),
  itsDisplay(0),
  itsOpts(new ToglOpts),
  itsGlx(0),

  itsFontListBase(0),

  itsUpdatePending(false),

  itsTimerToken(0)
{
DOTRACE("Togl::Impl::Impl");

  //
  // Configure the widget
  //

  itsDisplay = Tk_Display( itsTkWin );

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
                                Tcl_GetStringResult(itsInterp)));
    }

  //
  // Get the window mapped onscreen
  //

  Tk_GeometryRequest(itsTkWin, itsOpts->width, itsOpts->height);

  Assert(itsGlx.get() == 0);

  makeWindowExist();

  //
  // Set up handlers
  //

  Tk_CreateEventHandler(itsTkWin,
                        ExposureMask | StructureNotifyMask,
                        &cEventCallback,
                        static_cast<ClientData>(this));

  if (itsOpts->time > 0)
    {
      itsTimerToken =
        Tcl_CreateTimerHandler( itsOpts->time, &cTimerCallback,
                                static_cast<ClientData>(this) );
    }

  //
  // Set up canvas
  //

  if ( itsOpts->glx.rgbaFlag )
    {
      itsGlx->canvas().setColor(Gfx::RgbaColor(0.0, 0.0, 0.0, 1.0));
      itsGlx->canvas().setClearColor(Gfx::RgbaColor(1.0, 1.0, 1.0, 1.0));
    }
  else
    { // not using rgba
      if ( itsOpts->privateCmapFlag )
        {
          itsGlx->canvas().setColorIndex(0);
          itsGlx->canvas().setClearColorIndex(1);
        }
      else
        {
          itsGlx->canvas().setColorIndex(allocColor(0.0, 0.0, 0.0));
          itsGlx->canvas().setClearColorIndex(allocColor(1.0, 1.0, 1.0));
        }
    }
}

Togl::Impl::~Impl() throw()
{
DOTRACE("Togl::Impl::~Impl");

  Assert(itsTkWin != 0);

  Tcl_DeleteTimerHandler(itsTimerToken);

  Tcl_CancelIdleCall(cRenderCallback, static_cast<ClientData>(this));

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
    Tk_GetOptionInfo(itsInterp,
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

  if (Tk_SetOptions(itsInterp, reinterpret_cast<char*>(itsOpts.get()),
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

void Togl::Impl::cEventCallback(ClientData clientData, XEvent* eventPtr) throw()
{
  Impl* rep = static_cast<Impl*>(clientData);
  Tcl_Preserve(clientData);
  try
    {
      rep->eventProc(eventPtr);
    }
  catch (...)
    {
      Tcl::Interp(rep->itsInterp).handleLiveException("cEventCallback", true);
    }
  Tcl_Release(clientData);
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
      Tcl::Interp(rep->itsInterp).handleLiveException("cTimerCallback", true);
    }

  Tcl_Release(clientData);
}

// Called when the widget's contents must be redrawn.
void Togl::Impl::cRenderCallback(ClientData clientData) throw()
{
DOTRACE("Togl::Impl::cRenderCallback");
  Impl* rep = static_cast<Impl*>(clientData);

  Tcl_Preserve(clientData);

  try
    {
      rep->itsGlx->makeCurrent(rep->windowId());
      rep->itsOwner->displayCallback();
      rep->itsUpdatePending = false;
    }
  catch (...)
    {
      Tcl::Interp(rep->itsInterp).handleLiveException("cRenderCallback", true);
    }

  Tcl_Release(clientData);
}

void Togl::Impl::requestRedisplay()
{
DOTRACE("Togl::Impl::requestRedisplay");
  if (!itsUpdatePending)
    {
      Tk_DoWhenIdle( cRenderCallback, static_cast<ClientData>(this) );
      itsUpdatePending = true;
    }
}

void Togl::Impl::requestReconfigure()
{
DOTRACE("Togl::Impl::requestReconfigure");
  if (itsOpts->width != Tk_Width(itsTkWin) ||
      itsOpts->height != Tk_Height(itsTkWin))
    {
      itsOpts->width = Tk_Width(itsTkWin);
      itsOpts->height = Tk_Height(itsTkWin);
      XResizeWindow(itsDisplay, windowId(), itsOpts->width, itsOpts->height);

      itsGlx->makeCurrent(windowId());
    }

  itsOwner->reshapeCallback();
}

void Togl::Impl::swapBuffers() const
{
DOTRACE("Togl::Impl::swapBuffers");
  if (itsOpts->glx.doubleFlag)
    {
      glXSwapBuffers( itsDisplay, windowId() );
    }
  else
    {
      glFlush();
    }
}

void Togl::Impl::ensureSharedColormap(const char* what) const
{
DOTRACE("Togl::Impl::ensureSharedColormap");
  if (itsOpts->glx.rgbaFlag)
    {
      throw Util::Error(fstring(what, " not allowed in RGBA mode"));
    }

  if (itsOpts->privateCmapFlag)
    {
      throw Util::Error(fstring(what, " not allowed with private colormap"));
    }
}

unsigned long Togl::Impl::allocColor(float red, float green, float blue) const
{
DOTRACE("Togl::Impl::allocColor");

  ensureSharedColormap("Togl::allocColor");

  return X11Util::noFaultXAllocColor(itsDisplay, Tk_Colormap(itsTkWin),
                                     Tk_Visual(itsTkWin)->map_entries,
                                     red, green, blue);
}

void Togl::Impl::freeColor(unsigned long pixel) const
{
DOTRACE("Togl::Impl::freeColor");

  ensureSharedColormap("Togl::freeColor");

  XFreeColors(itsDisplay, Tk_Colormap(itsTkWin), &pixel, 1, 0);
}

void Togl::Impl::setColor(unsigned long index,
                          float red, float green, float blue) const
{
DOTRACE("Togl::Impl::setColor");

  ensureSharedColormap("Togl::allocColor");

  XColor xcol;

  xcol.pixel = index;
  xcol.red   = (short) (red   * 65535.0);
  xcol.green = (short) (green * 65535.0);
  xcol.blue  = (short) (blue  * 65535.0);
  xcol.flags = DoRed | DoGreen | DoBlue;

  XStoreColor( itsDisplay, Tk_Colormap(itsTkWin), &xcol );
}

void Togl::Impl::setColor(const Togl::Color& color) const
{
DOTRACE("Toglet::setColor");

  static const char* const bad_val_msg = "RGB values must be in [0.0, 1.0]";
  static const char* const bad_index_msg = "color index must be in [0, 255]";

  if (                     color.pixel > 255) { throw Util::Error(bad_index_msg); }
  if (color.red   < 0.0 || color.red   > 1.0) { throw Util::Error(bad_val_msg); }
  if (color.green < 0.0 || color.green > 1.0) { throw Util::Error(bad_val_msg); }
  if (color.blue  < 0.0 || color.blue  > 1.0) { throw Util::Error(bad_val_msg); }

  setColor(color.pixel, color.red, color.green, color.blue);
}

Togl::Color Togl::Impl::queryColor(unsigned int color_index) const
{
  Togl::Color col;
  queryColor(color_index, col);
  return col;
}

void Togl::Impl::queryColor(unsigned int color_index, Color& color) const
{
DOTRACE("Togl::Impl::queryColor");

  Colormap cmap = Tk_Colormap(itsTkWin);
  XColor col;

  col.pixel = color_index;
  XQueryColor(itsDisplay, cmap, &col);

  color.pixel = (unsigned int)col.pixel;
#ifdef HAVE_LIMITS
  const unsigned short usmax = std::numeric_limits<unsigned short>::max();
#else
  const unsigned short usmax = USHRT_MAX;
#endif

  color.red   = double(col.red)   / usmax;
  color.green = double(col.green) / usmax;
  color.blue  = double(col.blue)  / usmax;
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

void Togl::Impl::eventProc(XEvent* eventPtr)
{
DOTRACE("Togl::Impl::eventProc");

  switch (eventPtr->type)
    {
    case Expose:
      {
        DOTRACE("Togl::Impl::eventProc-Expose");
        if (eventPtr->xexpose.count == 0)
          {
            requestRedisplay();
          }
      }
      break;
    case ConfigureNotify:
      {
        DOTRACE("Togl::Impl::eventProc-ConfigureNotify");
        requestReconfigure();
      }
      break;
    default:
      /*nothing*/
      ;
    }
}

void Togl::Impl::makeWindowExist()
{
DOTRACE("Togl::Impl::makeWindowExist");

  TkUtil::destroyWindow(itsTkWin);

  int dummy;
  if (!glXQueryExtension(itsDisplay, &dummy, &dummy))
    {
      throw Util::Error("Togl: X server has no OpenGL GLX extension");
    }

  itsGlx.reset(GlxWrapper::make(itsDisplay, itsOpts->glx));

  Colormap cmap =
    X11Util::findColormap(itsDisplay, itsGlx->visInfo(),
                          itsOpts->glx.rgbaFlag, itsOpts->privateCmapFlag);

  TkUtil::createWindow(itsTkWin, itsGlx->visInfo(),
                       itsOpts->width, itsOpts->height, cmap);

  if (!itsOpts->glx.rgbaFlag)
    {
      X11Util::hackInstallColormap(itsDisplay, windowId(), cmap);
    }

  Tk_MapWindow(itsTkWin);

  // Bind the context to the window and make it the current context
  itsGlx->makeCurrent(windowId());

  // Check for a single/double buffering snafu
  if (itsOpts->glx.doubleFlag == 0 && itsGlx->isDoubleBuffered())
    {
      // We requested single buffering but had to accept a double buffered
      // visual.  Set the GL draw buffer to be the front buffer to
      // simulate single buffering.
      glDrawBuffer( GL_FRONT );
    }
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
}

void Togl::reshapeCallback()
{
DOTRACE("Togl::reshapeCallback");

  glViewport(0, 0, rep->itsOpts->width, rep->itsOpts->height);
}

void Togl::timerCallback()
{
DOTRACE("Togl::timerCallback");
}

Tcl_Obj* Togl::cget(Tcl_Obj* param) const
  { return rep->cget(param); }

void Togl::configure(int objc, Tcl_Obj* const objv[])
  { rep->configure(objc, objv); }

void Togl::makeCurrent() const          { rep->itsGlx->makeCurrent(rep->windowId()); }
void Togl::requestRedisplay()           { rep->requestRedisplay(); }
void Togl::requestReconfigure()         { rep->requestReconfigure(); }
void Togl::swapBuffers()                { rep->swapBuffers(); }
int Togl::width() const                 { return rep->itsOpts->width; }
int Togl::height() const                { return rep->itsOpts->height; }
bool Togl::isRgba() const               { return rep->itsOpts->glx.rgbaFlag; }
bool Togl::isDoubleBuffered() const     { return rep->itsOpts->glx.doubleFlag; }
unsigned int Togl::bitsPerPixel() const { return rep->itsGlx->visInfo()->depth; }
bool Togl::hasPrivateCmap() const       { return rep->itsOpts->privateCmapFlag; }

unsigned long Togl::allocColor( float red, float green, float blue ) const
  { return rep->allocColor(red, green, blue); }

void Togl::freeColor( unsigned long pixel ) const
  { rep->freeColor(pixel); }

void Togl::setColor(unsigned long index,
                    float red, float green, float blue) const
  { rep->setColor(index, red, green, blue); }

void Togl::setColor(const Togl::Color& color) const
  { rep->setColor(color); }

Togl::Color Togl::queryColor(unsigned int color_index) const
  { return rep->queryColor(color_index); }

void Togl::queryColor(unsigned int color_index, Color& color) const
  { rep->queryColor(color_index, color); }

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


void Togl::setWidth(int w)
{
DOTRACE("Togl::setWidth");
  rep->itsOpts->width = w;
  Tk_GeometryRequest(rep->itsTkWin, rep->itsOpts->width, rep->itsOpts->height);
}

void Togl::setHeight(int h)
{
DOTRACE("Togl::setHeight");
  rep->itsOpts->height = h;
  Tk_GeometryRequest(rep->itsTkWin, rep->itsOpts->width, rep->itsOpts->height);
}

Gfx::Canvas& Togl::getCanvas() const
{
DOTRACE("Togl::getCanvas");
  makeCurrent();
  return rep->itsGlx->canvas();
}

static const char vcid_togl_cc[] = "$Header$";
#endif // !TOGL_CC_DEFINED
