///////////////////////////////////////////////////////////////////////
//
// objtogl.cc
// Rob Peters
// created: Nov-98
// written: Mon Jul  5 18:56:56 1999
// $Id$
//
// This package provides functionality that allows a Togl widget to
// work with a Tlist, controlling its display, reshaping, etc.
//
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
#include "tclcmd.h"
#include "tclevalcmd.h"
#include "tclitempkg.h"
#include "toglconfig.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// ObjTogl namespace declarations
//
///////////////////////////////////////////////////////////////////////

namespace ObjTogl {
  string pathname = "";
  Togl* widget = 0;
  bool toglCreated = false;
  
  class ToglCmd;

  class InitCmd;

  class DumpCmapCmd;
  class DumpEpsCmd;
//   class HeightCmd;
  class InitedCmd;
//   class ScaleRectCmd;
  class SetColorCmd;
//  class SetFixedScaleCmd;
  class SetMinRectCmd;
//   class SetUnitAngleCmd;
//   class SetViewingDistanceCmd;
//   class SwapBuffersCmd;
//   class WidthCmd;

  class ObjToglPkg;

  inline double Togl_Aspect(const struct Togl* togl) {
    return double(Togl_Width(togl)) / double(Togl_Height(togl));
  }

  Tcl_Obj* getParamValue(const struct Togl* togl, char* param);
//   void reconfigureGL(const struct Togl* togl);

  void toglCreateCallback(struct Togl* togl);
  void toglDestroyCallback(struct Togl* togl);
  void toglDisplayCallback(struct Togl* togl);
  void toglEpsCallback(const struct Togl* togl);
  void toglReshapeCallback(struct Togl* togl);

  const char* const bad_index_msg = "colormap index out of range";
  const char* const bad_scale_msg = "invalid scaling factor";
}

///////////////////////////////////////////////////////////////////////
//
// ObjTogl namespace definitions
//
///////////////////////////////////////////////////////////////////////

const char* ObjTogl::thePathname() {
DOTRACE("ObjTogl::thePathname");
  if (!toglCreated) { throw TclError("Togl not yet initialized"); }
  Assert(pathname != "");
  return pathname.c_str();
}

Togl* ObjTogl::theWidget() {
DOTRACE("ObjTogl::theWidget");
  if (!toglCreated) { throw TclError("Togl not yet initalized"); }
  Assert(widget != 0);
  return widget;
}

//---------------------------------------------------------------------
//
// getParamValue --
//
//---------------------------------------------------------------------

Tcl_Obj* ObjTogl::getParamValue(const struct Togl* togl, char* param) {
DOTRACE("ObjTogl::getParamValue");
  // Build a Tcl command string to fetch the value of param
  // command will look like:
  //     pathname configure -param
  string cmd_str = 
	 string(Tk_PathName(reinterpret_cast<Tk_FakeWin *>(Togl_TkWin(togl))));
  cmd_str += " configure -";
  cmd_str += param;
  Tcl_Obj* cmd = Tcl_NewStringObj(cmd_str.c_str(),-1);

  // Remember the old value of the interp's result
  Tcl_Interp* interp = Togl_Interp(togl);

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
  Tcl_IncrRefCount(obj);

  // Restore the previous result value
  Tcl_ResetResult(interp);
  return obj;
}

//---------------------------------------------------------------------
//
// reconfigureGL --
//
//---------------------------------------------------------------------

// inline void ObjTogl::reconfigureGL(const struct Togl* togl) {
// DOTRACE("ObjTogl::reconfigureGL");
//   ToglConfig* config = static_cast<ToglConfig *>(Togl_GetClientData(togl));

//   glMatrixMode(GL_PROJECTION);
//   glLoadIdentity();
//   glViewport(0, 0, Togl_Width(togl), Togl_Height(togl));

//   if (config->usingFixedScale()) {
//     double scale = config->getFixedScale();
//     double l = -1 * (Togl_Width(togl)/2.0) / scale;
//     double r =      (Togl_Width(togl)/2.0) / scale;
//     double b = -1 * (Togl_Height(togl)/2.0) / scale;
//     double t =      (Togl_Height(togl)/2.0) / scale;
//     glOrtho(l, r, b, t, -1.0, 1.0);
//   }

//   else { // not usingFixedScale (i.e. minRect instead)

//     const Rect<double> minrect = config->getMinRect(); // the minimum Rect
//     Rect<double> therect(minrect);        // the actual Rect that we'll build
  
//     // the desired conditions are as follows:
//     //    (1) therect contains minrect
//     //    (2) therect.aspect() == Togl_Aspect(togl)
//     //    (3) therect is the smallest rectangle that meets (1) and (2)
    
//     double ratio_of_aspects = minrect.aspect() / Togl_Aspect(togl);
    
//     if ( ratio_of_aspects < 1 ) { // the available space is too wide...
//       therect.widen(1/ratio_of_aspects); // so use some extra width
//     }
//     else {                        // the available space is too tall...
//       therect.heighten(ratio_of_aspects); // and use some extra height
//     }
    
//     glOrtho(therect.l, therect.r, therect.b, therect.t, -1.0, 1.0);
    
// 	 DebugEval(minrect.l);
// 	 DebugEval(minrect.r);
// 	 DebugEval(minrect.b);
// 	 DebugEvalNL(minrect.t);
//     DebugEval(minrect.aspect());
//     DebugEvalNL(Togl_Aspect(togl));
// #ifdef LOCAL_DEBUG
// 	 cerr << "glViewport(0, 0, " << Togl_Width(togl) << ", "
// 			<< Togl_Height(togl) << ")" << endl;
//     cerr << "glOrtho(l=" << therect.l << ", r=" << therect.r
//          << ", b=" << therect.b << ", t=" << therect.t << ", -1.0, 1.0)" << endl;
// #endif
//   } // end not usingFixedScale
// }

void ObjTogl::toglCreateCallback(struct Togl* togl) {
DOTRACE("ObjTogl::toglCreateCallback");

  if (toglCreated) {
	 return;
  }
  else { 
	 toglCreated = true;
	 ObjTogl::widget = togl;
	 Tk_Window tkwin = Togl_TkWin(togl);
	 ObjTogl::pathname = Tk_PathName(reinterpret_cast<Tk_FakeWin *>(tkwin));
  }

  // Create a new ToglConfig object using the following default
  // settings: viewing distance = 30 inches, one GL unit == 2.05
  // degrees visual angle
  ToglConfig* config = new ToglConfig(togl, 30, 2.05);
  Togl_SetClientData(togl, static_cast<ClientData>(config));

  // Check for RGBA color
  int rgba_val=0;
  Tcl_Obj* rgba_obj = getParamValue(togl, "rgba");
  Tcl_GetIntFromObj(Togl_Interp(togl), rgba_obj, &rgba_val);
  Tcl_DecrRefCount(rgba_obj);
  GfxAttribs::setFlagsIf(GfxAttribs::RGBA_FLAG, rgba_val);
  

  // Check for private colormap
  int cmap_val=0;
  Tcl_Obj* cmap_obj = getParamValue(togl, "privatecmap");
  Tcl_GetIntFromObj(Togl_Interp(togl), cmap_obj, &cmap_val);
  Tcl_DecrRefCount(cmap_obj);
  GfxAttribs::setFlagsIf(GfxAttribs::PRIVATE_CMAP_FLAG, cmap_val);

  // Check for double-buffering
  int dbl_buffer_val=0;
  Tcl_Obj* dbl_buffer_obj = getParamValue(togl, "double");
  Tcl_GetIntFromObj(Togl_Interp(togl), dbl_buffer_obj, &dbl_buffer_val);
  Tcl_DecrRefCount(dbl_buffer_obj);
  GfxAttribs::setFlagsIf(GfxAttribs::DOUBLE_BUFFER_FLAG, dbl_buffer_val);

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

void ObjTogl::toglDestroyCallback(struct Togl* togl) {
DOTRACE("ObjTogl::toglDestroyCallback");
  ToglConfig* config = static_cast<ToglConfig *>(Togl_GetClientData(togl));
  delete config;
  Togl_SetClientData(togl, static_cast<ClientData>(NULL));
}

void ObjTogl::toglDisplayCallback(struct Togl* togl) {
DOTRACE("ObjTogl::toglDisplayCallback");

  DebugPrintNL("clearing back buffer...");
  glClear(GL_COLOR_BUFFER_BIT);

  DebugPrintNL("drawing the trial...");
  Tlist::theTlist().drawCurTrial();

  DebugPrintNL("swapping the buffers...");
  Togl_SwapBuffers(togl);

  DebugPrintNL("clearing back buffer...");
  glClear(GL_COLOR_BUFFER_BIT);
}

void ObjTogl::toglEpsCallback(const struct Togl*) {
DOTRACE("ObjTogl::toglEpsCallback");
  // save the old color indices for foreground and background
  GLint oldclear, oldindex;
  glGetIntegerv(GL_INDEX_CLEAR_VALUE, &oldclear);
  glGetIntegerv(GL_CURRENT_INDEX, &oldindex);

  glClearIndex(255);
  glClear(GL_COLOR_BUFFER_BIT);
  glIndexi(0);

  Tlist::theTlist().drawCurTrial();

  glFlush();
  
  // restore the old color indices
  glClearIndex(oldclear);
  glIndexi(oldindex);
}

void ObjTogl::toglReshapeCallback(struct Togl* togl) {
DOTRACE("ObjTogl::toglReshapeCallback");
  ToglConfig* config = static_cast<ToglConfig*>(Togl_GetClientData(togl));
  config->reconfigure();
}

//---------------------------------------------------------------------
//
// ToglCmd base class definition --
//
//---------------------------------------------------------------------

class ObjTogl::ToglCmd : public TclCmd {
public:
  ToglCmd(Tcl_Interp* interp, const char* cmd_name, const char* usage,
			 int objc_min, int objc_max, bool exact_objc=false) :
	 TclCmd(interp, cmd_name, usage, objc_min, objc_max, exact_objc) {}
protected:
  virtual void invoke() = 0;
  Togl* getTogl() {
	 if (ObjTogl::widget == 0) { throw TclError("Togl not yet initialized"); }
	 else return ObjTogl::widget;
  }
  ToglConfig* getToglConfig() {
	 if (ObjTogl::widget == 0) { throw TclError("Togl not yet initialized"); }
	 else return static_cast<ToglConfig *>(Togl_GetClientData(ObjTogl::widget));
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::DumpCmapCmd --
//
//---------------------------------------------------------------------

class ObjTogl::DumpCmapCmd : public ObjTogl::ToglCmd {
public:
  DumpCmapCmd(Tcl_Interp* interp, const char* cmd_name) :
	 ToglCmd(interp, cmd_name, "?start_index=0? ?end_index=255?", 1, 3, false) {}
protected:
  virtual void invoke() {
	 int start = (objc() < 2) ? 0   : getIntFromArg(1);
	 int end   = (objc() < 3) ? 255 : getIntFromArg(2);

	 if (start < 0 || end < 0 || start > 255 || end > 255) {
		throw TclError(bad_index_msg);
	 }

	 Togl* togl = getTogl();
	 Tk_Window tkwin = Togl_TkWin(togl);
	 Display* display = Tk_Display(reinterpret_cast<Tk_FakeWin *>(tkwin));
	 Colormap cmap = Togl_Colormap(togl);
	 XColor col;
	 
	 const int BUF_SIZE = 40;
	 char buf[BUF_SIZE];
	 int i;
	 for (i = start; i <= end; ++i) {
		buf[0] = '\0';
		ostrstream ost(buf, BUF_SIZE);
		col.pixel = i;
		XQueryColor(display, cmap, &col);
		ost << dec << i << '\t' 
			 << hex << (col.red / 256) << '\t' 
			 << hex << (col.green / 256) << '\t' 
			 << hex << (col.blue / 256) << '\n' << '\0';
		lappendVal(buf);
	 }
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::DumpEpsCmd --
//
//---------------------------------------------------------------------

class ObjTogl::DumpEpsCmd : public ObjTogl::ToglCmd {
public:
  DumpEpsCmd(Tcl_Interp* interp, const char* cmd_name) :
	 ToglCmd(interp, cmd_name, "filename", 2, 2) {}
protected:
  virtual void invoke() {
	 const char* dest_file = getCstringFromArg(1);

	 Togl* togl = getTogl();

	 const int rgbFlag = 0;
	 Togl_DumpToEpsFile(togl, dest_file, rgbFlag, toglEpsCallback);
	 toglDisplayCallback(togl);

	 returnVoid();
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::HeightCmd --
//
//---------------------------------------------------------------------

// class ObjTogl::HeightCmd : public ObjTogl::ToglCmd {
// public:
//   HeightCmd(Tcl_Interp* interp, const char* cmd_name) :
// 	 ToglCmd(interp, cmd_name, NULL, 1, 1) {}
// protected:
//   virtual void invoke() {
// 	 returnInt(getToglConfig()->getHeight());
// 	 //	 returnInt(Togl_Height(getTogl()));
//   }
// };

//---------------------------------------------------------------------
//
// ObjTogl::InitedCmd --
//
//---------------------------------------------------------------------

class ObjTogl::InitedCmd : public TclCmd {
public:
  InitedCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
	 returnBool(ObjTogl::toglCreated);
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::ScaleRectCmd --
//
//---------------------------------------------------------------------

// class ObjTogl::ScaleRectCmd : public ObjTogl::ToglCmd {
// public:
//   ScaleRectCmd(Tcl_Interp* interp, const char* cmd_name) :
// 	 ToglCmd(interp, cmd_name, "scale", 2, 2) {}
// protected:
//   virtual void invoke() {
// 	 double s = getDoubleFromArg(1);

// 	 if (s <= 0.0) { throw TclError(bad_scale_msg); }

// 	 Togl* togl = getTogl();
// 	 ToglConfig* config = getToglConfig();
// 	 Rect<double> rct = config->getMinRect();

// 	 config->setMinRectLTRB(s*rct.l,s*rct.t,s*rct.r,s*rct.b);
// 	 config->reconfigure();

// 	 returnVoid();
//   }
// };

//---------------------------------------------------------------------
//
// ObjTogl::SetColorCmd --
//
//---------------------------------------------------------------------

class ObjTogl::SetColorCmd : public ObjTogl::ToglCmd {
public:
  SetColorCmd(Tcl_Interp* interp, const char* cmd_name) :
	 ToglCmd(interp, cmd_name, "index r g b", 5, 5) {}
protected:
  virtual void invoke() {
	 int i = getIntFromArg(1);
	 double r = getDoubleFromArg(2);
	 double g = getDoubleFromArg(3);
	 double b = getDoubleFromArg(4);

	 if (i<0 || i>255) { throw TclError(bad_index_msg); }

	 Togl* togl = getTogl();
	 Togl_SetColor(togl, i, r, g, b);
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::SetFixedScaleCmd --
//
//---------------------------------------------------------------------

// class ObjTogl::SetFixedScaleCmd : public ObjTogl::ToglCmd {
// public:
//   SetFixedScaleCmd(Tcl_Interp* interp, const char* cmd_name) :
// 	 ToglCmd(interp, cmd_name, "scale", 2, 2) {}
// protected:
//   virtual void invoke() {
// 	 double s = getDoubleFromArg(1);

// 	 if (s <= 0.0) { throw TclError(bad_scale_msg); }

// 	 Togl* togl = getTogl();
// 	 ToglConfig* config = getToglConfig();
// 	 config->setFixedScale(s);
// 	 reconfigureGL(togl);
	 
// 	 returnVoid();
//   }
// };

//---------------------------------------------------------------------
//
// ObjTogl::SetMinRectCmd --
//
//---------------------------------------------------------------------

class ObjTogl::SetMinRectCmd : public ObjTogl::ToglCmd {
public:
  SetMinRectCmd(Tcl_Interp* interp, const char* cmd_name) :
	 ToglCmd(interp, cmd_name, "left top right bottom", 5, 5) {}
protected:
  virtual void invoke() {
	 double l = getDoubleFromArg(1);
	 double t = getDoubleFromArg(2);
	 double r = getDoubleFromArg(3);
	 double b = getDoubleFromArg(4);

	 // Test for valid rect: right > left && top > bottom. In
	 // particular, we must not have right == left or top == bottom
	 // since this collapses the space onto one dimension.
	 if (r <= l || t <= b) { throw TclError("invalid rect"); }
	 
	 Togl* togl = getTogl();
	 ToglConfig* config = getToglConfig();
	 config->setMinRectLTRB(l,t,r,b);
	 config->reconfigure();

	 returnVoid();
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::SetUnitAngleCmd --
//
//---------------------------------------------------------------------

// class ObjTogl::SetUnitAngleCmd : public ObjTogl::ToglCmd {
// public:
//   SetUnitAngleCmd(Tcl_Interp* interp, const char* cmd_name) :
// 	 ToglCmd(interp, cmd_name, "angle_in_degrees", 2, 2) {}
// protected:
//   virtual void invoke() {
// 	 double deg = getDoubleFromArg(1);

// 	 if (deg <= 0) { throw TclError("unit angle must be positive"); }

// 	 Togl* togl = getTogl();
// 	 ToglConfig* config = getToglConfig();
// 	 config->setUnitAngle(deg);
// 	 reconfigureGL(togl);

// 	 returnVoid();
//   }
// };

//---------------------------------------------------------------------
//
// ObjTogl::SetViewingDistanceCmd --
//
//---------------------------------------------------------------------

// class ObjTogl::SetViewingDistanceCmd : public ObjTogl::ToglCmd {
// public:
//   SetViewingDistanceCmd(Tcl_Interp* interp, const char* cmd_name) :
// 	 ToglCmd(interp, cmd_name, "distance_in_inches", 2, 2) {}
// protected:
//   virtual void invoke() {
// 	 ToglConfig* config = getToglConfig();

// 	 double dist = getDoubleFromArg(1);
// 	 if (dist <= 0.0) {
// 		throw TclError("look out! you'll hurt your nose trying to view "
// 							"the screen from that distance! try another value");
// 	 }
// 	 if (dist > 1000.0) {
// 		throw TclError("if you really insist on trying to view the screen "
// 							"from so far away, you should really invest "
// 							"in a good telescope! try another value");
// 	 }

// 	 config->setViewingDistIn(dist);
// 	 reconfigureGL(getTogl());
// 	 returnVoid();
//   }
// };

//---------------------------------------------------------------------
//
// ObjTogl::SwapBuffersCmd --
//
//---------------------------------------------------------------------

// class ObjTogl::SwapBuffersCmd : public ObjTogl::ToglCmd {
// public:
//   SwapBuffersCmd(Tcl_Interp* interp, const char* cmd_name) :
// 	 ToglCmd(interp, cmd_name, NULL, 1, 1) {}
// protected:
//   virtual void invoke() {
// 	 Togl_SwapBuffers(getTogl());
// 	 returnVoid();
//   }
// };

//---------------------------------------------------------------------
//
// ObjTogl::WidthCmd --
//
//---------------------------------------------------------------------

// class ObjTogl::WidthCmd : public ObjTogl::ToglCmd {
// public:
//   WidthCmd(Tcl_Interp* interp, const char* cmd_name) :
// 	 ToglCmd(interp, cmd_name, NULL, 1, 1) {}
// protected:
//   virtual void invoke() {
// 	 returnInt(getToglConfig()->getWidth());
// 	 //	 returnInt(Togl_Width(getTogl()));
//   }
// };

//---------------------------------------------------------------------
//
// ObjTogl::InitCmd --
//
//---------------------------------------------------------------------

class ObjTogl::InitCmd : public TclCmd {
public:
  InitCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name,
			  "init_args ?viewing_dist=30? ?gl_unit_angle=2.05?", 2, 4),
	 itsInterp(interp) {}
protected:
  virtual void invoke() {
	 if (toglCreated) { throw TclError("Togl widget already initialized"); }

	 const char* init_args     = getCstringFromArg(1);
	 int         viewing_dist  = (objc() < 3) ? 30 : getIntFromArg(2);
	 double      gl_unit_angle = (objc() < 4) ? 2.05 : getDoubleFromArg(3);

	 string create_cmd_str = string("togl .togl_private ") + init_args;
	 TclEvalCmd create_cmd(create_cmd_str.c_str());
	 if ( create_cmd.invoke(itsInterp) != TCL_OK ) { throw TclError(); }

	 if (ObjTogl::widget == 0) { throw TclError(); }
	 ToglConfig* config =
		static_cast<ToglConfig *>(Togl_GetClientData(ObjTogl::widget));

	 string pack_cmd_str = string("pack ") + pathname + " -expand 1 -fill both";
	 TclEvalCmd pack_cmd(pack_cmd_str.c_str());
	 if ( pack_cmd.invoke(itsInterp) != TCL_OK ) { throw TclError(); }

	 returnVoid();
  }
private:
  Tcl_Interp* itsInterp;
};

//---------------------------------------------------------------------
//
// ObjToglPkg class definition
//
//---------------------------------------------------------------------

class ObjTogl::ObjToglPkg : public TclPkg {
public:
  ObjToglPkg(Tcl_Interp* interp) :
	 TclPkg(interp, "ObjTogl", "3.0")
  {
	 addCommand( new InitCmd               (interp, "Togl::init") );

	 addCommand( new DumpCmapCmd           (interp, "Togl::dumpCmap") );
	 addCommand( new DumpEpsCmd            (interp, "Togl::dumpEps") );
// 	 addCommand( new HeightCmd             (interp, "Togl::height") );
	 addCommand( new InitedCmd             (interp, "Togl::inited") );
// 	 addCommand( new ScaleRectCmd          (interp, "Togl::scaleRect") );
	 addCommand( new SetColorCmd           (interp, "Togl::setColor") );
// 	 addCommand( new SetFixedScaleCmd      (interp, "Togl::setFixedScale") );
	 addCommand( new SetMinRectCmd         (interp, "Togl::setMinRect") );
// 	 addCommand( new SetUnitAngleCmd       (interp, "Togl::setUnitAngle") );
// 	 addCommand( new SetViewingDistanceCmd (interp, "Togl::setViewingDistance") );
// 	 addCommand( new SwapBuffersCmd        (interp, "Togl::swapBuffers") );
// 	 addCommand( new WidthCmd              (interp, "Togl::width") );

	 Togl_CreateFunc(toglCreateCallback);
	 Togl_DestroyFunc(toglDestroyCallback);
	 Togl_DisplayFunc(toglDisplayCallback);
	 Togl_ReshapeFunc(toglReshapeCallback);
  }
};

class ObjToglItemPkg : public CTclItemPkg<ToglConfig> {
public:
  ObjToglItemPkg(Tcl_Interp* interp) :
	 CTclItemPkg<ToglConfig>(interp, "Togl", "3.1", 0)
  {
	 declareAttrib("height",
						new CAttrib<ToglConfig, int>(&ToglConfig::getHeight,
															  &ToglConfig::setHeight));
	 declareSetter("loadFont",
						new CSetter<ToglConfig, const char*>(&ToglConfig::loadFont),
						"fontname");
	 declareSetter("loadFonti",
						new CSetter<ToglConfig, int>(&ToglConfig::loadFonti),
						"fontnumber");
	 declareSetter("scaleRect",
						new CSetter<ToglConfig, double>(&ToglConfig::scaleRect),
						"scale");
	 declareSetter("setFixedScale",
						new CSetter<ToglConfig, double>(&ToglConfig::setFixedScale),
						"scale");
	 declareSetter("setUnitAngle",
						new CSetter<ToglConfig, double>(&ToglConfig::setUnitAngle),
						"angle_in_degrees");
	 declareSetter("setViewingDistance", 
						new CSetter<ToglConfig, double>(
								&ToglConfig::setViewingDistIn),
						"distance_in_inches");
	 declareAction("swapBuffers",
						new CAction<ToglConfig>(&ToglConfig::swapBuffers));
	 declareGetter("usingFixedScale",
						new CGetter<ToglConfig, bool>(&ToglConfig::usingFixedScale));
	 declareAttrib("width",
						new CAttrib<ToglConfig, int>(&ToglConfig::getWidth,
															  &ToglConfig::setWidth));
  }
  ToglConfig* getCItemFromId(int) {
	 if (ObjTogl::widget == 0) { throw TclError("Togl not yet initialized"); }
	 else return static_cast<ToglConfig *>(Togl_GetClientData(ObjTogl::widget));
  }
};

//---------------------------------------------------------------------
//
// Objtogl_Init
//
//---------------------------------------------------------------------

int Objtogl_Init(Tcl_Interp* interp) {
DOTRACE("Objtogl_Init");

  new ObjTogl::ObjToglPkg(interp); 
  new ObjToglItemPkg(interp);

  return TCL_OK;
}

static const char vcid_objtogl_cc[] = "$Header$";
#endif // !OBJTOGL_CC_DEFINED
