///////////////////////////////////////////////////////////////////////
//
// togl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 23 13:11:59 2000
// written: Mon Aug  5 14:04:09 2002
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
#include "togl/glxwrapper.h"
#include "togl/tkutil.h"
#include "togl/x11util.h"

// Standard C headers
#include <cstdio>
#include <cstdlib>
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

#include "util/algo.h"
#include "util/error.h"
#include "util/pointers.h"

#include "util/trace.h"
#include "util/debug.h"


namespace
{
  // Defaults
  const char*  DEFAULT_WIDTH     = "400";
  const char*  DEFAULT_HEIGHT    = "400";
  const char*  DEFAULT_IDENT     = "";
  const char*  DEFAULT_FONTNAME  = "fixed";
  const char*  DEFAULT_TIME      = "1";

  inline int TCL_ERR(Tcl_Interp* interp, const char* msg)
    {
      Tcl_ResetResult(interp);
      Tcl_AppendResult(interp, msg, NULL);
      return TCL_ERROR;
    }

  const Togl::Impl* currentImpl = 0;
}

struct ToglOpts
{
  void toDefaults()
  {
    width = 0;
    height = 0;
    time = 0;
    privateCmapFlag = 0;
    overlayFlag = 0;
#ifndef NO_TK_CURSOR
    cursor = None;
#endif
    glx.toDefaults();
  }

  int width;
  int height;
  int time;
  int privateCmapFlag;
  int overlayFlag;
#ifndef NO_TK_CURSOR
  Tk_Cursor cursor;          /* The widget's cursor */
#endif
  GlxOpts glx;
};

class Togl::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(Togl* owner, Tcl_Interp* interp, const char* pathname);
  ~Impl();

  int configure(int objc, Tcl_Obj* const objv[]);

  void makeCurrent() const;

  // All callbacks cast to/from Togl::Impl*, _NOT_ Togl* !!!
  static void dummyDestroyProc(char* clientData);
  static void dummyEventProc(ClientData clientData, XEvent* eventPtr);
  static void dummyWidgetCmdDeletedProc(ClientData clientData);
  static void dummyTimerCallback(ClientData clientData);
  static void dummyRenderCallback(ClientData clientData);
  static void dummyOverlayRenderCallback(ClientData clientData);

  void setDisplayFunc(Togl_Callback* proc) { itsDisplayProc = proc; }
  void setReshapeFunc(Togl_Callback* proc) { itsReshapeProc = proc; }
  void setDestroyFunc(Togl_Callback* proc) { itsDestroyProc = proc; }

  void requestRedisplay()
  {
    if (!itsUpdatePending)
      {
        Tk_DoWhenIdle( dummyRenderCallback, static_cast<ClientData>(this) );
        itsUpdatePending = true;
      }
  }

  void requestReconfigure();

  void swapBuffers() const
  {
    if (itsOpts.glx.doubleFlag)
      {
        glXSwapBuffers( itsDisplay, windowId() );
      }
    else
      {
        glFlush();
      }
  }

  int width() const { return itsOpts.width; }
  int height() const { return itsOpts.height; }
  bool isRgba() const { return itsOpts.glx.rgbaFlag; }
  bool isDoubleBuffered() const { return itsOpts.glx.doubleFlag; }
  unsigned int bitsPerPixel() const { return itsBitsPerPixel; }
  bool hasPrivateCmap() const { return itsOpts.privateCmapFlag; }
  Tcl_Interp* interp() const { return itsInterp; }
  Tk_Window tkWin() const { return itsTkWin; }
  const char* pathname() const
  { return Tk_PathName(reinterpret_cast<Tk_FakeWin*>(itsTkWin)); }

  bool ensureSharedColormap(const char* where) const
  {
    if (itsOpts.glx.rgbaFlag)
      {
        fprintf(stderr, "Error: %s illegal in RGBA mode.\n", where);
        return false;
      }

    if (itsOpts.privateCmapFlag)
      {
        fprintf(stderr, "Error: %s illegal with private colormap\n", where);
        return false;
      }
  }

  unsigned long allocColor(float red, float green, float blue) const;
  void freeColor(unsigned long pixel) const;
  void setColor(unsigned long index,
                float red, float green, float blue) const;

  void useLayer(int layer);
  void showOverlay();
  void hideOverlay();
  void requestOverlayRedisplay();
  int existsOverlay() const { return itsOpts.overlayFlag; }
  int getOverlayTransparentValue() const { return itsOverlayTransparentPixel; }
  int isMappedOverlay() const { return (itsOpts.overlayFlag && itsOverlayIsMapped); }
  unsigned long allocColorOverlay(float red, float green, float blue) const;
  void freeColorOverlay(unsigned long pixel) const;

  ClientData getClientData() const { return itsClientData; }
  void setClientData(ClientData clientData) { itsClientData = clientData; }

  Display* display() const { return itsDisplay; }
  Screen* screen() const { return Tk_Screen(itsTkWin); }
  int screenNumber() const { return Tk_ScreenNumber(itsTkWin); }
  Colormap colormap() const { return Tk_Colormap(itsTkWin); }
  Window windowId() const { return Tk_WindowId(itsTkWin); }

  int dumpToEpsFile(const char* filename, int inColor,
                    void (*user_redraw)( const Togl* )) const;

private:
  void eventProc(XEvent* eventPtr);
  void widgetCmdDeletedProc();
  int makeWindowExist();
  bool checkForGLX();
  void setupOverlayIfNeeded();
  int setupOverlay();

public:

  Togl* itsOwner;

  shared_ptr<GlxWrapper> itsGlx; /* Normal planes GLX context */
  Display* itsDisplay;          /* X's token for the window's display. */
  Tk_Window itsTkWin;           /* Tk window structure */
  Tcl_Interp* itsInterp;        /* Tcl interpreter */
  Tcl_Command itsWidgetCmd;     /* Token for togl's widget command */

  Tcl_TimerToken itsTimerHandler; /* Token for togl's timer handler */

  ToglOpts itsOpts;

  int itsBitsPerPixel;

  ClientData itsClientData;     /* Pointer to user data */

  bool itsUpdatePending;        /* Should normal planes be redrawn? */

  Togl_Callback* itsDisplayProc; /* Callback when widget is rendered */
  Togl_Callback* itsReshapeProc; /* Callback when window size changes */
  Togl_Callback* itsDestroyProc; /* Callback when widget is destroyed */
  Togl_Callback* itsTimerProc;  /* Callback when widget is idle */

  /* Overlay stuff */
  // FIXME move overlay stuff into a separate struct
  shared_ptr<GlxWrapper> itsOverlayGlx; /* Overlay planes OpenGL context */
  Window itsOverlayWindow;      /* The overlay window, or 0 */
  Togl_Callback* itsOverlayDisplayProc; /* Overlay redraw proc */
  bool itsOverlayUpdatePending; /* Should overlay be redrawn? */
  Colormap itsOverlayCmap;      /* colormap for overlay is created */
  int itsOverlayTransparentPixel; /* transparent pixel */
  int itsOverlayIsMapped;
};




/*
 * Setup Togl widget configuration options:
 */

namespace
{
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

#ifndef NO_TK_CURSOR
    { TK_OPTION_CURSOR, (char*)"-cursor", (char*)"cursor", (char*)"Cursor",
      (char*)"", -1, Tk_Offset(ToglOpts, cursor), TK_OPTION_NULL_OK, NULL , 0},
#endif

    {TK_OPTION_INT, (char*)"-time", (char*)"time", (char*)"Time",
     (char*)DEFAULT_TIME, -1, Tk_Offset(ToglOpts, time), 0, NULL, 0},

    {TK_OPTION_END, (char*) 0, (char*) 0, (char *) 0,
     (char*) 0, 0, 0, 0, NULL, 0}
  };
}


static Togl_Callback *DefaultCreateProc = NULL;
static Togl_Callback *DefaultDisplayProc = NULL;
static Togl_Callback *DefaultReshapeProc = NULL;
static Togl_Callback *DefaultDestroyProc = NULL;
static Togl_Callback *DefaultOverlayDisplayProc = NULL;
static Togl_Callback *DefaultTimerProc = NULL;
static ClientData DefaultClientData = NULL;



void Togl_CreateFunc( Togl_Callback *proc )
  { DefaultCreateProc = proc; }

void Togl_DisplayFunc( Togl_Callback *proc )
  { DefaultDisplayProc = proc; }

void Togl_ReshapeFunc( Togl_Callback *proc )
  { DefaultReshapeProc = proc; }

void Togl_DestroyFunc( Togl_Callback *proc )
  { DefaultDestroyProc = proc; }

void Togl_TimerFunc( Togl_Callback *proc )
  { DefaultTimerProc = proc; }


/*
 * Reset default callback pointers to NULL.
 */
void Togl_ResetDefaultCallbacks( void )
{
DOTRACE("<togl.cc>::Togl_ResetDefaultCallbacks");
  DefaultCreateProc = NULL;
  DefaultDisplayProc = NULL;
  DefaultReshapeProc = NULL;
  DefaultDestroyProc = NULL;
  DefaultOverlayDisplayProc = NULL;
  DefaultTimerProc = NULL;
  DefaultClientData = NULL;
}

void Togl::setDisplayFunc( Togl_Callback *proc )
  { rep->setDisplayFunc(proc); }

void Togl::setReshapeFunc( Togl_Callback *proc )
  { rep->setReshapeFunc(proc); }

void Togl::setDestroyFunc( Togl_Callback *proc )
  { rep->setDestroyFunc(proc); }

int Togl::configure(int objc, Tcl_Obj* const objv[])
  { return rep->configure(objc, objv); }

void Togl::makeCurrent() const
  { rep->makeCurrent(); }

void Togl::requestRedisplay()
  { rep->requestRedisplay(); }

void Togl::requestReconfigure()
  { rep->requestReconfigure(); }

void Togl::swapBuffers() const
  { rep->swapBuffers(); }

int Togl::width() const
  { return rep->width(); }

int Togl::height() const
  { return rep->height(); }

bool Togl::isRgba() const
  { return rep->isRgba(); }

bool Togl::isDoubleBuffered() const
  { return rep->isDoubleBuffered(); }

unsigned int Togl::bitsPerPixel() const
  { return rep->bitsPerPixel(); }

bool Togl::hasPrivateCmap() const
  { return rep->hasPrivateCmap(); }

Tcl_Interp* Togl::interp() const
  { return rep->interp(); }

Tk_Window Togl::tkWin() const
  { return rep->tkWin(); }

const char* Togl::pathname() const
  { return rep->pathname(); }

int Togl_WidgetCmd(ClientData clientData, Tcl_Interp *interp,
                   int objc, Tcl_Obj* const objv[])
{
DOTRACE("<togl.cc>::Togl_WidgetCmd");
  Togl::Impl* impl = static_cast<Togl::Impl*>(clientData);
  int result = TCL_OK;

  if (objc < 2)
    {
      Tcl_AppendResult(interp, "wrong # args: should be \"",
                       Tcl_GetString(objv[0]), " ?options?\"", NULL);
      return TCL_ERROR;
    }

  Tk_Preserve((ClientData)impl);

  if (strcmp(Tcl_GetString(objv[1]), "configure") == 0)
    {
      if (objc == 2)
        {
          /* Return list of all configuration parameters */
          Tcl_Obj* objResult =
            Tk_GetOptionInfo(interp, (char*) &impl->itsOpts, toglOptionTable,
                             (Tcl_Obj*)NULL, impl->tkWin());
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
          if (strcmp(Tcl_GetString(objv[2]),"-extensions")==0)
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
                Tk_GetOptionInfo(interp, (char*) &impl->itsOpts, toglOptionTable,
                                 objv[2], impl->tkWin());
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
          result = impl->configure(objc-2, objv+2);
        }
    }
  else if (strcmp(Tcl_GetString(objv[1]), "render") == 0)
    {
      /* force the widget to be redrawn */
      Togl::Impl::dummyRenderCallback(static_cast<ClientData>(impl));
    }
  else if (strcmp(Tcl_GetString(objv[1]), "swapbuffers") == 0)
    {
      /* force the widget to be redrawn */
      impl->swapBuffers();
    }
  else if (strcmp(Tcl_GetString(objv[1]), "makecurrent") == 0)
    {
      /* force the widget to be redrawn */
      impl->makeCurrent();
    }
  else
    {
      Tcl_AppendResult(interp, "Togl: Unknown option: ",
                       Tcl_GetString(objv[1]), NULL);
      result = TCL_ERROR;
    }

  Tk_Release((ClientData)impl);
  return result;
}



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


void Togl::overlayDisplayFunc( Togl_Callback *proc )
{
DOTRACE("Togl::OverlayDisplayFunc");
  DefaultOverlayDisplayProc = proc;
}

void Togl::useLayer( int layer )
  { rep->useLayer(layer); }

void Togl::showOverlay()
  { rep->showOverlay(); }

void Togl::hideOverlay()
  { rep->hideOverlay(); }

void Togl::requestOverlayRedisplay()
  { rep->requestOverlayRedisplay(); }

int Togl::existsOverlay() const
  { return rep->existsOverlay(); }

int Togl::getOverlayTransparentValue() const
  { return rep->getOverlayTransparentValue(); }

int Togl::isMappedOverlay() const
  { return rep->isMappedOverlay(); }

unsigned long Togl::allocColorOverlay( float red, float green,
                                       float blue ) const
  { return rep->allocColorOverlay(red, green, blue); }

void Togl::freeColorOverlay( unsigned long pixel ) const
  { rep->freeColorOverlay(pixel); }



/*
 * User client data
 */

void Togl::defaultClientData( ClientData clientData )
{
DOTRACE("Togl::defaultClientData");
  DefaultClientData = clientData;
}

ClientData Togl::getClientData() const
  { return rep->getClientData(); }


void Togl::setClientData( ClientData clientData )
  { rep->setClientData(clientData); }



/*
 * X11-only functions
 * Contributed by Miguel A. De Riera Pasenau (miguel@DALILA.UPC.ES)
 */

Display* Togl::display() const
  { return rep->display(); }

Screen* Togl::screen() const
  { return rep->screen(); }

int Togl::screenNumber() const
  { return rep->screenNumber(); }

Colormap Togl::colormap() const
  { return rep->colormap(); }

Window Togl::windowId() const
  { return rep->windowId(); }

int Togl::dumpToEpsFile( const char *filename, int inColor,
                         void (*user_redraw)( const Togl* )) const
  { return rep->dumpToEpsFile(filename, inColor, user_redraw); }


///////////////////////////////////////////////////////////////////////
//
// Togl member definitions
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

//     * Creates a new window
//     * Creates an 'Togl' data structure
//     * Creates an event handler for this window
//     * Creates a command that handles this object
//     * Configures this Togl for the given arguments

Togl::Impl::Impl(Togl* owner, Tcl_Interp* interp, const char* pathname) :
  itsOwner(owner),

  itsGlx(0),
  itsDisplay(0),
  itsTkWin(0),
  itsInterp(interp),
  itsWidgetCmd(0),
  itsTimerHandler(0),
  itsBitsPerPixel(-1),
  itsClientData(DefaultClientData),
  itsUpdatePending(false),
  itsDisplayProc(DefaultDisplayProc),
  itsReshapeProc(DefaultReshapeProc),
  itsDestroyProc(DefaultDestroyProc),
  itsTimerProc(DefaultTimerProc),
  itsOverlayGlx(0),
  itsOverlayWindow(0),
  itsOverlayDisplayProc(DefaultOverlayDisplayProc),
  itsOverlayUpdatePending(false),
  itsOverlayCmap(),
  itsOverlayTransparentPixel(),
  itsOverlayIsMapped(0)
{
DOTRACE("Togl::Impl::Impl");

  /* Create the window. */
  Tk_Window mainwin = Tk_MainWindow(interp);
  itsTkWin = Tk_CreateWindowFromPath(itsInterp, mainwin,
                                     const_cast<char*>(pathname),
                                     (char *) 0);
  if (itsTkWin == NULL)
    {
      throw Util::Error("Togl constructor couldn't create Tk_Window");
    }

  itsDisplay = Tk_Display( itsTkWin );

  Tk_SetClass(itsTkWin, (char*)"Togl");

  if (toglOptionTable == 0)
    {
      toglOptionTable = Tk_CreateOptionTable(interp, optionSpecs);
    }

  itsOpts.toDefaults();

  if (Tk_InitOptions(interp, reinterpret_cast<char*>(&itsOpts),
                     toglOptionTable, itsTkWin) == TCL_ERROR)
    {
      Tk_DestroyWindow(itsTkWin);
      throw Util::Error(fstring("Togl couldn't initialize options:\n",
                                Tcl_GetStringResult(itsInterp)));
    }

  Tk_GeometryRequest(itsTkWin, itsOpts.width, itsOpts.height);

  // If OpenGL window wasn't already created by configure() we
  // create it now.  We can tell by checking if the GLX context has
  // been initialized.
  if (itsGlx.get() == 0)
    {
      if (makeWindowExist() == TCL_ERROR)
        {
          Tk_DestroyWindow(itsTkWin);
          throw Util::Error("Togl constructor couldn't create window");
        }
    }

  itsWidgetCmd = Tcl_CreateObjCommand(itsInterp, Tk_PathName(itsTkWin),
                                      Togl_WidgetCmd,
                                      static_cast<ClientData>(this),
                                      Togl::Impl::dummyWidgetCmdDeletedProc);

  Tk_CreateEventHandler(itsTkWin,
                        ExposureMask | StructureNotifyMask,
                        Togl::Impl::dummyEventProc,
                        static_cast<ClientData>(this));

  if (itsTimerProc)
    {
      Tk_CreateTimerHandler( itsOpts.time, Togl::Impl::dummyTimerCallback,
                             static_cast<ClientData>(this) );
    }

  if (DefaultCreateProc)
    {
      DefaultCreateProc(itsOwner);
    }

  Tcl_AppendResult(itsInterp, Tk_PathName(itsTkWin), NULL);
}

Togl::Impl::~Impl()
{
DOTRACE("Togl::Impl::~Impl");

  Tk_FreeConfigOptions((char*) &itsOpts, toglOptionTable, itsTkWin);

  if (itsDestroyProc)
    {
      itsDestroyProc(itsOwner);
    }
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

  if (mask & TOGL_GLX_OPTION)
    {
      /* Have to recreate the window and GLX context */
      if (makeWindowExist()==TCL_ERROR)
        {
          return TCL_ERROR;
        }
    }

  return TCL_OK;
}

///////////////////////////////////////////////////////////////////////
//
// Togl::Impl::makeCurrent
//
//   Bind the OpenGL rendering context to the specified
//   Togl widget.
//
///////////////////////////////////////////////////////////////////////

void Togl::Impl::makeCurrent() const
{
DOTRACE("Togl::Impl::makeCurrent");

  if ( this != currentImpl )
    {
      DOTRACE("Togl::Impl::makeCurrent-change context");

      itsGlx->makeCurrent(windowId());
      currentImpl = this;
    }
}


// Gets called when an Togl widget is destroyed.
void Togl::Impl::dummyDestroyProc(char* clientData)
{
DOTRACE("Togl::Impl::dummyDestroyProc");
  Impl* impl = reinterpret_cast<Impl*>(clientData);
  delete impl->itsOwner;
}

void Togl::Impl::dummyEventProc(ClientData clientData, XEvent* eventPtr)
{
  Impl* impl = static_cast<Impl*>(clientData);
  impl->eventProc(eventPtr);
}

void Togl::Impl::dummyWidgetCmdDeletedProc(ClientData clientData)
{
  Impl* impl = static_cast<Impl*>(clientData);
  impl->widgetCmdDeletedProc();
}

// Gets called from Tk_CreateTimerHandler.
void Togl::Impl::dummyTimerCallback(ClientData clientData)
{
DOTRACE("Togl::Impl::dummyTimerCallback");
  Impl* impl = static_cast<Impl*>(clientData);
  impl->itsTimerProc(impl->itsOwner);
  impl->itsTimerHandler =
    Tcl_CreateTimerHandler( impl->itsOpts.time, dummyTimerCallback,
                            static_cast<ClientData>(impl) );
}

// Called when the widget's contents must be redrawn.  Basically, we
// just call the user's render callback function.
//
// Note that the parameter type is ClientData so this function can be
// passed to Tk_DoWhenIdle().
void Togl::Impl::dummyRenderCallback(ClientData clientData)
{
DOTRACE("Togl::Impl::dummyRenderCallback");
  Impl* impl = static_cast<Impl*>(clientData);

  if (impl->itsDisplayProc)
    {
      impl->makeCurrent();
      impl->itsDisplayProc(impl->itsOwner);
    }
  impl->itsUpdatePending = false;
}

void Togl::Impl::dummyOverlayRenderCallback(ClientData clientData)
{
DOTRACE("Togl::Impl::dummyOverlayRenderCallback");
  Impl* impl = static_cast<Impl*>(clientData);

  if (impl->itsOpts.overlayFlag && impl->itsOverlayDisplayProc)
    {
      impl->itsOverlayGlx->makeCurrent(impl->itsOverlayWindow);
      impl->itsOverlayDisplayProc(impl->itsOwner);
    }
  impl->itsOverlayUpdatePending = false;
}


void Togl::Impl::requestReconfigure()
{
DOTRACE("Togl::Impl::requestReconfigure");
  if (itsOpts.width != Tk_Width(itsTkWin) || itsOpts.height != Tk_Height(itsTkWin))
    {
      itsOpts.width = Tk_Width(itsTkWin);
      itsOpts.height = Tk_Height(itsTkWin);
      XResizeWindow(itsDisplay, windowId(), itsOpts.width, itsOpts.height);

      if (itsOpts.overlayFlag)
      {
        XResizeWindow( itsDisplay, itsOverlayWindow, itsOpts.width, itsOpts.height );
        XRaiseWindow( itsDisplay, itsOverlayWindow );
      }
      makeCurrent();
    }

  if (itsReshapeProc)
    {
      itsReshapeProc(itsOwner);
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

unsigned long Togl::Impl::allocColor(float red, float green, float blue) const
{
DOTRACE("Togl::Impl::allocColor");

  if (!ensureSharedColormap("Togl::allocColor"))
    {
      return 0;
    }

  return X11Util::noFaultXAllocColor(itsDisplay, colormap(),
                                     Tk_Visual(itsTkWin)->map_entries,
                                     red, green, blue);
}

void Togl::Impl::freeColor(unsigned long pixel) const
{
DOTRACE("Togl::Impl::freeColor");

  if (ensureSharedColormap("Togl::freeColor"))
    {
      XFreeColors(itsDisplay, colormap(), &pixel, 1, 0);
    }
}

void Togl::Impl::setColor(unsigned long index,
                          float red, float green, float blue) const
{
DOTRACE("Togl::Impl::setColor");

  if (ensureSharedColormap("Togl::allocColor"))
    {
      XColor xcol;

      xcol.pixel = index;
      xcol.red   = (short) (red   * 65535.0);
      xcol.green = (short) (green * 65535.0);
      xcol.blue  = (short) (blue  * 65535.0);
      xcol.flags = DoRed | DoGreen | DoBlue;

      XStoreColor( itsDisplay, colormap(), &xcol );
    }
}

/*
 * Overlay functions
 */

void Togl::Impl::useLayer(int layer)
{
DOTRACE("Togl::Impl::useLayer");
  if (itsOverlayWindow)
    {
      if (layer==TOGL_OVERLAY)
        {
          itsOverlayGlx->makeCurrent(itsOverlayWindow);
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

void Togl::Impl::showOverlay()
{
DOTRACE("Togl::Impl::showOverlay");
  if (itsOverlayWindow)
    {
      XMapWindow( itsDisplay, itsOverlayWindow );
      XInstallColormap(itsDisplay, itsOverlayCmap);
      itsOverlayIsMapped = 1;
    }
}

void Togl::Impl::hideOverlay()
{
DOTRACE("Togl::Impl::hideOverlay");
  if (itsOverlayWindow && itsOverlayIsMapped)
    {
      XUnmapWindow( itsDisplay, itsOverlayWindow );
      itsOverlayIsMapped = 0;
    }
}

void Togl::Impl::requestOverlayRedisplay()
{
DOTRACE("Togl::Impl::requestOverlayRedisplay");
  if (!itsOverlayUpdatePending && itsOverlayWindow && itsOverlayDisplayProc)
    {
      Tk_DoWhenIdle( Togl::Impl::dummyOverlayRenderCallback,
                     static_cast<ClientData>(this) );
      itsOverlayUpdatePending = true;
    }
}

unsigned long Togl::Impl::allocColorOverlay(float red, float green,
                                            float blue) const
{
DOTRACE("Togl::Impl::allocColorOverlay");
  if (itsOpts.overlayFlag && itsOverlayCmap)
    {
      XColor xcol;
      xcol.red   = (short) (red* 65535.0);
      xcol.green = (short) (green* 65535.0);
      xcol.blue  = (short) (blue* 65535.0);

      if ( !XAllocColor(itsDisplay,itsOverlayCmap,&xcol) )
        return (unsigned long) -1;
      return xcol.pixel;
    }
  else
    {
      return (unsigned long) -1;
    }
}

void Togl::Impl::freeColorOverlay(unsigned long pixel) const
{
DOTRACE("Togl::Impl::freeColorOverlay");

  if (itsOpts.overlayFlag && itsOverlayCmap)
    {
      XFreeColors( itsDisplay, itsOverlayCmap, &pixel, 1, 0 );
    }
}

int Togl::Impl::dumpToEpsFile(const char* filename, int inColor,
                              void (*user_redraw)( const Togl* )) const
{
DOTRACE("Togl::Impl::dumpToEpsFile");

  user_redraw(itsOwner);

  glFlush();

  return GLUtil::generateEPS( filename, inColor, itsOpts.width, itsOpts.height);
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
          if (itsOpts.overlayFlag && itsOverlayIsMapped
              && eventPtr->xexpose.window==itsOverlayWindow)
            {
              requestOverlayRedisplay();
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
      if (itsTimerProc != 0)
        {
          Tcl_DeleteTimerHandler(itsTimerHandler);
          itsTimerProc = 0;
        }
      if (itsUpdatePending)
        {
          Tcl_CancelIdleCall(Togl::Impl::dummyRenderCallback,
                             static_cast<ClientData>(this));
          itsUpdatePending = 0;
        }

      Tcl_EventuallyFree( static_cast<ClientData>(this),
                          Togl::Impl::dummyDestroyProc );

      break;
    default:
      /*nothing*/
      ;
    }
}

///////////////////////////////////////////////////////////////////////
//
// Togl::Impl::widgetCmdDeletedProc
//
//      This procedure is invoked when a widget command is deleted.  If
//      the widget isn't already in the process of being destroyed,
//      this command destroys it.
//
// Results:
//      None.
//
// Side effects:
//      The widget is destroyed.
//
///////////////////////////////////////////////////////////////////////

void Togl::Impl::widgetCmdDeletedProc()
{
DOTRACE("Togl::Impl::widgetCmdDeletedProc");
  /*
   * This procedure could be invoked either because the window was
   * destroyed and the command was then deleted (in which case itsTkWin
   * is NULL) or because the command was deleted, and then this procedure
   * destroys the widget.
   */

  if (itsTkWin != 0)
    {
      Tk_DeleteEventHandler(itsTkWin,
                            ExposureMask | StructureNotifyMask,
                            Togl::Impl::dummyEventProc,
                            static_cast<ClientData>(this));

      if (itsOverlayWindow != 0)
        {
          TkWindow* winPtr = reinterpret_cast<TkWindow*>(itsTkWin);

          Tcl_HashEntry* entryPtr =
            Tcl_FindHashEntry(&winPtr->dispPtr->winTable,
                              (char*) itsOverlayWindow );

          Tcl_DeleteHashEntry(entryPtr);
        }

      Tk_DestroyWindow(itsTkWin);
      itsTkWin = 0;
    }
}

///////////////////////////////////////////////////////////////////////
//
// Togl::Impl::makeWindowExist
//
//   Modified version of Tk_MakeWindowExist.
//   Creates an OpenGL window for the Togl widget.
//
///////////////////////////////////////////////////////////////////////

int Togl::Impl::makeWindowExist()
{
DOTRACE("Togl::Impl::makeWindowExist");

  TkUtil::destroyWindow(itsTkWin);

  if ( !checkForGLX() )
    return TCL_ERROR;

  try
    {
      itsGlx.reset( GlxWrapper::make(itsDisplay, itsOpts.glx) );

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

      setupOverlayIfNeeded();

      // Issue a ConfigureNotify event if there were deferred changes
      TkUtil::issueConfigureNotify(itsTkWin);

      TkUtil::selectAllInput(itsTkWin);

      TkUtil::mapWindow(itsTkWin);

      // Bind the context to the window and make it the current context
      makeCurrent();

      // Check for a single/double buffering snafu
      if (itsOpts.glx.doubleFlag == 0 && itsGlx->isDoubleBuffered())
        {
          // We requested single buffering but had to accept a double buffered
          // visual.  Set the GL draw buffer to be the front buffer to
          // simulate single buffering.
          glDrawBuffer( GL_FRONT );
        }
    }
  catch (Util::Error& err)
    {
      return TCL_ERR(itsInterp, err.msg_cstr());
    }


  return TCL_OK;
}

bool Togl::Impl::checkForGLX()
{
DOTRACE("Togl::Impl::checkForGLX");

  /* Make sure OpenGL's GLX extension supported */
  int dummy;
  if (!glXQueryExtension(itsDisplay, &dummy, &dummy))
    {
      TCL_ERR(itsInterp, "Togl: X server has no OpenGL GLX extension");
      return false;
    }

  return true;
}

void Togl::Impl::setupOverlayIfNeeded()
{
DOTRACE("Togl::Impl::setupOverlayIfNeeded");
  if (itsOpts.overlayFlag)
    {
      if (setupOverlay() == TCL_ERROR)
        {
          fprintf(stderr,"Warning: couldn't setup overlay.\n");
          itsOpts.overlayFlag = 0;
        }
    }
}

/*
 * Do all the setup for overlay planes
 * Return:   TCL_OK or TCL_ERROR
 */
int Togl::Impl::setupOverlay()
{
DOTRACE("Togl::Impl::setupOverlay");

#ifdef GLX_TRANSPARENT_TYPE_EXT
  static int ovAttributeList[] =
    {
      GLX_BUFFER_SIZE, 2,
      GLX_LEVEL, 1,
      GLX_TRANSPARENT_TYPE_EXT, GLX_TRANSPARENT_INDEX_EXT,
      None
    };
#else
  static int ovAttributeList[] =
    {
      GLX_BUFFER_SIZE, 2,
      GLX_LEVEL, 1,
      None
    };
#endif

  TkWindow* winPtr = reinterpret_cast<TkWindow*>(itsTkWin);

  XVisualInfo* visinfo = glXChooseVisual( itsDisplay,
                                          DefaultScreen(itsDisplay),
                                          ovAttributeList );

  if (!visinfo)
    {
      Tcl_AppendResult(itsInterp,Tk_PathName(winPtr),
                       ": No suitable overlay index visual available",
                       (char *) NULL);
      itsOverlayGlx.reset( 0 );
      itsOverlayWindow = 0;
      itsOverlayCmap = 0;
      return TCL_ERROR;
    }

#ifdef GLX_TRANSPARENT_INDEX_EXT
  {
    int fail = glXGetConfig(itsDisplay, visinfo,
                            GLX_TRANSPARENT_INDEX_VALUE_EXT,
                            &itsOverlayTransparentPixel);
    if (fail)
      itsOverlayTransparentPixel=0; /* maybe, maybe ... */
  }
#else
  itsOverlayTransparentPixel=0; /* maybe, maybe ... */
#endif

  /* NEW in Togl 1.5 beta 3 */
  /* share display lists with normal layer context */

  itsOverlayGlx.reset( new GlxWrapper(itsDisplay, visinfo,
                                      !itsOpts.glx.indirect, itsGlx.get()) );

  XSetWindowAttributes swa;
  swa.colormap = XCreateColormap( itsDisplay,
                                  RootWindow(itsDisplay, visinfo->screen),
                                  visinfo->visual, AllocNone );
  itsOverlayCmap = swa.colormap;

#define ALL_EVENTS_MASK \
KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask| \
EnterWindowMask|LeaveWindowMask|PointerMotionMask|ExposureMask|   \
VisibilityChangeMask|FocusChangeMask|PropertyChangeMask|ColormapChangeMask

  swa.border_pixel = 0;
  swa.event_mask = ALL_EVENTS_MASK;
  itsOverlayWindow = XCreateWindow( itsDisplay, windowId(),
                                    0, 0,
                                    itsOpts.width, itsOpts.height, 0,
                                    visinfo->depth, InputOutput,
                                    visinfo->visual,
                                    CWBorderPixel|CWColormap|CWEventMask,
                                    &swa );

  int new_flag;
  Tcl_HashEntry* hPtr =
    Tcl_CreateHashEntry( &winPtr->dispPtr->winTable,
                         (char *) itsOverlayWindow, &new_flag );

  Tcl_SetHashValue( hPtr, winPtr );

  itsOverlayIsMapped = 0;

  /* Make sure window manager installs our colormap */
  XSetWMColormapWindows( itsDisplay, itsOverlayWindow, &itsOverlayWindow, 1 );

  return TCL_OK;
}

///////////////////////////////////////////////////////////////////////
//
// Togl Tcl package
//
///////////////////////////////////////////////////////////////////////

namespace
{
  int ToglCmd(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
  {
    DOTRACE("ToglCmd");
    if (objc <= 1)
      {
        return TCL_ERR(interp,
                       "wrong # args: should be \"pathName read filename\"");
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
