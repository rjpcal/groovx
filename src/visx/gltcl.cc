///////////////////////////////////////////////////////////////////////
//
// tclgl.cc
// Rob Peters
// created: Nov-98
// written: Tue Sep 14 14:12:31 1999
// $Id$
//
// This package provides some simple Tcl functions that are wrappers
// for C OpenGL functions. The function names, argument lists, and
// symbolic constants for the Tcl functions are identical to those in
// the analagous C functions.
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLGL_CC_DEFINED
#define TCLGL_CC_DEFINED

#include "tclgl.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <tcl.h>
#include <cmath>                // for sqrt() in drawThickLine
#include <string>
#include <vector>
#include <map>

#include "tclpkg.h"
#include "tclcmd.h"
#include "tclerror.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// TclGL Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

class GLError : public ErrorWithMsg {
public:
  GLError(const string& msg = "") : ErrorWithMsg(msg) {}
};

namespace TclGL {
  class GLPkg;									 

  class GLCmd;

  class glBeginCmd;
  class glCallListCmd;
  class glClearCmd;
  class glClearIndexCmd;
  class glColorCmd;
  class glDeleteListsCmd;
  class glDrawBufferCmd;
  class glEndCmd;
  class glEndListCmd;
  class glFlushCmd;
  class glFrustumCmd;
  class glGenListsCmd;
  class glGetCmd;
  class glGetErrorCmd;
  class glIndexiCmd;
  class glIsListCmd;
  class glLineWidthCmd;
  class glListBaseCmd;
  class glLoadIdentityCmd;
  class glLoadMatrixCmd;
  class glMatrixModeCmd;
  class glNewListCmd;
  class glOrthoCmd;
  class glPolygonModeCmd;
  class glPopMatrixCmd;
  class glPushMatrixCmd;
  class glRotateCmd;
  class glScaleCmd;
  class glTranslateCmd;
  class glVertexCmd;

  class gluLookAtCmd;
  class gluPerspectiveCmd;

  // Tcl functions
  Tcl_ObjCmdProc setBackgroundCmd;
  Tcl_ObjCmdProc setForegroundCmd;
  Tcl_ObjCmdProc drawOneLineCmd;
  Tcl_ObjCmdProc drawThickLineCmd;
  Tcl_ObjCmdProc antialiasCmd;
  Tcl_ObjCmdProc lineInfoCmd;
  Tcl_ObjCmdProc pixelCheckSumCmd;
}

///////////////////////////////////////////////////////////////////////
//
// TclGL Tcl package definitions
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
//
// TclGL::GLCmd base class --
//
// This class inherits from TclCmd, and adds a checkGL() function
// which throws an exception of type GLError with an appropriate
// message if there is an OpenGL error currently pending.
//
//---------------------------------------------------------------------

class TclGL::GLCmd : public TclCmd {
public:
  GLCmd(TclPkg* pkg, const char* cmd_name, const char* usage, 
			  int objc_min=0, int objc_max=100000, bool exact_objc=false) :
	 TclCmd(pkg->interp(), cmd_name, usage, objc_min, objc_max, exact_objc)
  {
 	 DebugEvalNL(cmd_name);
  }
protected:
  void checkGL() throw(GLError) {
	 GLenum status = glGetError();
	 if (status != GL_NO_ERROR) {
		string msg = reinterpret_cast<const char*>(gluErrorString(status));
		throw GLError(msg);
	 }
  }
};

//--------------------------------------------------------------------
//
// TclGL::glBeginCmd --
//
//--------------------------------------------------------------------

class TclGL::glBeginCmd : public TclGL::GLCmd {
public:
  glBeginCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "mode", 2, 2)
  {
	 pkg->linkVarCopy("GL_POINTS",         GL_POINTS);
	 pkg->linkVarCopy("GL_LINES",          GL_LINES);
	 pkg->linkVarCopy("GL_LINE_STRIP",     GL_LINE_STRIP);
	 pkg->linkVarCopy("GL_LINE_LOOP",      GL_LINE_LOOP);
	 pkg->linkVarCopy("GL_TRIANGLES",      GL_TRIANGLES);
	 pkg->linkVarCopy("GL_TRIANGLE_STRIP", GL_TRIANGLE_STRIP);
	 pkg->linkVarCopy("GL_TRIANGLE_FAN",   GL_TRIANGLE_FAN);
	 pkg->linkVarCopy("GL_QUADS",          GL_QUADS);
	 pkg->linkVarCopy("GL_QUAD_STRIP",     GL_QUAD_STRIP);
	 pkg->linkVarCopy("GL_POLYGON",        GL_POLYGON);
  }

protected:
  virtual void invoke() {
	 GLenum mode = getIntFromArg(1);
	 glBegin(mode);
	 checkGL();
  }
};

//---------------------------------------------------------------------
//
// TclGL::glCallListCmd --
//
//---------------------------------------------------------------------

class TclGL::glCallListCmd : public TclGL::GLCmd {
public:
  glCallListCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "list", 2, 2) {}
protected:
  virtual void invoke() {
	 GLuint list_id = getIntFromArg(1);
	 glCallList(list_id);
  }
};

//---------------------------------------------------------------------
//
// TclGL::glClearCmd --
//
//---------------------------------------------------------------------

class TclGL::glClearCmd : public TclGL::GLCmd {
public:
  glClearCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "mask_bits", 2, 2)
  {
	 pkg->linkVarCopy("GL_COLOR_BUFFER_BIT", GL_COLOR_BUFFER_BIT);
	 pkg->linkVarCopy("GL_DEPTH_BUFFER_BIT", GL_DEPTH_BUFFER_BIT);
	 pkg->linkVarCopy("GL_ACCUM_BUFFER_BIT", GL_ACCUM_BUFFER_BIT);
	 pkg->linkVarCopy("GL_STENCIL_BUFFER_BIT", GL_STENCIL_BUFFER_BIT);
  }
protected:
  virtual void invoke() {
	 GLbitfield mask = getIntFromArg(1);
	 glClear(mask);
	 checkGL();
  }
};

//---------------------------------------------------------------------
//
// TclGL::glClearIndexCmd --
//
//---------------------------------------------------------------------

class TclGL::glClearIndexCmd : public TclGL::GLCmd {
public:
  glClearIndexCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "index", 2, 2) {}
protected:
  virtual void invoke() {
	 // For some reason OpenGL takes float's for color-indices
	 GLfloat index = getIntFromArg(1);
	 glClearIndex(index);
	 checkGL();
  }
};

//---------------------------------------------------------------------
//
// TclGL::glColorCmd --
//
//---------------------------------------------------------------------

class TclGL::glColorCmd : public TclGL::GLCmd {
public:
  glColorCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "red green blue ?alpha=1.0?", 4, 5) {}
protected:
  virtual void invoke() {
	 GLdouble red   = getDoubleFromArg(1);
	 GLdouble green = getDoubleFromArg(2);
	 GLdouble blue  = getDoubleFromArg(3);
	 GLdouble alpha = (objc() < 5) ? 1.0 : getDoubleFromArg(4);

	 glColor4d(red, green, blue, alpha);
  }
};

//---------------------------------------------------------------------
//
// TclGL::glDeleteListsCmd --
//
//---------------------------------------------------------------------

class TclGL::glDeleteListsCmd : public TclGL::GLCmd {
public:
  glDeleteListsCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "list_id range", 3, 3) {}
protected:
  virtual void invoke() {
	 GLuint list_id = getIntFromArg(1);
	 GLsizei range = getIntFromArg(2);
	 glDeleteLists(list_id, range);
  }
};

//---------------------------------------------------------------------
//
// TclGL::glDrawBufferCmd --
//
//---------------------------------------------------------------------

class TclGL::glDrawBufferCmd : public TclGL::GLCmd {
public:
  glDrawBufferCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "mode", 2, 2)
  {
	 pkg->linkVarCopy("GL_NONE", GL_NONE);
	 pkg->linkVarCopy("GL_FRONT_LEFT", GL_FRONT_LEFT);
	 pkg->linkVarCopy("GL_FRONT_RIGHT", GL_FRONT_RIGHT);
	 pkg->linkVarCopy("GL_BACK_LEFT", GL_BACK_LEFT);
	 pkg->linkVarCopy("GL_BACK_RIGHT", GL_BACK_RIGHT);
	 pkg->linkVarCopy("GL_FRONT", GL_FRONT);
	 pkg->linkVarCopy("GL_BACK", GL_BACK);
	 pkg->linkVarCopy("GL_LEFT", GL_LEFT);
	 pkg->linkVarCopy("GL_RIGHT", GL_RIGHT);
	 pkg->linkVarCopy("GL_FRONT_AND_BACK", GL_FRONT_AND_BACK);
  }
protected:
  virtual void invoke() {
	 GLenum mode = getIntFromArg(1);
	 glDrawBuffer(mode);
	 checkGL();
  }
};

//--------------------------------------------------------------------
//
// TclGL::glEndCmd --
//
//--------------------------------------------------------------------

class TclGL::glEndCmd : public TclGL::GLCmd {
public:
  glEndCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() { glEnd(); }
};

//---------------------------------------------------------------------
//
// TclGL::glEndListCmd --
//
//---------------------------------------------------------------------

class TclGL::glEndListCmd : public TclGL::GLCmd {
public:
  glEndListCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() { glEndList(); checkGL(); }
};

//--------------------------------------------------------------------
//
// TclGL::glFlushCmd --
//
//--------------------------------------------------------------------

class TclGL::glFlushCmd : public TclGL::GLCmd {
public:
  glFlushCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() { glFlush(); }
};

//---------------------------------------------------------------------
//
// TclGL::glFrustumCmd --
//
//---------------------------------------------------------------------

class TclGL::glFrustumCmd : public TclGL::GLCmd {
public:
  glFrustumCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "left right bottom top zNear zFar", 7, 7) {}
protected:
  virtual void invoke() {
	 GLdouble left   = getDoubleFromArg(1);
	 GLdouble right  = getDoubleFromArg(2);
	 GLdouble bottom = getDoubleFromArg(3);
	 GLdouble top    = getDoubleFromArg(4);
	 GLdouble zNear  = getDoubleFromArg(5);
	 GLdouble zFar   = getDoubleFromArg(6);

	 glFrustum(left, right, bottom, top, zNear, zFar);
	 checkGL();
  }
};

//---------------------------------------------------------------------
//
// TclGL::glGenListsCmd --
//
//---------------------------------------------------------------------

class TclGL::glGenListsCmd : public TclGL::GLCmd {
public:
  glGenListsCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "range", 2, 2) {}
protected:
  virtual void invoke() {
	 GLsizei range = getIntFromArg(1);
	 GLuint base = glGenLists(range);	 
	 checkGL();
	 returnInt(base);
  }
};

//---------------------------------------------------------------------
//
// TclGL::glGetCmd --
//
//---------------------------------------------------------------------

class TclGL::glGetCmd : public TclGL::GLCmd {
protected:
  struct AttribInfo {
	 const char* param_name;
	 GLenum param_tag;
	 int num_values;
  };

  static map<GLenum, const AttribInfo*> theirAttribs;

  void initialize(TclPkg* pkg) {
	 static bool inited = false;

	 if ( inited ) return;

	 static const AttribInfo theAttribs[] = {
		{ "GL_ACCUM_CLEAR_VALUE", GL_ACCUM_CLEAR_VALUE, 4 },
		{ "GL_ACCUM_ALPHA_BITS", GL_ACCUM_ALPHA_BITS, 1 },
		{ "GL_ACCUM_BLUE_BITS", GL_ACCUM_BLUE_BITS, 1 },
		{ "GL_ACCUM_GREEN_BITS", GL_ACCUM_GREEN_BITS, 1 },
		{ "GL_ACCUM_RED_BITS", GL_ACCUM_RED_BITS, 1 },
		{ "GL_ALPHA_BITS", GL_ALPHA_BITS, 1 },
		{ "GL_BLUE_BITS", GL_BLUE_BITS, 1 },
		{ "GL_COLOR_CLEAR_VALUE", GL_COLOR_CLEAR_VALUE, 4 },
		{ "GL_CURRENT_COLOR", GL_CURRENT_COLOR, 4 },
		{ "GL_CURRENT_INDEX", GL_CURRENT_INDEX, 1 },
		{ "GL_DEPTH_BITS", GL_DEPTH_BITS, 1 },
		{ "GL_DEPTH_CLEAR_VALUE", GL_DEPTH_CLEAR_VALUE, 1 },
		{ "GL_INDEX_BITS", GL_INDEX_BITS, 1 },
		{ "GL_INDEX_CLEAR_VALUE", GL_INDEX_CLEAR_VALUE, 1 },
		{ "GL_GREEN_BITS", GL_GREEN_BITS, 1 },
		{ "GL_LINE_WIDTH", GL_LINE_WIDTH, 1 },
		{ "GL_LINE_WIDTH_GRANULARITY", GL_LINE_WIDTH_GRANULARITY, 1 },
		{ "GL_LINE_WIDTH_RANGE", GL_LINE_WIDTH_RANGE, 2 },
		{ "GL_LIST_BASE", GL_LIST_BASE, 1 },
		{ "GL_LIST_INDEX", GL_LIST_INDEX, 1 },
		{ "GL_MATRIX_MODE", GL_MATRIX_MODE, 1 },
		{ "GL_MODELVIEW_MATRIX", GL_MODELVIEW_MATRIX, 16 },
		{ "GL_POLYGON_MODE", GL_POLYGON_MODE, 2 },
		{ "GL_PROJECTION_MATRIX", GL_PROJECTION_MATRIX, 16 },
		{ "GL_RED_BITS", GL_RED_BITS, 1 },
		{ "GL_RGBA_MODE", GL_RGBA_MODE, 1 },
		{ "GL_SUBPIXEL_BITS", GL_SUBPIXEL_BITS, 1 },
		{ "GL_STENCIL_BITS", GL_STENCIL_BITS, 1 },
		{ "GL_STENCIL_CLEAR_VALUE", GL_STENCIL_CLEAR_VALUE, 1 },
		{ "GL_VIEWPORT", GL_VIEWPORT, 4 },

		/*
		{ "GL_CURRENT_NORMAL", GL_CURRENT_NORMAL,  },
		{ "GL_CURRENT_TEXTURE_COORDS", GL_CURRENT_TEXTURE_COORDS,  },
		{ "GL_CURRENT_RASTER_COLOR", GL_CURRENT_RASTER_COLOR,  },
		{ "GL_CURRENT_RASTER_INDEX", GL_CURRENT_RASTER_INDEX,  },
		{ "GL_CURRENT_RASTER_TEXTURE_COORDS", GL_CURRENT_RASTER_TEXTURE_COORDS,  },
		{ "GL_CURRENT_RASTER_POSITION", GL_CURRENT_RASTER_POSITION,  },
		{ "GL_CURRENT_RASTER_POSITION_VALID", GL_CURRENT_RASTER_POSITION_VALID,  },
		{ "GL_CURRENT_RASTER_DISTANCE", GL_CURRENT_RASTER_DISTANCE,  },
		{ "GL_POINT_SMOOTH", GL_POINT_SMOOTH,  },
		{ "GL_POINT_SIZE", GL_POINT_SIZE,  },
		{ "GL_POINT_SIZE_RANGE", GL_POINT_SIZE_RANGE,  },
		{ "GL_POINT_SIZE_GRANULARITY", GL_POINT_SIZE_GRANULARITY,  },
		{ "GL_LINE_SMOOTH", GL_LINE_SMOOTH,  },
		{ "GL_LINE_WIDTH_RANGE", GL_LINE_WIDTH_RANGE,  },
		{ "GL_LINE_WIDTH_GRANULARITY", GL_LINE_WIDTH_GRANULARITY,  },
		{ "GL_LINE_STIPPLE", GL_LINE_STIPPLE,  },
		{ "GL_LINE_STIPPLE_PATTERN", GL_LINE_STIPPLE_PATTERN,  },
		{ "GL_LINE_STIPPLE_REPEAT", GL_LINE_STIPPLE_REPEAT,  },
		{ "GL_LIST_MODE", GL_LIST_MODE,  },
		{ "GL_MAX_LIST_NESTING", GL_MAX_LIST_NESTING,  },
		{ "GL_POLYGON_SMOOTH", GL_POLYGON_SMOOTH,  },
		{ "GL_POLYGON_STIPPLE", GL_POLYGON_STIPPLE,  },
		{ "GL_EDGE_FLAG", GL_EDGE_FLAG,  },
		{ "GL_CULL_FACE", GL_CULL_FACE,  },
		{ "GL_CULL_FACE_MODE", GL_CULL_FACE_MODE,  },
		{ "GL_FRONT_FACE", GL_FRONT_FACE,  },
		{ "GL_LIGHTING", GL_LIGHTING,  },
		{ "GL_LIGHT_MODEL_LOCAL_VIEWER", GL_LIGHT_MODEL_LOCAL_VIEWER,  },
		{ "GL_LIGHT_MODEL_TWO_SIDE", GL_LIGHT_MODEL_TWO_SIDE,  },
		{ "GL_LIGHT_MODEL_AMBIENT", GL_LIGHT_MODEL_AMBIENT,  },
		{ "GL_SHADE_MODEL", GL_SHADE_MODEL,  },
		{ "GL_COLOR_MATERIAL_FACE", GL_COLOR_MATERIAL_FACE,  },
		{ "GL_COLOR_MATERIAL_PARAMETER", GL_COLOR_MATERIAL_PARAMETER,  },
		{ "GL_COLOR_MATERIAL", GL_COLOR_MATERIAL,  },
		{ "GL_FOG", GL_FOG,  },
		{ "GL_FOG_INDEX", GL_FOG_INDEX,  },
		{ "GL_FOG_DENSITY", GL_FOG_DENSITY,  },
		{ "GL_FOG_START", GL_FOG_START,  },
		{ "GL_FOG_END", GL_FOG_END,  },
		{ "GL_FOG_MODE", GL_FOG_MODE,  },
		{ "GL_FOG_COLOR", GL_FOG_COLOR,  },
		{ "GL_DEPTH_RANGE", GL_DEPTH_RANGE,  },
		{ "GL_DEPTH_TEST", GL_DEPTH_TEST,  },
		{ "GL_OCCLUSION_TEST_HP", GL_OCCLUSION_TEST_HP,  },
		{ "GL_OCCLUSION_RESULT_HP", GL_OCCLUSION_RESULT_HP,  },
		{ "GL_DEPTH_WRITEMASK", GL_DEPTH_WRITEMASK,  },
		{ "GL_DEPTH_FUNC", GL_DEPTH_FUNC,  },
		{ "GL_STENCIL_TEST", GL_STENCIL_TEST,  },
		{ "GL_STENCIL_FUNC", GL_STENCIL_FUNC,  },
		{ "GL_STENCIL_VALUE_MASK", GL_STENCIL_VALUE_MASK,  },
		{ "GL_STENCIL_FAIL", GL_STENCIL_FAIL,  },
		{ "GL_STENCIL_PASS_DEPTH_FAIL", GL_STENCIL_PASS_DEPTH_FAIL,  },
		{ "GL_STENCIL_PASS_DEPTH_PASS", GL_STENCIL_PASS_DEPTH_PASS,  },
		{ "GL_STENCIL_REF", GL_STENCIL_REF,  },
		{ "GL_STENCIL_WRITEMASK", GL_STENCIL_WRITEMASK,  },
		{ "GL_NORMALIZE", GL_NORMALIZE,  },
		{ "GL_MODELVIEW_STACK_DEPTH", GL_MODELVIEW_STACK_DEPTH,  },
		{ "GL_PROJECTION_STACK_DEPTH", GL_PROJECTION_STACK_DEPTH,  },
		{ "GL_TEXTURE_STACK_DEPTH", GL_TEXTURE_STACK_DEPTH,  },
		{ "GL_TEXTURE_MATRIX", GL_TEXTURE_MATRIX,  },
		{ "GL_ATTRIB_STACK_DEPTH", GL_ATTRIB_STACK_DEPTH,  },
		{ "GL_CLIENT_ATTRIB_STACK_DEPTH", GL_CLIENT_ATTRIB_STACK_DEPTH,  },
		{ "GL_ALPHA_TEST", GL_ALPHA_TEST,  },
		{ "GL_ALPHA_TEST_FUNC", GL_ALPHA_TEST_FUNC,  },
		{ "GL_ALPHA_TEST_REF", GL_ALPHA_TEST_REF,  },
		{ "GL_DITHER", GL_DITHER,  },
		{ "GL_BLEND_DST", GL_BLEND_DST,  },
		{ "GL_BLEND_SRC", GL_BLEND_SRC,  },
		{ "GL_BLEND", GL_BLEND,  },
		{ "GL_LOGIC_OP_MODE", GL_LOGIC_OP_MODE,  },
		{ "GL_INDEX_LOGIC_OP", GL_INDEX_LOGIC_OP,  },
		{ "GL_COLOR_LOGIC_OP", GL_COLOR_LOGIC_OP,  },
		{ "GL_AUX_BUFFERS", GL_AUX_BUFFERS,  },
		{ "GL_DRAW_BUFFER", GL_DRAW_BUFFER,  },
		{ "GL_READ_BUFFER", GL_READ_BUFFER,  },
		{ "GL_SCISSOR_BOX", GL_SCISSOR_BOX,  },
		{ "GL_SCISSOR_TEST", GL_SCISSOR_TEST,  },
		{ "GL_INDEX_WRITEMASK", GL_INDEX_WRITEMASK,  },
		{ "GL_COLOR_WRITEMASK", GL_COLOR_WRITEMASK,  },
		{ "GL_INDEX_MODE", GL_INDEX_MODE,  },
		{ "GL_DOUBLEBUFFER", GL_DOUBLEBUFFER,  },
		{ "GL_STEREO", GL_STEREO,  },
		{ "GL_RENDER_MODE", GL_RENDER_MODE,  },
		{ "GL_PERSPECTIVE_CORRECTION_HINT", GL_PERSPECTIVE_CORRECTION_HINT,  },
		{ "GL_POINT_SMOOTH_HINT", GL_POINT_SMOOTH_HINT,  },
		{ "GL_LINE_SMOOTH_HINT", GL_LINE_SMOOTH_HINT,  },
		{ "GL_POLYGON_SMOOTH_HINT", GL_POLYGON_SMOOTH_HINT,  },
		{ "GL_FOG_HINT", GL_FOG_HINT,  },
		{ "GL_GENERATE_MIPMAP_HINT_EXT", GL_GENERATE_MIPMAP_HINT_EXT,  },
		{ "GL_TEXTURE_GEN_S", GL_TEXTURE_GEN_S,  },
		{ "GL_TEXTURE_GEN_T", GL_TEXTURE_GEN_T,  },
		{ "GL_TEXTURE_GEN_R", GL_TEXTURE_GEN_R,  },
		{ "GL_TEXTURE_GEN_Q", GL_TEXTURE_GEN_Q,  },
		{ "GL_PIXEL_MAP_I_TO_I", GL_PIXEL_MAP_I_TO_I,  },
		{ "GL_PIXEL_MAP_S_TO_S", GL_PIXEL_MAP_S_TO_S,  },
		{ "GL_PIXEL_MAP_I_TO_R", GL_PIXEL_MAP_I_TO_R,  },
		{ "GL_PIXEL_MAP_I_TO_G", GL_PIXEL_MAP_I_TO_G,  },
		{ "GL_PIXEL_MAP_I_TO_B", GL_PIXEL_MAP_I_TO_B,  },
		{ "GL_PIXEL_MAP_I_TO_A", GL_PIXEL_MAP_I_TO_A,  },
		{ "GL_PIXEL_MAP_R_TO_R", GL_PIXEL_MAP_R_TO_R,  },
		{ "GL_PIXEL_MAP_G_TO_G", GL_PIXEL_MAP_G_TO_G,  },
		{ "GL_PIXEL_MAP_B_TO_B", GL_PIXEL_MAP_B_TO_B,  },
		{ "GL_PIXEL_MAP_A_TO_A", GL_PIXEL_MAP_A_TO_A,  },
		{ "GL_PIXEL_MAP_I_TO_I_SIZE", GL_PIXEL_MAP_I_TO_I_SIZE,  },
		{ "GL_PIXEL_MAP_S_TO_S_SIZE", GL_PIXEL_MAP_S_TO_S_SIZE,  },
		{ "GL_PIXEL_MAP_I_TO_R_SIZE", GL_PIXEL_MAP_I_TO_R_SIZE,  },
		{ "GL_PIXEL_MAP_I_TO_G_SIZE", GL_PIXEL_MAP_I_TO_G_SIZE,  },
		{ "GL_PIXEL_MAP_I_TO_B_SIZE", GL_PIXEL_MAP_I_TO_B_SIZE,  },
		{ "GL_PIXEL_MAP_I_TO_A_SIZE", GL_PIXEL_MAP_I_TO_A_SIZE,  },
		{ "GL_PIXEL_MAP_R_TO_R_SIZE", GL_PIXEL_MAP_R_TO_R_SIZE,  },
		{ "GL_PIXEL_MAP_G_TO_G_SIZE", GL_PIXEL_MAP_G_TO_G_SIZE,  },
		{ "GL_PIXEL_MAP_B_TO_B_SIZE", GL_PIXEL_MAP_B_TO_B_SIZE,  },
		{ "GL_PIXEL_MAP_A_TO_A_SIZE", GL_PIXEL_MAP_A_TO_A_SIZE,  },
		{ "GL_UNPACK_SWAP_BYTES", GL_UNPACK_SWAP_BYTES,  },
		{ "GL_UNPACK_LSB_FIRST", GL_UNPACK_LSB_FIRST,  },
		{ "GL_UNPACK_ROW_LENGTH", GL_UNPACK_ROW_LENGTH,  },
		{ "GL_UNPACK_SKIP_ROWS", GL_UNPACK_SKIP_ROWS,  },
		{ "GL_UNPACK_SKIP_PIXELS", GL_UNPACK_SKIP_PIXELS,  },
		{ "GL_UNPACK_ALIGNMENT", GL_UNPACK_ALIGNMENT,  },
		{ "GL_UNPACK_SKIP_IMAGES_EXT", GL_UNPACK_SKIP_IMAGES_EXT,  },
		{ "GL_UNPACK_IMAGE_HEIGHT_EXT", GL_UNPACK_IMAGE_HEIGHT_EXT,  },
		{ "GL_PACK_SWAP_BYTES", GL_PACK_SWAP_BYTES,  },
		{ "GL_PACK_LSB_FIRST", GL_PACK_LSB_FIRST,  },
		{ "GL_PACK_ROW_LENGTH", GL_PACK_ROW_LENGTH,  },
		{ "GL_PACK_SKIP_ROWS", GL_PACK_SKIP_ROWS,  },
		{ "GL_PACK_SKIP_PIXELS", GL_PACK_SKIP_PIXELS,  },
		{ "GL_PACK_ALIGNMENT", GL_PACK_ALIGNMENT,  },
		{ "GL_PACK_SKIP_IMAGES_EXT", GL_PACK_SKIP_IMAGES_EXT,  },
		{ "GL_PACK_IMAGE_HEIGHT_EXT", GL_PACK_IMAGE_HEIGHT_EXT,  },
		{ "GL_MAP_COLOR", GL_MAP_COLOR,  },
		{ "GL_MAP_STENCIL", GL_MAP_STENCIL,  },
		{ "GL_INDEX_SHIFT", GL_INDEX_SHIFT,  },
		{ "GL_INDEX_OFFSET", GL_INDEX_OFFSET,  },
		{ "GL_RED_SCALE", GL_RED_SCALE,  },
		{ "GL_RED_BIAS", GL_RED_BIAS,  },
		{ "GL_ZOOM_X", GL_ZOOM_X,  },
		{ "GL_ZOOM_Y", GL_ZOOM_Y,  },
		{ "GL_GREEN_SCALE", GL_GREEN_SCALE,  },
		{ "GL_GREEN_BIAS", GL_GREEN_BIAS,  },
		{ "GL_BLUE_SCALE", GL_BLUE_SCALE,  },
		{ "GL_BLUE_BIAS", GL_BLUE_BIAS,  },
		{ "GL_ALPHA_SCALE", GL_ALPHA_SCALE,  },
		{ "GL_ALPHA_BIAS", GL_ALPHA_BIAS,  },
		{ "GL_DEPTH_SCALE", GL_DEPTH_SCALE,  },
		{ "GL_DEPTH_BIAS", GL_DEPTH_BIAS,  },
		{ "GL_MAX_EVAL_ORDER", GL_MAX_EVAL_ORDER,  },
		{ "GL_MAX_LIGHTS", GL_MAX_LIGHTS,  },
		{ "GL_MAX_CLIP_PLANES", GL_MAX_CLIP_PLANES,  },
		{ "GL_MAX_TEXTURE_SIZE", GL_MAX_TEXTURE_SIZE,  },
		{ "GL_MAX_3D_TEXTURE_SIZE_EXT", GL_MAX_3D_TEXTURE_SIZE_EXT,  },
		{ "GL_MAX_PIXEL_MAP_TABLE", GL_MAX_PIXEL_MAP_TABLE,  },
		{ "GL_MAX_ATTRIB_STACK_DEPTH", GL_MAX_ATTRIB_STACK_DEPTH,  },
		{ "GL_MAX_MODELVIEW_STACK_DEPTH", GL_MAX_MODELVIEW_STACK_DEPTH,  },
		{ "GL_MAX_NAME_STACK_DEPTH", GL_MAX_NAME_STACK_DEPTH,  },
		{ "GL_MAX_PROJECTION_STACK_DEPTH", GL_MAX_PROJECTION_STACK_DEPTH,  },
		{ "GL_MAX_TEXTURE_STACK_DEPTH", GL_MAX_TEXTURE_STACK_DEPTH,  },
		{ "GL_MAX_VIEWPORT_DIMS", GL_MAX_VIEWPORT_DIMS,  },
		{ "GL_MAX_CLIENT_ATTRIB_STACK_DEPTH", GL_MAX_CLIENT_ATTRIB_STACK_DEPTH,  },
		{ "GL_SUBPIXEL_BITS", GL_SUBPIXEL_BITS,  },
		{ "GL_INDEX_BITS", GL_INDEX_BITS,  },
		{ "GL_RED_BITS", GL_RED_BITS,  },
		{ "GL_GREEN_BITS", GL_GREEN_BITS,  },
		{ "GL_BLUE_BITS", GL_BLUE_BITS,  },
		{ "GL_ALPHA_BITS", GL_ALPHA_BITS,  },
		{ "GL_DEPTH_BITS", GL_DEPTH_BITS,  },
		{ "GL_STENCIL_BITS", GL_STENCIL_BITS,  },
		{ "GL_ACCUM_RED_BITS", GL_ACCUM_RED_BITS,  },
		{ "GL_ACCUM_GREEN_BITS", GL_ACCUM_GREEN_BITS,  },
		{ "GL_ACCUM_BLUE_BITS", GL_ACCUM_BLUE_BITS,  },
		{ "GL_ACCUM_ALPHA_BITS", GL_ACCUM_ALPHA_BITS,  },
		{ "GL_NAME_STACK_DEPTH", GL_NAME_STACK_DEPTH,  },
		{ "GL_AUTO_NORMAL", GL_AUTO_NORMAL,  },
		{ "GL_MAP1_COLOR_4", GL_MAP1_COLOR_4,  },
		{ "GL_MAP1_INDEX", GL_MAP1_INDEX,  },
		{ "GL_MAP1_NORMAL", GL_MAP1_NORMAL,  },
		{ "GL_MAP1_TEXTURE_COORD_1", GL_MAP1_TEXTURE_COORD_1,  },
		{ "GL_MAP1_TEXTURE_COORD_2", GL_MAP1_TEXTURE_COORD_2,  },
		{ "GL_MAP1_TEXTURE_COORD_3", GL_MAP1_TEXTURE_COORD_3,  },
		{ "GL_MAP1_TEXTURE_COORD_4", GL_MAP1_TEXTURE_COORD_4,  },
		{ "GL_MAP1_VERTEX_3", GL_MAP1_VERTEX_3,  },
		{ "GL_MAP1_VERTEX_4", GL_MAP1_VERTEX_4,  },
		{ "GL_MAP2_COLOR_4", GL_MAP2_COLOR_4,  },
		{ "GL_MAP2_INDEX", GL_MAP2_INDEX,  },
		{ "GL_MAP2_NORMAL", GL_MAP2_NORMAL,  },
		{ "GL_MAP2_TEXTURE_COORD_1", GL_MAP2_TEXTURE_COORD_1,  },
		{ "GL_MAP2_TEXTURE_COORD_2", GL_MAP2_TEXTURE_COORD_2,  },
		{ "GL_MAP2_TEXTURE_COORD_3", GL_MAP2_TEXTURE_COORD_3,  },
		{ "GL_MAP2_TEXTURE_COORD_4", GL_MAP2_TEXTURE_COORD_4,  },
		{ "GL_MAP2_VERTEX_3", GL_MAP2_VERTEX_3,  },
		{ "GL_MAP2_VERTEX_4", GL_MAP2_VERTEX_4,  },
		{ "GL_MAP1_GRID_DOMAIN", GL_MAP1_GRID_DOMAIN,  },
		{ "GL_MAP1_GRID_SEGMENTS", GL_MAP1_GRID_SEGMENTS,  },
		{ "GL_MAP2_GRID_DOMAIN", GL_MAP2_GRID_DOMAIN,  },
		{ "GL_MAP2_GRID_SEGMENTS", GL_MAP2_GRID_SEGMENTS,  },
		{ "GL_TEXTURE_1D", GL_TEXTURE_1D,  },
		{ "GL_TEXTURE_2D", GL_TEXTURE_2D,  },
		{ "GL_TEXTURE_3D_EXT", GL_TEXTURE_3D_EXT,  },
		{ "GL_FEEDBACK_BUFFER_POINTER", GL_FEEDBACK_BUFFER_POINTER,  },
		{ "GL_FEEDBACK_BUFFER_SIZE", GL_FEEDBACK_BUFFER_SIZE,  },
		{ "GL_FEEDBACK_BUFFER_TYPE", GL_FEEDBACK_BUFFER_TYPE,  },
		{ "GL_SELECTION_BUFFER_POINTER", GL_SELECTION_BUFFER_POINTER,  },
		{ "GL_SELECTION_BUFFER_SIZE", GL_SELECTION_BUFFER_SIZE,  }
		*/
	 };

	 int num_params = sizeof(theAttribs) / sizeof(AttribInfo);
	 for (int i = 0; i < num_params; ++i) {
		pkg->linkVarCopy(theAttribs[i].param_name, (int)theAttribs[i].param_tag);
		theirAttribs[theAttribs[i].param_tag] = &(theAttribs[i]);
	 }

	 inited = true;
  }

public:
  glGetCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "param_name", 2, 2) { initialize(pkg); }

protected:
  virtual void getValues(const AttribInfo* theInfo) = 0;

  virtual void invoke() {
	 GLenum param_tag = getIntFromArg(1);
	 
	 const AttribInfo* theInfo = theirAttribs[param_tag];
	 if ( theInfo == 0 ) { throw TclError("invalid or unsupported enumerant"); }

	 getValues(theInfo);
  }
};

// Unique definition of static member of glGetCmd
map<GLenum, const TclGL::glGetCmd::AttribInfo*> TclGL::glGetCmd::theirAttribs;

//---------------------------------------------------------------------
//
// TclGL::glGetTypeCmd<T> --
//
// A typed template derived from glGetCmd for returning typed info
// from glGetXXX.
//
//---------------------------------------------------------------------

namespace TclGL {

template <class T>
class glGetTypeCmd : public glGetCmd {
public:
  glGetTypeCmd(TclPkg* pkg, const char* cmd_name) :
	 glGetCmd(pkg, cmd_name) {}

protected:
  void extractValues(GLenum tag, T* vals_out);

  virtual void getValues(const AttribInfo* theInfo) {
	 vector<T> theVals(theInfo->num_values);
	 extractValues(theInfo->param_tag, &(theVals[0]));
	 returnSequence(theVals.begin(), theVals.end());
  }
};

}

// Specializations of glGetTypeCmd::extractValues for basic types

template <>
void TclGL::glGetTypeCmd<GLboolean>::extractValues(GLenum tag, GLboolean* vals_out)
{ glGetBooleanv(tag, vals_out); }

template <>
void TclGL::glGetTypeCmd<GLdouble>::extractValues(GLenum tag, GLdouble* vals_out)
{ glGetDoublev(tag, vals_out); }

template <>
void TclGL::glGetTypeCmd<GLint>::extractValues(GLenum tag, GLint* vals_out)
{ glGetIntegerv(tag, vals_out); }

//---------------------------------------------------------------------
//
// TclGL::glGetErrorCmd --
//
//---------------------------------------------------------------------

class TclGL::glGetErrorCmd : public TclGL::GLCmd {
public:
  glGetErrorCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() { checkGL(); }
};

//--------------------------------------------------------------------
//
// TclGL::glIndexiCmd --
//
//--------------------------------------------------------------------

class TclGL::glIndexiCmd : public TclGL::GLCmd {
public:
  glIndexiCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "index", 2, 2) {}
protected:
  virtual void invoke() { 
	 int index = getIntFromArg(1);
	 glIndexi(index);
  }
};

//---------------------------------------------------------------------
//
// TclGL::glIsListCmd --
//
//---------------------------------------------------------------------

class TclGL::glIsListCmd : public TclGL::GLCmd {
public:
  glIsListCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "list_id", 2, 2) {}
protected:
  virtual void invoke() {
	 GLuint list_id = getIntFromArg(1);
	 GLboolean is_list = glIsList(list_id);
	 checkGL();
	 if (is_list == GL_TRUE) returnBool(true);
	 else returnBool(false);
  }
};

//--------------------------------------------------------------------
//
// TclGL::glLineWidthCmd --
//
//--------------------------------------------------------------------

class TclGL::glLineWidthCmd : public TclGL::GLCmd {
public:
  glLineWidthCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "width", 2, 2) {}
protected:
  virtual void invoke() {
	 GLdouble w = getDoubleFromArg(1);
	 glLineWidth(w);
	 checkGL();
  }
};

//---------------------------------------------------------------------
//
// TclGL::glListBaseCmd --
//
//---------------------------------------------------------------------

class TclGL::glListBaseCmd : public TclGL::GLCmd {
public:
  glListBaseCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "base", 2, 2) {}
protected:
  virtual void invoke() { 
	 GLuint base = getIntFromArg(1);
	 glListBase(base);
	 checkGL();
  }
};

//---------------------------------------------------------------------
//
// TclGL::glLoadIdentityCmd --
//
//---------------------------------------------------------------------

class TclGL::glLoadIdentityCmd : public TclGL::GLCmd {
public:
  glLoadIdentityCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
	 glLoadIdentity();
	 checkGL();
  }
};

//---------------------------------------------------------------------
//
// TclGL::glLoadMatrixCmd --
//
//---------------------------------------------------------------------

class TclGL::glLoadMatrixCmd : public TclGL::GLCmd {
public:
  glLoadMatrixCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "4x4_column_major_matrix", 2, 2) {}
protected:
  virtual void invoke() {
	 vector<GLdouble> matrix;

	 getSequenceFromArg(1, back_inserter(matrix), (GLdouble*) 0);

	 if (matrix.size() != 16) {
		throw TclError("matrix must have 16 entries in column-major order");
	 }

	 glLoadMatrixd(&matrix[0]);
	 checkGL();
  }
};

//---------------------------------------------------------------------
//
// TclGL::glMatrixModeCmd --
//
//---------------------------------------------------------------------

class TclGL::glMatrixModeCmd : public TclGL::GLCmd {
public:
  glMatrixModeCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "width", 2, 2)
  {
	 pkg->linkVarCopy("GL_MODELVIEW",  GL_MODELVIEW);
	 pkg->linkVarCopy("GL_PROJECTION", GL_PROJECTION);
	 pkg->linkVarCopy("GL_TEXTURE",    GL_TEXTURE);
  }
protected:
  virtual void invoke() {
	 GLenum mode = getIntFromArg(1);
	 glMatrixMode(mode);
	 checkGL();
  }
};

//---------------------------------------------------------------------
//
// TclGL::glNewListCmd --
//
//---------------------------------------------------------------------

class TclGL::glNewListCmd : public TclGL::GLCmd {
public:
  glNewListCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "list_id mode", 3, 3)
  {
	 pkg->linkVarCopy("GL_COMPILE", GL_COMPILE);
	 pkg->linkVarCopy("GL_COMPILE_AND_EXECUTE", GL_COMPILE_AND_EXECUTE);
  }
protected:
  virtual void invoke() {
	 GLuint list_id = getIntFromArg(1);
	 GLenum mode = getIntFromArg(2);
	 glNewList(list_id, mode);
	 checkGL();
  }
};

//---------------------------------------------------------------------
//
// TclGL::glOrthoCmd --
//
//---------------------------------------------------------------------

class TclGL::glOrthoCmd : public TclGL::GLCmd {
public:
  glOrthoCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "left right bottom top zNear zFar", 7, 7) {}
protected:
  virtual void invoke() {
	 GLdouble left   = getDoubleFromArg(1);
	 GLdouble right  = getDoubleFromArg(2);
	 GLdouble bottom = getDoubleFromArg(3);
	 GLdouble top    = getDoubleFromArg(4);
	 GLdouble zNear  = getDoubleFromArg(5);
	 GLdouble zFar   = getDoubleFromArg(6);

	 glOrtho(left, right, bottom, top, zNear, zFar);
	 checkGL();
  }
};

//---------------------------------------------------------------------
//
// TclGL::glPolygonModeCmd --
//
//---------------------------------------------------------------------

class TclGL::glPolygonModeCmd : public TclGL::GLCmd {
public:
  glPolygonModeCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "face mode", 3, 3)
  {
	 // Face enum's: these are already provided by glDrawBufferCmd
	 //  	 pkg->linkVarCopy("GL_FRONT", GL_FRONT);
	 //  	 pkg->linkVarCopy("GL_BACK", GL_BACK);
	 //  	 pkg->linkVarCopy("GL_FRONT_AND_BACK", GL_FRONT_AND_BACK);
	 // Mode enum's
	 pkg->linkVarCopy("GL_POINT", GL_POINT);
	 pkg->linkVarCopy("GL_LINE", GL_LINE);
	 pkg->linkVarCopy("GL_FILL", GL_FILL);
  }
protected:
  virtual void invoke() { 
	 GLenum face = getIntFromArg(1);
	 GLenum mode = getIntFromArg(2);
	 glPolygonMode(face, mode);
	 checkGL();
  }
};

//---------------------------------------------------------------------
//
// TclGL::glPopMatrixCmd --
//
//---------------------------------------------------------------------

class TclGL::glPopMatrixCmd : public TclGL::GLCmd {
public:
  glPopMatrixCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
	 glPopMatrix();
	 checkGL();
  }
};

//---------------------------------------------------------------------
//
// TclGL::glPushMatrixCmd --
//
//---------------------------------------------------------------------

class TclGL::glPushMatrixCmd : public TclGL::GLCmd {
public:
  glPushMatrixCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
	 glPushMatrix();
	 checkGL();
  }
};

//---------------------------------------------------------------------
//
// TclGL::glRotateCmd --
//
//---------------------------------------------------------------------

class TclGL::glRotateCmd : public TclGL::GLCmd {
public:
  glRotateCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "angle_in_degrees x y z", 5, 5) {}
protected:
  virtual void invoke() {
	 GLdouble angle = getDoubleFromArg(1);
	 GLdouble x = getDoubleFromArg(2);
	 GLdouble y = getDoubleFromArg(3);
	 GLdouble z = getDoubleFromArg(4);

	 glRotated(angle, x, y, z);
	 checkGL();
  }
};

//---------------------------------------------------------------------
//
// TclGL::glScaleCmd --
//
//---------------------------------------------------------------------

class TclGL::glScaleCmd : public TclGL::GLCmd {
public:
  glScaleCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "x y z", 4, 4) {}
protected:
  virtual void invoke() {
	 GLdouble x = getDoubleFromArg(1);
	 GLdouble y = getDoubleFromArg(2);
	 GLdouble z = getDoubleFromArg(3);

	 glScaled(x, y, z);
	 checkGL();
  }
};

//---------------------------------------------------------------------
//
// TclGL::glTranslateCmd --
//
//---------------------------------------------------------------------

class TclGL::glTranslateCmd : public TclGL::GLCmd {
public:
  glTranslateCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "x y z", 4, 4) {}
protected:
  virtual void invoke() {
	 GLdouble x = getDoubleFromArg(1);
	 GLdouble y = getDoubleFromArg(2);
	 GLdouble z = getDoubleFromArg(3);

	 glTranslated(x, y, z);
	 checkGL();
  }
};

//--------------------------------------------------------------------
//
// TclGL::glVertexCmd --
//
//--------------------------------------------------------------------

class TclGL::glVertexCmd : public TclGL::GLCmd {
public:
  glVertexCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "x y ?z=0? ?w=1?", 3, 5) {}
protected:
  virtual void invoke() {
	 GLdouble x = getDoubleFromArg(1);
	 GLdouble y = getDoubleFromArg(2);
	 GLdouble z = (objc() < 4) ? 0.0 : getDoubleFromArg(3);
	 GLdouble w = (objc() < 5) ? 1.0 : getDoubleFromArg(4);

	 if (objc() == 3)      { glVertex2d(x,y); }
	 else if (objc() == 4) { glVertex3d(x,y,z); }
	 else if (objc() == 5) { glVertex4d(x,y,z,w); }
  }
};

//---------------------------------------------------------------------
//
// TclGL::gluLookAtCmd --
//
//---------------------------------------------------------------------

class TclGL::gluLookAtCmd : public TclGL::GLCmd {
public:
  gluLookAtCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "eyeX eyeY eyeZ targX targY targZ upX upY upZ", 10, 10)
	 {}
protected:
  virtual void invoke() {
	 GLdouble eyeX = getDoubleFromArg(1);
	 GLdouble eyeY = getDoubleFromArg(2);
	 GLdouble eyeZ = getDoubleFromArg(3);
	 GLdouble targX = getDoubleFromArg(4);
	 GLdouble targY = getDoubleFromArg(5);
	 GLdouble targZ = getDoubleFromArg(6);
	 GLdouble upX = getDoubleFromArg(7);
	 GLdouble upY = getDoubleFromArg(8);
	 GLdouble upZ = getDoubleFromArg(9);
	 gluLookAt(eyeX, eyeY, eyeZ, targX, targY, targZ, upX, upY, upZ);
  }
};

//---------------------------------------------------------------------
//
// TclGL::gluPerspectiveCmd --
//
//---------------------------------------------------------------------

class TclGL::gluPerspectiveCmd : public TclGL::GLCmd {
public:
  gluPerspectiveCmd(TclPkg* pkg, const char* cmd_name) :
	 GLCmd(pkg, cmd_name, "field_of_view_y aspect zNear zFar", 5, 5) {}
protected:
  virtual void invoke() {
	 GLdouble fovy = getDoubleFromArg(1);
	 GLdouble aspect = getDoubleFromArg(2);
	 GLdouble zNear = getDoubleFromArg(3);
	 GLdouble zFar = getDoubleFromArg(4);
	 gluPerspective(fovy, aspect, zNear, zFar);
  }
};

//--------------------------------------------------------------------
//
// TclGL::setBackgroundCmd --
//
// This command takes either one or three arguments to set either the
// color index or the RGB color of the background in OpenGL. This is
// the color that the buffer will be cleared to when glClear() is called.
//
// Results:
// none.
//
// Side effects: 
// The background color is changed. (But no changes occur on the
// screen until the next call to glClear()).
//
//--------------------------------------------------------------------

int TclGL::setBackgroundCmd(ClientData, Tcl_Interp* interp,
                            int objc, Tcl_Obj* const objv[]) {
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
  
//--------------------------------------------------------------------
//
// TclGL::setForegroundCmd --
//
// This command takes either one or three arguments to set either the
// color index or the RGB color of the foreground in OpenGL. This is
// the color that will be used to render subsequent lines, polygons, etc.
//
// Results:
// none.
//
// Side effects:
// The OpenGL foreground color is changed.
//
//--------------------------------------------------------------------

int TclGL::setForegroundCmd(ClientData, Tcl_Interp* interp,
                            int objc, Tcl_Obj* const objv[]) {
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

//--------------------------------------------------------------------
//
// TclGL::drawOneLineCmd --
//
// This command takes four arguments specifying the x and y
// coordinates of two points between which a line will be drawn.
//
// Results:
// none
//
// Side effects:
// The line is rendered in the OpenGL window.
//
//--------------------------------------------------------------------

int TclGL::drawOneLineCmd(ClientData, Tcl_Interp* interp,
                          int objc, Tcl_Obj* const objv[]) {
DOTRACE("drawOneLineCmd");
  if (objc != 5) {
    Tcl_WrongNumArgs(interp, 1, objv, "x1 y1 x2 y2");
    return TCL_ERROR;
  }

  double* coord = new double[4];
  for (int i = 1; i < objc; ++i) {
    if (Tcl_GetDoubleFromObj(interp, objv[i], coord+i-1) != TCL_OK)
      return TCL_ERROR;
  }
  
  glBegin(GL_LINES);
  glVertex3d( coord[0], coord[1], 0.0);
  glVertex3d( coord[2], coord[3], 0.0);
  glEnd();
  glFlush();

  delete [] coord;
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// TclGL::drawThickLineCmd --
//
// This command takes five arguments specifying two points and a
// thickness. A thick "line" is drawn by drawing a filled rectangle.
//
// Results:
// none.
//
// Side effects:
// The line is rendered in the OpenGL window.
//
//--------------------------------------------------------------------

int TclGL::drawThickLineCmd(ClientData, Tcl_Interp* interp,
                            int objc, Tcl_Obj* const objv[]) {
DOTRACE("drawThickLineCmd");
  if (objc != 6) {
    Tcl_WrongNumArgs(interp, 1, objv, "x1 y1 x2 y2 thickness");
    return TCL_ERROR;
  }

  double* coord = new double[5]; // fill with x1 y1 x2 y2 thickness
  int i = 1;
  double* p = coord;
  for ( ; i < objc; ++i, ++p) {
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
  glVertex3d( coord[0]+c, coord[1]+d, 0.0);
  glVertex3d( coord[0]-c, coord[1]-d, 0.0);

  glVertex3d( coord[2]-c, coord[3]-d, 0.0);
  glVertex3d( coord[2]+c, coord[3]+d, 0.0);
  glEnd();
  glFlush();

  delete [] coord;
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// TclGL::antialiasCmd --
//
// Turns on or off the OpenGL modes required for antialiasing lines
// and polygons. Antialiasing works best with RGBA mode, and not so
// well with color index mode.
//
// Results:
// none.
//
// Side effects:
// The necessary OpenGL modes are enabled or disabled.
//
//--------------------------------------------------------------------

int TclGL::antialiasCmd(ClientData, Tcl_Interp* interp,
                        int objc, Tcl_Obj* const objv[]) {
DOTRACE("antialiasCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "on_off");
    return TCL_ERROR;
  }
    
  int on_off;
  if (Tcl_GetBooleanFromObj(interp, objv[1], &on_off) != TCL_OK) 
	 return TCL_ERROR;
  
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

//--------------------------------------------------------------------
//
// TclGL::lineInfoCmd --
//
// Prints the LINE_WIDTH_RANGE and LINE_WIDTH_GRANULARITY for the
// current OpenGL implementation.
//
// Results:
// Returns a string describing the line parameters.
//
// Side effects:
// none.
//
//--------------------------------------------------------------------

int TclGL::lineInfoCmd(ClientData, Tcl_Interp* interp,
                       int objc, Tcl_Obj* const objv[]) {
DOTRACE("lineInfoCmd");
  if (objc != 1) {
	 Tcl_WrongNumArgs(interp, 1, objv, NULL);
	 return TCL_ERROR;
  }

  GLdouble range[2] = {-1.0,-1.0}, gran=-1.0;
  glGetDoublev(GL_LINE_WIDTH_RANGE, &range[0]);
  glGetDoublev(GL_LINE_WIDTH_GRANULARITY, &gran);
  Tcl_Obj* r1 = Tcl_NewDoubleObj(range[0]);
  Tcl_Obj* r2 = Tcl_NewDoubleObj(range[1]);
  Tcl_Obj* g = Tcl_NewDoubleObj(gran);
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

//--------------------------------------------------------------------
//
// TclGL::pixelCheckSumCmd --
//
// This command returns the sum of the color indices of all the pixels
// in a specified rectangle. It can be used as an easy way to see if
// something is present within a given rectangle on the screen. For
// example, if the background is index 0, and the foreground is index
// 1, than this command can tell how many pixels were drawn by a
// particular drawing command. However-- it is not at all speedy, so
// it is best used for testing only.
//
//--------------------------------------------------------------------

int TclGL::pixelCheckSumCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("TclGL::pixelCheckSumCmd");
  if (objc != 1 && objc != 5) {
    Tcl_WrongNumArgs(interp, 1, objv, "x y width height");
    return TCL_ERROR;
  }

  int x,y,w,h;
  if (objc == 5) {
	 if (Tcl_GetIntFromObj(interp, objv[1], &x) != TCL_OK) return TCL_ERROR;
	 if (Tcl_GetIntFromObj(interp, objv[2], &y) != TCL_OK) return TCL_ERROR;
	 if (Tcl_GetIntFromObj(interp, objv[3], &w) != TCL_OK) return TCL_ERROR;
	 if (Tcl_GetIntFromObj(interp, objv[4], &h) != TCL_OK) return TCL_ERROR;
  }
  else {
	 GLint viewport[4];
	 glGetIntegerv(GL_VIEWPORT, viewport);
	 x = viewport[0];
	 y = viewport[1];
	 w = viewport[2];
	 h = viewport[3];
  }


  GLubyte* pixels = new GLubyte[w*h];
  glReadPixels(x,y,w,h,GL_COLOR_INDEX, GL_UNSIGNED_BYTE, pixels);

  long int sum = 0;
  for (int i = 0; i < w*h; ++i) {
	 sum += pixels[i];
  }
  delete [] pixels;

  Tcl_SetObjResult(interp, Tcl_NewIntObj(sum));
  return TCL_OK;
}

//---------------------------------------------------------------------
//
// GLPkg --
//
//---------------------------------------------------------------------

class TclGL::GLPkg : public TclPkg {
public:
  GLPkg(Tcl_Interp* interp) :
	 TclPkg(interp, "Tclgl", "2.0")
  {
	 addCommand( new glBeginCmd        (this, "glBegin") );
	 addCommand( new glCallListCmd     (this, "glCallList") );
	 addCommand( new glClearCmd        (this, "glClear") );
	 addCommand( new glClearIndexCmd   (this, "glClearIndex") );
	 addCommand( new glColorCmd        (this, "glColor") );
	 addCommand( new glDeleteListsCmd  (this, "glDeleteLists") );
	 addCommand( new glDrawBufferCmd   (this, "glDrawBuffer") );
	 addCommand( new glEndCmd          (this, "glEnd") );
	 addCommand( new glEndListCmd      (this, "glEndList") );
	 addCommand( new glFlushCmd        (this, "glFlush") );
	 addCommand( new glFrustumCmd      (this, "glFrustum") );
	 addCommand( new glGenListsCmd     (this, "glGenLists") );
	 addCommand( new glGetErrorCmd     (this, "glGetError") );
	 addCommand( new glGetTypeCmd<GLboolean>(this, "glGetBoolean") );
	 addCommand( new glGetTypeCmd<GLdouble>(this, "glGetDouble") );
	 addCommand( new glGetTypeCmd<GLint>(this, "glGetInteger") );
	 addCommand( new glIndexiCmd       (this, "glIndexi") );
	 addCommand( new glIsListCmd       (this, "glIsList") );
	 addCommand( new glLineWidthCmd    (this, "glLineWidth") );
	 addCommand( new glListBaseCmd     (this, "glListBase") );
	 addCommand( new glLoadIdentityCmd (this, "glLoadIdentity") );
	 addCommand( new glLoadMatrixCmd   (this, "glLoadMatrix") );	 
	 addCommand( new glMatrixModeCmd   (this, "glMatrixMode") );
	 addCommand( new glNewListCmd      (this, "glNewList") );
	 addCommand( new glOrthoCmd        (this, "glOrtho") );
	 addCommand( new glPolygonModeCmd  (this, "glPolygonMode") );
	 addCommand( new glPopMatrixCmd    (this, "glPopMatrix") );
	 addCommand( new glPushMatrixCmd   (this, "glPushMatrix") );
	 addCommand( new glRotateCmd       (this, "glRotate") );
	 addCommand( new glScaleCmd        (this, "glScale") );
	 addCommand( new glTranslateCmd    (this, "glTranslate") );
	 addCommand( new glVertexCmd       (this, "glVertex") );

	 addCommand( new gluLookAtCmd      (this, "gluLookAt") );
	 addCommand( new gluPerspectiveCmd (this, "gluPerspective") );
  }
};

//--------------------------------------------------------------------
//
// TclGL::Tclgl_Init --
//
//--------------------------------------------------------------------

int Tclgl_Init(Tcl_Interp* interp) {
DOTRACE("Tclgl_Init");

  new TclGL::GLPkg(interp); 

  using namespace TclGL;

  struct CmdName_CmdProc {
	 const char* cmdName;
	 Tcl_ObjCmdProc* cmdProc;
  };

  static CmdName_CmdProc Names_Procs[] = {
    { "setBackground", setBackgroundCmd },
    { "setForeground", setForegroundCmd },
    { "drawOneLine", drawOneLineCmd },
    { "drawThickLine", drawThickLineCmd },
    { "antialias", antialiasCmd },
    { "lineInfo", lineInfoCmd },
    { "pixelCheckSum", pixelCheckSumCmd }
  };

  // Add all commands to the global namespace.
  for (int i = 0; i < sizeof(Names_Procs)/sizeof(CmdName_CmdProc); ++i) {
    Tcl_CreateObjCommand(interp, 
                         const_cast<char *>(Names_Procs[i].cmdName),
                         Names_Procs[i].cmdProc,
                         (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  }

  return TCL_OK;
}

static const char vcid_tclgl_cc[] = "$Header$";
#endif // !TCLGL_CC_DEFINED
