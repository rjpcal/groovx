///////////////////////////////////////////////////////////////////////
//
// togl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 23 13:11:59 2000
// written: Mon Sep 16 17:18:18 2002
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

#include "togl/glutil.h"
#include "togl/glxopts.h"
#include "togl/glxoverlay.h"
#include "togl/glxwrapper.h"
#include "togl/tkutil.h"
#include "togl/x11util.h"

#include "util/error.h"
#include "util/pointers.h"
#include "util/ref.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include <tcl.h>
#include <tk.h>

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
    width = 0;
    height = 0;
    time = 0;
    privateCmapFlag = 0;
    overlayFlag = 0;
    glx.toDefaults();
  }

  int width;
  int height;
  int time;
  int privateCmapFlag;
  int overlayFlag;
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
  const char*  DEFAULT_WIDTH     = "400";
  const char*  DEFAULT_HEIGHT    = "400";
  const char*  DEFAULT_TIME      = "1";

  Togl::Callback* DefaultCreateProc = NULL;
  Togl::Callback* DefaultDisplayProc = NULL;
  Togl::Callback* DefaultReshapeProc = NULL;
  Togl::Callback* DefaultDestroyProc = NULL;
  Togl::Callback* DefaultTimerProc = NULL;
  ClientData DefaultClientData = NULL;

  // Tk option database machinery for Togl:

  const int TOGL_GLX_OPTION     = 1 << 0;
  const int TOGL_OVERLAY_OPTION = 1 << 1;
  const int TOGL_GEOM_OPTION    = 1 << 2;

  Tk_OptionTable toglOptionTable = 0;

  Tk_OptionSpec optionSpecs[] =
  {
    {TK_OPTION_PIXELS, (char*)"-width", (char*)"width", (char*)"Width",
     (char*)DEFAULT_WIDTH, -1, Tk_Offset(ToglOpts, width), 0, NULL, TOGL_GEOM_OPTION},

    {TK_OPTION_PIXELS, (char*)"-height", (char*)"height", (char*)"Height",
     (char*)DEFAULT_HEIGHT, -1, Tk_Offset(ToglOpts, height), 0, NULL, TOGL_GEOM_OPTION},

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

    {TK_OPTION_BOOLEAN, (char*)"-overlay", (char*)"overlay", (char*)"Overlay",
     (char*)"false", -1, Tk_Offset(ToglOpts, overlayFlag), 0, NULL, TOGL_OVERLAY_OPTION},

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
  Tk_Window itsTkWin;
  Display* itsDisplay;
  shared_ptr<ToglOpts> itsOpts;
  shared_ptr<GlxWrapper> itsGlx;

  bool itsUpdatePending;
  bool itsShutdownRequested;
  ClientData itsClientData;
  Togl::Callback* itsUserDisplayProc;
  Togl::Callback* itsUserReshapeProc;
  Togl::Callback* itsUserDestroyProc;
  Togl::Callback* itsUserTimerProc;
  Tcl_TimerToken itsTimerToken;

  GlxOverlay* itsOverlay;

  Impl(Togl* owner, Tcl_Interp* interp, const char* pathname);
  ~Impl() throw();

  int configure(int objc, Tcl_Obj* const objv[]);

  int handleConfigure(Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]);

  // All callbacks cast to/from Togl::Impl*, _NOT_ Togl* !!!
  static void cEventuallyFreeCallback(char* clientData);
  static void cEventCallback(ClientData clientData, XEvent* eventPtr);
  static void cTimerCallback(ClientData clientData);
  static void cRenderCallback(ClientData clientData);

  void setDisplayFunc(Togl::Callback* proc) { itsUserDisplayProc = proc; }
  void setReshapeFunc(Togl::Callback* proc) { itsUserReshapeProc = proc; }
  void setDestroyFunc(Togl::Callback* proc) { itsUserDestroyProc = proc; }

  void requestRedisplay();
  void requestReconfigure();
  void swapBuffers() const;

  void ensureSharedColormap(const char* what) const;

  unsigned long allocColor(float red, float green, float blue) const;
  void freeColor(unsigned long pixel) const;
  void setColor(unsigned long index, float red, float green, float blue) const;

  void useLayer(Togl::Layer layer);

  Window windowId() const { return Tk_WindowId(itsTkWin); }

  Gfx::Canvas& canvas() const { return itsGlx->canvas(); }

private:
  void eventProc(XEvent* eventPtr);
  void makeWindowExist(); // throws a Util::Error on failure
  void setupOverlay(); // throws a Util::Error on failure
  void requestShutdown() throw();
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

namespace
{
  class TkWinGuard
  {
  private:
    Tk_Window tkWin;
    bool dismissed;

  public:
    TkWinGuard(Tk_Window w) : tkWin(w), dismissed(false) {}
    ~TkWinGuard() { if (!dismissed) Tk_DestroyWindow(tkWin); }
    void dismiss() { dismissed = true; }
  };
}

Togl::Impl::Impl(Togl* owner, Tcl_Interp* interp, const char* pathname) :
  itsOwner(owner),
  itsInterp(interp),
  itsTkWin(0),
  itsDisplay(0),
  itsOpts(new ToglOpts),
  itsGlx(0),

  itsUpdatePending(false),
  itsShutdownRequested(false),
  itsClientData(DefaultClientData),
  itsUserDisplayProc(DefaultDisplayProc),
  itsUserReshapeProc(DefaultReshapeProc),
  itsUserDestroyProc(DefaultDestroyProc),
  itsUserTimerProc(DefaultTimerProc),
  itsTimerToken(0),

  itsOverlay(0)
{
DOTRACE("Togl::Impl::Impl");

  //
  // Create the window
  //

  itsTkWin = Tk_CreateWindowFromPath(interp, Tk_MainWindow(interp),
                                     const_cast<char*>(pathname),
                                     (char *) 0);

  if (itsTkWin == 0)
    {
      throw Util::Error("Togl constructor couldn't create Tk_Window");
    }

  TkWinGuard guard(itsTkWin); // destroys the widget in case of exception

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

  if (itsUserTimerProc)
    {
      itsTimerToken =
        Tcl_CreateTimerHandler( itsOpts->time, &cTimerCallback,
                                static_cast<ClientData>(this) );
    }

  if (DefaultCreateProc)
    {
      DefaultCreateProc(itsOwner);
    }

  guard.dismiss();

  Tcl_AppendResult(itsInterp, Tk_PathName(itsTkWin), NULL);

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

  if (itsUserDestroyProc)
    {
      itsUserDestroyProc(itsOwner);
    }

  Tcl_DeleteTimerHandler(itsTimerToken);

  Tcl_CancelIdleCall(cRenderCallback, static_cast<ClientData>(this));

  if (itsOverlay)
    {
      TkUtil::forgetWindow(itsTkWin, itsOverlay->windowId());
      delete itsOverlay;
    }

  Tk_FreeConfigOptions(reinterpret_cast<char*>(itsOpts.get()),
                       toglOptionTable, itsTkWin);

  Tk_DestroyWindow(itsTkWin);
}

//---------------------------------------------------------------------
//
// Process a configure read- and/or write-request
//
//---------------------------------------------------------------------

int Togl::Impl::handleConfigure(Tcl_Interp* interp,
                                int objc, Tcl_Obj* const objv[])
{
DOTRACE("Togl::Impl::handleConfigure");

  int result = TCL_OK;

  if (objc == 0)
    {
      /* Return list of all configuration parameters */
      Tcl_Obj* objResult =
        Tk_GetOptionInfo(interp,
                         reinterpret_cast<char*>(itsOpts.get()),
                         toglOptionTable,
                         (Tcl_Obj*)NULL, itsTkWin);

      if (objResult != 0) Tcl_SetObjResult(interp, objResult);
      else                result = TCL_ERROR;
    }
  else if (objc == 1)
    {
      /* Return a specific configuration parameter */
      Tcl_Obj* objResult =
        Tk_GetOptionInfo(interp,
                         reinterpret_cast<char*>(itsOpts.get()),
                         toglOptionTable,
                         objv[0], itsTkWin);

      if (objResult != 0) Tcl_SetObjResult(interp, objResult);
      else                result = TCL_ERROR;
    }
  else
    {
      /* Execute a configuration change */
      result = configure(objc, objv);
    }

  return result;
}

//---------------------------------------------------------------------
//
// It's possible to change with this function or in a script some
// options like RGBA - ColorIndex ; Z-buffer and so on
//
//---------------------------------------------------------------------

int Togl::Impl::configure(int objc, Tcl_Obj* const objv[])
{
DOTRACE("Togl::Impl::configure");

  int mask = 0;

  if (Tk_SetOptions(itsInterp, reinterpret_cast<char*>(itsOpts.get()),
                    toglOptionTable, objc, objv, itsTkWin,
                    (Tk_SavedOptions*) 0, &mask)
      == TCL_ERROR)
    {
      return TCL_ERROR;
    }

  try
    {
      // If any GLX options changed, then we have to recreate the window
      // and GLX context
      if (mask & TOGL_GLX_OPTION)
        makeWindowExist();
      else
        {
          if (mask & TOGL_GEOM_OPTION)
            Tk_GeometryRequest(itsTkWin, itsOpts->width, itsOpts->height);

          if (mask & TOGL_OVERLAY_OPTION)
            setupOverlay();
        }
    }
  catch (Util::Error& err)
    {
      Tcl_AppendResult(itsInterp, err.msg_cstr(), NULL);
      return TCL_ERROR;
    }

  return TCL_OK;
}

// Gets called when an Togl widget is destroyed.
void Togl::Impl::cEventuallyFreeCallback(char* clientData)
{
DOTRACE("Togl::Impl::cEventuallyFreeCallback");
  Impl* rep = reinterpret_cast<Impl*>(clientData);
  delete rep->itsOwner;
}

void Togl::Impl::cEventCallback(ClientData clientData, XEvent* eventPtr)
{
  Impl* rep = static_cast<Impl*>(clientData);
  Tcl_Preserve(clientData);
  rep->eventProc(eventPtr);
  Tcl_Release(clientData);
}

void Togl::Impl::cTimerCallback(ClientData clientData)
{
DOTRACE("Togl::Impl::cTimerCallback");
  Impl* rep = static_cast<Impl*>(clientData);
  Tcl_Preserve(clientData);
  if (rep->itsUserTimerProc == 0)
    {
      rep->itsTimerToken = 0;
    }
  else
    {
      rep->itsUserTimerProc(rep->itsOwner);
      rep->itsTimerToken =
        Tcl_CreateTimerHandler(rep->itsOpts->time, cTimerCallback,
                               static_cast<ClientData>(rep));
    }
  Tcl_Release(clientData);
}

// Called when the widget's contents must be redrawn.
void Togl::Impl::cRenderCallback(ClientData clientData)
{
DOTRACE("Togl::Impl::cRenderCallback");
  Impl* rep = static_cast<Impl*>(clientData);

  Tcl_Preserve(clientData);
  if (rep->itsUserDisplayProc)
    {
      rep->itsGlx->makeCurrent(rep->windowId());
      rep->itsUserDisplayProc(rep->itsOwner);
    }
  rep->itsUpdatePending = false;
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
  if (itsOpts->width != Tk_Width(itsTkWin) || itsOpts->height != Tk_Height(itsTkWin))
    {
      itsOpts->width = Tk_Width(itsTkWin);
      itsOpts->height = Tk_Height(itsTkWin);
      XResizeWindow(itsDisplay, windowId(), itsOpts->width, itsOpts->height);

      if (itsOverlay)
        itsOverlay->reconfigure(itsOpts->width, itsOpts->height);

      itsGlx->makeCurrent(windowId());
    }

  if (itsUserReshapeProc)
    {
      itsUserReshapeProc(itsOwner);
    }
  else
    {
      glViewport(0, 0, itsOpts->width, itsOpts->height);
      if (itsOpts->overlayFlag)
        {
          useLayer( Overlay );
          glViewport( 0, 0, itsOpts->width, itsOpts->height );
          useLayer( Normal );
        }
    }
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

void Togl::Impl::useLayer(Togl::Layer layer)
{
DOTRACE("Togl::Impl::useLayer");
  if (itsOverlay)
    {
      switch(layer)
        {
        case Normal:  itsGlx->makeCurrent(windowId()); break;
        case Overlay: itsOverlay->makeCurrent(); break;
        default:      Assert(0); break;
        }
    }
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
            if (eventPtr->xexpose.window==windowId())
              {
                requestRedisplay();
              }
            if (itsOverlay
                && eventPtr->xexpose.window==itsOverlay->windowId())
              {
                itsOverlay->requestRedisplay();
              }
          }
      }
      break;
    case ConfigureNotify:
      {
        DOTRACE("Togl::Impl::eventProc-ConfigureNotify");
        requestReconfigure();
      }
      break;
    case MapNotify:
      {
        DOTRACE("Togl::Impl::eventProc-MapNotify");
      }
      break;
    case DestroyNotify:
      {
        DOTRACE("Togl::Impl::eventProc-DestroyNotify");
        requestShutdown();
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

  setupOverlay();

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

void Togl::Impl::setupOverlay()
{
DOTRACE("Togl::Impl::setupOverlay");

  // We need to destroy the old overlay regardless of whether we will be
  // turning overlay on or off
  if (itsOverlay) { delete itsOverlay; itsOverlay = 0; }

  if (!itsOpts->overlayFlag)
    return;

  itsOverlay = new GlxOverlay(itsDisplay, this->windowId(),
                              !itsOpts->glx.indirect, itsGlx.get(),
                              itsOpts->width, itsOpts->height);

  Assert(itsOverlay != 0);

  TkUtil::addWindow(itsTkWin, itsOverlay->windowId());
}

void Togl::Impl::requestShutdown() throw()
{
  // If we end up calling ourselves recursively here (which is like to
  // happen via a chain of DestroyNotify and "command deleted" callbacks),
  // then just return without doing anything.
  if (itsShutdownRequested)
    return;

DOTRACE("Togl::Impl::requestShutdown");

  itsShutdownRequested = true;

  Tcl_EventuallyFree(static_cast<ClientData>(this), cEventuallyFreeCallback);
}

///////////////////////////////////////////////////////////////////////
//
// Togl member function definitions
//
///////////////////////////////////////////////////////////////////////

Togl::Togl(Tcl_Interp* interp, const char* pathname) :
  rep(new Impl(this, interp, pathname))
{
DOTRACE("Togl::Togl");
}

Togl::~Togl()
{
DOTRACE("Togl::~Togl");
  delete rep;
}


void Togl::setDefaultClientData(ClientData p)       { DefaultClientData = p; }
void Togl::setDefaultCreateFunc(Togl::Callback* p)  { DefaultCreateProc = p; }
void Togl::setDefaultDisplayFunc(Togl::Callback* p) { DefaultDisplayProc = p; }
void Togl::setDefaultReshapeFunc(Togl::Callback* p) { DefaultReshapeProc = p; }
void Togl::setDefaultDestroyFunc(Togl::Callback* p) { DefaultDestroyProc = p; }
void Togl::setDefaultTimerFunc(Togl::Callback* p)   { DefaultTimerProc = p; }

void Togl::resetDefaultCallbacks()
{
DOTRACE("Togl::resetDefaultCallbacks");
  DefaultCreateProc = 0;
  DefaultDisplayProc = 0;
  DefaultReshapeProc = 0;
  DefaultDestroyProc = 0;
  DefaultTimerProc = 0;
  DefaultClientData = 0;
}

void Togl::setDisplayFunc(Togl::Callback* proc) { rep->setDisplayFunc(proc); }
void Togl::setReshapeFunc(Togl::Callback* proc) { rep->setReshapeFunc(proc); }
void Togl::setDestroyFunc(Togl::Callback* proc) { rep->setDestroyFunc(proc); }

int Togl::configure(int objc, Tcl_Obj* const objv[])
  { return rep->configure(objc, objv); }

int Togl::handleConfigure(Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
  { return rep->handleConfigure(interp, objc, objv); }

void Togl::makeCurrent() const          { rep->itsGlx->makeCurrent(windowId()); }
void Togl::requestRedisplay()           { rep->requestRedisplay(); }
void Togl::requestReconfigure()         { rep->requestReconfigure(); }
void Togl::swapBuffers() const          { rep->swapBuffers(); }
int Togl::width() const                 { return rep->itsOpts->width; }
int Togl::height() const                { return rep->itsOpts->height; }
bool Togl::isRgba() const               { return rep->itsOpts->glx.rgbaFlag; }
bool Togl::isDoubleBuffered() const     { return rep->itsOpts->glx.doubleFlag; }
unsigned int Togl::bitsPerPixel() const { return rep->itsGlx->visInfo()->depth; }
bool Togl::hasPrivateCmap() const       { return rep->itsOpts->privateCmapFlag; }
Tcl_Interp* Togl::interp() const        { return rep->itsInterp; }
Tk_Window Togl::tkWin() const           { return rep->itsTkWin; }
const char* Togl::pathname() const      { return Tk_PathName(rep->itsTkWin); }

unsigned long Togl::allocColor( float red, float green, float blue ) const
  { return rep->allocColor(red, green, blue); }

void Togl::freeColor( unsigned long pixel ) const
  { rep->freeColor(pixel); }

void Togl::setColor( unsigned long index,
                     float red, float green, float blue ) const
  { rep->setColor(index, red, green, blue); }

GLuint Togl::loadBitmapFont( const char *fontname ) const
  { return GLUtil::loadBitmapFont(Tk_Display(rep->itsTkWin), fontname); }

GLuint Togl::loadBitmapFonti( int fontnumber ) const
  { return GLUtil::loadBitmapFont(Tk_Display(rep->itsTkWin),
                                  GLUtil::NamedFont(fontnumber)); }

void Togl::unloadBitmapFont( GLuint fontbase ) const
  { GLUtil::unloadBitmapFont(fontbase); }


void Togl::overlayDisplayFunc(Togl::OverlayCallback* proc)
{
DOTRACE("Togl::OverlayDisplayFunc");
  if (rep->itsOverlay)
    rep->itsOverlay->setDisplayProc(proc, static_cast<void*>(this));
}

void Togl::useLayer(Togl::Layer layer) { rep->useLayer(layer); }

void Togl::showOverlay() { if (rep->itsOverlay) rep->itsOverlay->show(); }
void Togl::hideOverlay() { if (rep->itsOverlay) rep->itsOverlay->hide(); }

void Togl::requestOverlayRedisplay()
  { if (rep->itsOverlay) rep->itsOverlay->requestRedisplay(); }

int Togl::existsOverlay() const
{
DOTRACE("Togl::existsOverlay");
  return rep->itsOpts->overlayFlag;
}

int Togl::getOverlayTransparentValue() const
{
DOTRACE("Togl::getOverlayTransparentValue");
  return rep->itsOverlay ? rep->itsOverlay->transparentPixel() : -1;
}

bool Togl::isMappedOverlay() const
{
DOTRACE("Togl::isMappedOverlay");
  return (rep->itsOverlay && rep->itsOverlay->isMapped());
}

unsigned long Togl::allocColorOverlay( float red, float green,
                                       float blue ) const
{
DOTRACE("Togl::allocColorOverlay");
  return rep->itsOverlay
    ? rep->itsOverlay->allocColor(red, green, blue)
    : (unsigned long) -1;
}

void Togl::freeColorOverlay( unsigned long pixel ) const
{
DOTRACE("Togl::freeColorOverlay");
  if (rep->itsOverlay)
    rep->itsOverlay->freeColor(pixel);
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

ClientData Togl::getClientData() const { return rep->itsClientData; }
void Togl::setClientData(ClientData p) { rep->itsClientData = p; }

Display* Togl::display() const  { return Tk_Display(rep->itsTkWin); }
Screen* Togl::screen() const    { return Tk_Screen(rep->itsTkWin); }
int Togl::screenNumber() const  { return Tk_ScreenNumber(rep->itsTkWin); }
Colormap Togl::colormap() const { return Tk_Colormap(rep->itsTkWin); }
Window Togl::windowId() const   { return Tk_WindowId(rep->itsTkWin); }

Gfx::Canvas& Togl::getCanvas() const { return rep->itsGlx->canvas(); }

static const char vcid_togl_cc[] = "$Header$";
#endif // !TOGL_CC_DEFINED
