///////////////////////////////////////////////////////////////////////
//
// togl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 23 13:11:59 2000
// written: Thu May 25 00:50:26 2000
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

#include "tcl/tclpkg.h"

#include <iostream.h>

// Currently support only X11
#define X11

// Standard C headers
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// X Window System headers
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>  /* for XA_RGB_DEFAULT_MAP atom */

#if defined(__vms)
#  include <X11/StdCmap.h>  /* for XmuLookupStandardColormap */
#else
#  include <X11/Xmu/StdCmap.h>  /* for XmuLookupStandardColormap */
#endif

#include <GL/glx.h>

// Tcl/Tk headers
#ifndef _TKPORT
#  define _TKPORT  // This eliminates need to include a bunch of Tk baggage
#endif
#include <tcl.h>
#include <tk.h>
#include "tkInt.h"

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"


namespace {

  // Defaults
  char*  DEFAULT_WIDTH     = "400";
  char*  DEFAULT_HEIGHT    = "400";
  char*  DEFAULT_IDENT     = "";
  char*  DEFAULT_FONTNAME  = "fixed";
  char*  DEFAULT_TIME      = "1";

  template <class T>
  inline T MAX(const T& a, const T& b)
	 { return (a>b) ? a : b; }

  inline int TCL_ERR(Tcl_Interp* interp, const char* msg)
	 {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp, msg, NULL);
		return TCL_ERROR;
	 }
}

#define ALL_EVENTS_MASK \
KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|	\
EnterWindowMask|LeaveWindowMask|PointerMotionMask|ExposureMask|	\
VisibilityChangeMask|FocusChangeMask|PropertyChangeMask|ColormapChangeMask


class Togl
{
public:
  Togl(Tcl_Interp* interp, Tk_Window mainwin, int argc, char** argv);
  virtual ~Togl();

  int configure(Tcl_Interp* interp, int argc, char* argv[], int flags);

  void makeCurrent() const;

  static void dummyDestroyProc(char* clientData);
  static void dummyEventProc(ClientData clientData, XEvent* eventPtr);
  static void dummyWidgetCmdDeletedProc(ClientData clientData);
  static void dummyTimerCallback(ClientData clientData);
  static void dummyRenderCallback(ClientData clientData);
  static void dummyOverlayRenderCallback(ClientData clientData);

  static Togl* findTogl(const char* ident);

  void setDisplayFunc(Togl_Callback* proc)
	 { itsDisplayProc = proc; }

  void setReshapeFunc(Togl_Callback* proc)
	 { itsReshapeProc = proc; }

  void setDestroyFunc(Togl_Callback* proc)
	 { itsDestroyProc = proc; }

  void postRedisplay()
	 {
		if (!itsUpdatePending) {
		  Tk_DoWhenIdle( dummyRenderCallback, static_cast<ClientData>(this) );
		  itsUpdatePending = GL_TRUE;
		}
	 }

  void swapBuffers() const
	 {
		if (itsDoubleFlag) {
		  glXSwapBuffers( itsDisplay, Tk_WindowId(itsTkWin) );
		}
		else {
		  glFlush();
		}
	 }

  char* ident() const { return itsIdent; }
  int width() const { return itsWidth; }
  int height() const { return itsHeight; }
  Tcl_Interp* interp() const { return itsInterp; }
  Tk_Window tkWin() const { return itsTkWin; }

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
  void setupEpsMaps();
  void freeEpsMaps();

  void addSelfToList();
  void removeSelfFromList();

public:

  Togl* itsNext;           /* next in linked list */

  GLXContext itsGLXContext;		/* Normal planes GLX context */
  Display* itsDisplay;		/* X's token for the window's display. */
  Tk_Window  itsTkWin;		/* Tk window structure */
  Tcl_Interp* itsInterp;		/* Tcl interpreter */
  Tcl_Command itsWidgetCmd;       /* Token for togl's widget command */
#ifndef NO_TK_CURSOR
  Tk_Cursor itsCursor;		/* The widget's cursor */
#endif
  int itsWidth;
  int itsHeight;		/* Dimensions of window */
  int itsTime;			/* Time value for timer */
  Tcl_TimerToken itsTimerHandler; /* Token for togl's timer handler */
  int itsRgbaFlag;		/* configuration flags (ala GLX parameters) */
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
  char* itsShareList;             /* name (ident) of Togl to share dlists with */
  char* itsShareContext;          /* name (ident) to share OpenGL context with */

  char* itsIdent;				/* User's identification string */
  ClientData itsClientData;		/* Pointer to user data */

  GLboolean itsUpdatePending;		/* Should normal planes be redrawn? */

  Togl_Callback* itsDisplayProc;		/* Callback when widget is rendered */
  Togl_Callback* itsReshapeProc;		/* Callback when window size changes */
  Togl_Callback* itsDestroyProc;		/* Callback when widget is destroyed */
  Togl_Callback* itsTimerProc;		/* Callback when widget is idle */

  /* Overlay stuff */
  GLXContext itsOverlayCtx;		/* Overlay planes OpenGL context */
  Window itsOverlayWindow;		/* The overlay window, or 0 */
  Togl_Callback* itsOverlayDisplayProc;	/* Overlay redraw proc */
  GLboolean itsOverlayUpdatePending;	/* Should overlay be redrawn? */
  Colormap itsOverlayCmap;		/* colormap for overlay is created */
  int itsOverlayTransparentPixel;		/* transparent pixel */
  int itsOverlayIsMapped;

  /* for DumpToEpsFile: Added by Miguel A. de Riera Pasenau 10.01.1997 */
  XVisualInfo* itsVisInfo;		/* Visual info of the current */
  /* context needed for DumpToEpsFile */
  GLfloat* itsEpsRedMap;		/* Index2RGB Maps for Color index modes */
  GLfloat* itsEpsGreenMap;
  GLfloat* itsEpsBlueMap;
  GLint itsEpsMapSize;            	/* = Number of indices in our Togl */
};


/* NTNTNT need to change to handle Windows Data Types */
/*
 * Prototypes for functions local to this file
 */
#ifdef MESA_COLOR_HACK
static int get_free_color_cells( Display *display, int screen,
                                 Colormap colormap);
static void free_default_color_cells( Display *display, Colormap colormap);
#endif



#if defined(__sgi) && defined(STEREO)
/* SGI-only stereo */
static void stereoMakeCurrent( Display *dpy, Window win, GLXContext ctx );
static void stereoInit( Togl* togl,int stereoEnabled );
#endif



/*
 * Setup Togl widget configuration options:
 */

static Tk_ConfigSpec configSpecs[] = {
  {TK_CONFIG_PIXELS, "-height", "height", "Height",
	DEFAULT_HEIGHT, Tk_Offset(Togl, itsHeight), 0, NULL},

  {TK_CONFIG_PIXELS, "-width", "width", "Width",
	DEFAULT_WIDTH, Tk_Offset(Togl, itsWidth), 0, NULL},

  {TK_CONFIG_BOOLEAN, "-rgba", "rgba", "Rgba",
	"true", Tk_Offset(Togl, itsRgbaFlag), 0, NULL},

  {TK_CONFIG_INT, "-redsize", "redsize", "RedSize",
	"1", Tk_Offset(Togl, itsRgbaRed), 0, NULL},

  {TK_CONFIG_INT, "-greensize", "greensize", "GreenSize",
	"1", Tk_Offset(Togl, itsRgbaGreen), 0, NULL},

  {TK_CONFIG_INT, "-bluesize", "bluesize", "BlueSize",
	"1", Tk_Offset(Togl, itsRgbaBlue), 0, NULL},

  {TK_CONFIG_BOOLEAN, "-double", "double", "Double",
	"false", Tk_Offset(Togl, itsDoubleFlag), 0, NULL},

  {TK_CONFIG_BOOLEAN, "-depth", "depth", "Depth",
	"false", Tk_Offset(Togl, itsDepthFlag), 0, NULL},

  {TK_CONFIG_INT, "-depthsize", "depthsize", "DepthSize",
	"1", Tk_Offset(Togl, itsDepthSize), 0, NULL},

  {TK_CONFIG_BOOLEAN, "-accum", "accum", "Accum",
	"false", Tk_Offset(Togl, itsAccumFlag), 0, NULL},

  {TK_CONFIG_INT, "-accumredsize", "accumredsize", "AccumRedSize",
	"1", Tk_Offset(Togl, itsAccumRed), 0, NULL},

  {TK_CONFIG_INT, "-accumgreensize", "accumgreensize", "AccumGreenSize",
	"1", Tk_Offset(Togl, itsAccumGreen), 0, NULL},

  {TK_CONFIG_INT, "-accumbluesize", "accumbluesize", "AccumBlueSize",
	"1", Tk_Offset(Togl, itsAccumBlue), 0, NULL},

  {TK_CONFIG_INT, "-accumalphasize", "accumalphasize", "AccumAlphaSize",
	"1", Tk_Offset(Togl, itsAccumAlpha), 0, NULL},

  {TK_CONFIG_BOOLEAN, "-alpha", "alpha", "Alpha",
	"false", Tk_Offset(Togl, itsAlphaFlag), 0, NULL},

  {TK_CONFIG_INT, "-alphasize", "alphasize", "AlphaSize",
	"1", Tk_Offset(Togl, itsAlphaSize), 0, NULL},

  {TK_CONFIG_BOOLEAN, "-stencil", "stencil", "Stencil",
	"false", Tk_Offset(Togl, itsStencilFlag), 0, NULL},

  {TK_CONFIG_INT, "-stencilsize", "stencilsize", "StencilSize",
	"1", Tk_Offset(Togl, itsStencilSize), 0, NULL},

  {TK_CONFIG_INT, "-auxbuffers", "auxbuffers", "AuxBuffers",
	"0", Tk_Offset(Togl, itsAuxNumber), 0, NULL},

  {TK_CONFIG_BOOLEAN, "-privatecmap", "privateCmap", "PrivateCmap",
	"false", Tk_Offset(Togl, itsPrivateCmapFlag), 0, NULL},

  {TK_CONFIG_BOOLEAN, "-overlay", "overlay", "Overlay",
	"false", Tk_Offset(Togl, itsOverlayFlag), 0, NULL},

  {TK_CONFIG_BOOLEAN, "-stereo", "stereo", "Stereo",
	"false", Tk_Offset(Togl, itsStereoFlag), 0, NULL},

#ifndef NO_TK_CURSOR
  { TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	 "", Tk_Offset(Togl, itsCursor), TK_CONFIG_NULL_OK },
#endif

  {TK_CONFIG_INT, "-time", "time", "Time",
	DEFAULT_TIME, Tk_Offset(Togl, itsTime), 0, NULL},

  {TK_CONFIG_STRING, "-sharelist", "sharelist", "ShareList",
	NULL, Tk_Offset(Togl, itsShareList), 0, NULL},

  {TK_CONFIG_STRING, "-sharecontext", "sharecontext", "ShareContext",
	NULL, Tk_Offset(Togl, itsShareContext), 0, NULL},

  {TK_CONFIG_STRING, "-ident", "ident", "Ident",
	DEFAULT_IDENT, Tk_Offset(Togl, itsIdent), 0, NULL},

  {TK_CONFIG_BOOLEAN, "-indirect", "indirect", "Indirect",
	"false", Tk_Offset(Togl, itsIndirect), 0, NULL},

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

static Togl* ToglHead = NULL;  /* head of linked list */


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
  if (visinfo->visual==DefaultVisual(dpy,scrnum)) {
	 /* use the default/root colormap */
	 Colormap cmap;
	 cmap = DefaultColormap( dpy, scrnum );
#ifdef MESA_COLOR_HACK
	 (void) get_free_color_cells( dpy, scrnum, cmap);
#endif
	 return cmap;
  }

  /*
	* Check if we're using Mesa.
	*/
  int using_mesa;
  if (strstr(glXQueryServerString( dpy, scrnum, GLX_VERSION ), "Mesa")) {
	 using_mesa = 1;
  }
  else {
	 using_mesa = 0;
  }

  /*
	* Next, if we're using Mesa and displaying on an HP with the "Color
	* Recovery" feature and the visual is 8-bit TrueColor, search for a
	* special colormap initialized for dithering.  Mesa will know how to
	* dither using this colormap.
	*/
  if (using_mesa) {
	 Atom hp_cr_maps = XInternAtom( dpy, "_HP_RGB_SMOOTH_MAP_LIST", True );
	 if (hp_cr_maps
		  && visinfo->visual->c_class==TrueColor
		  && visinfo->depth==8) {
		int numCmaps;
		XStandardColormap *standardCmaps;
		Status status = XGetRGBColormaps( dpy, root, &standardCmaps,
													 &numCmaps, hp_cr_maps );
		if (status) {
		  for (int i=0; i<numCmaps; i++) {
	       if (standardCmaps[i].visualid == visinfo->visual->visualid) {
				Colormap cmap = standardCmaps[i].colormap;
				XFree( standardCmaps );
				return cmap;
	       }
		  }
		  XFree(standardCmaps);
		}
	 }
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
  if (status == 1) {
	 int numCmaps;
	 XStandardColormap *standardCmaps;
	 Status status = XGetRGBColormaps( dpy, root, &standardCmaps,
												  &numCmaps, XA_RGB_DEFAULT_MAP);
	 if (status == 1) {
		for (int i = 0; i < numCmaps; i++) {
		  if (standardCmaps[i].visualid == visinfo->visualid) {
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



/*
 * Register a C function to be called when an Togl widget is realized.
 */
void Togl_CreateFunc( Togl_Callback *proc )
{
DOTRACE("<togl.cc>::Togl_CreateFunc");
  DefaultCreateProc = proc;
}


/*
 * Register a C function to be called when an Togl widget must be redrawn.
 */
void Togl_DisplayFunc( Togl_Callback *proc )
{
DOTRACE("<togl.cc>::Togl_DisplayFunc");
  DefaultDisplayProc = proc;
}


/*
 * Register a C function to be called when an Togl widget is resized.
 */
void Togl_ReshapeFunc( Togl_Callback *proc )
{
DOTRACE("<togl.cc>::Togl_ReshapeFunc");
  DefaultReshapeProc = proc;
}


/*
 * Register a C function to be called when an Togl widget is destroyed.
 */
void Togl_DestroyFunc( Togl_Callback *proc )
{
DOTRACE("<togl.cc>::Togl_DestroyFunc");
  DefaultDestroyProc = proc;
}


/*
 * Register a C function to be called from TimerEventHandler.
 */
void Togl_TimerFunc( Togl_Callback *proc )
{
DOTRACE("<togl.cc>::Togl_TimerFunc");
  DefaultTimerProc = proc;
}


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


/*
 * Change the display/redraw callback for a specific Togl widget.
 */
void Togl_SetDisplayFunc( Togl* togl, Togl_Callback *proc )
{
DOTRACE("<togl.cc>::Togl_SetDisplayFunc");
  togl->setDisplayFunc(proc);
}


/*
 * Change the reshape callback for a specific Togl widget.
 */
void Togl_SetReshapeFunc( Togl* togl, Togl_Callback *proc )
{
DOTRACE("<togl.cc>::Togl_SetReshapeFunc");
  togl->setReshapeFunc(proc);
}


/*
 * Change the destroy callback for a specific Togl widget.
 */
void Togl_SetDestroyFunc( Togl* togl, Togl_Callback *proc )
{
DOTRACE("<togl.cc>::Togl_SetDestroyFunc");
  togl->setDestroyFunc(proc);
}


/*
 * Togl_CreateCommand
 *
 *   Declares a new C sub-command of Togl callable from Tcl.
 *   Every time the sub-command is called from Tcl, the
 *   C routine will be called with all the arguments from Tcl.
 */
void Togl_CreateCommand( char *cmd_name, Togl_CmdProc *cmd_proc)
{
DOTRACE("<togl.cc>::Togl_CreateCommand");
  int new_item;
  Tcl_HashEntry *entry =
	 Tcl_CreateHashEntry(&CommandTable, cmd_name, &new_item);
  Tcl_SetHashValue(entry, cmd_proc);
}


void Togl_MakeCurrent( const Togl* togl )
{
  togl->makeCurrent();
}

/*
 * It's possible to change with this function or in a script some
 * options like RGBA - ColorIndex ; Z-buffer and so on
 */
int Togl_Configure(Tcl_Interp *interp, Togl* togl,
                   int argc, char *argv[], int flags)
{
  togl->configure(interp, argc, argv, flags);
}


int Togl_Widget(ClientData clientData, Tcl_Interp *interp,
					 int argc, char *argv[])
{
DOTRACE("<togl.cc>::Togl_Widget");
  Togl* togl = (Togl*)clientData;
  int result = TCL_OK;

  if (argc < 2) {
	 Tcl_AppendResult(interp, "wrong # args: should be \"",
							argv[0], " ?options?\"", NULL);
	 return TCL_ERROR;
  }

  Tk_Preserve((ClientData)togl);

  if (!strncmp(argv[1], "configure", MAX((unsigned int)1, strlen(argv[1])))) {
	 if (argc == 2) {
		/* Return list of all configuration parameters */
		result = Tk_ConfigureInfo(interp, Togl_TkWin(togl), configSpecs,
										  (char *)togl, (char *)NULL, 0);
	 }
	 else if (argc == 3) {
		if (strcmp(argv[2],"-extensions")==0) {
		  /* Return a list of OpenGL extensions available */
		  char *extensions;
		  extensions = (char *) glGetString(GL_EXTENSIONS);
		  Tcl_SetResult( interp, extensions, TCL_STATIC );
		  result = TCL_OK;
		}
		else {
		  /* Return a specific configuration parameter */
		  result = Tk_ConfigureInfo(interp, Togl_TkWin(togl), configSpecs,
											 (char *)togl, argv[2], 0);
		}
	 }
	 else {
		/* Execute a configuration change */
		result = togl->configure(interp, argc-2, argv+2, TK_CONFIG_ARGV_ONLY);
	 }
  }
  else if (!strncmp(argv[1], "render", MAX((unsigned int)1, strlen(argv[1])))) {
	 /* force the widget to be redrawn */
	 Togl::dummyRenderCallback(static_cast<ClientData>(togl));
  }
  else if (!strncmp(argv[1], "swapbuffers", MAX((unsigned int)1, strlen(argv[1])))) {
	 /* force the widget to be redrawn */
	 Togl_SwapBuffers(togl);
  }
  else if (!strncmp(argv[1], "makecurrent", MAX((unsigned int)1, strlen(argv[1])))) {
	 /* force the widget to be redrawn */
	 Togl_MakeCurrent(togl);
  }
  else {
	 /* Probably a user-defined function */
	 Tcl_HashEntry* entry = Tcl_FindHashEntry(&CommandTable, argv[1]);
	 if (entry != NULL) {
		Togl_CmdProc* cmd_proc = (Togl_CmdProc *)Tcl_GetHashValue(entry);
		result = cmd_proc(togl, argc, argv);
	 }
	 else {
		Tcl_AppendResult(interp, "Togl: Unknown option: ", argv[1], "\n",
							  "Try: configure or render\n",
							  "or one of the user-defined commands:\n",
							  NULL);
		Tcl_HashSearch search;
		entry = Tcl_FirstHashEntry(&CommandTable, &search);
		while (entry) {
		  Tcl_AppendResult(interp, "  ",
								 Tcl_GetHashKey(&CommandTable, entry),
								 "\n", NULL);
		  entry = Tcl_NextHashEntry(&search);
		}
		result = TCL_ERROR;
	 }
  }

  Tk_Release((ClientData)togl);
  return result;
}


void Togl_PostRedisplay( Togl* togl )
{
DOTRACE("<togl.cc>::Togl_PostRedisplay");
  togl->postRedisplay();
}


void Togl_SwapBuffers( const Togl* togl )
{
DOTRACE("<togl.cc>::Togl_SwapBuffers");
  togl->swapBuffers();
}


char* Togl_Ident( const Togl* togl )
{
DOTRACE("<togl.cc>::Togl_Ident");
  return togl->ident();
}


int Togl_Width( const Togl* togl )
{
DOTRACE("<togl.cc>::Togl_Width");
  return togl->width();
}


int Togl_Height( const Togl* togl )
{
DOTRACE("<togl.cc>::Togl_Height");
  return togl->height();
}


Tcl_Interp* Togl_Interp( const Togl* togl )
{
DOTRACE("<togl.cc>::Togl_Interp");
  return togl->interp();
}


Tk_Window Togl_TkWin( const Togl* togl )
{
DOTRACE("<togl.cc>::Togl_TkWin");
  return togl->tkWin();
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
  XColor *ctable, subColor;
  int i, bestmatch;
  double mindist;       /* 3*2^16^2 exceeds long int precision.
								 */

  /* First try just using XAllocColor. */
  if (XAllocColor(dpy, cmap, color)) {
	 *exact = 1;
	 return;
  }

  /* Retrieve color table entries. */
  /* XXX alloca candidate. */
  ctable = (XColor *) malloc(cmapSize * sizeof(XColor));
  for (i = 0; i < cmapSize; i++) {
	 ctable[i].pixel = i;
  }
  XQueryColors(dpy, cmap, ctable, cmapSize);

  /* Find best match. */
  bestmatch = -1;
  mindist = 0.0;
  for (i = 0; i < cmapSize; i++) {
	 double dr = (double) color->red - (double) ctable[i].red;
	 double dg = (double) color->green - (double) ctable[i].green;
	 double db = (double) color->blue - (double) ctable[i].blue;
	 double dist = dr * dr + dg * dg + db * db;
	 if (bestmatch < 0 || dist < mindist) {
		bestmatch = i;
		mindist = dist;
	 }
  }

  /* Return result. */
  subColor.red = ctable[bestmatch].red;
  subColor.green = ctable[bestmatch].green;
  subColor.blue = ctable[bestmatch].blue;
  free(ctable);
  /* Try to allocate the closest match color.  This should only
	* fail if the cell is read/write.  Otherwise, we're incrementing
	* the cell's reference count.
	*/
  if (!XAllocColor(dpy, cmap, &subColor)) {
	 /* do this to work around a problem reported by Frank Ortega */
	 subColor.pixel = (unsigned long) bestmatch;
	 subColor.red   = ctable[bestmatch].red;
	 subColor.green = ctable[bestmatch].green;
	 subColor.blue  = ctable[bestmatch].blue;
	 subColor.flags = DoRed | DoGreen | DoBlue;
  }
  *color = subColor;
}


/// XXX

unsigned long Togl_AllocColor( const Togl* togl,
                               float red, float green, float blue )
{
DOTRACE("<togl.cc>::Togl_AllocColor");
  XColor xcol;
  int exact;

  if (togl->itsRgbaFlag) {
	 fprintf(stderr,"Error: Togl_AllocColor illegal in RGBA mode.\n");
	 return 0;
  }
  /* TODO: maybe not... */
  if (togl->itsPrivateCmapFlag) {
	 fprintf(stderr,"Error: Togl_FreeColor illegal with private colormap\n");
	 return 0;
  }

  xcol.red   = (short) (red   * 65535.0);
  xcol.green = (short) (green * 65535.0);
  xcol.blue  = (short) (blue  * 65535.0);

  noFaultXAllocColor( Tk_Display(togl->itsTkWin), Tk_Colormap(togl->itsTkWin),
							 Tk_Visual(togl->itsTkWin)->map_entries, &xcol, &exact );


  /* for EPS output */
  togl->itsEpsRedMap[ xcol.pixel] = xcol.red / 65535.0;
  togl->itsEpsGreenMap[ xcol.pixel] = xcol.green / 65535.0;
  togl->itsEpsBlueMap[ xcol.pixel] = xcol.blue / 65535.0;

  return xcol.pixel;
}



void Togl_FreeColor( const Togl* togl, unsigned long pixel )
{
DOTRACE("<togl.cc>::Togl_FreeColor");
  if (togl->itsRgbaFlag) {
	 fprintf(stderr,"Error: Togl_AllocColor illegal in RGBA mode.\n");
	 return;
  }
  /* TODO: maybe not... */
  if (togl->itsPrivateCmapFlag) {
	 fprintf(stderr,"Error: Togl_FreeColor illegal with private colormap\n");
	 return;
  }

  XFreeColors( Tk_Display(togl->itsTkWin), Tk_Colormap(togl->itsTkWin),
					&pixel, 1, 0 );
}



void Togl_SetColor( const Togl* togl,
                    unsigned long index, float red, float green, float blue )
{
DOTRACE("<togl.cc>::Togl_SetColor");
  XColor xcol;

  if (togl->itsRgbaFlag) {
	 fprintf(stderr,"Error: Togl_AllocColor illegal in RGBA mode.\n");
	 return;
  }
  if (!togl->itsPrivateCmapFlag) {
	 fprintf(stderr,"Error: Togl_SetColor requires a private colormap\n");
	 return;
  }

  xcol.pixel = index;
  xcol.red   = (short) (red   * 65535.0);
  xcol.green = (short) (green * 65535.0);
  xcol.blue  = (short) (blue  * 65535.0);
  xcol.flags = DoRed | DoGreen | DoBlue;

  XStoreColor( Tk_Display(togl->itsTkWin), Tk_Colormap(togl->itsTkWin), &xcol );

  /* for EPS output */
  togl->itsEpsRedMap[ xcol.pixel] = xcol.red / 65535.0;
  togl->itsEpsGreenMap[ xcol.pixel] = xcol.green / 65535.0;
  togl->itsEpsBlueMap[ xcol.pixel] = xcol.blue / 65535.0;
}



#define MAX_FONTS 1000
static GLuint ListBase[MAX_FONTS];
static GLuint ListCount[MAX_FONTS];



/*
 * Load the named bitmap font as a sequence of bitmaps in a display list.
 * fontname may be one of the predefined fonts like TOGL_BITMAP_8_BY_13
 * or an X font name, or a Windows font name, etc.
 */
GLuint Togl_LoadBitmapFont( const Togl* togl, const char *fontname )
{
DOTRACE("<togl.cc>::Togl_LoadBitmapFont");
  static int FirstTime = 1;
  XFontStruct *fontinfo;
  int first, last, count;
  GLuint fontbase;
  const char *name;

  /* Initialize the ListBase and ListCount arrays */
  if (FirstTime) {
	 int i;
	 for (i=0;i<MAX_FONTS;i++) {
		ListBase[i] = ListCount[i] = 0;
	 }
	 FirstTime = 0;
  }

  /*
	* This method of selecting X fonts according to a TOGL_ font name
	* is a kludge.  To be fixed when I find time...
	*/
  if (fontname==TOGL_BITMAP_8_BY_13) {
	 name = "8x13";
  }
  else if (fontname==TOGL_BITMAP_9_BY_15) {
	 name = "9x15";
  }
  else if (fontname==TOGL_BITMAP_TIMES_ROMAN_10) {
	 name = "-adobe-times-medium-r-normal--10-100-75-75-p-54-iso8859-1";
  }
  else if (fontname==TOGL_BITMAP_TIMES_ROMAN_24) {
	 name = "-adobe-times-medium-r-normal--24-240-75-75-p-124-iso8859-1";
  }
  else if (fontname==TOGL_BITMAP_HELVETICA_10) {
	 name = "-adobe-helvetica-medium-r-normal--10-100-75-75-p-57-iso8859-1";
  }
  else if (fontname==TOGL_BITMAP_HELVETICA_12) {
	 name = "-adobe-helvetica-medium-r-normal--12-120-75-75-p-67-iso8859-1";
  }
  else if (fontname==TOGL_BITMAP_HELVETICA_18) {
	 name = "-adobe-helvetica-medium-r-normal--18-180-75-75-p-98-iso8859-1";
  }
  else if (!fontname) {
	 name = DEFAULT_FONTNAME;
  }
  else {
	 name = (const char *) fontname;
  }

  assert( name );

  fontinfo = XLoadQueryFont( Tk_Display(togl->itsTkWin), name );
  if (!fontinfo) {
	 return 0;
  }

  first = fontinfo->min_char_or_byte2;
  last = fontinfo->max_char_or_byte2;
  count = last-first+1;

  fontbase = glGenLists( (GLuint) (last+1) );
  if (fontbase==0) {
	 return 0;
  }
  glXUseXFont( fontinfo->fid, first, count, (int) fontbase+first );

  /* Record the list base and number of display lists
	* for Togl_UnloadBitmapFont().
	*/
  {
	 int i;
	 for (i=0;i<MAX_FONTS;i++) {
		if (ListBase[i]==0) {
		  ListBase[i] = fontbase;
		  ListCount[i] = last+1;
		  break;
		}
	 }
  }

  return fontbase;
}



/*
 * Release the display lists which were generated by Togl_LoadBitmapFont().
 */
void Togl_UnloadBitmapFont( const Togl* togl, GLuint fontbase )
{
DOTRACE("<togl.cc>::Togl_UnloadBitmapFont");
  int i;
  (void) togl;
  for (i=0;i<MAX_FONTS;i++) {
	 if (ListBase[i]==fontbase) {
		glDeleteLists( ListBase[i], ListCount[i] );
		ListBase[i] = ListCount[i] = 0;
		return;
	 }
  }
}


/*
 * Overlay functions
 */


void Togl_UseLayer( Togl* togl, int layer )
{
DOTRACE("<togl.cc>::Togl_UseLayer");
  if (togl->itsOverlayWindow) {
	 if (layer==TOGL_OVERLAY) {
		glXMakeCurrent( Tk_Display(togl->itsTkWin),
							 togl->itsOverlayWindow,
							 togl->itsOverlayCtx );
#if defined(__sgi) && defined(STEREO)
		stereoMakeCurrent( Tk_Display(togl->itsTkWin),
								 togl->itsOverlayWindow,
								 togl->itsOverlayCtx );
#endif /* __sgi STEREO */
	 }
	 else if (layer==TOGL_NORMAL) {
		glXMakeCurrent( Tk_Display(togl->itsTkWin),
							 Tk_WindowId(togl->itsTkWin),
							 togl->itsGLXContext );
#if defined(__sgi) && defined(STEREO)
		stereoMakeCurrent( Tk_Display(togl->itsTkWin),
								 Tk_WindowId(togl->itsTkWin),
								 togl->itsGLXContext );
#endif /* __sgi STEREO */
	 }
	 else {
		/* error */
	 }
  }
}


void Togl_ShowOverlay( Togl* togl )
{
DOTRACE("<togl.cc>::Togl_ShowOverlay");
  if (togl->itsOverlayWindow) {
	 XMapWindow( Tk_Display(togl->itsTkWin), togl->itsOverlayWindow );
	 XInstallColormap(Tk_Display(togl->itsTkWin),togl->itsOverlayCmap);
	 togl->itsOverlayIsMapped = 1;
  }
}



void Togl_HideOverlay( Togl* togl )
{
DOTRACE("<togl.cc>::Togl_HideOverlay");
  if (togl->itsOverlayWindow && togl->itsOverlayIsMapped) {
	 XUnmapWindow( Tk_Display(togl->itsTkWin), togl->itsOverlayWindow );
	 togl->itsOverlayIsMapped=0;
  }
}



void Togl_PostOverlayRedisplay( Togl* togl )
{
DOTRACE("<togl.cc>::Togl_PostOverlayRedisplay");
  if (!togl->itsOverlayUpdatePending
		&& togl->itsOverlayWindow && togl->itsOverlayDisplayProc) {
	 Tk_DoWhenIdle( Togl::dummyOverlayRenderCallback,
						 static_cast<ClientData>(togl) );
	 togl->itsOverlayUpdatePending = 1;
  }
}


void Togl_OverlayDisplayFunc( Togl_Callback *proc )
{
DOTRACE("<togl.cc>::Togl_OverlayDisplayFunc");
  DefaultOverlayDisplayProc = proc;
}


int Togl_ExistsOverlay( const Togl* togl )
{
DOTRACE("<togl.cc>::Togl_ExistsOverlay");
  return togl->itsOverlayFlag;
}


int Togl_GetOverlayTransparentValue( const Togl* togl )
{
DOTRACE("<togl.cc>::Togl_GetOverlayTransparentValue");
  return togl->itsOverlayTransparentPixel;
}


int Togl_IsMappedOverlay( const Togl* togl )
{
DOTRACE("<togl.cc>::Togl_IsMappedOverlay");
  return togl->itsOverlayFlag && togl->itsOverlayIsMapped;
}


unsigned long Togl_AllocColorOverlay( const Togl* togl,
                                      float red, float green, float blue )
{
DOTRACE("<togl.cc>::Togl_AllocColorOverlay");
  if (togl->itsOverlayFlag && togl->itsOverlayCmap) {
	 XColor xcol;
	 xcol.red   = (short) (red* 65535.0);
	 xcol.green = (short) (green* 65535.0);
	 xcol.blue  = (short) (blue* 65535.0);
	 if (!XAllocColor(Tk_Display(togl->itsTkWin),togl->itsOverlayCmap,&xcol))
		return (unsigned long) -1;
	 return xcol.pixel;
  }
  else {
	 return (unsigned long) -1;
  }
}


void Togl_FreeColorOverlay( const Togl* togl, unsigned long pixel )
{
DOTRACE("<togl.cc>::Togl_FreeColorOverlay");

  if (togl->itsOverlayFlag && togl->itsOverlayCmap) {
	 XFreeColors( Tk_Display(togl->itsTkWin), togl->itsOverlayCmap,
					  &pixel, 1, 0 );
  }
}



/*
 * User client data
 */

void Togl_ClientData( ClientData clientData )
{
DOTRACE("<togl.cc>::Togl_ClientData");
  DefaultClientData = clientData;
}


ClientData Togl_GetClientData( const Togl* togl )
{
DOTRACE("<togl.cc>::Togl_GetClientData");
  return togl->itsClientData;
}


void Togl_SetClientData( Togl* togl, ClientData clientData )
{
DOTRACE("<togl.cc>::Togl_SetClientData");
  togl->itsClientData = clientData;
}



/*
 * X11-only functions
 * Contributed by Miguel A. De Riera Pasenau (miguel@DALILA.UPC.ES)
 */

Display* Togl_Display( const Togl* togl)
{
DOTRACE("<togl.cc>::Togl_Display");
  return Tk_Display(togl->itsTkWin);
}

Screen* Togl_Screen( const Togl* togl)
{
DOTRACE("<togl.cc>::Togl_Screen");
  return Tk_Screen(togl->itsTkWin);
}

int Togl_ScreenNumber( const Togl* togl)
{
DOTRACE("<togl.cc>::Togl_ScreenNumber");
  return Tk_ScreenNumber(togl->itsTkWin);
}

Colormap Togl_Colormap( const Togl* togl)
{
DOTRACE("<togl.cc>::Togl_Colormap");
  return Tk_Colormap(togl->itsTkWin);
}

Window Togl_Window( const Togl* togl)
{
DOTRACE("<togl.cc>::Togl_Window");
  return Tk_WindowId(togl->itsTkWin);
}

#ifdef MESA_COLOR_HACK
/*
 * Let's know how many free colors do we have
 */
#if 0
static unsigned char rojo[] = { 4, 39, 74, 110, 145, 181, 216, 251},
  verde[] = { 4, 39, 74, 110, 145, 181, 216, 251},
  azul[] = { 4, 39, 74, 110, 145, 181, 216, 251};

unsigned char rojo[] = { 4, 36, 72, 109, 145, 182, 218, 251},
	 verde[] = { 4, 36, 72, 109, 145, 182, 218, 251},
  azul[] = { 4, 36, 72, 109, 145, 182, 218, 251};
azul[] = { 0, 85, 170, 255};
#endif

#define RLEVELS     5
#define GLEVELS     9
#define BLEVELS     5

/* to free dithered_rgb_colormap pixels allocated by Mesa */
static unsigned long *ToglMesaUsedPixelCells = NULL;
static int ToglMesaUsedFreeCells = 0;

static int get_free_color_cells( Display *display, int screen,
                                 Colormap colormap)
{
DOTRACE("<togl.cc>::get_free_color_cells");
  if ( !ToglMesaUsedPixelCells) {
	 XColor xcol;
	 int i;
	 int colorsfailed, ncolors = XDisplayCells( display, screen);

	 long r, g, b;

	 ToglMesaUsedPixelCells = ( unsigned long *)calloc( ncolors, sizeof( unsigned long));

	 /* Allocate X colors and initialize color_table[], red_table[], etc */
	 /* de Mesa 2.1: xmesa1.c setup_dithered_(...) */
	 i = colorsfailed = 0;
	 for (r = 0; r < RLEVELS; r++)
		for (g = 0; g < GLEVELS; g++)
		  for (b = 0; b < BLEVELS; b++) {
			 int exact;
			 xcol.red   = ( r*65535)/(RLEVELS-1);
			 xcol.green = ( g*65535)/(GLEVELS-1);
			 xcol.blue  = ( b*65535)/(BLEVELS-1);
			 noFaultXAllocColor( display, colormap, ncolors,
										&xcol, &exact );
			 ToglMesaUsedPixelCells[ i++] = xcol.pixel;
			 if (!exact) {
				colorsfailed++;
			 }
		  }
	 ToglMesaUsedFreeCells = i;

	 XFreeColors( display, colormap, ToglMesaUsedPixelCells,
					  ToglMesaUsedFreeCells, 0x00000000);
  }
  return ToglMesaUsedFreeCells;
}


static void free_default_color_cells( Display *display, Colormap colormap)
{
DOTRACE("<togl.cc>::free_default_color_cells");
  if ( ToglMesaUsedPixelCells) {
	 XFreeColors( display, colormap, ToglMesaUsedPixelCells,
					  ToglMesaUsedFreeCells, 0x00000000);
	 free( ( char *)ToglMesaUsedPixelCells);
	 ToglMesaUsedPixelCells = NULL;
	 ToglMesaUsedFreeCells = 0;
  }
}
#endif


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
  GLvoid *buffer;
  GLint swapbytes, lsbfirst, rowlength;
  GLint skiprows, skippixels, alignment;
  GLenum format;
  unsigned int size;

  if (inColor) {
	 format = GL_RGB;
	 size = width * height * 3;
  }
  else {
	 format = GL_COLOR_INDEX;
	 /* format = GL_LUMINANCE; */
	 size = width * height * 1;
  }

  buffer = (GLvoid *) malloc(size);
  if (buffer == NULL)
	 return NULL;

  /* Save current modes. */
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
  FILE *fp;
  GLvoid *pixels;
  unsigned char *curpix;
  unsigned int components, i;
  int pos;
  unsigned char bitpixel;

  pixels = grabPixels(inColor, width, height);
  if (pixels == NULL)
	 return 1;
  if (inColor)
	 components = 3;     /* Red, green, blue. */
  else
	 components = 1;     /* Luminance. */

  fp = fopen(filename, "w");
  if (fp == NULL) {
	 return 2;
  }
  fprintf(fp, "%%!PS-Adobe-2.0 EPSF-1.2\n");
  fprintf(fp, "%%%%Creator: OpenGL pixmap render output\n");
  fprintf(fp, "%%%%BoundingBox: 0 0 %d %d\n", width, height);
  fprintf(fp, "%%%%EndComments\n");

  i = ((( width * height) + 7) / 8 ) / 40; /* # of lines, 40 bytes per line */
  fprintf(fp, "%%%%BeginPreview: %d %d %d %d\n%%", width, height, 1, i);
  pos = 0;
  curpix = ( unsigned char *)pixels;
  for ( i = 0; i < width * height * components; ) {
	 bitpixel = 0;
	 if ( inColor) {
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
	 else {
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
	 if (++pos >= 40) {
		fprintf(fp, "\n%%");
		pos = 0;
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
  for (i = width * height * components; i > 0; i--) {
	 fprintf(fp, "%02hx", *curpix++);
	 if (++pos >= 40) {
		fprintf(fp, "\n");
		pos = 0;
	 }
  }
  if (pos)
	 fprintf(fp, "\n");

  fprintf(fp, "grestore\n");
  free(pixels);
  fclose(fp);
  return 0;
}


/* int Togl_DumpToEpsFile( const Togl* togl, const char *filename,
                        int inColor, void (*user_redraw)(void)) */
/* changed by GG */
int Togl_DumpToEpsFile( const Togl* togl, const char *filename,
                        int inColor, void (*user_redraw)( const Togl* ))
{
DOTRACE("<togl.cc>::Togl_DumpToEpsFile");
  int using_mesa = 0;
#if 0
  Pixmap eps_pixmap;
  GLXPixmap eps_glxpixmap;
  XVisualInfo *vi = togl->itsVisInfo;
  Window win = Tk_WindowId( togl->itsTkWin);
#endif
  Display *dpy = Tk_Display( togl->itsTkWin);
  int retval;
  int scrnum = Tk_ScreenNumber(togl->itsTkWin);
  unsigned int width = togl->itsWidth, height = togl->itsHeight;

  if (strstr(glXQueryServerString( dpy, scrnum, GLX_VERSION ), "Mesa"))
	 using_mesa = 1;
  else
	 using_mesa = 0;
  /* I don't use Pixmap do drawn into, because the code should link
	* with Mesa libraries and OpenGL libraries, and the which library
	* we use at run time should not matter, but the name of the calls
	* differs one from another:
	* MesaGl: glXCreateGLXPixmapMESA( dpy, vi, eps_pixmap, Tk_Colormap(togl->itsTkWin))
	* OpenGl: glXCreateGLXPixmap( dpy, vi, eps_pixmap);
	*
	* instead of this I read direct from back buffer of the screeen.
	*/
#if 0
  eps_pixmap = XCreatePixmap( dpy, win, width, height, vi->depth);
  if ( using_mesa) {
	 /* eps_glxpixmap = glXCreateGLXPixmapMESA( dpy, vi, eps_pixmap, 
		 Tk_Colormap(togl->itsTkWin)); */
  }
  else
	 eps_glxpixmap = glXCreateGLXPixmap( dpy, vi, eps_pixmap);

  glXMakeCurrent( dpy, eps_glxpixmap, togl->itsGLXContext);
  user_redraw();
#endif
  if ( !togl->itsRgbaFlag) {
	 glPixelMapfv( GL_PIXEL_MAP_I_TO_R, togl->itsEpsMapSize, togl->itsEpsRedMap);
	 glPixelMapfv( GL_PIXEL_MAP_I_TO_G, togl->itsEpsMapSize, togl->itsEpsGreenMap);
	 glPixelMapfv( GL_PIXEL_MAP_I_TO_B, togl->itsEpsMapSize, togl->itsEpsBlueMap);
  }
  /*  user_redraw(); */
  user_redraw(togl);  /* changed by GG */
  glReadBuffer( GL_FRONT);
  /* by default it read GL_BACK in double buffer mode*/
  glFlush();
  retval = generateEPS( filename, inColor, width, height);
#if 0
  glXMakeCurrent( dpy, win, togl->itsGLXContext );
  glXDestroyGLXPixmap( dpy, eps_glxpixmap);
  XFreePixmap( dpy, eps_pixmap);
#endif
  return retval;
}

/*
 * Full screen stereo for SGI graphics
 * Contributed by Ben Evans (Ben.Evans@anusf.anu.edu.au)
 * This code was based on SGI's /usr/share/src/OpenGL/teach/stereo
 */

#if defined(__sgi) && defined(STEREO)

static struct stereoStateRec {
  Bool        useSGIStereo;
  Display     *currentDisplay;
  Window      currentWindow;
  GLXContext  currentContext;
  GLenum      currentDrawBuffer;
  int         currentStereoBuffer;
  Bool        enabled;
  char        *stereoCommand;
  char        *restoreCommand;
} stereo;

/* call instead of glDrawBuffer */
void
Togl_StereoDrawBuffer(GLenum mode)
{
DOTRACE("<togl.cc>::Togl_StereoDrawBuffer");
  if (stereo.useSGIStereo) {
    stereo.currentDrawBuffer = mode;
    switch (mode) {
    case GL_FRONT:
    case GL_BACK:
    case GL_FRONT_AND_BACK:
      /*
      ** Simultaneous drawing to both left and right buffers isn't
      ** really possible if we don't have a stereo capable visual.
      ** For now just fall through and use the left buffer.
      */
    case GL_LEFT:
    case GL_FRONT_LEFT:
    case GL_BACK_LEFT:
      stereo.currentStereoBuffer = STEREO_BUFFER_LEFT;
      break;
    case GL_RIGHT:
    case GL_FRONT_RIGHT: 
      stereo.currentStereoBuffer = STEREO_BUFFER_RIGHT;
      mode = GL_FRONT;
      break;
    case GL_BACK_RIGHT:
      stereo.currentStereoBuffer = STEREO_BUFFER_RIGHT;
      mode = GL_BACK;
      break;
    default:
      break;
    }
    if (stereo.currentDisplay && stereo.currentWindow) {
      glXWaitGL();  /* sync with GL command stream before calling X */
      XSGISetStereoBuffer(stereo.currentDisplay,
								  stereo.currentWindow,
								  stereo.currentStereoBuffer);
      glXWaitX();   /* sync with X command stream before calling GL */
    }
  }
  glDrawBuffer(mode);
}

/* call instead of glClear */
void
Togl_StereoClear(GLbitfield mask)
{
  GLenum drawBuffer;
  if (stereo.useSGIStereo) {
    drawBuffer = stereo.currentDrawBuffer;
    switch (drawBuffer) {
    case GL_FRONT:
      stereoDrawBuffer(GL_FRONT_RIGHT);
      glClear(mask);
      stereoDrawBuffer(drawBuffer);
      break;
    case GL_BACK:
      stereoDrawBuffer(GL_BACK_RIGHT);
      glClear(mask);
      stereoDrawBuffer(drawBuffer);
      break;
    case GL_FRONT_AND_BACK:
      stereoDrawBuffer(GL_RIGHT);
      glClear(mask);
      stereoDrawBuffer(drawBuffer);
      break;
    case GL_LEFT:
    case GL_FRONT_LEFT:
    case GL_BACK_LEFT:
    case GL_RIGHT:
    case GL_FRONT_RIGHT:
    case GL_BACK_RIGHT:
    default:
      break;
    }
  }
  glClear(mask);
}

static void
stereoMakeCurrent(Display *dpy, Window win, GLXContext ctx)
{
DOTRACE("<togl.cc>::stereoMakeCurrent");
  
  if (stereo.useSGIStereo) {
    if (dpy && (dpy != stereo.currentDisplay)) {
      int event, error;
      /* Make sure new Display supports SGIStereo */
      if (XSGIStereoQueryExtension(dpy, &event, &error) == False) {
		  dpy = NULL;
      }
    }
    if (dpy && win && (win != stereo.currentWindow)) {
      /* Make sure new Window supports SGIStereo */
      if (XSGIQueryStereoMode(dpy, win) == X_STEREO_UNSUPPORTED) {
		  win = None;
      }
    }
    if (ctx && (ctx != stereo.currentContext)) {
      GLint drawBuffer;
      glGetIntegerv(GL_DRAW_BUFFER, &drawBuffer);
      stereoDrawBuffer((GLenum) drawBuffer);
    }
    stereo.currentDisplay = dpy;
    stereo.currentWindow = win;
    stereo.currentContext = ctx;
  }
}


/* call before using stereo */
static void
stereoInit(Togl* togl,int stereoEnabled)
{
DOTRACE("<togl.cc>::stereoInit");
  stereo.useSGIStereo = stereoEnabled;
  stereo.currentDisplay = NULL;
  stereo.currentWindow = None;
  stereo.currentContext = NULL;
  stereo.currentDrawBuffer = GL_NONE;
  stereo.currentStereoBuffer = STEREO_BUFFER_NONE;
  stereo.enabled = False;
}


void
Togl_StereoFrustum(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top,
						 GLfloat near, GLfloat far, GLfloat eyeDist, GLfloat eyeOffset)
{
DOTRACE("<togl.cc>::Togl_StereoFrustum");
  GLfloat eyeShift = (eyeDist - near) * (eyeOffset / eyeDist);
  
  glFrustum(left+eyeShift, right+eyeShift, bottom, top, near, far);
  glTranslatef(-eyeShift, 0.0, 0.0);
}

#endif /* __sgi STEREO */

///////////////////////////////////////////////////////////////////////
//
// Togl member definitions
//
///////////////////////////////////////////////////////////////////////

//     * Creates a new window
//     * Creates an 'Togl' data structure
//     * Creates an event handler for this window
//     * Creates a command that handles this object
//     * Configures this Togl for the given arguments

Togl::Togl(Tcl_Interp* interp, Tk_Window mainwin, int argc, char** argv) :
  itsNext(NULL),
  itsGLXContext(NULL),
  itsDisplay(0),
  itsTkWin(0),
  itsInterp(interp),
#ifndef NO_TK_CURSOR
  itsCursor(None),
#endif
  itsWidth(0),
  itsHeight(0),
  itsTime(0),
  itsRgbaFlag(1),
  itsRgbaRed(1),
  itsRgbaGreen(1),
  itsRgbaBlue(1),
  itsDoubleFlag(0),
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
  itsOverlayFlag(0),
  itsStereoFlag(0),
  itsAuxNumber(0),
  itsIndirect(GL_FALSE),
  itsUpdatePending(GL_FALSE),
  itsDisplayProc(DefaultDisplayProc),
  itsReshapeProc(DefaultReshapeProc),
  itsDestroyProc(DefaultDestroyProc),
  itsTimerProc(DefaultTimerProc),
  itsOverlayCtx(NULL),
  itsOverlayWindow(0),
  itsOverlayDisplayProc(DefaultOverlayDisplayProc),
  itsOverlayUpdatePending(GL_FALSE),
  itsShareList(NULL),
  itsShareContext(NULL),
  itsIdent(NULL),
  itsClientData(DefaultClientData),

  /* for EPS Output */
  itsEpsRedMap(NULL),
  itsEpsGreenMap(NULL),
  itsEpsBlueMap(NULL),
  itsEpsMapSize(0)
{
DOTRACE("Togl::Togl");

  /* Create the window. */
  char* name = argv[1];
  itsTkWin = Tk_CreateWindowFromPath(itsInterp, mainwin,
												 name, (char *) NULL);
  if (itsTkWin == NULL) {
	 throw TCL_ERROR;
  }

  itsDisplay = Tk_Display( itsTkWin );

  Tk_SetClass(itsTkWin, "Togl");

  /* Create command event handler */
  itsWidgetCmd = Tcl_CreateCommand(itsInterp, Tk_PathName(itsTkWin),
											  Togl_Widget,
											  static_cast<ClientData>(this),
											  Togl::dummyWidgetCmdDeletedProc);

  Tk_CreateEventHandler(itsTkWin,
								ExposureMask | StructureNotifyMask,
								Togl::dummyEventProc,
								static_cast<ClientData>(this));

  /* Configure Togl widget */
  if (configure(itsInterp, argc-2, argv+2, 0) == TCL_ERROR) {
	 Tk_DestroyWindow(itsTkWin);
	 throw TCL_ERROR;
  }

  /*
	* If OpenGL window wasn't already created by Togl_Configure() we
	* create it now.  We can tell by checking if the GLX context has
	* been initialized.
	*/
  if (!itsGLXContext) {
	 if (makeWindowExist() == TCL_ERROR) {
		Tk_DestroyWindow(itsTkWin);
		throw TCL_ERROR;
	 }
  }

  /* If defined, call create callback */
  if (DefaultCreateProc) {
	 DefaultCreateProc(this);
  }

  /* If defined, call reshape proc */
  if (itsReshapeProc) {
	 itsReshapeProc(this);
  }

  /* If defined, setup timer */
  if (itsTimerProc) {
	 Tk_CreateTimerHandler( itsTime, Togl::dummyTimerCallback,
									static_cast<ClientData>(this) );
  }

  Tcl_AppendResult(itsInterp, Tk_PathName(itsTkWin), NULL);

  /* Add to linked list */
  addSelfToList();
}

Togl::~Togl() {
DOTRACE("Togl::~Togl");

  Tk_FreeOptions(configSpecs, (char *) this, itsDisplay, 0);

#ifndef NO_TK_CURSOR
  if (itsCursor != None) {
	 Tk_FreeCursor(itsDisplay, itsCursor);
  }
#endif
  if (itsDestroyProc) {
	 itsDestroyProc(this);
  }

  /* remove from linked list */
  removeSelfFromList();
}

int Togl::configure(Tcl_Interp* interp, int argc, char* argv[], int flags) {
DOTRACE("Togl::configure");

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
								 argc, argv, (char *)this, flags) == TCL_ERROR) {
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
		|| itsAuxNumber != oldAuxNumber) {
#ifdef MESA_COLOR_HACK
	 free_default_color_cells( Tk_Display(itsTkWin),
										Tk_Colormap(itsTkWin) );
#endif
	 /* Have to recreate the window and GLX context */
	 if (makeWindowExist()==TCL_ERROR) {
		return TCL_ERROR;
	 }
  }

#if defined(__sgi) && defined(STEREO)
  stereoInit(this,itsStereoFlag);
#endif

  return TCL_OK; 
}

///////////////////////////////////////////////////////////////////////
//
// Togl::makeCurrent
//
//   Bind the OpenGL rendering context to the specified
//   Togl widget.
//
///////////////////////////////////////////////////////////////////////

void Togl::makeCurrent() const {
DOTRACE("Togl::makeCurrent");
  glXMakeCurrent( itsDisplay,
						Tk_WindowId(itsTkWin),
						itsGLXContext );
#if defined(__sgi) && defined(STEREO)
  stereoMakeCurrent( itsDisplay,
							Tk_WindowId(itsTkWin),
							itsGLXContext );
#endif /*__sgi STEREO */
}


// Gets called when an Togl widget is destroyed.
void Togl::dummyDestroyProc(char* clientData) {
  Togl* togl = (Togl*)clientData;
  delete togl;
}

void Togl::dummyEventProc(ClientData clientData, XEvent* eventPtr) {
  Togl* togl = static_cast<Togl*>(clientData);
  togl->eventProc(eventPtr);
}

void Togl::dummyWidgetCmdDeletedProc(ClientData clientData) {
  Togl* togl = static_cast<Togl*>(clientData);
  togl->widgetCmdDeletedProc();
}

// Gets called from Tk_CreateTimerHandler.
void Togl::dummyTimerCallback(ClientData clientData) {
DOTRACE("Togl::dummyTimerCallback");
  Togl* togl = static_cast<Togl*>(clientData);
  togl->itsTimerProc(togl);
  togl->itsTimerHandler =
	 Tcl_CreateTimerHandler( togl->itsTime, dummyTimerCallback,
									 static_cast<ClientData>(togl) );
}

// Called when the widget's contents must be redrawn.  Basically, we
// just call the user's render callback function.
//
// Note that the parameter type is ClientData so this function can be
// passed to Tk_DoWhenIdle().
void Togl::dummyRenderCallback(ClientData clientData) {
DOTRACE("Togl::dummyRenderCallback");
  Togl* togl = static_cast<Togl*>(clientData);

  if (togl->itsDisplayProc) {
	 Togl_MakeCurrent(togl);
	 togl->itsDisplayProc(togl);
  }
  togl->itsUpdatePending = GL_FALSE;
}

void Togl::dummyOverlayRenderCallback(ClientData clientData) {
DOTRACE("Togl::dummyOverlayRenderCallback");
  Togl* togl = static_cast<Togl*>(clientData);

  if (togl->itsOverlayFlag && togl->itsOverlayDisplayProc) {
	 glXMakeCurrent( Tk_Display(togl->itsTkWin),
						  togl->itsOverlayWindow,
						  togl->itsOverlayCtx );
#if defined(__sgi) && defined(STEREO)
	 stereoMakeCurrent( Tk_Display(togl->itsTkWin),
							  togl->itsOverlayWindow,
							  togl->itsOverlayCtx );	
#endif /*__sgi STEREO */	
	 togl->itsOverlayDisplayProc(togl);
  }
  togl->itsOverlayUpdatePending = GL_FALSE;
}


Togl* Togl::findTogl(const char* ident) {
DOTRACE("Togl::findTogl");
  Togl* t = ToglHead;
  while (t) {
	 if (strcmp(t->itsIdent, ident) == 0)
		return t;
	 t = t->itsNext;
  }
  return NULL;
}

void Togl::eventProc(XEvent* eventPtr) {
DOTRACE("Togl::eventProc");

  switch (eventPtr->type) {
  case Expose:
	 if (eventPtr->xexpose.count == 0) {
		if (!itsUpdatePending && 
			 eventPtr->xexpose.window==Tk_WindowId(itsTkWin)) {
		  Togl_PostRedisplay(this);
		}
		if (!itsOverlayUpdatePending && itsOverlayFlag
			 && itsOverlayIsMapped
			 && eventPtr->xexpose.window==itsOverlayWindow){
		  Togl_PostOverlayRedisplay(this);
		}
	 }
	 break;
  case ConfigureNotify:
	 if (itsWidth != Tk_Width(itsTkWin) || itsHeight != Tk_Height(itsTkWin))
		{
		  itsWidth = Tk_Width(itsTkWin);
		  itsHeight = Tk_Height(itsTkWin);
		  XResizeWindow(itsDisplay, Tk_WindowId(itsTkWin), itsWidth, itsHeight);

		  if (itsOverlayFlag) {
			 XResizeWindow( itsDisplay, itsOverlayWindow, itsWidth, itsHeight );
			 XRaiseWindow( itsDisplay, itsOverlayWindow );
		  }
		  makeCurrent();
		  if (itsReshapeProc) {
			 itsReshapeProc(this);
		  }
		  else {
			 glViewport(0, 0, itsWidth, itsHeight);
			 if (itsOverlayFlag) {
				Togl_UseLayer( this, TOGL_OVERLAY );
				glViewport( 0, 0, itsWidth, itsHeight );
				Togl_UseLayer( this, TOGL_NORMAL );
			 }
		  }
		}
	 break;
  case MapNotify:
	 break;
  case DestroyNotify:
	 if (itsTkWin != NULL) {
		itsTkWin = NULL;
		Tcl_DeleteCommandFromToken( itsInterp, itsWidgetCmd );
	 }
	 if (itsTimerProc != NULL) {
		Tcl_DeleteTimerHandler(itsTimerHandler);
	 }
	 if (itsUpdatePending) {
		Tcl_CancelIdleCall(Togl::dummyRenderCallback,
								 static_cast<ClientData>(this));
	 }

	 Tcl_EventuallyFree( static_cast<ClientData>(this),
								Togl::dummyDestroyProc );

	 break;
  default:
	 /*nothing*/
	 ;
  }
}

///////////////////////////////////////////////////////////////////////
//
// Togl::widgetCmdDeletedProc
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

void Togl::widgetCmdDeletedProc() {
DOTRACE("Togl::widgetCmdDeletedProc");
  /*
	* This procedure could be invoked either because the window was
	* destroyed and the command was then deleted (in which case itsTkWin
	* is NULL) or because the command was deleted, and then this procedure
	* destroys the widget.
	*/

  /* NEW in togl 1.5 beta 3 */
  if (this && itsTkWin) {
	 Tk_DeleteEventHandler(itsTkWin,
								  ExposureMask | StructureNotifyMask,
								  Togl::dummyEventProc,
								  (ClientData)this);
  }

  /* NEW in togl 1.5 beta 3 */
  if (itsGLXContext) {
	 /* XXX this might be bad if two or more Togl widgets share a context */
	 glXDestroyContext( itsDisplay, itsGLXContext );
	 itsGLXContext = NULL;
  }
  if (itsOverlayCtx) {
	 Tcl_HashEntry *entryPtr;
	 TkWindow *winPtr = (TkWindow *) itsTkWin;
	 if (winPtr) {
		entryPtr = Tcl_FindHashEntry(&winPtr->dispPtr->winTable,
											  (char *) itsOverlayWindow );
		Tcl_DeleteHashEntry(entryPtr);
	 }
	 glXDestroyContext( itsDisplay, itsOverlayCtx );
	 itsOverlayCtx = NULL;
  }

  if (itsTkWin != NULL) {
	 Tk_DestroyWindow(itsTkWin);
	 itsTkWin = NULL;
  }
}

///////////////////////////////////////////////////////////////////////
//
// Togl::makeWindowExist
//
//   Modified version of Tk_MakeWindowExist.
//   Creates an OpenGL window for the Togl widget.
//
///////////////////////////////////////////////////////////////////////

int Togl::makeWindowExist() {
DOTRACE("Togl::makeWindowExist");

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

  // Request the X window to be displayed
  XMapWindow(itsDisplay, Tk_WindowId(itsTkWin));

  // Bind the context to the window and make it the current context
  Togl_MakeCurrent(this);

  // Check for a single/double buffering snafu
  checkDblBufferSnafu();

  // for EPS Output
  setupEpsMaps();

  return TCL_OK;

} // end Togl::makeWindowExist()

void Togl::destroyCurrentWindow() {
DOTRACE("Togl::destroyCurrentWindow");
  TkWindow *winPtr = (TkWindow *) itsTkWin;

  if (winPtr->window != None) {
	 XDestroyWindow(itsDisplay, winPtr->window);
	 winPtr->window = 0;
  }
}

int Togl::checkForGLX() {
DOTRACE("Togl::checkForGLX");

  /* Make sure OpenGL's GLX extension supported */
  int dummy;
  if (!glXQueryExtension(itsDisplay, &dummy, &dummy)) {
	 return TCL_ERR(itsInterp, "Togl: X server has no OpenGL GLX extension");
  }

  return TCL_OK;
}

int Togl::setupVisInfoAndContext() {
DOTRACE("Togl::setupVisInfoAndContext");

  if (itsShareContext && Togl::findTogl(itsShareContext)) {
	 /* share OpenGL context with existing Togl widget */
	 Togl* shareWith = Togl::findTogl(itsShareContext);
	 assert(shareWith);
	 assert(shareWith->itsGLXContext);
	 itsGLXContext = shareWith->itsGLXContext;
	 itsVisInfo = shareWith->itsVisInfo;
	 printf("SHARE CTX\n");
  }
  else /* !(itsShareContext && Togl::findTogl(itsShareContext)) */ {
	 int attrib_list[1000];

	 const int MAX_ATTEMPTS = 12;

	 static int ci_depths[MAX_ATTEMPTS] = {
		8, 4, 2, 1, 12, 16, 8, 4, 2, 1, 12, 16
	 };
	 static int dbl_flags[MAX_ATTEMPTS] = {
		0, 0, 0, 0,  0,  0, 1, 1, 1, 1,  1,  1
	 };

	 /* It may take a few tries to get a visual */
	 for (int attempt=0; attempt<MAX_ATTEMPTS; attempt++) {
 		buildAttribList(attrib_list, ci_depths[attempt], dbl_flags[attempt]);

		itsVisInfo = glXChooseVisual( itsDisplay,
												DefaultScreen(itsDisplay), attrib_list );

		if (itsVisInfo) {
		  /* found a GLX visual! */
		  break;
		}
	 } // end for loop

	 if (itsVisInfo==NULL) {
		return TCL_ERR(itsInterp, "Togl: couldn't get visual");
	 }

	 // Create a new OpenGL rendering context.
	 setupGLXContext();

	 if (itsGLXContext == NULL) {
		return TCL_ERR(itsInterp, "could not create rendering context");
	 }

  } // end else clause

  return TCL_OK;
}

void Togl::buildAttribList(int* attrib_list, int ci_depth, int dbl_flag) {
DOTRACE("Togl::buildAttribList");
  int attrib_count = 0;

  attrib_list[attrib_count++] = GLX_USE_GL;

  if (itsRgbaFlag) {
	 /* RGB[A] mode */
	 attrib_list[attrib_count++] = GLX_RGBA;
	 attrib_list[attrib_count++] = GLX_RED_SIZE;
	 attrib_list[attrib_count++] = itsRgbaRed;
	 attrib_list[attrib_count++] = GLX_GREEN_SIZE;
	 attrib_list[attrib_count++] = itsRgbaGreen;
	 attrib_list[attrib_count++] = GLX_BLUE_SIZE;
	 attrib_list[attrib_count++] = itsRgbaBlue;
	 if (itsAlphaFlag) {
		attrib_list[attrib_count++] = GLX_ALPHA_SIZE;
		attrib_list[attrib_count++] = itsAlphaSize;
	 }

	 /* for EPS Output */
	 freeEpsMaps();
  }
  else {
	 /* Color index mode */
	 attrib_list[attrib_count++] = GLX_BUFFER_SIZE;
	 attrib_list[attrib_count++] = ci_depth;
  }

  if (itsDepthFlag) {
	 attrib_list[attrib_count++] = GLX_DEPTH_SIZE;
	 attrib_list[attrib_count++] = itsDepthSize;
  }

  if (itsDoubleFlag || dbl_flag) {
	 attrib_list[attrib_count++] = GLX_DOUBLEBUFFER;
  }

  if (itsStencilFlag) {
	 attrib_list[attrib_count++] = GLX_STENCIL_SIZE;
	 attrib_list[attrib_count++] = itsStencilSize;
  }

  if (itsAccumFlag) {
	 attrib_list[attrib_count++] = GLX_ACCUM_RED_SIZE;
	 attrib_list[attrib_count++] = itsAccumRed;
	 attrib_list[attrib_count++] = GLX_ACCUM_GREEN_SIZE;
	 attrib_list[attrib_count++] = itsAccumGreen;
	 attrib_list[attrib_count++] = GLX_ACCUM_BLUE_SIZE;
	 attrib_list[attrib_count++] = itsAccumBlue;
	 if (itsAlphaFlag) {
		attrib_list[attrib_count++] = GLX_ACCUM_ALPHA_SIZE;
		attrib_list[attrib_count++] = itsAccumAlpha;
	 }
  }

  if (itsAuxNumber != 0) {
	 attrib_list[attrib_count++] = GLX_AUX_BUFFERS;
	 attrib_list[attrib_count++] = itsAuxNumber;
  }

  /* stereo hack */
  /*
	 if (itsStereoFlag) {
	 attrib_list[attrib_count++] = GLX_STEREO;
	 }
  */

  attrib_list[attrib_count++] = None;
}

void Togl::setupGLXContext() {
DOTRACE("Togl::setupGLXContext");
  int directCtx = itsIndirect ? GL_FALSE : GL_TRUE;

  if (itsShareList) {
	 /* share display lists with existing this widget */
	 Togl* shareWith = Togl::findTogl(itsShareList);
	 GLXContext shareCtx;
	 if (shareWith)
		shareCtx = shareWith->itsGLXContext;
	 else
		shareCtx = None;
	 itsGLXContext = glXCreateContext(itsDisplay, itsVisInfo,
												 shareCtx, directCtx);
  }
  else {
	 /* don't share display lists */
	 itsGLXContext = glXCreateContext(itsDisplay, itsVisInfo, None, directCtx);
  }
}

void Togl::createWindow() {
DOTRACE("Togl::createWindow");

  TkWindow *winPtr = (TkWindow *) itsTkWin;

  Colormap cmap = findColormap();

  /* Make sure Tk knows to switch to the new colormap when the cursor
	* is over this window when running in color index mode.
	*/
  /*   Tk_SetWindowVisual(itsTkWin, itsVisInfo->visual, itsVisInfo->depth, cmap);*/
										  /* Rob's */
  Tk_SetWindowColormap(itsTkWin, cmap);

  // Find parent of window (necessary for creation)
  Window parent = findParent();

  XSetWindowAttributes swa;
  swa.colormap = cmap;
  swa.border_pixel = 0;
  swa.event_mask = ALL_EVENTS_MASK;
  winPtr->window = XCreateWindow(itsDisplay, parent,
											0, 0, itsWidth, itsHeight,
											0, itsVisInfo->depth,
											InputOutput, itsVisInfo->visual,
											CWBorderPixel | CWColormap | CWEventMask,
											&swa);

  /* Make sure window manager installs our colormap */
  XSetWMColormapWindows( itsDisplay, winPtr->window, &(winPtr->window), 1 );
  int dummy_new_flag;
  Tcl_HashEntry *hPtr = Tcl_CreateHashEntry(&winPtr->dispPtr->winTable,
												(char *) winPtr->window, &dummy_new_flag);
  Tcl_SetHashValue(hPtr, winPtr);

  winPtr->dirtyAtts = 0;
  winPtr->dirtyChanges = 0;
#ifdef TK_USE_INPUT_METHODS
  winPtr->inputContext = NULL;
#endif /* TK_USE_INPUT_METHODS */
}

Window Togl::findParent() {
DOTRACE("Togl::findParent");

  TkWindow *winPtr = (TkWindow *) itsTkWin;

  if ((winPtr->parentPtr == NULL) || (winPtr->flags & TK_TOP_LEVEL)) {
	 return XRootWindow(winPtr->display, winPtr->screenNum);
  }
  // else...
  if (winPtr->parentPtr->window == None) {
	 Tk_MakeWindowExist((Tk_Window) winPtr->parentPtr);
  }
  return winPtr->parentPtr->window;
}

Colormap Togl::findColormap() {
DOTRACE("Togl::findColormap");

  int scrnum = DefaultScreen(itsDisplay);
  Colormap cmap;
  if (itsRgbaFlag) {
	 /* Colormap for RGB mode */
	 cmap = get_rgb_colormap( itsDisplay, scrnum, itsVisInfo );
  }
  else {
	 /* Colormap for CI mode */
	 if (itsPrivateCmapFlag) {
		/* need read/write colormap so user can store own color entries */
		cmap = XCreateColormap(itsDisplay,
									  RootWindow(itsDisplay, itsVisInfo->screen),
									  itsVisInfo->visual, AllocAll);
	 }
	 else {
		if (itsVisInfo->visual==DefaultVisual(itsDisplay, scrnum)) {
		  /* share default/root colormap */
		  cmap = DefaultColormap(itsDisplay,scrnum);
		}
		else {
		  /* make a new read-only colormap */
		  cmap = XCreateColormap(itsDisplay,
										 RootWindow(itsDisplay, itsVisInfo->screen),
										 itsVisInfo->visual, AllocNone);
		}
	 }
  }

  return cmap;
}

void Togl::setupStackingOrder() {
DOTRACE("Togl::setupStackingOrder");
  TkWindow *winPtr = (TkWindow *) itsTkWin;

  if (!(winPtr->flags & TK_TOP_LEVEL)) {
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
			winPtr2 = winPtr2->nextPtr) {
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
		  (winPtr->atts.colormap != winPtr->parentPtr->atts.colormap)) {
		TkWmAddToColormapWindows(winPtr);
	 }
  }
}

void Togl::setupOverlayIfNeeded() {
DOTRACE("Togl::setupOverlayIfNeeded");
  if (itsOverlayFlag) {
	 if (setupOverlay() == TCL_ERROR) {
		fprintf(stderr,"Warning: couldn't setup overlay.\n");
		itsOverlayFlag = 0;
	 }
  }
}

/*
 * Do all the setup for overlay planes
 * Return:   TCL_OK or TCL_ERROR
 */
int Togl::setupOverlay() {
DOTRACE("Togl::setupOverlay");

#ifdef GLX_TRANSPARENT_TYPE_EXT
  static int ovAttributeList[] = {
	 GLX_BUFFER_SIZE, 2,
	 GLX_LEVEL, 1,
	 GLX_TRANSPARENT_TYPE_EXT, GLX_TRANSPARENT_INDEX_EXT,
	 None
  };
#else
  static int ovAttributeList[] = {
	 GLX_BUFFER_SIZE, 2,
	 GLX_LEVEL, 1,
	 None
  };
#endif

  TkWindow *winPtr = (TkWindow *) itsTkWin;

  XVisualInfo* visinfo = glXChooseVisual( itsDisplay,
														DefaultScreen(itsDisplay),
														ovAttributeList );

  if (!visinfo) {
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
  itsOverlayWindow = XCreateWindow( itsDisplay, Tk_WindowId(itsTkWin),
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

void Togl::issueConfigureNotify() {
DOTRACE("Togl::issueConfigureNotify");

  /*
	* Issue a ConfigureNotify event if there were deferred configuration
	* changes (but skip it if the window is being deleted;  the
	* ConfigureNotify event could cause problems if we're being called
	* from Tk_DestroyWindow under some conditions).
	*/
  TkWindow *winPtr = (TkWindow *) itsTkWin;

  if ((winPtr->flags & TK_NEED_CONFIG_NOTIFY)
		&& !(winPtr->flags & TK_ALREADY_DEAD)){

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
	 if (winPtr->changes.stack_mode == Above) {
		event.xconfigure.above = winPtr->changes.sibling;
	 }
	 else {
		event.xconfigure.above = None;
	 }
	 event.xconfigure.override_redirect = winPtr->atts.override_redirect;
	 Tk_HandleEvent(&event);
  }
}

void Togl::checkDblBufferSnafu() {
DOTRACE("Togl::checkDblBufferSnafu");
  int dbl_flag;
  if (glXGetConfig( itsDisplay, itsVisInfo, GLX_DOUBLEBUFFER, &dbl_flag )) {
	 if (itsDoubleFlag==0 && dbl_flag) {
		/* We requested single buffering but had to accept a */
		/* double buffered visual.  Set the GL draw buffer to */
		/* be the front buffer to simulate single buffering. */
		glDrawBuffer( GL_FRONT );
	 }
  }
}

void Togl::setupEpsMaps() {
DOTRACE("Togl::setupEpsMaps");
  if ( !itsRgbaFlag ) {
	 GLint index_bits;
	 glGetIntegerv( GL_INDEX_BITS, &index_bits );

	 int index_size = 1 << index_bits;
	 if ( itsEpsMapSize != index_size ) {
		if ( itsEpsRedMap) free( ( char *)itsEpsRedMap);
		if ( itsEpsGreenMap) free( ( char *)itsEpsGreenMap);
		if ( itsEpsBlueMap) free( ( char *)itsEpsBlueMap);
		itsEpsMapSize = index_size;
		itsEpsRedMap = ( GLfloat *)calloc( index_size, sizeof( GLfloat));
		itsEpsGreenMap = ( GLfloat *)calloc( index_size, sizeof( GLfloat));
		itsEpsBlueMap = ( GLfloat *)calloc( index_size, sizeof( GLfloat));
	 }
  }
}

void Togl::freeEpsMaps() {
DOTRACE("Togl::freeEpsMaps");
  if (itsEpsRedMap) free( ( char *)itsEpsRedMap);
  if (itsEpsGreenMap) free( ( char *)itsEpsGreenMap);
  if (itsEpsBlueMap) free( ( char *)itsEpsBlueMap);
  itsEpsRedMap = itsEpsGreenMap = itsEpsBlueMap = NULL;
  itsEpsMapSize = 0;
}

void Togl::addSelfToList() {
DOTRACE("Togl::addSelfToList");
  itsNext = ToglHead;
  ToglHead = this;
}

void Togl::removeSelfFromList() {
DOTRACE("Togl::removeSelfFromList");
  Togl* prev = NULL;
  Togl* pos = ToglHead;
  while (pos) {
	 if (pos == this) {
		if (prev) {
		  prev->itsNext = pos->itsNext;
		}
		else {
		  ToglHead = pos->itsNext;
		}
		return;
	 }
	 prev = pos;
	 pos = pos->itsNext;
  }
}

///////////////////////////////////////////////////////////////////////
//
// Togl Tcl package
//
///////////////////////////////////////////////////////////////////////

namespace ToglTcl {
  int ToglCmd(ClientData clientData, Tcl_Interp *interp,
				  int argc, char **argv);

  class ToglPkg;
}

int ToglTcl::ToglCmd(ClientData clientData, Tcl_Interp *interp,
							int argc, char **argv)
{
DOTRACE("ToglTcl::ToglCmd");
  if (argc <= 1) {
	 return TCL_ERR(interp,
						 "wrong # args: should be \"pathName read filename\"");
  }

  Tk_Window mainwin = (Tk_Window)clientData;

  /* Create Togl data structure */
  try {
	 new Togl(interp, mainwin, argc, argv);
  }
  catch (...) {
	 return TCL_ERROR;
  }

  return TCL_OK;
}

class ToglTcl::ToglPkg : public Tcl::TclPkg {
public:
  ToglPkg(Tcl_Interp* interp) :
	 TclPkg(interp, "Togl", TOGL_VERSION)
	 {
		Tcl_CreateCommand(interp, "togl", ToglTcl::ToglCmd,
								(ClientData) Tk_MainWindow(interp), NULL);
		Tcl_InitHashTable(&CommandTable, TCL_STRING_KEYS);
	 }
};

extern "C" int Togl_Init(Tcl_Interp *interp)
{
DOTRACE("Togl_Init");

  new ToglTcl::ToglPkg(interp);

  return TCL_OK;
}

static const char vcid_togl_cc[] = "$Header$";
#endif // !TOGL_CC_DEFINED
