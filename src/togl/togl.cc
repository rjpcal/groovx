///////////////////////////////////////////////////////////////////////
//
// togl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 23 13:11:59 2000
// written: Tue Jul 30 11:20:35 2002
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

// Currently support only X11
#define X11

// Standard C headers
#include <cstdio>
#include <cstdlib>
#include <cstring>

// X Window System headers
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>  /* for XA_RGB_DEFAULT_MAP atom */
#include <X11/Xmu/StdCmap.h>  /* for XmuLookupStandardColormap */

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

#define ALL_EVENTS_MASK \
KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask| \
EnterWindowMask|LeaveWindowMask|PointerMotionMask|ExposureMask|   \
VisibilityChangeMask|FocusChangeMask|PropertyChangeMask|ColormapChangeMask


class Togl::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(Togl* owner, Tcl_Interp* interp,
       const char* pathname, int config_argc, const char** config_argv);
  ~Impl();

  int configure(Tcl_Interp* interp, int argc, const char* argv[], int flags);

  void makeCurrent() const;

  // All callbacks cast to/from Togl::Impl*, _NOT_ Togl* !!!
  static void dummyDestroyProc(char* clientData);
  static void dummyEventProc(ClientData clientData, XEvent* eventPtr);
  static void dummyWidgetCmdDeletedProc(ClientData clientData);
  static void dummyTimerCallback(ClientData clientData);
  static void dummyRenderCallback(ClientData clientData);
  static void dummyOverlayRenderCallback(ClientData clientData);

  void setDisplayFunc(Togl_Callback* proc)
  { itsDisplayProc = proc; }

  void setReshapeFunc(Togl_Callback* proc)
  { itsReshapeProc = proc; }

  void setDestroyFunc(Togl_Callback* proc)
  { itsDestroyProc = proc; }

  void postRedisplay()
  {
    if (!itsUpdatePending)
      {
        Tk_DoWhenIdle( dummyRenderCallback, static_cast<ClientData>(this) );
        itsUpdatePending = GL_TRUE;
      }
  }

  void postReconfigure();

  void swapBuffers() const
  {
    if (itsDoubleFlag)
      {
        glXSwapBuffers( itsDisplay, windowId() );
      }
    else
      {
        glFlush();
      }
  }

  int width() const { return itsWidth; }
  int height() const { return itsHeight; }
  bool isRgba() const { return itsRgbaFlag; }
  bool isDoubleBuffered() const { return itsDoubleFlag; }
  unsigned int bitsPerPixel() const { return itsBitsPerPixel; }
  bool hasPrivateCmap() const { return itsPrivateCmapFlag; }
  Tcl_Interp* interp() const { return itsInterp; }
  Tk_Window tkWin() const { return itsTkWin; }
  const char* pathname() const
  { return Tk_PathName(reinterpret_cast<Tk_FakeWin*>(itsTkWin)); }

  unsigned long allocColor(float red, float green, float blue) const;
  void freeColor(unsigned long pixel) const;
  void setColor(unsigned long index,
                float red, float green, float blue) const;

  GLuint loadBitmapFont(const char* fontname) const;
  GLuint loadBitmapFonti(int fontnumber) const;
  void unloadBitmapFont(GLuint fontbase) const;

  void useLayer(int layer);
  void showOverlay();
  void hideOverlay();
  void postOverlayRedisplay();
  int existsOverlay() const { return itsOverlayFlag; }
  int getOverlayTransparentValue() const { return itsOverlayTransparentPixel; }
  int isMappedOverlay() const { return (itsOverlayFlag && itsOverlayIsMapped); }
  unsigned long allocColorOverlay(float red, float green, float blue) const;
  void freeColorOverlay(unsigned long pixel) const;

  ClientData getClientData() const { return itsClientData; }
  void setClientData(ClientData clientData) { itsClientData = clientData; }

  Display* display() const { return itsDisplay; }
  Screen* screen() const { return Tk_Screen(itsTkWin); }
  int screenNumber() const { return Tk_ScreenNumber(itsTkWin); }
  Colormap colormap() const { return Tk_Colormap(itsTkWin); }
  Window windowId() const { return itsWindowId; }

  int dumpToEpsFile(const char* filename, int inColor,
                    void (*user_redraw)( const Togl* )) const;

private:
  void eventProc(XEvent* eventPtr);
  void widgetCmdDeletedProc();

  int makeWindowExist();

  void destroyCurrentWindow();
  int checkForGLX();
  int setupVisInfoAndContext();
  void buildAttribList(int* attrib_list, int ci_depth, int dbl_flag);
  void setupGLXContext();
  void createWindow();
  Window findParent();
  Colormap findColormap();
  void setupStackingOrder();
  void setupOverlayIfNeeded();
  int setupOverlay();
  void issueConfigureNotify();
  void checkDblBufferSnafu();

public:

  Togl* itsOwner;

  GLXContext itsGLXContext;      /* Normal planes GLX context */
  Display* itsDisplay;     /* X's token for the window's display. */
  Tk_Window  itsTkWin;     /* Tk window structure */
  Window itsWindowId;
  Tcl_Interp* itsInterp;      /* Tcl interpreter */
  Tcl_Command itsWidgetCmd;       /* Token for togl's widget command */
#ifndef NO_TK_CURSOR
  Tk_Cursor itsCursor;     /* The widget's cursor */
#endif
  int itsWidth;
  int itsHeight;     /* Dimensions of window */
  int itsTime;       /* Time value for timer */
  Tcl_TimerToken itsTimerHandler; /* Token for togl's timer handler */
  int itsRgbaFlag;      /* configuration flags (ala GLX parameters) */
  int itsRgbaRed;
  int itsRgbaGreen;
  int itsRgbaBlue;
  int itsDoubleFlag;
  int itsDepthFlag;
  int itsDepthSize;
  int itsAccumFlag;
  int itsAccumRed;
  int itsAccumGreen;
  int itsAccumBlue;
  int itsAccumAlpha;
  int itsAlphaFlag;
  int itsAlphaSize;
  int itsStencilFlag;
  int itsStencilSize;
  int itsPrivateCmapFlag;
  int itsOverlayFlag;
  int itsStereoFlag;
  int itsAuxNumber;
  int itsIndirect;

  ClientData itsClientData;      /* Pointer to user data */

  GLboolean itsUpdatePending;    /* Should normal planes be redrawn? */

  Togl_Callback* itsDisplayProc;    /* Callback when widget is rendered */
  Togl_Callback* itsReshapeProc;    /* Callback when window size changes */
  Togl_Callback* itsDestroyProc;    /* Callback when widget is destroyed */
  Togl_Callback* itsTimerProc;      /* Callback when widget is idle */

  /* Overlay stuff */
  GLXContext itsOverlayCtx;      /* Overlay planes OpenGL context */
  Window itsOverlayWindow;    /* The overlay window, or 0 */
  Togl_Callback* itsOverlayDisplayProc;   /* Overlay redraw proc */
  GLboolean itsOverlayUpdatePending;   /* Should overlay be redrawn? */
  Colormap itsOverlayCmap;    /* colormap for overlay is created */
  int itsOverlayTransparentPixel;      /* transparent pixel */
  int itsOverlayIsMapped;

  /* for DumpToEpsFile: Added by Miguel A. de Riera Pasenau 10.01.1997 */
  XVisualInfo* itsVisInfo;    /* Visual info of the current */
                              /* context needed for DumpToEpsFile */

  unsigned int itsBitsPerPixel;
};




/*
 * Setup Togl widget configuration options:
 */

static Tk_ConfigSpec configSpecs[] =
{
  {TK_CONFIG_PIXELS, (char*)"-height", (char*)"height", (char*)"Height",
   (char*)DEFAULT_HEIGHT, Tk_Offset(Togl::Impl, itsHeight), 0, NULL},

  {TK_CONFIG_PIXELS, (char*)"-width", (char*)"width", (char*)"Width",
   (char*)DEFAULT_WIDTH, Tk_Offset(Togl::Impl, itsWidth), 0, NULL},

  {TK_CONFIG_BOOLEAN, (char*)"-rgba", (char*)"rgba", (char*)"Rgba",
#ifndef NO_RGBA
   (char*)"true",
#else
   (char*)"false",
#endif
   Tk_Offset(Togl::Impl, itsRgbaFlag), 0, NULL},

  {TK_CONFIG_INT, (char*)"-redsize", (char*)"redsize", (char*)"RedSize",
   (char*)"1", Tk_Offset(Togl::Impl, itsRgbaRed), 0, NULL},

  {TK_CONFIG_INT, (char*)"-greensize", (char*)"greensize", (char*)"GreenSize",
   (char*)"1", Tk_Offset(Togl::Impl, itsRgbaGreen), 0, NULL},

  {TK_CONFIG_INT, (char*)"-bluesize", (char*)"bluesize", (char*)"BlueSize",
   (char*)"1", Tk_Offset(Togl::Impl, itsRgbaBlue), 0, NULL},

  {TK_CONFIG_BOOLEAN, (char*)"-double", (char*)"double", (char*)"Double",
#ifndef NO_DOUBLE_BUFFER
   (char*)"true",
#else
   (char*)"false",
#endif
   Tk_Offset(Togl::Impl, itsDoubleFlag), 0, NULL},

  {TK_CONFIG_BOOLEAN, (char*)"-depth", (char*)"depth", (char*)"Depth",
   (char*)"true", Tk_Offset(Togl::Impl, itsDepthFlag), 0, NULL},

  {TK_CONFIG_INT, (char*)"-depthsize", (char*)"depthsize", (char*)"DepthSize",
   (char*)"8", Tk_Offset(Togl::Impl, itsDepthSize), 0, NULL},

  {TK_CONFIG_BOOLEAN, (char*)"-accum", (char*)"accum", (char*)"Accum",
   (char*)"false", Tk_Offset(Togl::Impl, itsAccumFlag), 0, NULL},

  {TK_CONFIG_INT, (char*)"-accumredsize", (char*)"accumredsize", (char*)"AccumRedSize",
   (char*)"1", Tk_Offset(Togl::Impl, itsAccumRed), 0, NULL},

  {TK_CONFIG_INT, (char*)"-accumgreensize", (char*)"accumgreensize", (char*)"AccumGreenSize",
   (char*)"1", Tk_Offset(Togl::Impl, itsAccumGreen), 0, NULL},

  {TK_CONFIG_INT, (char*)"-accumbluesize", (char*)"accumbluesize", (char*)"AccumBlueSize",
   (char*)"1", Tk_Offset(Togl::Impl, itsAccumBlue), 0, NULL},

  {TK_CONFIG_INT, (char*)"-accumalphasize", (char*)"accumalphasize", (char*)"AccumAlphaSize",
   (char*)"1", Tk_Offset(Togl::Impl, itsAccumAlpha), 0, NULL},

  {TK_CONFIG_BOOLEAN, (char*)"-alpha", (char*)"alpha", (char*)"Alpha",
   (char*)"false", Tk_Offset(Togl::Impl, itsAlphaFlag), 0, NULL},

  {TK_CONFIG_INT, (char*)"-alphasize", (char*)"alphasize", (char*)"AlphaSize",
   (char*)"1", Tk_Offset(Togl::Impl, itsAlphaSize), 0, NULL},

  {TK_CONFIG_BOOLEAN, (char*)"-stencil", (char*)"stencil", (char*)"Stencil",
   (char*)"false", Tk_Offset(Togl::Impl, itsStencilFlag), 0, NULL},

  {TK_CONFIG_INT, (char*)"-stencilsize", (char*)"stencilsize", (char*)"StencilSize",
   (char*)"1", Tk_Offset(Togl::Impl, itsStencilSize), 0, NULL},

  {TK_CONFIG_INT, (char*)"-auxbuffers", (char*)"auxbuffers", (char*)"AuxBuffers",
   (char*)"0", Tk_Offset(Togl::Impl, itsAuxNumber), 0, NULL},

  {TK_CONFIG_BOOLEAN, (char*)"-privatecmap", (char*)"privateCmap", (char*)"PrivateCmap",
   (char*)"false", Tk_Offset(Togl::Impl, itsPrivateCmapFlag), 0, NULL},

  {TK_CONFIG_BOOLEAN, (char*)"-overlay", (char*)"overlay", (char*)"Overlay",
   (char*)"false", Tk_Offset(Togl::Impl, itsOverlayFlag), 0, NULL},

  {TK_CONFIG_BOOLEAN, (char*)"-stereo", (char*)"stereo", (char*)"Stereo",
   (char*)"false", Tk_Offset(Togl::Impl, itsStereoFlag), 0, NULL},

#ifndef NO_TK_CURSOR
  { TK_CONFIG_ACTIVE_CURSOR, (char*)"-cursor", (char*)"cursor", (char*)"Cursor",
    (char*)"", Tk_Offset(Togl::Impl, itsCursor), TK_CONFIG_NULL_OK, NULL },
#endif

  {TK_CONFIG_INT, (char*)"-time", (char*)"time", (char*)"Time",
   (char*)DEFAULT_TIME, Tk_Offset(Togl::Impl, itsTime), 0, NULL},

  {TK_CONFIG_BOOLEAN, (char*)"-indirect", (char*)"indirect", (char*)"Indirect",
   (char*)"false", Tk_Offset(Togl::Impl, itsIndirect), 0, NULL},

  {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
   (char *) NULL, 0, 0, NULL}
};


static Togl_Callback *DefaultCreateProc = NULL;
static Togl_Callback *DefaultDisplayProc = NULL;
static Togl_Callback *DefaultReshapeProc = NULL;
static Togl_Callback *DefaultDestroyProc = NULL;
static Togl_Callback *DefaultOverlayDisplayProc = NULL;
static Togl_Callback *DefaultTimerProc = NULL;
static ClientData DefaultClientData = NULL;
static Tcl_HashTable CommandTable;


/*
 * Return an X colormap to use for OpenGL RGB-mode rendering.
 * Input:  dpy - the X display
 *         scrnum - the X screen number
 *         visinfo - the XVisualInfo as returned by glXChooseVisual()
 * Return:  an X Colormap or 0 if there's a _serious_ error.
 */
static Colormap
get_rgb_colormap( Display *dpy, int scrnum, const XVisualInfo *visinfo )
{
DOTRACE("<togl.cc>::get_rgb_colormap");
  Window root = RootWindow(dpy,scrnum);

  /*
   * First check if visinfo's visual matches the default/root visual.
   */
  if (visinfo->visual==DefaultVisual(dpy,scrnum))
    {
      /* use the default/root colormap */
      Colormap cmap;
      cmap = DefaultColormap( dpy, scrnum );
      return cmap;
    }

  /*
   * Next, try to find a standard X colormap.
   */
#ifndef SOLARIS_BUG
  Status status = XmuLookupStandardColormap( dpy, visinfo->screen,
                                             visinfo->visualid, visinfo->depth,
                                             XA_RGB_DEFAULT_MAP,
                                             /* replace */ False,
                                             /* retain */ True);
  if (status == 1)
    {
      int numCmaps;
      XStandardColormap *standardCmaps;
      Status status = XGetRGBColormaps( dpy, root, &standardCmaps,
                                        &numCmaps, XA_RGB_DEFAULT_MAP);
      if (status == 1)
        {
          for (int i = 0; i < numCmaps; i++)
            {
              if (standardCmaps[i].visualid == visinfo->visualid)
                {
                  Colormap cmap = standardCmaps[i].colormap;
                  XFree(standardCmaps);
                  return cmap;
                }
            }
          XFree(standardCmaps);
        }
    }
#endif

  /*
   * If we get here, give up and just allocate a new colormap.
   */
  return XCreateColormap( dpy, root, visinfo->visual, AllocNone );
}


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
  { itsImpl->setDisplayFunc(proc); }

void Togl::setReshapeFunc( Togl_Callback *proc )
  { itsImpl->setReshapeFunc(proc); }

void Togl::setDestroyFunc( Togl_Callback *proc )
  { itsImpl->setDestroyFunc(proc); }

int Togl::configure(Tcl_Interp *interp, int argc, const char *argv[], int flags)
  { return itsImpl->configure(interp, argc, argv, flags); }

void Togl::makeCurrent() const
  { itsImpl->makeCurrent(); }

/*
 * Togl_CreateCommand
 *
 *   Declares a new C sub-command of Togl callable from Tcl.
 *   Every time the sub-command is called from Tcl, the
 *   C routine will be called with all the arguments from Tcl.
 */
void Togl::createCommand( char *cmd_name, Togl_CmdProc *cmd_proc)
{
DOTRACE("Togl::createCommand");
  int new_item;
  Tcl_HashEntry *entry =
    Tcl_CreateHashEntry(&CommandTable, cmd_name, &new_item);
  Tcl_SetHashValue(entry, cmd_proc);
}

void Togl::postRedisplay()
  { itsImpl->postRedisplay(); }

void Togl::postReconfigure()
  { itsImpl->postReconfigure(); }

void Togl::swapBuffers() const
  { itsImpl->swapBuffers(); }

int Togl::width() const
  { return itsImpl->width(); }

int Togl::height() const
  { return itsImpl->height(); }

bool Togl::isRgba() const
  { return itsImpl->isRgba(); }

bool Togl::isDoubleBuffered() const
  { return itsImpl->isDoubleBuffered(); }

unsigned int Togl::bitsPerPixel() const
  { return itsImpl->bitsPerPixel(); }

bool Togl::hasPrivateCmap() const
  { return itsImpl->hasPrivateCmap(); }

Tcl_Interp* Togl::interp() const
  { return itsImpl->interp(); }

Tk_Window Togl::tkWin() const
  { return itsImpl->tkWin(); }

const char* Togl::pathname() const
  { return itsImpl->pathname(); }

int Togl_WidgetCmd(ClientData clientData, Tcl_Interp *interp,
                   int argc, char *argv[])
{
DOTRACE("<togl.cc>::Togl_WidgetCmd");
  Togl::Impl* impl = static_cast<Togl::Impl*>(clientData);
  int result = TCL_OK;

  if (argc < 2)
    {
      Tcl_AppendResult(interp, "wrong # args: should be \"",
                       argv[0], " ?options?\"", NULL);
      return TCL_ERROR;
    }

  Tk_Preserve((ClientData)impl);

  if (!strncmp(argv[1], "configure", Util::max((std::size_t)1,
                                               strlen(argv[1]))))
    {
      if (argc == 2)
        {
          /* Return list of all configuration parameters */
          result = Tk_ConfigureInfo(interp, impl->tkWin(), configSpecs,
                                    (char *)impl, (char *)NULL, 0);
        }
      else if (argc == 3)
        {
          if (strcmp(argv[2],"-extensions")==0)
            {
              /* Return a list of OpenGL extensions available */
              char *extensions;
              extensions = (char *) glGetString(GL_EXTENSIONS);
              Tcl_SetResult( interp, extensions, TCL_STATIC );
              result = TCL_OK;
            }
          else
            {
              /* Return a specific configuration parameter */
              result = Tk_ConfigureInfo(interp, impl->tkWin(), configSpecs,
                                        (char *)impl, argv[2], 0);
            }
        }
      else
        {
          /* Execute a configuration change */
          result = impl->configure(interp, argc-2,
                                   const_cast<const char**>(argv+2),
                                   TK_CONFIG_ARGV_ONLY);
        }
    }
  else if (!strncmp(argv[1], "render", Util::max((std::size_t)1,
                                                 strlen(argv[1]))))
    {
      /* force the widget to be redrawn */
      Togl::Impl::dummyRenderCallback(static_cast<ClientData>(impl));
    }
  else if (!strncmp(argv[1], "swapbuffers", Util::max((std::size_t)1,
                                                      strlen(argv[1]))))
    {
      /* force the widget to be redrawn */
      impl->swapBuffers();
    }
  else if (!strncmp(argv[1], "makecurrent", Util::max((std::size_t)1,
                                                      strlen(argv[1]))))
    {
      /* force the widget to be redrawn */
      impl->makeCurrent();
    }
  else
    {
      /* Probably a user-defined function */
      Tcl_HashEntry* entry = Tcl_FindHashEntry(&CommandTable, argv[1]);
      if (entry != NULL)
        {
          Togl_CmdProc* cmd_proc = (Togl_CmdProc *)Tcl_GetHashValue(entry);
          result = cmd_proc(impl->itsOwner, argc, argv);
        }
      else
        {
          Tcl_AppendResult(interp, "Togl: Unknown option: ", argv[1], "\n",
                           "Try: configure or render\n",
                           "or one of the user-defined commands:\n",
                           NULL);
          Tcl_HashSearch search;
          entry = Tcl_FirstHashEntry(&CommandTable, &search);
          while (entry)
            {
              Tcl_AppendResult(interp, "  ",
                               Tcl_GetHashKey(&CommandTable, entry),
                               "\n", NULL);
              entry = Tcl_NextHashEntry(&search);
            }
          result = TCL_ERROR;
        }
    }

  Tk_Release((ClientData)impl);
  return result;
}


/*
 * A replacement for XAllocColor.  This function should never
 * fail to allocate a color.  When XAllocColor fails, we return
 * the nearest matching color.  If we have to allocate many colors
 * this function isn't too efficient; the XQueryColors() could be
 * done just once.
 * Written by Michael Pichler, Brian Paul, Mark Kilgard
 * Input:  dpy - X display
 *         cmap - X colormap
 *         cmapSize - size of colormap
 * In/Out: color - the XColor struct
 * Output:  exact - 1=exact color match, 0=closest match
 */
static void
noFaultXAllocColor( Display *dpy, Colormap cmap, int cmapSize,
                    XColor *color, int *exact )
{
DOTRACE("<togl.cc>::noFaultXAllocColor");

  /* First try just using XAllocColor. */
  if (XAllocColor(dpy, cmap, color))
    {
      *exact = 1;
      return;
    }

  /* Retrieve color table entries. */
  /* XXX alloca candidate. */
  XColor* ctable = (XColor *) malloc(cmapSize * sizeof(XColor));
  {
    for (int i = 0; i < cmapSize; ++i)
      {
        ctable[i].pixel = i;
      }
  }
  XQueryColors(dpy, cmap, ctable, cmapSize);

  /* Find best match. */
  int bestmatch = -1;
  double mindist = 0.0; // 3*2^16^2 exceeds long int precision.

  {
    for (int i = 0; i < cmapSize; ++i)
      {
        double dr = (double) color->red - (double) ctable[i].red;
        double dg = (double) color->green - (double) ctable[i].green;
        double db = (double) color->blue - (double) ctable[i].blue;
        double dist = dr * dr + dg * dg + db * db;
        if (bestmatch < 0 || dist < mindist)
          {
            bestmatch = i;
            mindist = dist;
          }
      }
  }

  /* Return result. */
  XColor subColor;
  subColor.red = ctable[bestmatch].red;
  subColor.green = ctable[bestmatch].green;
  subColor.blue = ctable[bestmatch].blue;

  free(ctable);

  /* Try to allocate the closest match color.  This should only
   * fail if the cell is read/write.  Otherwise, we're incrementing
   * the cell's reference count.
   */
  if (!XAllocColor(dpy, cmap, &subColor))
    {
      /* do this to work around a problem reported by Frank Ortega */
      subColor.pixel = (unsigned long) bestmatch;
      subColor.red   = ctable[bestmatch].red;
      subColor.green = ctable[bestmatch].green;
      subColor.blue  = ctable[bestmatch].blue;
      subColor.flags = DoRed | DoGreen | DoBlue;
    }
  *color = subColor;
}


unsigned long Togl::allocColor( float red, float green, float blue ) const
  { return itsImpl->allocColor(red, green, blue); }

void Togl::freeColor( unsigned long pixel ) const
  { itsImpl->freeColor(pixel); }

void Togl::setColor( unsigned long index,
                     float red, float green, float blue ) const
  { itsImpl->setColor(index, red, green, blue); }

GLuint Togl::loadBitmapFont( const char *fontname ) const
  { return itsImpl->loadBitmapFont(fontname); }

GLuint Togl::loadBitmapFonti( int fontnumber ) const
  { return itsImpl->loadBitmapFonti(fontnumber); }

void Togl::unloadBitmapFont( GLuint fontbase ) const
  { itsImpl->unloadBitmapFont(fontbase); }


void Togl::overlayDisplayFunc( Togl_Callback *proc )
{
DOTRACE("Togl::OverlayDisplayFunc");
  DefaultOverlayDisplayProc = proc;
}

void Togl::useLayer( int layer )
  { itsImpl->useLayer(layer); }

void Togl::showOverlay()
  { itsImpl->showOverlay(); }

void Togl::hideOverlay()
  { itsImpl->hideOverlay(); }

void Togl::postOverlayRedisplay()
  { itsImpl->postOverlayRedisplay(); }

int Togl::existsOverlay() const
  { return itsImpl->existsOverlay(); }

int Togl::getOverlayTransparentValue() const
  { return itsImpl->getOverlayTransparentValue(); }

int Togl::isMappedOverlay() const
  { return itsImpl->isMappedOverlay(); }

unsigned long Togl::allocColorOverlay( float red, float green,
                                       float blue ) const
  { return itsImpl->allocColorOverlay(red, green, blue); }

void Togl::freeColorOverlay( unsigned long pixel ) const
  { itsImpl->freeColorOverlay(pixel); }



/*
 * User client data
 */

void Togl::defaultClientData( ClientData clientData )
{
DOTRACE("Togl::defaultClientData");
  DefaultClientData = clientData;
}

ClientData Togl::getClientData() const
  { return itsImpl->getClientData(); }


void Togl::setClientData( ClientData clientData )
  { itsImpl->setClientData(clientData); }



/*
 * X11-only functions
 * Contributed by Miguel A. De Riera Pasenau (miguel@DALILA.UPC.ES)
 */

Display* Togl::display() const
  { return itsImpl->display(); }

Screen* Togl::screen() const
  { return itsImpl->screen(); }

int Togl::screenNumber() const
  { return itsImpl->screenNumber(); }

Colormap Togl::colormap() const
  { return itsImpl->colormap(); }

Window Togl::windowId() const
  { return itsImpl->windowId(); }


/*
 * Generate EPS file.
 * Contributed by Miguel A. De Riera Pasenau (miguel@DALILA.UPC.ES)
 */

/* Function that creates a EPS File from a created pixmap on the current
 * context.
 * Based on the code from Copyright (c) Mark J. Kilgard, 1996.
 * Parameters: name_file, b&w / Color flag, redraw function.
 * The redraw function is needed in order to draw things into the new
 * created pixmap.
 */

/* Copyright (c) Mark J. Kilgard, 1996. */

static GLvoid *grabPixels(int inColor, unsigned int width, unsigned int height)
{
DOTRACE("<togl.cc>::grabPixels");
  GLenum format;
  unsigned int size;

  if (inColor)
    {
      format = GL_RGB;
      size = width * height * 3;
    }
  else
    {
      format = GL_COLOR_INDEX;
      /* format = GL_LUMINANCE; */
      size = width * height * 1;
    }

  GLvoid* buffer = (GLvoid *) malloc(size);
  if (buffer == NULL)
    return NULL;

  /* Save current modes. */
  GLint swapbytes, lsbfirst, rowlength;
  GLint skiprows, skippixels, alignment;

  glGetIntegerv(GL_PACK_SWAP_BYTES, &swapbytes);
  glGetIntegerv(GL_PACK_LSB_FIRST, &lsbfirst);
  glGetIntegerv(GL_PACK_ROW_LENGTH, &rowlength);
  glGetIntegerv(GL_PACK_SKIP_ROWS, &skiprows);
  glGetIntegerv(GL_PACK_SKIP_PIXELS, &skippixels);
  glGetIntegerv(GL_PACK_ALIGNMENT, &alignment);

  /* Little endian machines (DEC Alpha for example) could
     benefit from setting GL_PACK_LSB_FIRST to GL_TRUE
     instead of GL_FALSE, but this would require changing the
     generated bitmaps too. */
  glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
  glPixelStorei(GL_PACK_LSB_FIRST, GL_FALSE);
  glPixelStorei(GL_PACK_ROW_LENGTH, 0);
  glPixelStorei(GL_PACK_SKIP_ROWS, 0);
  glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  /* Actually read the pixels. */
  glReadPixels(0, 0, width, height, format,
               GL_UNSIGNED_BYTE, (GLvoid *) buffer);

  /* Restore saved modes. */
  glPixelStorei(GL_PACK_SWAP_BYTES, swapbytes);
  glPixelStorei(GL_PACK_LSB_FIRST, lsbfirst);
  glPixelStorei(GL_PACK_ROW_LENGTH, rowlength);
  glPixelStorei(GL_PACK_SKIP_ROWS, skiprows);
  glPixelStorei(GL_PACK_SKIP_PIXELS, skippixels);
  glPixelStorei(GL_PACK_ALIGNMENT, alignment);
  return buffer;
}


static int generateEPS(const char *filename, int inColor,
                       unsigned int width, unsigned int height)
{
DOTRACE("<togl.cc>::generateEPS");
  GLvoid* pixels = grabPixels(inColor, width, height);
  if (pixels == NULL)
    return 1;

  unsigned int components =
    inColor ? 3 /* Red, green, blue. */ : 1 /* Luminance. */;

  FILE* fp = fopen(filename, "w");
  if (fp == NULL)
    {
      return 2;
    }
  fprintf(fp, "%%!PS-Adobe-2.0 EPSF-1.2\n");
  fprintf(fp, "%%%%Creator: OpenGL pixmap render output\n");
  fprintf(fp, "%%%%BoundingBox: 0 0 %d %d\n", width, height);
  fprintf(fp, "%%%%EndComments\n");

  /* # of lines, 40 bytes per line */
  unsigned int num_lines = ((( width * height) + 7) / 8 ) / 40;
  fprintf(fp, "%%%%BeginPreview: %d %d %d %d\n%%",
          width, height, 1, num_lines);

  int pos = 0;
  unsigned char* curpix = (unsigned char*) pixels;

  {for ( unsigned int i = 0; i < width * height * components; )
    {
      unsigned char bitpixel = 0;
      if (inColor)
        {
          double pix = 0.0;
          pix = 0.30 * ( double)curpix[ i++] + 0.59 * ( double)curpix[ i++] + 0.11 * ( double)curpix[ i++];
          if ( pix > 127.0) bitpixel |= 0x80;
          pix = 0.30 * ( double)curpix[ i++] + 0.59 * ( double)curpix[ i++] + 0.11 * ( double)curpix[ i++];
          if ( pix > 127.0) bitpixel |= 0x40;
          pix = 0.30 * ( double)curpix[ i++] + 0.59 * ( double)curpix[ i++] + 0.11 * ( double)curpix[ i++];
          if ( pix > 127.0) bitpixel |= 0x20;
          pix = 0.30 * ( double)curpix[ i++] + 0.59 * ( double)curpix[ i++] + 0.11 * ( double)curpix[ i++];
          if ( pix > 127.0) bitpixel |= 0x10;
          pix = 0.30 * ( double)curpix[ i++] + 0.59 * ( double)curpix[ i++] + 0.11 * ( double)curpix[ i++];
          if ( pix > 127.0) bitpixel |= 0x08;
          pix = 0.30 * ( double)curpix[ i++] + 0.59 * ( double)curpix[ i++] + 0.11 * ( double)curpix[ i++];
          if ( pix > 127.0) bitpixel |= 0x04;
          pix = 0.30 * ( double)curpix[ i++] + 0.59 * ( double)curpix[ i++] + 0.11 * ( double)curpix[ i++];
          if ( pix > 127.0) bitpixel |= 0x02;
          pix = 0.30 * ( double)curpix[ i++] + 0.59 * ( double)curpix[ i++] + 0.11 * ( double)curpix[ i++];
          if ( pix > 127.0) bitpixel |= 0x01;
        }
      else
        {
          if ( curpix[ i++] > 0x7f) bitpixel |= 0x80;
          if ( curpix[ i++] > 0x7f) bitpixel |= 0x40;
          if ( curpix[ i++] > 0x7f) bitpixel |= 0x20;
          if ( curpix[ i++] > 0x7f) bitpixel |= 0x10;
          if ( curpix[ i++] > 0x7f) bitpixel |= 0x08;
          if ( curpix[ i++] > 0x7f) bitpixel |= 0x04;
          if ( curpix[ i++] > 0x7f) bitpixel |= 0x02;
          if ( curpix[ i++] > 0x7f) bitpixel |= 0x01;
        }
      fprintf(fp, "%02hx", bitpixel);
      if (++pos >= 40)
        {
          fprintf(fp, "\n%%");
          pos = 0;
        }
    }
  }

  if (pos)
    fprintf(fp, "\n%%%%EndPreview\n");
  else
    fprintf(fp, "%%EndPreview\n");

  fprintf(fp, "gsave\n");
  fprintf(fp, "/bwproc {\n");
  fprintf(fp, "    rgbproc\n");
  fprintf(fp, "    dup length 3 idiv string 0 3 0\n");
  fprintf(fp, "    5 -1 roll {\n");
  fprintf(fp, "    add 2 1 roll 1 sub dup 0 eq\n");
  fprintf(fp, "    { pop 3 idiv 3 -1 roll dup 4 -1 roll dup\n");
  fprintf(fp, "        3 1 roll 5 -1 roll put 1 add 3 0 }\n");
  fprintf(fp, "    { 2 1 roll } ifelse\n");
  fprintf(fp, "    } forall\n");
  fprintf(fp, "    pop pop pop\n");
  fprintf(fp, "} def\n");
  fprintf(fp, "systemdict /colorimage known not {\n");
  fprintf(fp, "    /colorimage {\n");
  fprintf(fp, "        pop\n");
  fprintf(fp, "        pop\n");
  fprintf(fp, "        /rgbproc exch def\n");
  fprintf(fp, "        { bwproc } image\n");
  fprintf(fp, "    } def\n");
  fprintf(fp, "} if\n");
  fprintf(fp, "/picstr %d string def\n", width * components);
  fprintf(fp, "%d %d scale\n", width, height);
  fprintf(fp, "%d %d %d\n", width, height, 8);
  fprintf(fp, "[%d 0 0 %d 0 0]\n", width, height);
  fprintf(fp, "{currentfile picstr readhexstring pop}\n");
  fprintf(fp, "false %d\n", components);
  fprintf(fp, "colorimage\n");

  curpix = (unsigned char *) pixels;
  pos = 0;
  {for (unsigned int i = width * height * components; i > 0; i--)
    {
      fprintf(fp, "%02hx", *curpix++);
      if (++pos >= 40)
        {
          fprintf(fp, "\n");
          pos = 0;
        }
    }
  }

  if (pos)
    fprintf(fp, "\n");

  fprintf(fp, "grestore\n");
  free(pixels);
  fclose(fp);
  return 0;
}

int Togl::dumpToEpsFile( const char *filename, int inColor,
                         void (*user_redraw)( const Togl* )) const
  { return itsImpl->dumpToEpsFile(filename, inColor, user_redraw); }


///////////////////////////////////////////////////////////////////////
//
// Togl member definitions
//
///////////////////////////////////////////////////////////////////////

Togl::Togl(Tcl_Interp* interp, const char* pathname,
           int config_argc, const char** config_argv) :
  itsImpl(new Impl(this, interp, pathname, config_argc, config_argv))
{
DOTRACE("Togl::Togl");
}

Togl::~Togl()
{
DOTRACE("Togl::~Togl");
  delete itsImpl;
}

//     * Creates a new window
//     * Creates an 'Togl' data structure
//     * Creates an event handler for this window
//     * Creates a command that handles this object
//     * Configures this Togl for the given arguments

Togl::Impl::Impl(Togl* owner, Tcl_Interp* interp,
                 const char* pathname,
                 int config_argc, const char** config_argv) :
  itsOwner(owner),

  itsGLXContext(NULL),
  itsDisplay(0),
  itsTkWin(0),
  itsWindowId(0),
  itsInterp(interp),
  itsWidgetCmd(0),
#ifndef NO_TK_CURSOR
  itsCursor(None),
#endif
  itsWidth(0),
  itsHeight(0),
  itsTime(0),
  itsTimerHandler(0),
#ifndef NO_RGBA
  itsRgbaFlag(1),
#else
  itsRgbaFlag(0),
#endif
  itsRgbaRed(1),
  itsRgbaGreen(1),
  itsRgbaBlue(1),
#ifndef NO_DOUBLE_BUFFER
  itsDoubleFlag(1),
#else
  itsDoubleFlag(0),
#endif
  itsDepthFlag(0),
  itsDepthSize(1),
  itsAccumFlag(0),
  itsAccumRed(1),
  itsAccumGreen(1),
  itsAccumBlue(1),
  itsAccumAlpha(1),
  itsAlphaFlag(0),
  itsAlphaSize(1),
  itsStencilFlag(0),
  itsStencilSize(1),
  itsPrivateCmapFlag(0),
  itsOverlayFlag(0),
  itsStereoFlag(0),
  itsAuxNumber(0),
  itsIndirect(GL_FALSE),
  itsClientData(DefaultClientData),
  itsUpdatePending(GL_FALSE),
  itsDisplayProc(DefaultDisplayProc),
  itsReshapeProc(DefaultReshapeProc),
  itsDestroyProc(DefaultDestroyProc),
  itsTimerProc(DefaultTimerProc),
  itsOverlayCtx(NULL),
  itsOverlayWindow(0),
  itsOverlayDisplayProc(DefaultOverlayDisplayProc),
  itsOverlayUpdatePending(GL_FALSE),
  itsOverlayCmap(),
  itsOverlayTransparentPixel(),
  itsOverlayIsMapped(0),
  itsVisInfo(0),

  itsBitsPerPixel(0)
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

  // Configure the widget
  if (config_argc > 0 )
    {
      if (configure(itsInterp, config_argc, config_argv, 0) == TCL_ERROR)
        {
          Tk_DestroyWindow(itsTkWin);
          throw Util::Error("Togl constructor couldn't configure widget");
        }
    }
  else
    {
      // Generate a default argc/argv array
      const char* init_args = "-stereo false";

      Tcl_SplitList(interp, init_args, &config_argc, &config_argv);

      if (configure(itsInterp, config_argc, config_argv, 0) == TCL_ERROR)
        {
          Tk_DestroyWindow(itsTkWin);
          throw Util::Error("Togl constructor couldn't configure widget");
        }

      Tcl_Free((char*) config_argv);
    }


  // If OpenGL window wasn't already created by configure() we
  // create it now.  We can tell by checking if the GLX context has
  // been initialized.
  if (!itsGLXContext)
    {
      if (makeWindowExist() == TCL_ERROR)
        {
          Tk_DestroyWindow(itsTkWin);
          throw Util::Error("Togl constructor couldn't create window");
        }
    }

  itsWidgetCmd = Tcl_CreateCommand(itsInterp, Tk_PathName(itsTkWin),
                                   Togl_WidgetCmd,
                                   static_cast<ClientData>(this),
                                   Togl::Impl::dummyWidgetCmdDeletedProc);

  Tk_CreateEventHandler(itsTkWin,
                        ExposureMask | StructureNotifyMask,
                        Togl::Impl::dummyEventProc,
                        static_cast<ClientData>(this));

  if (itsTimerProc)
    {
      Tk_CreateTimerHandler( itsTime, Togl::Impl::dummyTimerCallback,
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

  Tk_FreeOptions(configSpecs, (char *) this, itsDisplay, 0);

#ifndef NO_TK_CURSOR
  if (itsCursor != None)
    {
      Tk_FreeCursor(itsDisplay, itsCursor);
    }
#endif
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

int Togl::Impl::configure(Tcl_Interp* interp,
                          int argc, const char* argv[], int flags)
{
DOTRACE("Togl::Impl::configure");

  int oldRgbaFlag    = itsRgbaFlag;
  int oldRgbaRed     = itsRgbaRed;
  int oldRgbaGreen   = itsRgbaGreen;
  int oldRgbaBlue    = itsRgbaBlue;
  int oldDoubleFlag  = itsDoubleFlag;
  int oldDepthFlag   = itsDepthFlag;
  int oldDepthSize   = itsDepthSize;
  int oldAccumFlag   = itsAccumFlag;
  int oldAccumRed    = itsAccumRed;
  int oldAccumGreen  = itsAccumGreen;
  int oldAccumBlue   = itsAccumBlue;
  int oldAccumAlpha  = itsAccumAlpha;
  int oldAlphaFlag   = itsAlphaFlag;
  int oldAlphaSize   = itsAlphaSize;
  int oldStencilFlag = itsStencilFlag;
  int oldStencilSize = itsStencilSize;
  int oldAuxNumber   = itsAuxNumber;

  if (Tk_ConfigureWidget(interp, itsTkWin, configSpecs,
                         argc, const_cast<char**>(argv),
                         reinterpret_cast<char *>(this), flags) == TCL_ERROR)
    {
      return TCL_ERROR;
    }

  Tk_GeometryRequest(itsTkWin, itsWidth, itsHeight);

  if (itsRgbaFlag != oldRgbaFlag
      || itsRgbaRed != oldRgbaRed
      || itsRgbaGreen != oldRgbaGreen
      || itsRgbaBlue != oldRgbaBlue
      || itsDoubleFlag != oldDoubleFlag
      || itsDepthFlag != oldDepthFlag
      || itsDepthSize != oldDepthSize
      || itsAccumFlag != oldAccumFlag
      || itsAccumRed != oldAccumRed
      || itsAccumGreen != oldAccumGreen
      || itsAccumBlue != oldAccumBlue
      || itsAccumAlpha != oldAccumAlpha
      || itsAlphaFlag != oldAlphaFlag
      || itsAlphaSize != oldAlphaSize
      || itsStencilFlag != oldStencilFlag
      || itsStencilSize != oldStencilSize
      || itsAuxNumber != oldAuxNumber)
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
      glXMakeCurrent( itsDisplay,
                      windowId(),
                      itsGLXContext );
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
    Tcl_CreateTimerHandler( impl->itsTime, dummyTimerCallback,
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
  impl->itsUpdatePending = GL_FALSE;
}

void Togl::Impl::dummyOverlayRenderCallback(ClientData clientData)
{
DOTRACE("Togl::Impl::dummyOverlayRenderCallback");
  Impl* impl = static_cast<Impl*>(clientData);

  if (impl->itsOverlayFlag && impl->itsOverlayDisplayProc)
    {
      glXMakeCurrent( impl->itsDisplay,
                      impl->itsOverlayWindow,
                      impl->itsOverlayCtx );
      impl->itsOverlayDisplayProc(impl->itsOwner);
    }
  impl->itsOverlayUpdatePending = GL_FALSE;
}


void Togl::Impl::postReconfigure()
{
DOTRACE("Togl::Impl::postReconfigure");
  if (itsWidth != Tk_Width(itsTkWin) || itsHeight != Tk_Height(itsTkWin))
    {
      itsWidth = Tk_Width(itsTkWin);
      itsHeight = Tk_Height(itsTkWin);
      XResizeWindow(itsDisplay, windowId(), itsWidth, itsHeight);

      if (itsOverlayFlag)
      {
        XResizeWindow( itsDisplay, itsOverlayWindow, itsWidth, itsHeight );
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
      glViewport(0, 0, itsWidth, itsHeight);
      if (itsOverlayFlag)
        {
          useLayer( TOGL_OVERLAY );
          glViewport( 0, 0, itsWidth, itsHeight );
          useLayer( TOGL_NORMAL );
        }
    }
}

unsigned long Togl::Impl::allocColor(float red, float green, float blue) const
{
DOTRACE("Togl::Impl::allocColor");
  if (itsRgbaFlag)
    {
      fprintf(stderr,"Error: Togl::allocColor illegal in RGBA mode.\n");
      return 0;
    }
  /* TODO: maybe not... */
  if (itsPrivateCmapFlag)
    {
      fprintf(stderr,"Error: Togl::allocColor illegal with private colormap\n");
      return 0;
    }

  XColor xcol;
  xcol.red   = (short) (red   * 65535.0);
  xcol.green = (short) (green * 65535.0);
  xcol.blue  = (short) (blue  * 65535.0);

  int exact;
  noFaultXAllocColor( itsDisplay, colormap(),
                      Tk_Visual(itsTkWin)->map_entries, &xcol, &exact );


  return xcol.pixel;
}

void Togl::Impl::freeColor(unsigned long pixel) const
{
DOTRACE("Togl::Impl::freeColor");
  if (itsRgbaFlag)
    {
      fprintf(stderr,"Error: Togl::freeColor illegal in RGBA mode.\n");
      return;
    }
  /* TODO: maybe not... */
  if (itsPrivateCmapFlag)
    {
      fprintf(stderr,"Error: Togl::freeColor illegal with private colormap\n");
      return;
    }

  XFreeColors( itsDisplay, colormap(),
               &pixel, 1, 0 );
}

void Togl::Impl::setColor(unsigned long index,
                          float red, float green, float blue) const
{
DOTRACE("Togl::Impl::setColor");
  if (itsRgbaFlag)
    {
      fprintf(stderr,"Error: Togl::setColor illegal in RGBA mode.\n");
      return;
    }
  if (!itsPrivateCmapFlag)
    {
      fprintf(stderr,"Error: Togl::setColor requires a private colormap\n");
      return;
    }

  XColor xcol;

  xcol.pixel = index;
  xcol.red   = (short) (red   * 65535.0);
  xcol.green = (short) (green * 65535.0);
  xcol.blue  = (short) (blue  * 65535.0);
  xcol.flags = DoRed | DoGreen | DoBlue;

  XStoreColor( itsDisplay, colormap(), &xcol );
}


#define MAX_FONTS 1000
static GLuint ListBase[MAX_FONTS];
static GLuint ListCount[MAX_FONTS];


//---------------------------------------------------------------------
//
// Load the named bitmap font as a sequence of bitmaps in a display list.
// fontname may be one of the predefined fonts like TOGL_BITMAP_8_BY_13
// or an X font name, or a Windows font name, etc.
//
//---------------------------------------------------------------------

GLuint Togl::Impl::loadBitmapFont(const char* fontname) const
{
DOTRACE("Togl::Impl::loadBitmapFont");
  static int FirstTime = 1;

  /* Initialize the ListBase and ListCount arrays */
  if (FirstTime)
    {
      for (int i=0;i<MAX_FONTS;++i)
        {
          ListBase[i] = ListCount[i] = 0;
        }
      FirstTime = 0;
    }

  if (fontname == 0)
    fontname = DEFAULT_FONTNAME;

  Assert( fontname );

  XFontStruct *fontinfo = XLoadQueryFont( Tk_Display(this->itsTkWin), fontname );
  if (!fontinfo)
    {
      return 0;
    }

  int first = fontinfo->min_char_or_byte2;
  int last = fontinfo->max_char_or_byte2;
  int count = last-first+1;

  GLuint fontbase = glGenLists( (GLuint) (last+1) );
  if (fontbase==0)
    {
      return 0;
    }
  glXUseXFont( fontinfo->fid, first, count, (int) fontbase+first );

  /* Record the list base and number of display lists
   * for unloadBitmapFont().
   */
  {
    for (int i=0;i<MAX_FONTS;++i)
      {
        if (ListBase[i]==0)
          {
            ListBase[i] = fontbase;
            ListCount[i] = last+1;
            break;
          }
      }
  }

  return fontbase;
}

GLuint Togl::Impl::loadBitmapFonti(int fontnumber) const
{

  /*
   * This method of selecting X fonts according to a TOGL_ font name
   * is a kludge.  To be fixed when I find time...
   */

  const char* name = DEFAULT_FONTNAME;

  switch (fontnumber)
    {
    case TOGL_BITMAP_8_BY_13:
      name = "8x13";
      break;
    case TOGL_BITMAP_9_BY_15:
      name = "9x15";
      break;
    case TOGL_BITMAP_TIMES_ROMAN_10:
      name = "-adobe-times-medium-r-normal--10-100-75-75-p-54-iso8859-1";
      break;
    case TOGL_BITMAP_TIMES_ROMAN_24:
      name = "-adobe-times-medium-r-normal--24-240-75-75-p-124-iso8859-1";
      break;
    case TOGL_BITMAP_HELVETICA_10:
      name = "-adobe-helvetica-medium-r-normal--10-100-75-75-p-57-iso8859-1";
      break;
    case TOGL_BITMAP_HELVETICA_12:
      name = "-adobe-helvetica-medium-r-normal--12-120-75-75-p-67-iso8859-1";
      break;
    case TOGL_BITMAP_HELVETICA_18:
      name = "-adobe-helvetica-medium-r-normal--18-180-75-75-p-98-iso8859-1";
      break;
    }

  return loadBitmapFont(name);
}

// Release the display lists which were generated by loadBitmapFont().
void Togl::Impl::unloadBitmapFont(GLuint fontbase) const
{
DOTRACE("Togl::Impl::unloadBitmapFont");
  for (int i=0; i<MAX_FONTS; ++i)
    {
      DebugEvalNL(i);
      if (ListBase[i]==fontbase)
        {
          DebugEvalNL(fontbase);
          DebugEval(ListBase[i]); DebugEvalNL(ListCount[i]);
          glDeleteLists( ListBase[i], ListCount[i] );
          ListBase[i] = ListCount[i] = 0;
          return;
        }
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
          glXMakeCurrent( itsDisplay,
                          itsOverlayWindow,
                          itsOverlayCtx );
        }
      else if (layer==TOGL_NORMAL)
        {
          glXMakeCurrent( itsDisplay,
                          windowId(),
                          itsGLXContext );
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

void Togl::Impl::postOverlayRedisplay()
{
DOTRACE("Togl::Impl::postOverlayRedisplay");
  if (!itsOverlayUpdatePending && itsOverlayWindow && itsOverlayDisplayProc)
    {
      Tk_DoWhenIdle( Togl::Impl::dummyOverlayRenderCallback,
                     static_cast<ClientData>(this) );
      itsOverlayUpdatePending = 1;
    }
}

unsigned long Togl::Impl::allocColorOverlay(float red, float green,
                                            float blue) const
{
DOTRACE("Togl::Impl::allocColorOverlay");
  if (itsOverlayFlag && itsOverlayCmap)
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

  if (itsOverlayFlag && itsOverlayCmap)
    {
      XFreeColors( itsDisplay, itsOverlayCmap, &pixel, 1, 0 );
    }
}

int Togl::Impl::dumpToEpsFile(const char* filename, int inColor,
                        void (*user_redraw)( const Togl* )) const
{
DOTRACE("Togl::Impl::dumpToEpsFile");

  user_redraw(itsOwner);

  glReadBuffer(GL_FRONT); // by default it read GL_BACK in double buffer mode

  glFlush();

  return generateEPS( filename, inColor, itsWidth, itsHeight);
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
          if (!itsUpdatePending &&
              eventPtr->xexpose.window==windowId())
            {
              postRedisplay();
            }
          if (!itsOverlayUpdatePending && itsOverlayFlag
              && itsOverlayIsMapped
              && eventPtr->xexpose.window==itsOverlayWindow)
            {
              postOverlayRedisplay();
            }
        }
      break;
    case ConfigureNotify:
      DebugPrintNL("ConfigureNotify");
      postReconfigure();
      break;
    case MapNotify:
      DebugPrintNL("MapNotify");
      break;
    case DestroyNotify:
      DebugPrintNL("DestroyNotify");
      if (itsTkWin != NULL)
        {
          itsTkWin = NULL;
          Tcl_DeleteCommandFromToken( itsInterp, itsWidgetCmd );
        }
      if (itsTimerProc != NULL)
        {
          Tcl_DeleteTimerHandler(itsTimerHandler);
        }
      if (itsUpdatePending)
        {
          Tcl_CancelIdleCall(Togl::Impl::dummyRenderCallback,
                             static_cast<ClientData>(this));
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

  /* NEW in togl 1.5 beta 3 */
  if (this && itsTkWin)
    {
      Tk_DeleteEventHandler(itsTkWin,
                            ExposureMask | StructureNotifyMask,
                            Togl::Impl::dummyEventProc,
                            static_cast<ClientData>(this));
    }

  /* NEW in togl 1.5 beta 3 */
  if (itsGLXContext)
    {
      glXDestroyContext( itsDisplay, itsGLXContext );
      itsGLXContext = NULL;
    }
  if (itsOverlayCtx)
    {
      Tcl_HashEntry *entryPtr;
      TkWindow *winPtr = (TkWindow *) itsTkWin;
      if (winPtr)
        {
          entryPtr = Tcl_FindHashEntry(&winPtr->dispPtr->winTable,
                                       (char *) itsOverlayWindow );
          Tcl_DeleteHashEntry(entryPtr);
        }
      glXDestroyContext( itsDisplay, itsOverlayCtx );
      itsOverlayCtx = NULL;
    }

  if (itsTkWin != NULL)
    {
      Tk_DestroyWindow(itsTkWin);
      itsTkWin = NULL;
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

  destroyCurrentWindow();

  int status = checkForGLX();
  if ( status != TCL_OK ) return status;

  status = setupVisInfoAndContext();
  if ( status != TCL_OK ) return status;

  createWindow();

  setupStackingOrder();

  setupOverlayIfNeeded();

  // Issue a ConfigureNotify event if there were deferred changes
  issueConfigureNotify();

  XSelectInput(itsDisplay, itsWindowId, ALL_EVENTS_MASK);

  // Request the X window to be displayed
  XMapWindow(itsDisplay, windowId());

  // Bind the context to the window and make it the current context
  makeCurrent();

  // Check for a single/double buffering snafu
  checkDblBufferSnafu();

  return TCL_OK;
}

void Togl::Impl::destroyCurrentWindow()
{
DOTRACE("Togl::Impl::destroyCurrentWindow");
  TkWindow *winPtr = (TkWindow *) itsTkWin;

  if (winPtr->window != None)
    {
      XDestroyWindow(itsDisplay, winPtr->window);
      winPtr->window = 0;
    }
}

int Togl::Impl::checkForGLX()
{
DOTRACE("Togl::Impl::checkForGLX");

  /* Make sure OpenGL's GLX extension supported */
  int dummy;
  if (!glXQueryExtension(itsDisplay, &dummy, &dummy))
    {
      return TCL_ERR(itsInterp, "Togl: X server has no OpenGL GLX extension");
    }

  return TCL_OK;
}

int Togl::Impl::setupVisInfoAndContext()
{
DOTRACE("Togl::Impl::setupVisInfoAndContext");

  int attrib_list[1000];

  const int MAX_ATTEMPTS = 12;

  static int ci_depths[MAX_ATTEMPTS] =
    {
      8, 4, 2, 1, 12, 16, 8, 4, 2, 1, 12, 16
    };
  static int dbl_flags[MAX_ATTEMPTS] =
    {
      0, 0, 0, 0,  0,  0, 1, 1, 1, 1,  1,  1
    };

  /* It may take a few tries to get a visual */
  for (int attempt=0; attempt<MAX_ATTEMPTS; attempt++)
    {
      DebugEvalNL(attempt);
      buildAttribList(attrib_list, ci_depths[attempt], dbl_flags[attempt]);

      itsVisInfo = glXChooseVisual( itsDisplay,
                                    DefaultScreen(itsDisplay), attrib_list );

      if (itsVisInfo)
        {
          /* found a GLX visual! */
          itsBitsPerPixel = itsVisInfo->depth;

          DebugEvalNL((void*)itsVisInfo->visualid);
          DebugEvalNL(itsVisInfo->depth);
          DebugEvalNL(itsVisInfo->bits_per_rgb);
          break;
        }
    }

  if (itsVisInfo==NULL)
    {
      return TCL_ERR(itsInterp, "Togl: couldn't get visual");
    }

  // Create a new OpenGL rendering context.
  setupGLXContext();   DebugEvalNL(itsGLXContext);

  if (itsGLXContext == NULL)
    {
      return TCL_ERR(itsInterp, "could not create rendering context");
    }

  // Make sure we don't try to use a depth buffer with indirect rendering
  if ( glXIsDirect(itsDisplay, itsGLXContext) == False &&
       itsDepthFlag == true )
    {
      glXDestroyContext(itsDisplay, itsGLXContext);
      itsGLXContext = 0;
      itsDepthFlag = false;
      return setupVisInfoAndContext();
    }

  itsIndirect = glXIsDirect(itsDisplay, itsGLXContext);

  return TCL_OK;
}

void Togl::Impl::buildAttribList(int* attrib_list, int ci_depth, int dbl_flag)
{
DOTRACE("Togl::Impl::buildAttribList");
  int attrib_count = 0;

  attrib_list[attrib_count++] = GLX_USE_GL;

  if (itsRgbaFlag)
    {
      /* RGB[A] mode */
      attrib_list[attrib_count++] = GLX_RGBA;
      attrib_list[attrib_count++] = GLX_RED_SIZE;
      attrib_list[attrib_count++] = itsRgbaRed;     DebugEval(itsRgbaRed);
      attrib_list[attrib_count++] = GLX_GREEN_SIZE;
      attrib_list[attrib_count++] = itsRgbaGreen;   DebugEval(itsRgbaGreen);
      attrib_list[attrib_count++] = GLX_BLUE_SIZE;
      attrib_list[attrib_count++] = itsRgbaBlue;    DebugEvalNL(itsRgbaBlue);
      if (itsAlphaFlag)
        {
          attrib_list[attrib_count++] = GLX_ALPHA_SIZE;
          attrib_list[attrib_count++] = itsAlphaSize;  DebugEvalNL(itsAlphaSize);
        }
    }
  else
    {
      /* Color index mode */
      attrib_list[attrib_count++] = GLX_BUFFER_SIZE;
      attrib_list[attrib_count++] = ci_depth;
    }

  if (itsDepthFlag)
    {
      attrib_list[attrib_count++] = GLX_DEPTH_SIZE;
      attrib_list[attrib_count++] = itsDepthSize;
    }

  if (itsDoubleFlag || dbl_flag)
    {
      attrib_list[attrib_count++] = GLX_DOUBLEBUFFER;
    }

  if (itsStencilFlag)
    {
      attrib_list[attrib_count++] = GLX_STENCIL_SIZE;
      attrib_list[attrib_count++] = itsStencilSize;
    }

  if (itsAccumFlag)
    {
      attrib_list[attrib_count++] = GLX_ACCUM_RED_SIZE;
      attrib_list[attrib_count++] = itsAccumRed;
      attrib_list[attrib_count++] = GLX_ACCUM_GREEN_SIZE;
      attrib_list[attrib_count++] = itsAccumGreen;
      attrib_list[attrib_count++] = GLX_ACCUM_BLUE_SIZE;
      attrib_list[attrib_count++] = itsAccumBlue;
      if (itsAlphaFlag)
        {
          attrib_list[attrib_count++] = GLX_ACCUM_ALPHA_SIZE;
          attrib_list[attrib_count++] = itsAccumAlpha;
        }
    }

  if (itsAuxNumber != 0)
    {
      attrib_list[attrib_count++] = GLX_AUX_BUFFERS;
      attrib_list[attrib_count++] = itsAuxNumber;
    }

  attrib_list[attrib_count++] = None;
}

void Togl::Impl::setupGLXContext()
{
DOTRACE("Togl::Impl::setupGLXContext");
  int directCtx = itsIndirect ? GL_FALSE : GL_TRUE;

  itsGLXContext = glXCreateContext(itsDisplay, itsVisInfo, None, directCtx);
}

void Togl::Impl::createWindow()
{
DOTRACE("Togl::Impl::createWindow");

  TkWindow *winPtr = (TkWindow *) itsTkWin;

  Colormap cmap = findColormap();

  /* Make sure Tk knows to switch to the new colormap when the cursor
   * is over this window when running in color index mode.
   */
  Tk_SetWindowVisual(itsTkWin, itsVisInfo->visual, itsVisInfo->depth, cmap);

  // Find parent of window (necessary for creation)
  Window parent = findParent();

  DebugEvalNL(parent);

  winPtr->atts.colormap = cmap;
  winPtr->atts.border_pixel = 0;
  winPtr->atts.event_mask = ALL_EVENTS_MASK;
  itsWindowId = XCreateWindow(itsDisplay,
                              parent,
                              0, 0, itsWidth, itsHeight,
                              0, itsVisInfo->depth,
                              InputOutput, itsVisInfo->visual,
                              CWBorderPixel | CWColormap | CWEventMask,
                              &winPtr->atts);

  DebugEvalNL(itsWindowId);

  winPtr->window = itsWindowId;

  // This is a hack to get the Togl widget's colormap to be
  // visible... basically we install this colormap in all windows up
  // the window hierarchy up to (but not including) the root
  // window. It should be possible to get the window manager to
  // install the colormap when the Togl widget becomes the active
  // window, but this has not worked yet.

  if (!itsRgbaFlag)
    {

      Window current = itsWindowId;
      Window root = XRootWindow( itsDisplay, DefaultScreen(itsDisplay) );

      while (current != root)
        {

          DebugEval((void*)current); DebugEvalNL((void*)root);

          XSetWindowColormap(itsDisplay, current, cmap);

          Window parent;
          Window* children;
          unsigned int nchildren;

          XQueryTree(itsDisplay, current, &root, &parent, &children, &nchildren);
          XFree(children);

          current = parent;
        }
    }

  int dummy_new_flag;
  Tcl_HashEntry *hPtr = Tcl_CreateHashEntry(&winPtr->dispPtr->winTable,
                                            (char *) windowId(), &dummy_new_flag);
  Tcl_SetHashValue(hPtr, winPtr);

  winPtr->dirtyAtts = 0;
  winPtr->dirtyChanges = 0;
#ifdef TK_USE_INPUT_METHODS
  winPtr->inputContext = NULL;
#endif /* TK_USE_INPUT_METHODS */
}

Window Togl::Impl::findParent()
{
DOTRACE("Togl::Impl::findParent");

  TkWindow *winPtr = (TkWindow *) itsTkWin;

  if ((winPtr->parentPtr == NULL) || (winPtr->flags & TK_TOP_LEVEL))
    {
      return XRootWindow(winPtr->display, winPtr->screenNum);
    }
  // else...
  if (winPtr->parentPtr->window == None)
    {
      Tk_MakeWindowExist((Tk_Window) winPtr->parentPtr);
    }
  return winPtr->parentPtr->window;
}

Colormap Togl::Impl::findColormap()
{
DOTRACE("Togl::Impl::findColormap");

  int scrnum = DefaultScreen(itsDisplay);
  Colormap cmap;
  if (itsRgbaFlag)
    {
      /* Colormap for RGB mode */
      cmap = get_rgb_colormap( itsDisplay, scrnum, itsVisInfo );
    }
  else
    {
      /* Colormap for CI mode */
      if (itsPrivateCmapFlag)
        {
          /* need read/write colormap so user can store own color entries */
          cmap = XCreateColormap(itsDisplay,
                                 RootWindow(itsDisplay, itsVisInfo->screen),
                                 itsVisInfo->visual, AllocAll);
        }
      else
        {
          if (itsVisInfo->visual==DefaultVisual(itsDisplay, scrnum))
            {
              /* share default/root colormap */
              cmap = DefaultColormap(itsDisplay,scrnum);
            }
          else
            {
              /* make a new read-only colormap */
              cmap = XCreateColormap(itsDisplay,
                                     RootWindow(itsDisplay, itsVisInfo->screen),
                                     itsVisInfo->visual, AllocNone);
            }
        }
    }

  return cmap;
}

void Togl::Impl::setupStackingOrder()
{
DOTRACE("Togl::Impl::setupStackingOrder");
  TkWindow *winPtr = (TkWindow *) itsTkWin;

  if (!(winPtr->flags & TK_TOP_LEVEL))
    {
      /*
       * If any siblings higher up in the stacking order have already
       * been created then move this window to its rightful position
       * in the stacking order.
       *
       * NOTE: this code ignores any changes anyone might have made
       * to the sibling and stack_mode field of the window's attributes,
       * so it really isn't safe for these to be manipulated except
       * by calling Tk_RestackWindow.
       */

      for (TkWindow* winPtr2 = winPtr->nextPtr; winPtr2 != NULL;
           winPtr2 = winPtr2->nextPtr)
        {
          if ((winPtr2->window != None) && !(winPtr2->flags & TK_TOP_LEVEL)) {
            XWindowChanges changes;
            changes.sibling = winPtr2->window;
            changes.stack_mode = Below;
            XConfigureWindow(winPtr->display, winPtr->window,
                             CWSibling|CWStackMode, &changes);
            break;
          }
        }

      /*
       * If this window has a different colormap than its parent, add
       * the window to the WM_COLORMAP_WINDOWS property for its top-level.
       */

      if ((winPtr->parentPtr != NULL) &&
          (winPtr->atts.colormap != winPtr->parentPtr->atts.colormap))
        {
          TkWmAddToColormapWindows(winPtr);
        }
    }
}

void Togl::Impl::setupOverlayIfNeeded()
{
DOTRACE("Togl::Impl::setupOverlayIfNeeded");
  if (itsOverlayFlag)
    {
      if (setupOverlay() == TCL_ERROR)
        {
          fprintf(stderr,"Warning: couldn't setup overlay.\n");
          itsOverlayFlag = 0;
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

  TkWindow *winPtr = (TkWindow *) itsTkWin;

  XVisualInfo* visinfo = glXChooseVisual( itsDisplay,
                                          DefaultScreen(itsDisplay),
                                          ovAttributeList );

  if (!visinfo)
    {
      Tcl_AppendResult(itsInterp,Tk_PathName(winPtr),
                       ": No suitable overlay index visual available",
                       (char *) NULL);
      itsOverlayCtx = 0;
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

  /*
    itsOverlayCtx = glXCreateContext( itsDisplay, visinfo, None, GL_TRUE );
  */
  /* NEW in Togl 1.5 beta 3 */
  /* share display lists with normal layer context */
  itsOverlayCtx = glXCreateContext( itsDisplay, visinfo,
                                    itsGLXContext, !itsIndirect );

  XSetWindowAttributes swa;
  swa.colormap = XCreateColormap( itsDisplay,
                                  RootWindow(itsDisplay, visinfo->screen),
                                  visinfo->visual, AllocNone );
  itsOverlayCmap = swa.colormap;

  swa.border_pixel = 0;
  swa.event_mask = ALL_EVENTS_MASK;
  itsOverlayWindow = XCreateWindow( itsDisplay, windowId(),
                                    0, 0,
                                    itsWidth, itsHeight, 0,
                                    visinfo->depth, InputOutput,
                                    visinfo->visual,
                                    CWBorderPixel|CWColormap|CWEventMask,
                                    &swa );

  int new_flag;
  Tcl_HashEntry* hPtr = Tcl_CreateHashEntry( &winPtr->dispPtr->winTable,
                                             (char *) itsOverlayWindow, &new_flag );

  Tcl_SetHashValue( hPtr, winPtr );

  /*   XMapWindow( itsDisplay, itsOverlayWindow );*/
  itsOverlayIsMapped = 0;

  /* Make sure window manager installs our colormap */
  XSetWMColormapWindows( itsDisplay, itsOverlayWindow, &itsOverlayWindow, 1 );

  return TCL_OK;
}

void Togl::Impl::issueConfigureNotify()
{
DOTRACE("Togl::Impl::issueConfigureNotify");

  /*
   * Issue a ConfigureNotify event if there were deferred configuration
   * changes (but skip it if the window is being deleted;  the
   * ConfigureNotify event could cause problems if we're being called
   * from Tk_DestroyWindow under some conditions).
   */
  TkWindow *winPtr = (TkWindow *) itsTkWin;

  if ((winPtr->flags & TK_NEED_CONFIG_NOTIFY)
      && !(winPtr->flags & TK_ALREADY_DEAD))
    {
      winPtr->flags &= ~TK_NEED_CONFIG_NOTIFY;

      XEvent event;

      event.type = ConfigureNotify;
      event.xconfigure.serial = LastKnownRequestProcessed(winPtr->display);
      event.xconfigure.send_event = False;
      event.xconfigure.display = winPtr->display;
      event.xconfigure.event = winPtr->window;
      event.xconfigure.window = winPtr->window;
      event.xconfigure.x = winPtr->changes.x;
      event.xconfigure.y = winPtr->changes.y;
      event.xconfigure.width = winPtr->changes.width;
      event.xconfigure.height = winPtr->changes.height;
      event.xconfigure.border_width = winPtr->changes.border_width;
      if (winPtr->changes.stack_mode == Above)
        {
          event.xconfigure.above = winPtr->changes.sibling;
        }
      else
        {
          event.xconfigure.above = None;
        }
      event.xconfigure.override_redirect = winPtr->atts.override_redirect;
      Tk_HandleEvent(&event);
    }
}

void Togl::Impl::checkDblBufferSnafu()
{
DOTRACE("Togl::Impl::checkDblBufferSnafu");
  int dbl_flag;
  if (glXGetConfig( itsDisplay, itsVisInfo, GLX_DOUBLEBUFFER, &dbl_flag ))
    {
      if (itsDoubleFlag==0 && dbl_flag)
        {
          /* We requested single buffering but had to accept a */
          /* double buffered visual.  Set the GL draw buffer to */
          /* be the front buffer to simulate single buffering. */
          glDrawBuffer( GL_FRONT );
        }
    }
}

///////////////////////////////////////////////////////////////////////
//
// Togl Tcl package
//
///////////////////////////////////////////////////////////////////////

namespace
{
  int ToglCmd(ClientData, Tcl_Interp *interp, int argc, char **argv)
  {
    DOTRACE("ToglCmd");
    if (argc <= 1)
      {
        return TCL_ERR(interp,
                       "wrong # args: should be \"pathName read filename\"");
      }

    /* Create Togl data structure */
    try
      {
        new Togl(interp, argv[1], argc-2, const_cast<const char**>(argv+2));
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

  Tcl_CreateCommand(interp, "togl", ToglCmd, (ClientData) 0, NULL);

  Tcl_InitHashTable(&CommandTable, TCL_STRING_KEYS);

  return TCL_OK;
}

static const char vcid_togl_cc[] = "$Header$";
#endif // !TOGL_CC_DEFINED
