///////////////////////////////////////////////////////////////////////
//
// togl.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 23 15:36:01 2000
// written: Tue May 23 15:37:57 2000
// $Id$
//
// This is a modified version of the Togl widget by Brian Paul and Ben
// Bederson; see the original copyright notice and log info below.
//
// Win32 support has been removed from this version, and the code has
// been C++-ified.
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGL_H_DEFINED
#define TOGL_H_DEFINED

/*
 * Togl - a Tk OpenGL widget
 * Version 1.5
 * Copyright (C) 1996-1997  Brian Paul and Ben Bederson
 * See the LICENSE file for copyright details.
 */


/*
 * $Log$
 * Revision 2.2  2000/05/24 14:14:11  rjpeters
 * Changed 'struct Togl' to 'class Togl'.
 *
 * Revision 2.1  2000/05/23 22:37:07  rjpeters
 * *** empty log message ***
 *
 * Revision 1.17  1997/11/15 04:14:37  brianp
 * changed version to 1.5
 *
 * Revision 1.16  1997/11/15 02:58:48  brianp
 * added Togl_TkWin() per Glenn Lewis
 *
 * Revision 1.15  1997/10/01 02:49:45  brianp
 * added SGI stereo functions from Ben Evans
 *
 * Revision 1.14  1997/08/26 02:05:19  brianp
 * added Togl_ResetDefaultCallbacks() and Togl_ClientData() (Greg Couch)
 *
 * Revision 1.13  1997/08/26 01:35:41  brianp
 * added Togl_Set*Func() functions from Matthias Ott
 *
 * Revision 1.12  1997/08/22 02:48:25  brianp
 * added WIN32 header stuff.  changed Togl version to 1.4.
 *
 * Revision 1.11  1997/04/11 01:37:22  brianp
 * added Togl_TimerFunc() and related code from Elmar Gerwalin
 *
 * Revision 1.10  1997/02/16 10:03:33  brianp
 * removed GL/glx.h include, added X11/Xlib.h include
 *
 * Revision 1.9  1997/02/16 01:25:34  brianp
 * added new overlay and EPS functions from Miguel A. De Riera Pasenau
 *
 * Revision 1.8  1996/12/13 21:24:41  brianp
 * added Togl_DestroyFunc() contributed by scotter@iname.com
 *
 * Revision 1.7  1996/11/14 00:49:54  brianp
 * added Togl_Get/SetClientData() functions
 *
 * Revision 1.6  1996/11/05 02:40:12  brianp
 * added Togl_Configure() and Togl_UnloadBitmapFont()
 *
 * Revision 1.5  1996/10/25 03:44:06  brianp
 * updated version to 1.2
 *
 * Revision 1.4  1996/10/24 01:20:17  brianp
 * added overlay constants and functions
 *
 * Revision 1.3  1996/10/24 00:13:34  brianp
 * added const qualifier to many function parameters
 *
 * Revision 1.2  1996/10/23 23:28:06  brianp
 * updated version to 1.1
 * added TOGL_BITMAP_* constants
 * added Togl_LoadBitmapFont()
 *
 * Revision 1.1  1996/10/23 23:15:27  brianp
 * Initial revision
 *
 */

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


#ifndef NULL
#define NULL    0
#endif


#ifdef __cplusplus
extern "C" {
#endif



#define TOGL_VERSION "1.5"
#define TOGL_MAJOR_VERSION 1
#define TOGL_MINOR_VERSION 5



/*
 * "Standard" fonts which can be specified to Togl_LoadBitmapFont()
 */
#define TOGL_BITMAP_8_BY_13		((char *) 1)
#define TOGL_BITMAP_9_BY_15		((char *) 2)
#define TOGL_BITMAP_TIMES_ROMAN_10	((char *) 3)
#define TOGL_BITMAP_TIMES_ROMAN_24	((char *) 4)
#define TOGL_BITMAP_HELVETICA_10	((char *) 5)
#define TOGL_BITMAP_HELVETICA_12	((char *) 6)
#define TOGL_BITMAP_HELVETICA_18	((char *) 7)
 

/*
 * Normal and overlay plane constants
 */
#define TOGL_NORMAL	1
#define TOGL_OVERLAY	2



class Togl;

typedef void (Togl_Callback) (Togl* togl);
typedef int  (Togl_CmdProc) (Togl* togl, int argc, char *argv[]);



extern int Togl_Init( Tcl_Interp *interp );



/*
 * Callback setup functions
 */

extern void Togl_CreateFunc( Togl_Callback *proc );

extern void Togl_DisplayFunc( Togl_Callback *proc );

extern void Togl_ReshapeFunc( Togl_Callback *proc );

extern void Togl_DestroyFunc( Togl_Callback *proc );

extern void Togl_TimerFunc( Togl_Callback *proc );

extern void Togl_ResetDefaultCallbacks( void );


/*
 * Change callbacks for existing widget
 */

extern void Togl_SetCreateFunc( Togl* togl, Togl_Callback *proc );

extern void Togl_SetDisplayFunc( Togl* togl, Togl_Callback *proc );

extern void Togl_SetReshapeFunc( Togl* togl, Togl_Callback *proc );

extern void Togl_SetDestroyFunc( Togl* togl, Togl_Callback *proc );


/*
 * Miscellaneous
 */

extern int Togl_Configure( Tcl_Interp *interp, Togl* togl, 
                           int argc, char *argv[], int flags );

extern void Togl_MakeCurrent( const Togl* togl );

extern void Togl_CreateCommand( char *cmd_name,
                                Togl_CmdProc *cmd_proc );

extern void Togl_PostRedisplay( Togl* togl );

extern void Togl_SwapBuffers( const Togl* togl );


/*
 * Query functions
 */

extern char *Togl_Ident( const Togl* togl );

extern int Togl_Width( const Togl* togl );

extern int Togl_Height( const Togl* togl );

extern Tcl_Interp *Togl_Interp( const Togl* togl );

extern Tk_Window Togl_TkWin( const Togl* togl );


/*
 * Color Index mode
 */

extern unsigned long Togl_AllocColor( const Togl* togl,
                                      float red, float green, float blue );

extern void Togl_FreeColor( const Togl* togl, unsigned long index );

extern void Togl_SetColor( const Togl* togl, unsigned long index,
                           float red, float green, float blue );


/*
 * Bitmap fonts
 */

extern GLuint Togl_LoadBitmapFont( const Togl* togl,
                                   const char *fontname );

extern void Togl_UnloadBitmapFont( const Togl* togl, GLuint fontbase );


/*
 * Overlay functions
 */

extern void Togl_UseLayer( Togl* togl, int layer );

extern void Togl_ShowOverlay( Togl* togl );

extern void Togl_HideOverlay( Togl* togl );

extern void Togl_PostOverlayRedisplay( Togl* togl );

extern void Togl_OverlayDisplayFunc( Togl_Callback *proc );

extern int Togl_ExistsOverlay( const Togl* togl );

extern int Togl_GetOverlayTransparentValue( const Togl* togl );

extern int Togl_IsMappedOverlay( const Togl* togl );

extern unsigned long Togl_AllocColorOverlay( const Togl* togl,
                                             float red, float green, 
                                             float blue );

extern void Togl_FreeColorOverlay( const Togl* togl, 
                                   unsigned long index );

/*
 * User client data
 */

extern void Togl_ClientData( ClientData clientData );

extern ClientData Togl_GetClientData( const Togl* togl );

extern void Togl_SetClientData( Togl* togl, ClientData clientData );


/*
 * X11-only commands.
 * Contributed by Miguel A. De Riera Pasenau (miguel@DALILA.UPC.ES)
 */

/* #ifdef TOGL_X11 */
extern Display *Togl_Display( const Togl* togl );
extern Screen *Togl_Screen( const Togl* togl );
extern int Togl_ScreenNumber( const Togl* togl );
extern Colormap Togl_Colormap( const Togl* togl );
extern Window Togl_Window( const Togl* togl ); 
/* #endif */


/*
 * SGI stereo-only commands.
 * Contributed by Ben Evans (Ben.Evans@anusf.anu.edu.au)
 */

#ifdef __sgi
extern void Togl_StereoDrawBuffer( GLenum mode );
extern void Togl_StereoFrustum( GLfloat left, GLfloat right,
                                GLfloat bottom, GLfloat top,
                                GLfloat near, GLfloat far,
                                GLfloat eyeDist, GLfloat eyeOffset );
extern void Togl_StereoClear( GLbitfield mask );
#endif


/*
 * Generate EPS file.
 * Contributed by Miguel A. De Riera Pasenau (miguel@DALILA.UPC.ES)
 */

extern int Togl_DumpToEpsFile( const Togl* togl,
                               const char *filename,
                               int inColor,
                               void (*user_redraw)(const Togl*) );

#ifdef __cplusplus
}
#endif

static const char vcid_togl_h[] = "$Header$";
#endif // !TOGL_H_DEFINED
