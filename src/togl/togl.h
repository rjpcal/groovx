///////////////////////////////////////////////////////////////////////
//
// togl.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 23 15:36:01 2000
// written: Fri Jun  1 16:56:31 2001
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
#include <GL/gl.h>
#ifdef TOGL_X11
#include <X11/Xlib.h>
#endif

#ifdef __sgi
#include <GL/glx.h>
#include <X11/extensions/SGIStereo.h>
#endif


#define TOGL_VERSION "1.5"
#define TOGL_MAJOR_VERSION 1
#define TOGL_MINOR_VERSION 5


/*
 * "Standard" fonts which can be specified to Togl_LoadBitmapFont()
 */
#define TOGL_BITMAP_8_BY_13		1
#define TOGL_BITMAP_9_BY_15		2
#define TOGL_BITMAP_TIMES_ROMAN_10	3
#define TOGL_BITMAP_TIMES_ROMAN_24	4
#define TOGL_BITMAP_HELVETICA_10	5
#define TOGL_BITMAP_HELVETICA_12	6
#define TOGL_BITMAP_HELVETICA_18	7
 

/*
 * Normal and overlay plane constants
 */
#define TOGL_NORMAL	1
#define TOGL_OVERLAY	2


class Togl;


#ifdef __cplusplus
extern "C" {
#endif

typedef void (Togl_Callback) (Togl* togl);
typedef int  (Togl_CmdProc) (Togl* togl, int argc, char *argv[]);

extern int Togl_Init( Tcl_Interp *interp );

// Default callback setup functions

extern void Togl_CreateFunc( Togl_Callback *proc );
extern void Togl_DisplayFunc( Togl_Callback *proc );
extern void Togl_ReshapeFunc( Togl_Callback *proc );
extern void Togl_DestroyFunc( Togl_Callback *proc );
extern void Togl_TimerFunc( Togl_Callback *proc );
extern void Togl_ResetDefaultCallbacks( void );

#ifdef __cplusplus
}
#endif


///////////////////////////////////////////////////////////////////////
//
// Togl class definition
//
///////////////////////////////////////////////////////////////////////

class Togl {
private:
  Togl(const Togl&);
  Togl& operator=(const Togl&);

public:
  Togl(Tcl_Interp* interp, const char* pathname,
		 int config_argc = 0, char** config_argv = 0);
  virtual ~Togl();

  // Change callbacks for existing widget
  void setDisplayFunc( Togl_Callback *proc );
  void setReshapeFunc( Togl_Callback *proc );
  void setDestroyFunc( Togl_Callback *proc );

  // Miscellaneous
  int configure( Tcl_Interp *interp, int argc, char *argv[], int flags );
  void makeCurrent() const;
  static void createCommand( char *cmd_name, Togl_CmdProc *cmd_proc );
  void postRedisplay();
  void postReconfigure();
  void swapBuffers() const;

  // Query functions
  char* ident() const;
  int width() const;
  int height() const;
  bool usesRgba() const;
  bool isDoubleBuffered() const;
  bool hasPrivateCmap() const;
  Tcl_Interp* interp() const;
  Tk_Window tkWin() const;
  const char* pathname() const;

  // Color index mode
  unsigned long allocColor( float red, float green, float blue ) const;  
  void freeColor( unsigned long index ) const;
  void setColor( unsigned long index,
					  float red, float green, float blue ) const;

  // Bitmap fonts
  GLuint loadBitmapFont( const char *fontname ) const;
  GLuint loadBitmapFonti( int fontnumber ) const;
  void unloadBitmapFont( GLuint fontbase ) const;

  // Overlay functions
  static void overlayDisplayFunc( Togl_Callback *proc );
  void useLayer( int layer );
  void showOverlay();
  void hideOverlay();
  void postOverlayRedisplay();
  int existsOverlay() const;
  int getOverlayTransparentValue() const;
  int isMappedOverlay() const;
  unsigned long allocColorOverlay( float red, float green, float blue ) const;
  void freeColorOverlay( unsigned long index ) const;

  // User client data
  static void defaultClientData( ClientData clientData );
  ClientData getClientData() const;
  void setClientData( ClientData clientData );

  // X11-only commands
  Display* display() const;
  Screen* screen() const;
  int screenNumber() const;
  Colormap colormap() const;
  Window windowId() const;

  // SGI stereo-only commands.
  // Contributed by Ben Evans (Ben.Evans@anusf.anu.edu.au)
#ifdef __sgi
  static void stereoDrawBuffer( GLenum mode );
  static void stereoFrustum( GLfloat left, GLfloat right,
									  GLfloat bottom, GLfloat top,
									  GLfloat near, GLfloat far,
									  GLfloat eyeDist, GLfloat eyeOffset );
  static void stereoClear( GLbitfield mask );
#endif

  // Generate EPS file.
  // Contributed by Miguel A. De Riera Pasenau (miguel@DALILA.UPC.ES)
  int dumpToEpsFile( const char *filename, int inColor,
							void (*user_redraw)(const Togl*) ) const;

  class Impl;
  friend class Impl;

private:
  Impl* const itsImpl;
};

static const char vcid_togl_h[] = "$Header$";
#endif // !TOGL_H_DEFINED
