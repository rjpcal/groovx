///////////////////////////////////////////////////////////////////////
// objtogl.cc
// Rob Peters
// created: Nov-98
// written: Fri Apr 16 16:14:14 1999
// $Id$
//
// This package provides functionality that allows a Togl widget to
// work with a Tlist, controlling its display, reshaping, etc.
///////////////////////////////////////////////////////////////////////

#ifndef OBJTOGL_CC_DEFINED
#define OBJTOGL_CC_DEFINED

#include "objtogl.h"

#include <strstream.h>
#include <GL/gl.h>
#include <togl.h>
#include <tk.h>
#include <X11/Xlib.h>
#include <string>

#include "errmsg.h"
#include "gfxattribs.h"
#include "tlist.h"
#include "tlisttcl.h"
#include "toglconfig.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"
  
///////////////////////////////////////////////////////////////////////
// ObjTogl namespace declarations
///////////////////////////////////////////////////////////////////////

namespace ObjTogl {
  inline float Togl_Aspect(const struct Togl *togl) {
    return float(Togl_Width(togl)) / float(Togl_Height(togl));
  }

  int dumpCmapCmd(struct Togl *togl, int argc, char *argv[]);
  int dumpEpsCmd(struct Togl *togl, int argc, char *argv[]);
  Tcl_Obj* getParamValue(const struct Togl *togl, char *param);
  void reconfigureGL(const struct Togl *togl);
  int scaleRectCmd(struct Togl *togl, int argc, char *argv[]);
  int setColorCmd(struct Togl *togl, int argc, char *argv[]);
  int setFixedScaleCmd(struct Togl *togl, int argc, char *argv[]);
  int setMinRectCmd(struct Togl *togl, int argc, char *argv[]);
  int setUnitAngleCmd(struct Togl *togl, int argc, char *argv[]);
  int setViewingDistanceCmd(struct Togl *togl, int argc, char *argv[]);
  void toglCreateCallback(struct Togl *togl);
  void toglDestroyCallback(struct Togl *togl);
  void toglDisplayCallback(struct Togl *togl);
  void toglEpsCallback(const struct Togl *togl);
  void toglReshapeCallback(struct Togl *togl);

  const char* const bad_index_msg = "colormap index out of range";
  const char* const bad_scale_msg = "invalid scaling factor";
}

///////////////////////////////////////////////////////////////////////
// ObjTogl namespace definitions
///////////////////////////////////////////////////////////////////////

int ObjTogl::dumpCmapCmd(struct Togl *togl, int argc, char *argv[]) {
DOTRACE("ObjTogl::dumpCmapCmd");
  Tcl_Interp *interp = Togl_Interp(togl);
  if (argc < 2 || argc > 4) {
    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
                           "wrong # args: should be \"pathname ",
                           argv[1], " [start_index] [end_index]\"",
                           (char *) NULL);
    return TCL_ERROR;
  }

  int start=0;
  int end=255;
  if (argc > 2)
    if (Tcl_GetInt(interp, argv[2], &start) != TCL_OK) return TCL_ERROR;
  if (argc > 3)
    if (Tcl_GetInt(interp, argv[3], &end) != TCL_OK) return TCL_ERROR;

  if (start < 0 || end < 0 || start > 255 || end > 255) {
	 err_message(interp, argv+1, bad_index_msg);
	 return TCL_ERROR;
  }

  Tk_Window tkwin = Togl_TkWin(togl);
  Display *display = Tk_Display(reinterpret_cast<Tk_FakeWin *>(tkwin));
  Colormap cmap = Togl_Colormap(togl);
  XColor col;

  const int BUF_SIZE = 40;
  char buf[BUF_SIZE];
  int i;
  for (i = start; i <= end; i++) {
	 buf[0] = '\0';
	 ostrstream ost(buf, BUF_SIZE);
    col.pixel = i;
    XQueryColor(display, cmap, &col);
    ost << dec << i << '\t' 
		  << hex << (col.red / 256) << '\t' 
		  << hex << (col.green / 256) << '\t' 
		  << hex << (col.blue / 256) << '\n' << '\0';
    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), buf, NULL);
  }
  return TCL_OK;
}

int ObjTogl::dumpEpsCmd(struct Togl *togl, int argc, char *argv[]) {
DOTRACE("ObjTogl::dumpEpsCmd");
  Tcl_Interp *interp = Togl_Interp(togl);
  if (argc != 3) {
    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), 
                           "wrong # args: should be \"pathname ",
                           argv[1], " dest_file\"", (char *) NULL);
    // this will have to wait until Togl supports Tcl objects
    //    Tcl_WrongNumArgs(interp, 1, objv, "dest_file");
    return TCL_ERROR;
  }

  char *dest_file = argv[2];
  if (dest_file == NULL) {
    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), argv[1],
                           ": bad filename", (char *) NULL);
    return TCL_ERROR;
  }

  const int rgbFlag = 0;
  Togl_DumpToEpsFile(togl, dest_file, rgbFlag, toglEpsCallback);
  toglDisplayCallback(togl);
  return TCL_OK;
}

Tcl_Obj* ObjTogl::getParamValue(const struct Togl *togl, char *param) {
  // Build a Tcl command string to fetch the value of param
  // command will look like:
  //     pathname configure -param
  string cmd_str = 
	 string(Tk_PathName(reinterpret_cast<Tk_FakeWin *>(Togl_TkWin(togl))));
  cmd_str += " configure -";
  cmd_str += param;
  Tcl_Obj* cmd = Tcl_NewStringObj(cmd_str.c_str(),-1);

  // Remember the old value of the interp's result
  Tcl_Interp *interp = Togl_Interp(togl);
  Tcl_Obj* old_result = Tcl_GetObjResult(interp);

  // Execute the command
  if ( Tcl_EvalObjEx(interp, cmd, 0) != TCL_OK ) return NULL;

  // The resulting list will look like:
  //          -rgba rgba Rgba true 0
  // (i.e.)   -cmd_name all_lower with_upper default_val current_val
  // So...
  // get the current value, the fifth object from the resulting list 
  Tcl_Obj* list = Tcl_GetObjResult(interp);
  Tcl_Obj* obj = NULL;
  if ( Tcl_ListObjIndex(interp, list, 4, &obj) != TCL_OK ) return NULL;

  // Restore the previous result value
  Tcl_SetObjResult(interp, old_result);
  return obj;
}

inline void ObjTogl::reconfigureGL(const struct Togl *togl) {
DOTRACE("ObjTogl::reconfigureGL");
  ToglConfig *config = static_cast<ToglConfig *>(Togl_GetClientData(togl));

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, Togl_Width(togl), Togl_Height(togl));

  if (config->usingFixedScale()) {
    float scale = config->getFixedScale();
    float l = -1 * (Togl_Width(togl)/2.0) / scale;
    float r =      (Togl_Width(togl)/2.0) / scale;
    float b = -1 * (Togl_Height(togl)/2.0) / scale;
    float t =      (Togl_Height(togl)/2.0) / scale;
    glOrtho(l, r, b, t, -1.0, 1.0);
  }

  else { // not usingFixedScale (i.e. minRect instead)

    const Rect minrect = config->getMinRect(); // the minimum Rect
    Rect therect(minrect);        // the actual Rect that we'll build
  
    // the desired conditions are as follows:
    //    (1) therect contains minrect
    //    (2) therect.aspect() == Togl_Aspect(togl)
    //    (3) therect is the smallest rectangle that meets (1) and (2)
    
    float ratio_of_aspects = minrect.aspect() / Togl_Aspect(togl);
    
    if ( ratio_of_aspects < 1 ) { // the available space is too wide...
      therect.widen(1/ratio_of_aspects); // so use some extra width
    }
    else {                        // the available space is too tall...
      therect.heighten(ratio_of_aspects); // and use some extra height
    }
    
    glOrtho(therect.l, therect.r, therect.b, therect.t, -1.0, 1.0);
    
#ifdef LOCAL_DEBUG
    cerr << "minrect: l = " << minrect.l << ", r = " << minrect.r
         << ", b = " << minrect.b << ", t = " << minrect.t << endl;
    cerr << "minrect.aspect() = " << minrect.aspect();
    cerr << "Togl_Aspect(togl) = " << Togl_Aspect(togl) << endl;
    cerr << "glViewport(0, 0, " 
         << Togl_Width(togl) << ", " << Togl_Height(togl) << ")" << endl;
    cerr << "glOrtho(l=" << therect.l << ", r=" << therect.r
         << ", b=" << therect.b << ", t=" << therect.t << ", -1.0, 1.0)" << endl;
#endif
  } // end not usingFixedScale
}

int ObjTogl::setColorCmd(struct Togl *togl, int argc, char *argv[]) {
DOTRACE("ObjTogl::setColorCmd");
  Tcl_Interp *interp = Togl_Interp(togl);
  if (argc != 6) {
    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), 
                           "wrong # args: should be \"pathname ",
                           argv[1], " index r g b\"", (char *) NULL);
    //Tcl_WrongNumArgs(interp, 2, objv, "index r g b");
    return TCL_ERROR;
  }

  int i;
  double r,g,b;
  if (Tcl_GetInt(interp, argv[2], &i) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetDouble(interp, argv[3], &r) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetDouble(interp, argv[4], &g) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetDouble(interp, argv[5], &b) != TCL_OK) return TCL_ERROR;

  if (i<0 || i>255) {
	 err_message(interp, argv+1, bad_index_msg);
	 return TCL_ERROR;
  }

  Togl_SetColor(togl, i, r, g, b);
  return TCL_OK;
}

// this function sets a fixed scaling between pixels and GL coordinates
int ObjTogl::setFixedScaleCmd(struct Togl *togl, int argc, char *argv[]) {
DOTRACE("ObjTogl::setFixedScaleCmd");
  Tcl_Interp *interp = Togl_Interp(togl);
  if (argc != 3) {
    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), 
                           "wrong # args: should be \"pathname ",
                           argv[1], " scale\"", (char *) NULL);
    //    Tcl_WrongNumArgs(interp, 2, objv, "left top right bottom");
    return TCL_ERROR;
  }

  double s;
  if (Tcl_GetDouble(interp, argv[2], &s) != TCL_OK) return TCL_ERROR;
  if (s <= 0.0) {
	 err_message(interp, argv+1, bad_scale_msg);
	 return TCL_ERROR;
  }

  ToglConfig *config = static_cast<ToglConfig *>(Togl_GetClientData(togl));
  config->setFixedScale(s);
  reconfigureGL(togl);
  return TCL_OK;
}

// this function sets the minimum Rect that will be requested when
// OpenGL sets up its projection
int ObjTogl::setMinRectCmd(struct Togl *togl, int argc, char *argv[]) {
DOTRACE("ObjTogl::setMinRectCmd");
  Tcl_Interp *interp = Togl_Interp(togl);
  if (argc != 6) {
    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), 
                           "wrong # args: should be \"pathname ",
                           argv[1], " left top right bottom\"", (char *) NULL);
    return TCL_ERROR;
  }

  double l,t,r,b;
  if (Tcl_GetDouble(interp, argv[2], &l) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetDouble(interp, argv[3], &t) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetDouble(interp, argv[4], &r) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetDouble(interp, argv[5], &b) != TCL_OK) return TCL_ERROR;

  // Test for valid rect: right > left && top > bottom. In particular,
  // we must not have right == left or top == bottom since this
  // collapses the space onto one dimension.
  if (r <= l || t <= b) {
	 err_message(interp, argv+1, "invalid rect");
	 return TCL_ERROR;
  }

  ToglConfig *config = static_cast<ToglConfig *>(Togl_GetClientData(togl));
  config->setMinRectLTRB(l,t,r,b);
  reconfigureGL(togl);
  return TCL_OK;
}

// this function sets the angle that is subtended by one unit in GL coords
int ObjTogl::setUnitAngleCmd(struct Togl *togl, int argc, char *argv[]) {
DOTRACE("ObjTogl::setUnitAngleCmd");
  Tcl_Interp *interp = Togl_Interp(togl);
  if (argc != 3) {
    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), 
                           "wrong # args: should be \"pathname ",
                           argv[1], " angle_in_degrees\"", (char *) NULL);
    return TCL_ERROR;
  }

  double deg;
  if (Tcl_GetDouble(interp, argv[2], &deg) != TCL_OK) return TCL_ERROR;
  if (deg <= 0) {
	 err_message(interp, argv+1, "invalid value of unit angle");
	 return TCL_ERROR;
  }

  ToglConfig *config = static_cast<ToglConfig *>(Togl_GetClientData(togl));
  config->setUnitAngle(deg);
  reconfigureGL(togl);
  return TCL_OK;
}

// this function sets the viewing distance
int ObjTogl::setViewingDistanceCmd(struct Togl *togl, int argc, char *argv[]) {
DOTRACE("ObjTogl::setViewingDistanceCmd");
  Tcl_Interp *interp = Togl_Interp(togl);
  if (argc != 3) {
    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), 
                           "wrong # args: should be \"pathname ",
                           argv[1], " viewing_distance\"", (char *) NULL);
    return TCL_ERROR;
  }

  double d;
  if (Tcl_GetDouble(interp, argv[2], &d) != TCL_OK) return TCL_ERROR;

  if (d <= 0.0) {
	 const char* const bad_distance_msg = 
		"look out! you'll hurt your nose trying to view "
		"the screen from that distance! try another value";
	 err_message(interp, argv+1, bad_distance_msg);
    return TCL_ERROR;
  }
  if (d > 1000.0) {
	 const char* const too_far_msg =
		"if you really insist on trying to view the screen from so far away, "
		"you should really invest in a good telescope! try another value";
	 err_message(interp, argv+1, too_far_msg);
	 return TCL_ERROR;
  }

  ToglConfig *config = static_cast<ToglConfig *>(Togl_GetClientData(togl));
  config->setViewingDistIn(d);
  reconfigureGL(togl);
  return TCL_OK;
}

// this function scales the minimum Rect
int ObjTogl::scaleRectCmd(struct Togl *togl, int argc, char *argv[]) {
DOTRACE("ObjTogl::scaleRectCmd");
  Tcl_Interp *interp = Togl_Interp(togl);
  if (argc != 3) {
    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), 
                           "wrong # args: should be \"pathname ",
                           argv[1], " scale\"", (char *) NULL);
    //    Tcl_WrongNumArgs(interp, 2, objv, "left top right bottom");
    return TCL_ERROR;
  }

  double s;
  if (Tcl_GetDouble(interp, argv[2], &s) != TCL_OK) return TCL_ERROR;
  if (s <= 0.0) {
	 err_message(interp, argv+1, bad_scale_msg);
	 return TCL_ERROR;
  }

  ToglConfig *config = static_cast<ToglConfig *>(Togl_GetClientData(togl));
  Rect rct = config->getMinRect();

  config->setMinRectLTRB(s*rct.l,s*rct.t,s*rct.r,s*rct.b);
  reconfigureGL(togl);
  return TCL_OK;
}

void ObjTogl::toglCreateCallback(struct Togl *togl) {
DOTRACE("ObjTogl::toglCreateCallback");
  // Create a new ToglConfig object using the following default
  // settings: viewing distance = 30 inches, one GL unit == 2.05
  // degrees visual angle
  ToglConfig *config = new ToglConfig(TlistTcl::getTlist(), 30, 2.05);
  Togl_SetClientData(togl, (ClientData) config);

  // check if rgba
  int rgba_val=0;
  Tcl_Obj* rgba_obj = getParamValue(togl, "rgba");
  Tcl_GetIntFromObj(Togl_Interp(togl), rgba_obj, &rgba_val);
  GfxAttribs::setFlagsIf(GfxAttribs::RGBA_FLAG, rgba_val);
  
  int cmap_val=0;
  Tcl_Obj* cmap_obj = getParamValue(togl, "privatecmap");
  Tcl_GetIntFromObj(Togl_Interp(togl), cmap_obj, &cmap_val);
  GfxAttribs::setFlagsIf(GfxAttribs::PRIVATE_CMAP_FLAG, cmap_val);

  if (GfxAttribs::isTrue(GfxAttribs::RGBA_FLAG)) {
    glColor3f(1.0, 1.0, 1.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
  }
  else { // not using rgba
    if (GfxAttribs::isTrue(GfxAttribs::PRIVATE_CMAP_FLAG)) {
      glClearIndex(0);
      glIndexi(1);
    }
    else {
      glClearIndex(Togl_AllocColor(togl, 0.0, 0.0, 0.0));
      glIndexi(Togl_AllocColor(togl, 1.0, 1.0, 1.0));
    }
  }
}

void ObjTogl::toglDestroyCallback(struct Togl *togl) {
DOTRACE("ObjTogl::toglDestroyCallback");
  ToglConfig *config = static_cast<ToglConfig *>(Togl_GetClientData(togl));
  delete config;
}

void ObjTogl::toglDisplayCallback(struct Togl *togl) {
DOTRACE("ObjTogl::toglDisplayCallback");
  ToglConfig *config = static_cast<ToglConfig *>(Togl_GetClientData(togl));
  const Tlist& tlist = config->getTlist();

  glClear(GL_COLOR_BUFFER_BIT);
  tlist.drawCurTrial();
  Togl_SwapBuffers(togl);
}

void ObjTogl::toglEpsCallback(const struct Togl *togl) {
DOTRACE("ObjTogl::toglEpsCallback");
  ToglConfig *config = static_cast<ToglConfig *>(Togl_GetClientData(togl));
  const Tlist& tlist = config->getTlist();

  // save the old color indices for foreground and background
  GLint oldclear, oldindex;
  glGetIntegerv(GL_INDEX_CLEAR_VALUE, &oldclear);
  glGetIntegerv(GL_CURRENT_INDEX, &oldindex);

  glClearIndex(255);
  glClear(GL_COLOR_BUFFER_BIT);
  glIndexi(0);
  tlist.drawCurTrial();
  glFlush();
  
  // restore the old color indices
  glClearIndex(oldclear);
  glIndexi(oldindex);
}

void ObjTogl::toglReshapeCallback(struct Togl *togl) {
DOTRACE("ObjTogl::toglReshapeCallback");
  reconfigureGL(togl);
}

// package initialization procedure
int ObjTogl::Objtogl_Init(Tcl_Interp *interp) {
DOTRACE("ObjTogl::Objtogl_Init");
  Togl_CreateFunc(toglCreateCallback);
  Togl_DestroyFunc(toglDestroyCallback);
  Togl_DisplayFunc(toglDisplayCallback);
  Togl_ReshapeFunc(toglReshapeCallback);

  Togl_CreateCommand("dumpCmap", dumpCmapCmd);
  Togl_CreateCommand("dumpEps", dumpEpsCmd);
  Togl_CreateCommand("scaleRect", scaleRectCmd);
  Togl_CreateCommand("setColor", setColorCmd);
  Togl_CreateCommand("setFixedScale", setFixedScaleCmd);
  Togl_CreateCommand("setMinRect", setMinRectCmd);
  Togl_CreateCommand("setUnitAngle", setUnitAngleCmd);
  Togl_CreateCommand("setViewingDistance", setViewingDistanceCmd);

  Tcl_PkgProvide(interp, "Objtogl", "2.1");
  return TCL_OK;
}

static const char vcid_objtogl_cc[] = "$Header$";
#endif // !OBJTOGL_CC_DEFINED
