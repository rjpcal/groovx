///////////////////////////////////////////////////////////////////////
// tclgl.cc
// Rob Peters
// created: Nov-98
// written: Tue Mar 16 19:26:46 1999
// $Id$
//
// This package provides some simple Tcl functions that are wrappers
// for C OpenGL functions. The function names, argument lists, and
// symbolic constants for the Tcl functions are identical to those in
// the analagous C functions.
///////////////////////////////////////////////////////////////////////

#ifndef TCLGL_CC_DEFINED
#define TCLGL_CC_DEFINED

#include "tclgl.h"

#include <GL/gl.h>
#include <tcl.h>
#include <cmath>                // for sqrt() in drawThickLine
#include <map>
#include <string>

#include "errmsg.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// TclGL Tcl package declarations
///////////////////////////////////////////////////////////////////////

namespace TclGL {
  // utility maps for GLenums
  typedef map<string,GLenum> GLenum_map;
  const char* const no_such_enum = ": invalid GLenum constant";

  GLenum_map glBegin_mode;
  void glBegin_mode_init(GLenum_map& m);
  
  // Tcl functions
  Tcl_ObjCmdProc glBeginCmd;
  Tcl_ObjCmdProc glEndCmd;
  Tcl_ObjCmdProc glVertex3fCmd;
  Tcl_ObjCmdProc glIndexiCmd;
  Tcl_ObjCmdProc glLineWidthCmd;
  Tcl_ObjCmdProc glFlushCmd;
  Tcl_ObjCmdProc setBackgroundCmd;
  Tcl_ObjCmdProc setForegroundCmd;
  Tcl_ObjCmdProc drawOneLineCmd;
  Tcl_ObjCmdProc drawThickLineCmd;
  Tcl_ObjCmdProc antialiasCmd;
  Tcl_ObjCmdProc lineInfoCmd;
}

///////////////////////////////////////////////////////////////////////
// TclGL Tcl package definitions
///////////////////////////////////////////////////////////////////////

// utility maps for GLenums
void TclGL::glBegin_mode_init() {
  glBegin_mode["GL_POINTS"]               = GL_POINTS;
  glBegin_mode["GL_LINES"]                = GL_LINES;
  glBegin_mode["GL_LINE_STRIP"]           = GL_LINE_STRIP;
  glBegin_mode["GL_LINE_LOOP"]            = GL_LINE_LOOP;
  glBegin_mode["GL_TRIANGLES"]            = GL_TRIANGLES;
  glBegin_mode["GL_TRIANGLE_STRIP"]       = GL_TRIANGLE_STRIP;
  glBegin_mode["GL_TRIANGLE_FAN"]         = GL_TRIANGLE_FAN;
  glBegin_mode["GL_QUADS"]                = GL_QUADS;
  glBegin_mode["GL_QUAD_STRIP"]           = GL_QUAD_STRIP;
  glBegin_mode["GL_POLYGON"]              = GL_POLYGON;
}

// Tcl functions
int TclGL::glBeginCmd(ClientData, Tcl_Interp *interp,
                      int objc, Tcl_Obj *const objv[]) {
DOTRACE("glBeginCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "mode");
    return TCL_ERROR;
  }

  char* mode_str = Tcl_GetStringFromObj(objv[1], (int *) NULL);

  GLenum_map::iterator itr;
  if ( (itr=glBegin_mode.find(mode_str)) == glBegin_mode.end() ) {
    err_message(interp, objv, no_such_enum);
    return TCL_ERROR;
  }
  GLenum mode = (*itr).second;
  glBegin(mode);
  return TCL_OK;
}

int TclGL::glEndCmd(ClientData, Tcl_Interp *interp,
                    int objc, Tcl_Obj *const objv[]) {
DOTRACE("glEndCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  
  glEnd();
  return TCL_OK;
}

int TclGL::glVertex3fCmd(ClientData, Tcl_Interp *interp,
                         int objc, Tcl_Obj *const objv[]) {
DOTRACE("glVertex3fCmd");
  if (objc != 4) {
    Tcl_WrongNumArgs(interp, 1, objv, "x y z");
    return TCL_ERROR;
  }
  
  double x,y,z;
  if (Tcl_GetDoubleFromObj(interp, objv[1], &x) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetDoubleFromObj(interp, objv[2], &y) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetDoubleFromObj(interp, objv[3], &z) != TCL_OK) return TCL_ERROR;

  glVertex3f((float)x,(float)y,(float)z);
  return TCL_OK;
}

int TclGL::glIndexiCmd(ClientData, Tcl_Interp *interp,
                       int objc, Tcl_Obj *const objv[]) {
DOTRACE("glIndexiCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "index");
    return TCL_ERROR;
  }
  
  int i;
  if (Tcl_GetIntFromObj(interp, objv[1], &i) != TCL_OK) return TCL_ERROR;

  glIndexi(i);
  return TCL_OK;
}

int TclGL::glLineWidthCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("glLineWidthCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "width");
    return TCL_ERROR;
  }
  
  double w;
  if (Tcl_GetDoubleFromObj(interp, objv[1], &w) != TCL_OK) return TCL_ERROR;

  glLineWidth( (GLfloat) w );
  return TCL_OK;
}

int TclGL::glFlushCmd(ClientData, Tcl_Interp *interp,
                      int objc, Tcl_Obj *const objv[]) {
DOTRACE("glFlushCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  glFlush();
  return TCL_OK;
}

// This command takes either one or three arguments to set either the
// color index or the RGB color of the background in OpenGL. This is
// the color that the buffer will be cleared to when glClear() is called.
int TclGL::setBackgroundCmd(ClientData, Tcl_Interp *interp,
                            int objc, Tcl_Obj *const objv[]) {
DOTRACE("setBackgroundCmd");
  if (objc != 2 && objc != 4) {
    Tcl_WrongNumArgs(interp, 1, objv, "{ [index] or [R G B] }");
    return TCL_ERROR;
  }
  
  if (objc == 2) {
    int i;
    if (Tcl_GetIntFromObj(interp, objv[1], &i) != TCL_OK) return TCL_ERROR;
    glClearIndex(i);
    return TCL_OK;
  }
  else {
    double r, g, b;
    if (Tcl_GetDoubleFromObj(interp, objv[1], &r) != TCL_OK) return TCL_ERROR;
    if (Tcl_GetDoubleFromObj(interp, objv[2], &g) != TCL_OK) return TCL_ERROR;
    if (Tcl_GetDoubleFromObj(interp, objv[3], &b) != TCL_OK) return TCL_ERROR;
    glClearColor(r, g, b, 0.0);
    return TCL_OK;
  }
}
  
// This command takes either one or three arguments to set either the
// color index or the RGB color of the foreground in OpenGL. This is
// the color that will be used to render subsequent lines, polygons, etc.
int TclGL::setForegroundCmd(ClientData, Tcl_Interp *interp,
                            int objc, Tcl_Obj *const objv[]) {
DOTRACE("setForegroundCmd");
  if (objc != 2 && objc != 4) {
    Tcl_WrongNumArgs(interp, 1, objv, "{ [index] or [R G B] }");
    return TCL_ERROR;
  }
  
  if (objc == 2) {
    int i;
    if (Tcl_GetIntFromObj(interp, objv[1], &i) != TCL_OK) return TCL_ERROR;
    glIndexi(i);
    return TCL_OK;
  }
  else {
    double r, g, b;
    if (Tcl_GetDoubleFromObj(interp, objv[1], &r) != TCL_OK) return TCL_ERROR;
    if (Tcl_GetDoubleFromObj(interp, objv[2], &g) != TCL_OK) return TCL_ERROR;
    if (Tcl_GetDoubleFromObj(interp, objv[3], &b) != TCL_OK) return TCL_ERROR;
    glColor3f(r, g, b);
    return TCL_OK;
  }
}

// This command takes four arguments specifying two points between
// which a line will be drawn.
int TclGL::drawOneLineCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("drawOneLineCmd");
  if (objc != 5) {
    Tcl_WrongNumArgs(interp, 1, objv, "x1 y1 x2 y2");
    return TCL_ERROR;
  }

  double *coord = new double[4];
  for (int i = 1; i < objc; i++) {
    if (Tcl_GetDoubleFromObj(interp, objv[i], coord+i-1) != TCL_OK)
      return TCL_ERROR;
  }
  
  glBegin(GL_LINES);
  glVertex3f( (GLfloat) coord[0], (GLfloat) coord[1], 0.0);
  glVertex3f( (GLfloat) coord[2], (GLfloat) coord[3], 0.0);
  glEnd();
  glFlush();

  delete [] coord;
  return TCL_OK;
}

// This command takes five arguments specifying two points and a
// thickness. A thick "line" is drawn by drawing a filled rectangle.
int TclGL::drawThickLineCmd(ClientData, Tcl_Interp *interp,
                            int objc, Tcl_Obj *const objv[]) {
DOTRACE("drawThickLineCmd");
  if (objc != 6) {
    Tcl_WrongNumArgs(interp, 1, objv, "x1 y1 x2 y2 thickness");
    return TCL_ERROR;
  }

  double *coord = new double[5]; // fill with x1 y1 x2 y2 thickness
  int i = 1;
  double *p = coord;
  for ( ; i < objc; i++, p++) {
    if (Tcl_GetDoubleFromObj(interp, objv[i], p) != TCL_OK)
      return TCL_ERROR;
  }
  
  // construct the normal vector
  double a, b, c, d, norm;
  a = coord[2] - coord[0];      // (a, b) is the given vector
  b = coord[3] - coord[1];

  norm = sqrt(a*a + b*b);       // (c, d) is the normal
  c = -b/norm*coord[4]/2;
  d = a/norm*coord[4]/2;
  
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBegin(GL_POLYGON);
  glVertex3f( (GLfloat) coord[0]+c, (GLfloat) coord[1]+d, 0.0);
  glVertex3f( (GLfloat) coord[0]-c, (GLfloat) coord[1]-d, 0.0);

  glVertex3f( (GLfloat) coord[2]-c, (GLfloat) coord[3]-d, 0.0);
  glVertex3f( (GLfloat) coord[2]+c, (GLfloat) coord[3]+d, 0.0);
  glEnd();
  glFlush();

  delete [] coord;
  return TCL_OK;
}

// Turns on or off the GL modes required for antialiasing lines and
// polygons.
int TclGL::antialiasCmd(ClientData, Tcl_Interp *interp,
                        int objc, Tcl_Obj *const objv[]) {
DOTRACE("antialiasCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "on_off");
    return TCL_ERROR;
  }
    
  int on_off;
  if (Tcl_GetIntFromObj(interp, objv[1], &on_off) != TCL_OK) return TCL_ERROR;
  
  if (on_off == 1) {            // turn antialiasing on
    glEnable(GL_BLEND);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
  }
  else {                        // turn antialiasing off
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);
  }

  return TCL_OK;
}

// Prints the LINE_WIDTH_RANGE and LINE_WIDTH_GRANULARITY for the
// current OpenGL implementation.
int TclGL::lineInfoCmd(ClientData, Tcl_Interp *interp,
                       int objc, Tcl_Obj *const objv[]) {
DOTRACE("lineInfoCmd");
  GLdouble range[2] = {-1.0,-1.0}, gran=-1.0;
  glGetDoublev(GL_LINE_WIDTH_RANGE, &range[0]);
  glGetDoublev(GL_LINE_WIDTH_GRANULARITY, &gran);
  Tcl_Obj *r1 = Tcl_NewDoubleObj(range[0]);
  Tcl_Obj *r2 = Tcl_NewDoubleObj(range[1]);
  Tcl_Obj *g = Tcl_NewDoubleObj(gran);
#if (TCL_MAJOR_VERSION > 8) || \
        ( (TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 1) )
  Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), 
        "range = ", Tcl_GetString(r1), " ", Tcl_GetString(r2),
        ", granularity ", Tcl_GetString(g), (char *) NULL);
#else
  Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), 
        "range = ",
        Tcl_GetStringFromObj(r1, (int *) NULL), " ",
        Tcl_GetStringFromObj(r2, (int *) NULL), ", granularity ",
        Tcl_GetStringFromObj(g, (int *) NULL), (char *) NULL);
#endif
  return TCL_OK;
}

int TclGL::Tclgl_Init(Tcl_Interp *interp) {
DOTRACE("Tclgl_Init");
  glBegin_mode_init();

  Tcl_CreateObjCommand(interp, "glBegin", glBeginCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "glEnd", glEndCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "glVertex3f", glVertex3fCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "glIndexi", glIndexiCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "glLineWidth", glLineWidthCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "glFlush", glFlushCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "setBackground", setBackgroundCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "setForeground", setForegroundCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "drawOneLine", drawOneLineCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "drawThickLine", drawThickLineCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "antialias", antialiasCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "lineInfo", lineInfoCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_PkgProvide(interp, "Tclgl", "1.4");
  return TCL_OK;
}

static const char vcid_tclgl_cc[] = "$Header$";
#endif // !TCLGL_CC_DEFINED
