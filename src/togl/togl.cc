///////////////////////////////////////////////////////////////////////
//
// togl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 23 13:11:59 2000
// written: Mon Aug  5 19:01:44 2002
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

#include "togl/glutil.h"
#include "togl/glxopts.h"
#include "togl/glxoverlay.h"
#include "togl/glxwrapper.h"
#include "togl/tkutil.h"
#include "togl/x11util.h"

#include "util/error.h"
#include "util/pointers.h"

#include <cstring>

// X Window System headers
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <GL/glx.h>

// Tcl/Tk headers
#ifndef _TKPORT
#  define _TKPORT  // This eliminates need to include a bunch of Tk baggage
#endif
#include <tcl.h>
#include <tk.h>
#include <tkInt.h> // needed to access "dispPtr->winTable" field of TkWindow

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
    cursor = None;
    glx.toDefaults();
  }

  int width;
  int height;
  int time;
  int privateCmapFlag;
  int overlayFlag;
  Tk_Cursor cursor;          /* The widget's cursor */
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
  const char*  DEFAULT_IDENT     = "";
  const char*  DEFAULT_FONTNAME  = "fixed";
  const char*  DEFAULT_TIME      = "1";

  Togl_Callback* DefaultCreateProc = NULL;
  Togl_Callback* DefaultDisplayProc = NULL;
  Togl_Callback* DefaultReshapeProc = NULL;
  Togl_Callback* DefaultDestroyProc = NULL;
  Togl_Callback* DefaultTimerProc = NULL;
  ClientData DefaultClientData = NULL;

  int Togl_WidgetCmd(ClientData clientData, Tcl_Interp *interp,
                     int objc, Tcl_Obj* const objv[]);

  // Tk option database machinery for Togl:

  const int TOGL_GLX_OPTION = 0x1;

  Tk_OptionTable toglOptionTable = 0;

  Tk_OptionSpec optionSpecs[] =
  {
    {TK_OPTION_PIXELS, (char*)"-width", (char*)"width", (char*)"Width",
     (char*)DEFAULT_WIDTH, -1, Tk_Offset(ToglOpts, width), 0, NULL, 0},

    {TK_OPTION_PIXELS, (char*)"-height", (char*)"height", (char*)"Height",
     (char*)DEFAULT_HEIGHT, -1, Tk_Offset(ToglOpts, height), 0, NULL, 0},

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
     (char*)"false", -1, Tk_Offset(ToglOpts, overlayFlag), 0, NULL, 0},

    { TK_OPTION_CURSOR, (char*)"-cursor", (char*)"cursor", (char*)"Cursor",
      (char*)"", -1, Tk_Offset(ToglOpts, cursor), TK_OPTION_NULL_OK, NULL , 0},

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
  ToglOpts itsOpts;
  shared_ptr<GlxWrapper> itsGlx;
  Tcl_Command itsWidgetCmd;

  bool itsUpdatePending;
  ClientData itsClientData;
  Togl_Callback* itsUserDisplayProc;
  Togl_Callback* itsUserReshapeProc;
  Togl_Callback* itsUserDestroyProc;
  Togl_Callback* itsUserTimerProc;
  Tcl_TimerToken itsTimerToken;

  GlxOverlay* itsOverlay;


  Impl(Togl* owner, Tcl_Interp* interp, const char* pathname);
  ~Impl();

  int configure(int objc, Tcl_Obj* const objv[]);

  // All callbacks cast to/from Togl::Impl*, _NOT_ Togl* !!!
  static void cDestroyCallback(char* clientData);
  static void cEventCallback(ClientData clientData, XEvent* eventPtr);
  static void cWidgetCmdDeletedCallback(ClientData clientData);
  static void cTimerCallback(ClientData clientData);
  static void cRenderCallback(ClientData clientData);

  void setDisplayFunc(Togl_Callback* proc) { itsUserDisplayProc = proc; }
  void setReshapeFunc(Togl_Callback* proc) { itsUserReshapeProc = proc; }
  void setDestroyFunc(Togl_Callback* proc) { itsUserDestroyProc = proc; }

  void requestRedisplay();
  void requestReconfigure();
  void swapBuffers() const;

  void ensureSharedColormap(const char* what) const;

  unsigned long allocColor(float red, float green, float blue) const;
  void freeColor(unsigned long pixel) const;
  void setColor(unsigned long index, float red, float green, float blue) const;

  void useLayer(int layer);

  Window windowId() const { return Tk_WindowId(itsTkWin); }

private:
  void eventProc(XEvent* eventPtr);
  void widgetCmdDeletedProc();
  void makeWindowExist(); // throws a Util::Error on failure
  void setupOverlay(); // throws a Util::Error on failure
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
  itsOpts(),
  itsGlx(0),
  itsWidgetCmd(0),

  itsUpdatePending(false),
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

  if (itsTkWin == NULL)
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

  itsOpts.toDefaults();

  if (Tk_InitOptions(interp, reinterpret_cast<char*>(&itsOpts),
                     toglOptionTable, itsTkWin) == TCL_ERROR)
    {
      throw Util::Error(fstring("Togl couldn't initialize options:\n",
                                Tcl_GetStringResult(itsInterp)));
    }

  //
  // Get the window mapped onscreen
  //

  Tk_GeometryRequest(itsTkWin, itsOpts.width, itsOpts.height);

  Assert(itsGlx.get() == 0);

  makeWindowExist();

  //
  // Set up handlers
  //

  itsWidgetCmd = Tcl_CreateObjCommand(itsInterp, Tk_PathName(itsTkWin),
                                      Togl_WidgetCmd,
                                      static_cast<ClientData>(this),
                                      &cWidgetCmdDeletedCallback);

  Tk_CreateEventHandler(itsTkWin,
                        ExposureMask | StructureNotifyMask,
                        &cEventCallback,
                        static_cast<ClientData>(this));

  if (itsUserTimerProc)
    {
      itsTimerToken =
        Tcl_CreateTimerHandler( itsOpts.time, &cTimerCallback,
                                static_cast<ClientData>(this) );
    }

  if (DefaultCreateProc)
    {
      DefaultCreateProc(itsOwner);
    }

  guard.dismiss();

  Tcl_AppendResult(itsInterp, Tk_PathName(itsTkWin), NULL);
}

Togl::Impl::~Impl()
{
DOTRACE("Togl::Impl::~Impl");

  Tk_FreeConfigOptions((char*) &itsOpts, toglOptionTable, itsTkWin);

  if (itsUserDestroyProc)
    {
      itsUserDestroyProc(itsOwner);
    }

  delete itsOverlay;
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

  if (Tk_SetOptions(itsInterp, reinterpret_cast<char *>(&itsOpts),
                    toglOptionTable, objc, objv, itsTkWin,
                    (Tk_SavedOptions*) 0, &mask)
      == TCL_ERROR)
    {
      return TCL_ERROR;
    }

  Tk_GeometryRequest(itsTkWin, itsOpts.width, itsOpts.height);

  // If any GLX options changed, then we have to recreate the window and
  // GLX context
  if (mask & TOGL_GLX_OPTION)
    {
      try
        {
          makeWindowExist();
        }
      catch (Util::Error& err)
        {
          Tcl_AppendResult(itsInterp, err.msg_cstr(), NULL);
          return TCL_ERROR;
        }
    }

  return TCL_OK;
}

// Gets called when an Togl widget is destroyed.
void Togl::Impl::cDestroyCallback(char* clientData)
{
DOTRACE("Togl::Impl::cDestroyCallback");
  Impl* impl = reinterpret_cast<Impl*>(clientData);
  delete impl->itsOwner;
}

void Togl::Impl::cEventCallback(ClientData clientData, XEvent* eventPtr)
{
  Impl* impl = static_cast<Impl*>(clientData);
  impl->eventProc(eventPtr);
}

void Togl::Impl::cWidgetCmdDeletedCallback(ClientData clientData)
{
  Impl* impl = static_cast<Impl*>(clientData);
  impl->widgetCmdDeletedProc();
}

void Togl::Impl::cTimerCallback(ClientData clientData)
{
DOTRACE("Togl::Impl::cTimerCallback");
  Impl* impl = static_cast<Impl*>(clientData);
  impl->itsUserTimerProc(impl->itsOwner);
  impl->itsTimerToken =
    Tcl_CreateTimerHandler( impl->itsOpts.time, cTimerCallback,
                            static_cast<ClientData>(impl) );
}

// Called when the widget's contents must be redrawn.
void Togl::Impl::cRenderCallback(ClientData clientData)
{
DOTRACE("Togl::Impl::cRenderCallback");
  Impl* rep = static_cast<Impl*>(clientData);

  if (rep->itsUserDisplayProc)
    {
      rep->itsGlx->makeCurrent(rep->windowId());
      rep->itsUserDisplayProc(rep->itsOwner);
    }
  rep->itsUpdatePending = false;
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
  if (itsOpts.width != Tk_Width(itsTkWin) || itsOpts.height != Tk_Height(itsTkWin))
    {
      itsOpts.width = Tk_Width(itsTkWin);
      itsOpts.height = Tk_Height(itsTkWin);
      XResizeWindow(itsDisplay, windowId(), itsOpts.width, itsOpts.height);

      if (itsOverlay)
        itsOverlay->reconfigure(itsOpts.width, itsOpts.height);

      itsGlx->makeCurrent(windowId());
    }

  if (itsUserReshapeProc)
    {
      itsUserReshapeProc(itsOwner);
    }
  else
    {
      glViewport(0, 0, itsOpts.width, itsOpts.height);
      if (itsOpts.overlayFlag)
        {
          useLayer( TOGL_OVERLAY );
          glViewport( 0, 0, itsOpts.width, itsOpts.height );
          useLayer( TOGL_NORMAL );
        }
    }
}

void Togl::Impl::swapBuffers() const
{
DOTRACE("Togl::Impl::swapBuffers");
  if (itsOpts.glx.doubleFlag)
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
  if (itsOpts.glx.rgbaFlag)
    {
      throw Util::Error(fstring(what, " not allowed in RGBA mode"));
    }

  if (itsOpts.privateCmapFlag)
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

void Togl::Impl::useLayer(int layer)
{
DOTRACE("Togl::Impl::useLayer");
  if (itsOverlay)
    {
      if (layer==TOGL_OVERLAY)
        {
          itsOverlay->makeCurrent();
        }
      else if (layer==TOGL_NORMAL)
        {
          itsGlx->makeCurrent(windowId());
        }
      else
        {
          /* error */
        }
    }
}

void Togl::Impl::eventProc(XEvent* eventPtr)
{
DOTRACE("Togl::Impl::eventProc");

  switch (eventPtr->type)
    {
    case Expose:
      DebugPrintNL("Expose");
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
      break;
    case ConfigureNotify:
      DebugPrintNL("ConfigureNotify");
      requestReconfigure();
      break;
    case MapNotify:
      DebugPrintNL("MapNotify");
      break;
    case DestroyNotify:
      DebugPrintNL("DestroyNotify");
      if (itsWidgetCmd != 0)
        {
          Tcl_DeleteCommandFromToken( itsInterp, itsWidgetCmd );
          itsWidgetCmd = 0;
        }
      if (itsUserTimerProc != 0)
        {
          Tcl_DeleteTimerHandler(itsTimerToken);
          itsUserTimerProc = 0;
        }
      if (itsUpdatePending)
        {
          Tcl_CancelIdleCall(Togl::Impl::cRenderCallback,
                             static_cast<ClientData>(this));
          itsUpdatePending = 0;
        }

      Tcl_EventuallyFree( static_cast<ClientData>(this),
                          Togl::Impl::cDestroyCallback );

      break;
    default:
      /*nothing*/
      ;
    }
}

void Togl::Impl::widgetCmdDeletedProc()
{
DOTRACE("Togl::Impl::widgetCmdDeletedProc");

  // This procedure could be invoked either because the window was
  // destroyed and the command was then deleted (in which case itsTkWin is
  // NULL) or because the command was deleted, and then this procedure
  // destroys the widget.

  if (itsTkWin != 0)
    {
      Tk_DeleteEventHandler(itsTkWin,
                            ExposureMask | StructureNotifyMask,
                            Togl::Impl::cEventCallback,
                            static_cast<ClientData>(this));

      if (itsOverlay)
        {
          TkWindow* winPtr = reinterpret_cast<TkWindow*>(itsTkWin);

          Tcl_HashEntry* entryPtr =
            Tcl_FindHashEntry(&winPtr->dispPtr->winTable,
                              (char*) itsOverlay->windowId() );

          Tcl_DeleteHashEntry(entryPtr);
        }

      Tk_DestroyWindow(itsTkWin);
      itsTkWin = 0;
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

  itsGlx.reset(GlxWrapper::make(itsDisplay, itsOpts.glx));

  Colormap cmap =
    X11Util::findColormap(itsDisplay, itsGlx->visInfo(),
                          itsOpts.glx.rgbaFlag, itsOpts.privateCmapFlag);

  TkUtil::createWindow(itsTkWin, itsGlx->visInfo(),
                       itsOpts.width, itsOpts.height, cmap);

  if (!itsOpts.glx.rgbaFlag)
    {
      X11Util::hackInstallColormap(itsDisplay, windowId(), cmap);
    }

  TkUtil::setupStackingOrder(itsTkWin);

  setupOverlay();

  // Issue a ConfigureNotify event if there were deferred changes
  TkUtil::issueConfigureNotify(itsTkWin);

  TkUtil::selectAllInput(itsTkWin);

  TkUtil::mapWindow(itsTkWin);

  // Bind the context to the window and make it the current context
  itsGlx->makeCurrent(windowId());

  // Check for a single/double buffering snafu
  if (itsOpts.glx.doubleFlag == 0 && itsGlx->isDoubleBuffered())
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

  if (!itsOpts.overlayFlag)
    return;

  if (itsOverlay) { delete itsOverlay; itsOverlay = 0; }

  itsOverlay = new GlxOverlay(itsDisplay, this->windowId(),
                              !itsOpts.glx.indirect, itsGlx.get(),
                              itsOpts.width, itsOpts.height);

  Assert(itsOverlay != 0);

  TkWindow* winPtr = reinterpret_cast<TkWindow*>(itsTkWin);

  int new_flag;
  Tcl_HashEntry* hPtr =
    Tcl_CreateHashEntry( &winPtr->dispPtr->winTable,
                         (char *) itsOverlay->windowId(), &new_flag );

  Tcl_SetHashValue( hPtr, winPtr );
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

void Togl::setDefaultCreateFunc(Togl_Callback* p) { DefaultCreateProc = p; }
void Togl::setDefaultDisplayFunc(Togl_Callback* p) { DefaultDisplayProc = p; }
void Togl::setDefaultReshapeFunc(Togl_Callback* p) { DefaultReshapeProc = p; }
void Togl::setDefaultDestroyFunc(Togl_Callback* p) { DefaultDestroyProc = p; }
void Togl::setDefaultTimerFunc(Togl_Callback* p) { DefaultTimerProc = p; }

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

void Togl::setDisplayFunc(Togl_Callback* proc) { rep->setDisplayFunc(proc); }
void Togl::setReshapeFunc(Togl_Callback* proc) { rep->setReshapeFunc(proc); }
void Togl::setDestroyFunc(Togl_Callback* proc) { rep->setDestroyFunc(proc); }

int Togl::configure(int objc, Tcl_Obj* const objv[])
  { return rep->configure(objc, objv); }

void Togl::makeCurrent() const          { rep->itsGlx->makeCurrent(windowId()); }
void Togl::requestRedisplay()           { rep->requestRedisplay(); }
void Togl::requestReconfigure()         { rep->requestReconfigure(); }
void Togl::swapBuffers() const          { rep->swapBuffers(); }
int Togl::width() const                 { return rep->itsOpts.width; }
int Togl::height() const                { return rep->itsOpts.height; }
bool Togl::isRgba() const               { return rep->itsOpts.glx.rgbaFlag; }
bool Togl::isDoubleBuffered() const     { return rep->itsOpts.glx.doubleFlag; }
unsigned int Togl::bitsPerPixel() const { return rep->itsGlx->visInfo()->depth; }
bool Togl::hasPrivateCmap() const       { return rep->itsOpts.privateCmapFlag; }
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


void Togl::overlayDisplayFunc(Togl_OverlayCallback* proc)
{
DOTRACE("Togl::OverlayDisplayFunc");
  if (rep->itsOverlay)
    rep->itsOverlay->setDisplayProc(proc, static_cast<void*>(this));
}

void Togl::useLayer( int layer )
  { rep->useLayer(layer); }

void Togl::showOverlay()
  { if (rep->itsOverlay) rep->itsOverlay->show(); }

void Togl::hideOverlay()
  { if (rep->itsOverlay) rep->itsOverlay->hide(); }

void Togl::requestOverlayRedisplay()
  { if (rep->itsOverlay) rep->itsOverlay->requestRedisplay(); }

int Togl::existsOverlay() const
{
DOTRACE("Togl::existsOverlay");
  return rep->itsOpts.overlayFlag;
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

void Togl::defaultClientData( ClientData clientData )
{
DOTRACE("Togl::defaultClientData");
  DefaultClientData = clientData;
}

ClientData Togl::getClientData() const { return rep->itsClientData; }
void Togl::setClientData(ClientData p) { rep->itsClientData = p; }

Display* Togl::display() const  { return Tk_Display(rep->itsTkWin); }
Screen* Togl::screen() const    { return Tk_Screen(rep->itsTkWin); }
int Togl::screenNumber() const  { return Tk_ScreenNumber(rep->itsTkWin); }
Colormap Togl::colormap() const { return Tk_Colormap(rep->itsTkWin); }
Window Togl::windowId() const   { return Tk_WindowId(rep->itsTkWin); }

int Togl::dumpToEpsFile( const char *filename, int inColor,
                         void (*user_redraw)( const Togl* )) const
{
DOTRACE("Togl::dumpToEpsFile");

  user_redraw(this);
  glFlush();

  return GLUtil::generateEPS(filename, inColor, width(), height());
}

///////////////////////////////////////////////////////////////////////
//
// Togl Tcl package
//
///////////////////////////////////////////////////////////////////////

namespace
{
  int Togl_WidgetCmd(ClientData clientData, Tcl_Interp *interp,
                     int objc, Tcl_Obj* const objv[])
  {
   DOTRACE("<togl.cc>::Togl_WidgetCmd");

    Togl::Impl* rep = static_cast<Togl::Impl*>(clientData);
    int result = TCL_OK;

    if (objc < 2)
      {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
                         Tcl_GetString(objv[0]), " ?options?\"", NULL);
        return TCL_ERROR;
      }

    Tk_Preserve((ClientData)rep);

    if (strcmp(Tcl_GetString(objv[1]), "configure") == 0)
      {
        if (objc == 2)
          {
            /* Return list of all configuration parameters */
            Tcl_Obj* objResult =
              Tk_GetOptionInfo(interp, (char*) &rep->itsOpts, toglOptionTable,
                               (Tcl_Obj*)NULL, rep->itsTkWin);
            if (objResult != 0)
              {
                Tcl_SetObjResult(interp, objResult);
              }
            else
              {
                result = TCL_ERROR;
              }
          }
        else if (objc == 3)
          {
            if (strcmp(Tcl_GetString(objv[2]), "-extensions") == 0)
              {
                /* Return a list of OpenGL extensions available */
                Tcl_SetResult(interp, (char*) glGetString(GL_EXTENSIONS),
                              TCL_STATIC);
                result = TCL_OK;
              }
            else
              {
                /* Return a specific configuration parameter */
                Tcl_Obj* objResult =
                  Tk_GetOptionInfo(interp, (char*) &rep->itsOpts,
                                   toglOptionTable, objv[2], rep->itsTkWin);
                if (objResult != 0)
                  {
                    Tcl_SetObjResult(interp, objResult);
                  }
                else
                  {
                    result = TCL_ERROR;
                  }
              }
          }
        else
          {
            /* Execute a configuration change */
            result = rep->configure(objc-2, objv+2);
          }
      }
    else if (strcmp(Tcl_GetString(objv[1]), "render") == 0)
      {
        /* force the widget to be redrawn */
        Togl::Impl::cRenderCallback(static_cast<ClientData>(rep));
      }
    else if (strcmp(Tcl_GetString(objv[1]), "swapbuffers") == 0)
      {
        /* force the widget to be redrawn */
        rep->swapBuffers();
      }
    else if (strcmp(Tcl_GetString(objv[1]), "makecurrent") == 0)
      {
        /* force the widget to be redrawn */
        rep->itsGlx->makeCurrent(rep->windowId());
      }
    else
      {
        Tcl_AppendResult(interp, "Togl: Unknown option: ",
                         Tcl_GetString(objv[1]), NULL);
        result = TCL_ERROR;
      }

    Tk_Release((ClientData)rep);
    return result;
  }

  int ToglCmd(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
  {
    DOTRACE("ToglCmd");
    if (objc <= 1)
      {
        Tcl_AppendResult(interp, "wrong # args: should be "
                         "\"pathName command ?options?\"", NULL);
        return TCL_ERROR;
      }

    /* Create Togl data structure */
    try
      {
        Togl* p = new Togl(interp, Tcl_GetString(objv[1]));
        p->configure(objc-2, objv+2);
      }
    catch (...)
      {
        return TCL_ERROR;
      }

    return TCL_OK;
  }
}

extern "C" int Togl_Init(Tcl_Interp *interp)
{
DOTRACE("Togl_Init");

  Tcl_PkgProvide(interp, "Togl", TOGL_VERSION);

  Tcl_CreateObjCommand(interp, "togl", ToglCmd, (ClientData) 0, NULL);

  return TCL_OK;
}

static const char vcid_togl_cc[] = "$Header$";
#endif // !TOGL_CC_DEFINED
