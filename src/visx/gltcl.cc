///////////////////////////////////////////////////////////////////////
//
// gltcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Nov-98
// written: Sat Jun 16 18:39:18 2001
// $Id$
//
// This package provides some simple Tcl functions that are wrappers
// for C OpenGL functions. The function names, argument lists, and
// symbolic constants for the Tcl functions are identical to those in
// the analagous C functions.
//
///////////////////////////////////////////////////////////////////////

#ifndef GLTCL_CC_DEFINED
#define GLTCL_CC_DEFINED

#include "tcl/tclpkg.h"
#include "tcl/tclcmd.h"
#include "tcl/tclerror.h"

#include "util/arrays.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <tcl.h>
#include <cmath>                // for sqrt() in drawThickLine
#include <map>

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// TclGL Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

class GLError : public ErrorWithMsg {
public:
  GLError(const char* msg = "") : ErrorWithMsg(msg) {}
};

namespace GLTcl {
  class GLPkg;

  class GLCmd;

  class glBeginCmd;
  class glBlendFuncCmd;
  class glCallListCmd;
  class glClearCmd;
  class glClearColorCmd;
  class glClearIndexCmd;
  class glColorCmd;
  class glDeleteListsCmd;
  class glDrawBufferCmd;
  class glEnableCmd;
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
// GLTcl Tcl package definitions
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
//
// GLTcl::GLCmd base class --
//
// This class inherits from TclCmd, and adds a checkGL() function
// which throws an exception of type GLError with an appropriate
// message if there is an OpenGL error currently pending.
//
//---------------------------------------------------------------------

class GLTcl::GLCmd : public Tcl::TclCmd {
public:
  GLCmd(Tcl::TclPkg* pkg, const char* cmd_name, const char* usage,
           int objc_min=0, int objc_max=100000, bool exact_objc=false) :
    Tcl::TclCmd(pkg->interp(), cmd_name, usage, objc_min, objc_max, exact_objc)
  {
    DebugEvalNL(cmd_name);
  }
protected:
  void checkGL() throw(GLError) {
    GLenum status = glGetError();
    if (status != GL_NO_ERROR) {
      const char* msg = reinterpret_cast<const char*>(gluErrorString(status));
      throw GLError(msg);
    }
  }
};

//--------------------------------------------------------------------
//
// GLTcl::glBeginCmd --
//
//--------------------------------------------------------------------

class GLTcl::glBeginCmd : public GLTcl::GLCmd {
public:
  glBeginCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::glBlendFuncCmd --
//
//---------------------------------------------------------------------

class GLTcl::glBlendFuncCmd : public GLTcl::GLCmd {
public:
  glBlendFuncCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    GLCmd(pkg, cmd_name, "sfactor dfactor", 3, 3)
  {
    pkg->linkVarCopy("GL_ZERO", GL_ZERO);
    pkg->linkVarCopy("GL_ONE", GL_ONE);
    pkg->linkVarCopy("GL_DST_COLOR", GL_DST_COLOR);
    pkg->linkVarCopy("GL_ONE_MINUS_DST_COLOR", GL_ONE_MINUS_DST_COLOR);
    pkg->linkVarCopy("GL_SRC_COLOR", GL_SRC_COLOR);
    pkg->linkVarCopy("GL_ONE_MINUS_SRC_COLOR", GL_ONE_MINUS_SRC_COLOR);
    pkg->linkVarCopy("GL_SRC_ALPHA", GL_SRC_ALPHA);
    pkg->linkVarCopy("GL_ONE_MINUS_SRC_ALPHA", GL_ONE_MINUS_SRC_ALPHA);
    pkg->linkVarCopy("GL_DST_ALPHA", GL_DST_ALPHA);
    pkg->linkVarCopy("GL_ONE_MINUS_DST_ALPHA", GL_ONE_MINUS_DST_ALPHA);
    pkg->linkVarCopy("GL_SRC_ALPHA_SATURATE", GL_SRC_ALPHA_SATURATE);
  }

protected:
  virtual void invoke() {
    GLenum sfactor = getIntFromArg(1);
    GLenum dfactor = getIntFromArg(2);

    glBlendFunc(sfactor, dfactor);
    checkGL();
  }
};

//---------------------------------------------------------------------
//
// GLTcl::glCallListCmd --
//
//---------------------------------------------------------------------

class GLTcl::glCallListCmd : public GLTcl::GLCmd {
public:
  glCallListCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    GLCmd(pkg, cmd_name, "list", 2, 2) {}
protected:
  virtual void invoke() {
    GLuint list_id = getIntFromArg(1);
    glCallList(list_id);
  }
};

//---------------------------------------------------------------------
//
// GLTcl::glClearCmd --
//
//---------------------------------------------------------------------

class GLTcl::glClearCmd : public GLTcl::GLCmd {
public:
  glClearCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::glClearColorCmd --
//
//---------------------------------------------------------------------

class GLTcl::glClearColorCmd : public GLTcl::GLCmd {
public:
  glClearColorCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    GLCmd(pkg, cmd_name, "red green blue alpha", 5, 5) {}
protected:
  virtual void invoke() {
    GLclampf red   = getDoubleFromArg(1);
    GLclampf green = getDoubleFromArg(2);
    GLclampf blue  = getDoubleFromArg(3);
    GLclampf alpha = getDoubleFromArg(4);

    glClearColor(red, green, blue, alpha);
    checkGL();
  }
};

//---------------------------------------------------------------------
//
// GLTcl::glClearIndexCmd --
//
//---------------------------------------------------------------------

class GLTcl::glClearIndexCmd : public GLTcl::GLCmd {
public:
  glClearIndexCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::glColorCmd --
//
//---------------------------------------------------------------------

class GLTcl::glColorCmd : public GLTcl::GLCmd {
public:
  glColorCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::glDeleteListsCmd --
//
//---------------------------------------------------------------------

class GLTcl::glDeleteListsCmd : public GLTcl::GLCmd {
public:
  glDeleteListsCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::glDrawBufferCmd --
//
//---------------------------------------------------------------------

class GLTcl::glDrawBufferCmd : public GLTcl::GLCmd {
public:
  glDrawBufferCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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

//---------------------------------------------------------------------
//
// GLTcl::glEnableCmd --
//
//---------------------------------------------------------------------

class GLTcl::glEnableCmd : public GLTcl::GLCmd {
private:
  static void initEnums(Tcl::TclPkg* pkg) {
    static bool inited = false;

    if (inited) return;

    pkg->linkVarCopy("GL_ALPHA_TEST",  GL_ALPHA_TEST);
    pkg->linkVarCopy("GL_BLEND",  GL_BLEND);
    pkg->linkVarCopy("GL_FOG",  GL_FOG);
    pkg->linkVarCopy("GL_LIGHTING",  GL_LIGHTING);
//     pkg->linkVarCopy("GL_TEXTURE_1D",  GL_TEXTURE_1D);
//     pkg->linkVarCopy("GL_TEXTURE_2D",  GL_TEXTURE_2D);
//     pkg->linkVarCopy("GL_TEXTURE_3D_EXT",  GL_TEXTURE_3D_EXT);
    pkg->linkVarCopy("GL_LINE_STIPPLE",  GL_LINE_STIPPLE);
    pkg->linkVarCopy("GL_POLYGON_STIPPLE",  GL_POLYGON_STIPPLE);
    pkg->linkVarCopy("GL_CULL_FACE",  GL_CULL_FACE);
    pkg->linkVarCopy("GL_INDEX_LOGIC_OP",  GL_INDEX_LOGIC_OP);
    pkg->linkVarCopy("GL_COLOR_LOGIC_OP",  GL_COLOR_LOGIC_OP);
    pkg->linkVarCopy("GL_DITHER",  GL_DITHER);
    pkg->linkVarCopy("GL_STENCIL_TEST",  GL_STENCIL_TEST);
    pkg->linkVarCopy("GL_DEPTH_TEST",  GL_DEPTH_TEST);
//     pkg->linkVarCopy("GL_CLIP_PLANE0",  GL_CLIP_PLANE0);
//     pkg->linkVarCopy("GL_CLIP_PLANE1",  GL_CLIP_PLANE1);
//     pkg->linkVarCopy("GL_CLIP_PLANE2",  GL_CLIP_PLANE2);
//     pkg->linkVarCopy("GL_CLIP_PLANE3",  GL_CLIP_PLANE3);
//     pkg->linkVarCopy("GL_CLIP_PLANE4",  GL_CLIP_PLANE4);
//     pkg->linkVarCopy("GL_CLIP_PLANE5",  GL_CLIP_PLANE5);
//     pkg->linkVarCopy("GL_LIGHT0",  GL_LIGHT0);
//     pkg->linkVarCopy("GL_LIGHT1",  GL_LIGHT1);
//     pkg->linkVarCopy("GL_LIGHT2",  GL_LIGHT2);
//     pkg->linkVarCopy("GL_LIGHT3",  GL_LIGHT3);
//     pkg->linkVarCopy("GL_LIGHT4",  GL_LIGHT4);
//     pkg->linkVarCopy("GL_LIGHT5",  GL_LIGHT5);
//     pkg->linkVarCopy("GL_LIGHT6",  GL_LIGHT6);
//     pkg->linkVarCopy("GL_LIGHT7",  GL_LIGHT7);
//     pkg->linkVarCopy("GL_TEXTURE_GEN_S",  GL_TEXTURE_GEN_S);
//     pkg->linkVarCopy("GL_TEXTURE_GEN_T",  GL_TEXTURE_GEN_T);
//     pkg->linkVarCopy("GL_TEXTURE_GEN_R",  GL_TEXTURE_GEN_R);
//     pkg->linkVarCopy("GL_TEXTURE_GEN_Q",  GL_TEXTURE_GEN_Q);
    pkg->linkVarCopy("GL_MAP1_VERTEX_3",  GL_MAP1_VERTEX_3);
    pkg->linkVarCopy("GL_MAP1_VERTEX_4",  GL_MAP1_VERTEX_4);
    pkg->linkVarCopy("GL_MAP1_COLOR_4",  GL_MAP1_COLOR_4);
    pkg->linkVarCopy("GL_MAP1_INDEX",  GL_MAP1_INDEX);
    pkg->linkVarCopy("GL_MAP1_NORMAL",  GL_MAP1_NORMAL);
//     pkg->linkVarCopy("GL_MAP1_TEXTURE_COORD_1",  GL_MAP1_TEXTURE_COORD_1);
//     pkg->linkVarCopy("GL_MAP1_TEXTURE_COORD_2",  GL_MAP1_TEXTURE_COORD_2);
//     pkg->linkVarCopy("GL_MAP1_TEXTURE_COORD_3",  GL_MAP1_TEXTURE_COORD_3);
//     pkg->linkVarCopy("GL_MAP1_TEXTURE_COORD_4",  GL_MAP1_TEXTURE_COORD_4);
//     pkg->linkVarCopy("GL_MAP2_VERTEX_3",  GL_MAP2_VERTEX_3);
//     pkg->linkVarCopy("GL_MAP2_VERTEX_4",  GL_MAP2_VERTEX_4);
//     pkg->linkVarCopy("GL_MAP2_COLOR_4",  GL_MAP2_COLOR_4);
//     pkg->linkVarCopy("GL_MAP2_INDEX",  GL_MAP2_INDEX);
//     pkg->linkVarCopy("GL_MAP2_NORMAL",  GL_MAP2_NORMAL);
//     pkg->linkVarCopy("GL_MAP2_TEXTURE_COORD_1",  GL_MAP2_TEXTURE_COORD_1);
//     pkg->linkVarCopy("GL_MAP2_TEXTURE_COORD_2",  GL_MAP2_TEXTURE_COORD_2);
//     pkg->linkVarCopy("GL_MAP2_TEXTURE_COORD_3",  GL_MAP2_TEXTURE_COORD_3);
//     pkg->linkVarCopy("GL_MAP2_TEXTURE_COORD_4",  GL_MAP2_TEXTURE_COORD_4);
    pkg->linkVarCopy("GL_POINT_SMOOTH",  GL_POINT_SMOOTH);
    pkg->linkVarCopy("GL_LINE_SMOOTH",  GL_LINE_SMOOTH);
    pkg->linkVarCopy("GL_POLYGON_SMOOTH",  GL_POLYGON_SMOOTH);
    pkg->linkVarCopy("GL_SCISSOR_TEST",  GL_SCISSOR_TEST);
    pkg->linkVarCopy("GL_COLOR_MATERIAL",  GL_COLOR_MATERIAL);
    pkg->linkVarCopy("GL_NORMALIZE",  GL_NORMALIZE);
    pkg->linkVarCopy("GL_AUTO_NORMAL",  GL_AUTO_NORMAL);
    pkg->linkVarCopy("GL_VERTEX_ARRAY",  GL_VERTEX_ARRAY);
    pkg->linkVarCopy("GL_NORMAL_ARRAY",  GL_NORMAL_ARRAY);
    pkg->linkVarCopy("GL_COLOR_ARRAY",  GL_COLOR_ARRAY);
    pkg->linkVarCopy("GL_INDEX_ARRAY",  GL_INDEX_ARRAY);
    pkg->linkVarCopy("GL_TEXTURE_COORD_ARRAY",  GL_TEXTURE_COORD_ARRAY);
    pkg->linkVarCopy("GL_EDGE_FLAG_ARRAY",  GL_EDGE_FLAG_ARRAY);
    pkg->linkVarCopy("GL_POLYGON_OFFSET_POINT",  GL_POLYGON_OFFSET_POINT);
    pkg->linkVarCopy("GL_POLYGON_OFFSET_LINE",  GL_POLYGON_OFFSET_LINE);
    pkg->linkVarCopy("GL_POLYGON_OFFSET_FILL",  GL_POLYGON_OFFSET_FILL);
    pkg->linkVarCopy("GL_RESCALE_NORMAL_EXT",  GL_RESCALE_NORMAL_EXT);

    inited = true;
  }

public:
  glEnableCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    GLCmd(pkg, cmd_name, "capability", 2, 2)
  {
    initEnums(pkg);
  }
protected:
  virtual void invoke() {
    const char* func = getCstringFromArg(0);
    GLenum cap = getIntFromArg(1);

         if ( func[2] == 'E' ) { glEnable(cap); }
    else if ( func[2] == 'D' ) { glDisable(cap); }

    checkGL();
  }
};

//--------------------------------------------------------------------
//
// GLTcl::glEndCmd --
//
//--------------------------------------------------------------------

class GLTcl::glEndCmd : public GLTcl::GLCmd {
public:
  glEndCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    GLCmd(pkg, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() { glEnd(); }
};

//---------------------------------------------------------------------
//
// GLTcl::glEndListCmd --
//
//---------------------------------------------------------------------

class GLTcl::glEndListCmd : public GLTcl::GLCmd {
public:
  glEndListCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    GLCmd(pkg, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() { glEndList(); checkGL(); }
};

//--------------------------------------------------------------------
//
// GLTcl::glFlushCmd --
//
//--------------------------------------------------------------------

class GLTcl::glFlushCmd : public GLTcl::GLCmd {
public:
  glFlushCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    GLCmd(pkg, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() { glFlush(); }
};

//---------------------------------------------------------------------
//
// GLTcl::glFrustumCmd --
//
//---------------------------------------------------------------------

class GLTcl::glFrustumCmd : public GLTcl::GLCmd {
public:
  glFrustumCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::glGenListsCmd --
//
//---------------------------------------------------------------------

class GLTcl::glGenListsCmd : public GLTcl::GLCmd {
public:
  glGenListsCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::glGetCmd --
//
//---------------------------------------------------------------------

class GLTcl::glGetCmd : public GLTcl::GLCmd {
protected:
  struct AttribInfo {
    const char* param_name;
    GLenum param_tag;
    int num_values;
  };

  static std::map<GLenum, const AttribInfo*> theirAttribs;

  void initialize(Tcl::TclPkg* pkg) {
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
  glGetCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    GLCmd(pkg, cmd_name, "param_name", 2, 2) { initialize(pkg); }

protected:
  virtual void getValues(const AttribInfo* theInfo) = 0;

  virtual void invoke() {
    GLenum param_tag = getIntFromArg(1);

    const AttribInfo* theInfo = theirAttribs[param_tag];
    if ( theInfo == 0 ) { throw Tcl::TclError("invalid or unsupported enumerant"); }

    getValues(theInfo);
  }
};

// Unique definition of static member of glGetCmd
std::map<GLenum, const GLTcl::glGetCmd::AttribInfo*> GLTcl::glGetCmd::theirAttribs;

//---------------------------------------------------------------------
//
// GLTcl::glGetTypeCmd<T> --
//
// A typed template derived from glGetCmd for returning typed info
// from glGetXXX.
//
//---------------------------------------------------------------------

namespace GLTcl {

template <class T>
class glGetTypeCmd : public glGetCmd {
public:
  glGetTypeCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    glGetCmd(pkg, cmd_name) {}

protected:
  void extractValues(GLenum tag, T* vals_out);

  virtual void getValues(const AttribInfo* theInfo) {
    fixed_block<T> theVals(theInfo->num_values);
    extractValues(theInfo->param_tag, &(theVals[0]));
    returnSequence(theVals.begin(), theVals.end());
  }
};

}

// Specializations of glGetTypeCmd::extractValues for basic types

template <>
void GLTcl::glGetTypeCmd<GLboolean>::extractValues(GLenum tag, GLboolean* vals_out)
{ glGetBooleanv(tag, vals_out); }

template <>
void GLTcl::glGetTypeCmd<GLdouble>::extractValues(GLenum tag, GLdouble* vals_out)
{ glGetDoublev(tag, vals_out); }

template <>
void GLTcl::glGetTypeCmd<GLint>::extractValues(GLenum tag, GLint* vals_out)
{ glGetIntegerv(tag, vals_out); }

//---------------------------------------------------------------------
//
// GLTcl::glGetErrorCmd --
//
//---------------------------------------------------------------------

class GLTcl::glGetErrorCmd : public GLTcl::GLCmd {
public:
  glGetErrorCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    GLCmd(pkg, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() { checkGL(); }
};

//--------------------------------------------------------------------
//
// GLTcl::glIndexiCmd --
//
//--------------------------------------------------------------------

class GLTcl::glIndexiCmd : public GLTcl::GLCmd {
public:
  glIndexiCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    GLCmd(pkg, cmd_name, "index", 2, 2) {}
protected:
  virtual void invoke() {
    int index = getIntFromArg(1);
    glIndexi(index);
  }
};

//---------------------------------------------------------------------
//
// GLTcl::glIsListCmd --
//
//---------------------------------------------------------------------

class GLTcl::glIsListCmd : public GLTcl::GLCmd {
public:
  glIsListCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::glLineWidthCmd --
//
//--------------------------------------------------------------------

class GLTcl::glLineWidthCmd : public GLTcl::GLCmd {
public:
  glLineWidthCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::glListBaseCmd --
//
//---------------------------------------------------------------------

class GLTcl::glListBaseCmd : public GLTcl::GLCmd {
public:
  glListBaseCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::glLoadIdentityCmd --
//
//---------------------------------------------------------------------

class GLTcl::glLoadIdentityCmd : public GLTcl::GLCmd {
public:
  glLoadIdentityCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    GLCmd(pkg, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
    glLoadIdentity();
    checkGL();
  }
};

//---------------------------------------------------------------------
//
// GLTcl::glLoadMatrixCmd --
//
//---------------------------------------------------------------------

class GLTcl::glLoadMatrixCmd : public GLTcl::GLCmd {
public:
  glLoadMatrixCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    GLCmd(pkg, cmd_name, "4x4_column_major_matrix", 2, 2) {}
protected:
  virtual void invoke() {
    fixed_block<GLdouble> matrix(16);

    unsigned int i = 0;
    for (Tcl::ListIterator<GLdouble>
           itr = beginOfArg(2, (GLdouble*)0),
           end = endOfArg(2, (GLdouble*)0);
         itr != end && i < matrix.size();
         ++itr, ++i)
      {
        matrix[i] = *itr;
      }

    if (matrix.size() != i) {
      throw Tcl::TclError("matrix must have 16 entries in column-major order");
    }

    glLoadMatrixd(&matrix[0]);
    checkGL();
  }
};

//---------------------------------------------------------------------
//
// GLTcl::glMatrixModeCmd --
//
//---------------------------------------------------------------------

class GLTcl::glMatrixModeCmd : public GLTcl::GLCmd {
public:
  glMatrixModeCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::glNewListCmd --
//
//---------------------------------------------------------------------

class GLTcl::glNewListCmd : public GLTcl::GLCmd {
public:
  glNewListCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::glOrthoCmd --
//
//---------------------------------------------------------------------

class GLTcl::glOrthoCmd : public GLTcl::GLCmd {
public:
  glOrthoCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::glPolygonModeCmd --
//
//---------------------------------------------------------------------

class GLTcl::glPolygonModeCmd : public GLTcl::GLCmd {
public:
  glPolygonModeCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    GLCmd(pkg, cmd_name, "face mode", 3, 3)
  {
    // Face enum's: these are already provided by glDrawBufferCmd
    //    pkg->linkVarCopy("GL_FRONT", GL_FRONT);
    //    pkg->linkVarCopy("GL_BACK", GL_BACK);
    //    pkg->linkVarCopy("GL_FRONT_AND_BACK", GL_FRONT_AND_BACK);
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
// GLTcl::glPopMatrixCmd --
//
//---------------------------------------------------------------------

class GLTcl::glPopMatrixCmd : public GLTcl::GLCmd {
public:
  glPopMatrixCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    GLCmd(pkg, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
    glPopMatrix();
    checkGL();
  }
};

//---------------------------------------------------------------------
//
// GLTcl::glPushMatrixCmd --
//
//---------------------------------------------------------------------

class GLTcl::glPushMatrixCmd : public GLTcl::GLCmd {
public:
  glPushMatrixCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    GLCmd(pkg, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
    glPushMatrix();
    checkGL();
  }
};

//---------------------------------------------------------------------
//
// GLTcl::glRotateCmd --
//
//---------------------------------------------------------------------

class GLTcl::glRotateCmd : public GLTcl::GLCmd {
public:
  glRotateCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::glScaleCmd --
//
//---------------------------------------------------------------------

class GLTcl::glScaleCmd : public GLTcl::GLCmd {
public:
  glScaleCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::glTranslateCmd --
//
//---------------------------------------------------------------------

class GLTcl::glTranslateCmd : public GLTcl::GLCmd {
public:
  glTranslateCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::glVertexCmd --
//
//--------------------------------------------------------------------

class GLTcl::glVertexCmd : public GLTcl::GLCmd {
public:
  glVertexCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::gluLookAtCmd --
//
//---------------------------------------------------------------------

class GLTcl::gluLookAtCmd : public GLTcl::GLCmd {
public:
  gluLookAtCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::gluPerspectiveCmd --
//
//---------------------------------------------------------------------

class GLTcl::gluPerspectiveCmd : public GLTcl::GLCmd {
public:
  gluPerspectiveCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
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
// GLTcl::setBackgroundCmd --
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

int GLTcl::setBackgroundCmd(ClientData, Tcl_Interp* interp,
                            int objc, Tcl_Obj* const objv[]) {
DOTRACE("setBackgroundCmd");
  if (objc != 2 && objc != 5) {
    Tcl_WrongNumArgs(interp, 1, objv, "{ [index] or [R G B A] }");
    return TCL_ERROR;
  }

  if (objc == 2) {
    int i;
    if (Tcl_GetIntFromObj(interp, objv[1], &i) != TCL_OK) return TCL_ERROR;
    glClearIndex(i);
  }
  else {
    double r, g, b, a;
    if (Tcl_GetDoubleFromObj(interp, objv[1], &r) != TCL_OK) return TCL_ERROR;
    if (Tcl_GetDoubleFromObj(interp, objv[2], &g) != TCL_OK) return TCL_ERROR;
    if (Tcl_GetDoubleFromObj(interp, objv[3], &b) != TCL_OK) return TCL_ERROR;
    if (Tcl_GetDoubleFromObj(interp, objv[4], &a) != TCL_OK) return TCL_ERROR;
    glClearColor(r, g, b, a);
  }

  return TCL_OK;
}

//--------------------------------------------------------------------
//
// GLTcl::setForegroundCmd --
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

int GLTcl::setForegroundCmd(ClientData, Tcl_Interp* interp,
                            int objc, Tcl_Obj* const objv[]) {
DOTRACE("setForegroundCmd");
  if (objc != 2 && objc != 5) {
    Tcl_WrongNumArgs(interp, 1, objv, "{ [index] or [R G B A] }");
    return TCL_ERROR;
  }

  if (objc == 2) {
    int i;
    if (Tcl_GetIntFromObj(interp, objv[1], &i) != TCL_OK) return TCL_ERROR;
    glIndexi(i);
  }
  else {
    double r, g, b, a;
    if (Tcl_GetDoubleFromObj(interp, objv[1], &r) != TCL_OK) return TCL_ERROR;
    if (Tcl_GetDoubleFromObj(interp, objv[2], &g) != TCL_OK) return TCL_ERROR;
    if (Tcl_GetDoubleFromObj(interp, objv[3], &b) != TCL_OK) return TCL_ERROR;
    if (Tcl_GetDoubleFromObj(interp, objv[4], &a) != TCL_OK) return TCL_ERROR;
    glColor4d(r, g, b, a);
  }

  return TCL_OK;
}

//--------------------------------------------------------------------
//
// GLTcl::drawOneLineCmd --
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

int GLTcl::drawOneLineCmd(ClientData, Tcl_Interp* interp,
                          int objc, Tcl_Obj* const objv[]) {
DOTRACE("drawOneLineCmd");
  if (objc != 5) {
    Tcl_WrongNumArgs(interp, 1, objv, "x1 y1 x2 y2");
    return TCL_ERROR;
  }

  fixed_block<double> coord(4);
  for (int i = 1; i < objc; ++i) {
    if (Tcl_GetDoubleFromObj(interp, objv[i], &(coord.at(i-1))) != TCL_OK)
      return TCL_ERROR;
  }

  glBegin(GL_LINES);
  glVertex3d( coord[0], coord[1], 0.0);
  glVertex3d( coord[2], coord[3], 0.0);
  glEnd();
  glFlush();

  return TCL_OK;
}

//--------------------------------------------------------------------
//
// GLTcl::drawThickLineCmd --
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

int GLTcl::drawThickLineCmd(ClientData, Tcl_Interp* interp,
                            int objc, Tcl_Obj* const objv[]) {
DOTRACE("drawThickLineCmd");
  if (objc != 6) {
    Tcl_WrongNumArgs(interp, 1, objv, "x1 y1 x2 y2 thickness");
    return TCL_ERROR;
  }

  fixed_block<double> coord(5); // fill with x1 y1 x2 y2 thickness
  for (int i = 1; i < objc; ++i) {
    if (Tcl_GetDoubleFromObj(interp, objv[i], &(coord.at(i-1))) != TCL_OK)
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

  return TCL_OK;
}

//--------------------------------------------------------------------
//
// GLTcl::antialiasCmd --
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

int GLTcl::antialiasCmd(ClientData, Tcl_Interp* interp,
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
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
// GLTcl::lineInfoCmd --
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

int GLTcl::lineInfoCmd(ClientData, Tcl_Interp* interp,
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
// GLTcl::pixelCheckSumCmd --
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

int GLTcl::pixelCheckSumCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("GLTcl::pixelCheckSumCmd");
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
    x = viewport[0];                             DebugEval(x);
    y = viewport[1];                             DebugEval(y);
    w = viewport[2];                             DebugEval(w);
    h = viewport[3];                             DebugEvalNL(h);
  }

  GLboolean isRgba;
  glGetBooleanv(GL_RGBA_MODE, &isRgba);

  if (GL_TRUE == isRgba)
    {
      fixed_block<GLfloat> pixels(w*h*3);
      glPixelStorei(GL_PACK_ALIGNMENT, 1);

      glPushAttrib(GL_PIXEL_MODE_BIT);
      glReadBuffer(GL_FRONT);
      glReadPixels(x,y,w,h,GL_RGB, GL_FLOAT, &pixels[0]);
      glPopAttrib();

      GLfloat sum = 0;
      for (unsigned int i = 0; i < pixels.size(); ++i) {
        sum += pixels[i];
      }
      Tcl_SetObjResult(interp, Tcl_NewDoubleObj(sum));
    }
  else
    {
      fixed_block<GLubyte> pixels(w*h);
      glPixelStorei(GL_PACK_ALIGNMENT, 1);

      glPushAttrib(GL_PIXEL_MODE_BIT);
      glReadBuffer(GL_FRONT);
      glReadPixels(x,y,w,h,GL_COLOR_INDEX, GL_UNSIGNED_BYTE, &pixels[0]);
      glPopAttrib();

      long int sum = 0;
      for (unsigned int i = 0; i < pixels.size(); ++i) {
        sum += pixels[i];
      }
      Tcl_SetObjResult(interp, Tcl_NewIntObj(sum));
    }

  return TCL_OK;
}

//---------------------------------------------------------------------
//
// GLPkg --
//
//---------------------------------------------------------------------

class GLTcl::GLPkg : public Tcl::TclPkg {
public:
  GLPkg(Tcl_Interp* interp) :
    Tcl::TclPkg(interp, "Tclgl", "$Revision$")
  {
    addCommand( new glBeginCmd        (this, "glBegin") );
    addCommand( new glBlendFuncCmd    (this, "glBlendFunc") );
    addCommand( new glCallListCmd     (this, "glCallList") );
    addCommand( new glClearCmd        (this, "glClear") );
    addCommand( new glClearColorCmd   (this, "glClearColor") );
    addCommand( new glClearIndexCmd   (this, "glClearIndex") );
    addCommand( new glColorCmd        (this, "glColor") );
    addCommand( new glDeleteListsCmd  (this, "glDeleteLists") );
    addCommand( new glDrawBufferCmd   (this, "glDrawBuffer") );
    addCommand( new glEnableCmd       (this, "glDisable") );
    addCommand( new glEnableCmd       (this, "glEnable") );
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
// GLTcl::Tclgl_Init --
//
//--------------------------------------------------------------------

extern "C"
int Gltcl_Init(Tcl_Interp* interp) {
DOTRACE("Gltcl_Init");

  new GLTcl::GLPkg(interp);

  using namespace GLTcl;

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
  for (size_t i = 0; i < sizeof(Names_Procs)/sizeof(CmdName_CmdProc); ++i) {
    Tcl_CreateObjCommand(interp,
                         const_cast<char *>(Names_Procs[i].cmdName),
                         Names_Procs[i].cmdProc,
                         (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  }

#ifdef ACC_COMPILER
//   typeid(out_of_range);
//   typeid(length_error);
#endif

  return TCL_OK;
}

static const char vcid_gltcl_cc[] = "$Header$";
#endif // !GLTCL_CC_DEFINED
