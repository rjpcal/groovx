///////////////////////////////////////////////////////////////////////
//
// gltcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Nov-98
// written: Fri Jun 22 09:28:44 2001
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

#include "tcl/functor.h"
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
#define LOCAL_ASSERT
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

  class glFrustumCmd;
  class glGetCmd;
  class glLoadMatrixCmd;
  class glOrthoCmd;
  class gluLookAtCmd;

  class AntialiasCmd;
  class DrawOneLineCmd;
  class DrawThickLineCmd;
  class LineInfoCmd;
  class PixelCheckSumCmd;
}

///////////////////////////////////////////////////////////////////////
//
// GLTcl Tcl package definitions
//
///////////////////////////////////////////////////////////////////////

namespace GLTcl {
  void checkGL();
}

void GLTcl::checkGL() {
  GLenum status = glGetError();
  if (status != GL_NO_ERROR) {
    const char* msg = reinterpret_cast<const char*>(gluErrorString(status));
    throw GLError(msg);
  }
}

//---------------------------------------------------------------------
//
// GLTcl::glFrustumCmd --
//
//---------------------------------------------------------------------

class GLTcl::glFrustumCmd : public Tcl::TclCmd {
public:
  glFrustumCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    Tcl::TclCmd(pkg->interp(), cmd_name, "left right bottom top zNear zFar", 7, 7) {}
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
// GLTcl::glGetCmd --
//
//---------------------------------------------------------------------

class GLTcl::glGetCmd : public Tcl::TclCmd {
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
    Tcl::TclCmd(pkg->interp(), cmd_name, "param_name", 2, 2) { initialize(pkg); }

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
// GLTcl::glLoadMatrixCmd --
//
//---------------------------------------------------------------------

class GLTcl::glLoadMatrixCmd : public Tcl::TclCmd {
public:
  glLoadMatrixCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    Tcl::TclCmd(pkg->interp(), cmd_name, "4x4_column_major_matrix", 2, 2) {}
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
// GLTcl::glOrthoCmd --
//
//---------------------------------------------------------------------

class GLTcl::glOrthoCmd : public Tcl::TclCmd {
public:
  glOrthoCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    Tcl::TclCmd(pkg->interp(), cmd_name, "left right bottom top zNear zFar", 7, 7) {}
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
// GLTcl::gluLookAtCmd --
//
//---------------------------------------------------------------------

class GLTcl::gluLookAtCmd : public Tcl::TclCmd {
public:
  gluLookAtCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    Tcl::TclCmd(pkg->interp(), cmd_name, "eyeX eyeY eyeZ targX targY targZ upX upY upZ", 10, 10)
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

//--------------------------------------------------------------------
//
// GLTcl::AntialiasCmd --
//
// Turns on or off the OpenGL modes required for antialiasing lines
// and polygons. Antialiasing works best with RGBA mode, and not so
// well with color index mode.
//
//--------------------------------------------------------------------

class GLTcl::AntialiasCmd : public Tcl::TclCmd {
public:
  AntialiasCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, "on_off", 2)
  {}

protected:
  virtual void invoke()
    {
      bool on_off = getBoolFromArg(1);

      if (on_off) {            // turn antialiasing on
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
    }
};

//--------------------------------------------------------------------
//
// GLTcl::DrawOneLineCmd --
//
// This command takes four arguments specifying the x and y
// coordinates of two points between which a line will be drawn.
//
//--------------------------------------------------------------------

class GLTcl::DrawOneLineCmd : public Tcl::TclCmd {
public:
  DrawOneLineCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, "x1 y1 x2 y2", 5)
  {}

protected:
  virtual void invoke()
    {
      fixed_block<double> coord(4);

      for (int i = 0; i < 3; ++i)
        coord.at(i) = getDoubleFromArg(i+1);

      glBegin(GL_LINES);
      glVertex3d( coord[0], coord[1], 0.0);
      glVertex3d( coord[2], coord[3], 0.0);
      glEnd();
      glFlush();
    }
};

//--------------------------------------------------------------------
//
// GLTcl::DrawThickLineCmd --
//
// This command takes five arguments specifying two points and a
// thickness. A thick "line" is drawn by drawing a filled rectangle.
//
//--------------------------------------------------------------------

class GLTcl::DrawThickLineCmd : public Tcl::TclCmd {
public:
  DrawThickLineCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, "x1 y1 x2 y2 thickness", 6)
  {}

protected:
  virtual void invoke()
    {
      fixed_block<double> coord(5); // fill with x1 y1 x2 y2 thickness

      for (int i = 0; i < 4; ++i)
        coord.at(i) = getDoubleFromArg(i+1);

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
    }
};

//--------------------------------------------------------------------
//
// GLTcl::LineInfoCmd --
//
// Returns a string describing the LINE_WIDTH_RANGE and
// LINE_WIDTH_GRANULARITY for the current OpenGL implementation.
//
//--------------------------------------------------------------------

class GLTcl::LineInfoCmd : public Tcl::TclCmd {
public:
  LineInfoCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, 0, 1)
  {}

protected:
  virtual void invoke()
    {
      GLdouble range[2] = {-1.0,-1.0};
      GLdouble gran=-1.0;
      glGetDoublev(GL_LINE_WIDTH_RANGE, &range[0]);
      glGetDoublev(GL_LINE_WIDTH_GRANULARITY, &gran);

      lappendVal("range"); lappendVal(range[0]), lappendVal(range[1]);
      lappendVal("granularity"); lappendVal(gran);
    }
};

//--------------------------------------------------------------------
//
// GLTcl::PixelCheckSumCmd --
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

class GLTcl::PixelCheckSumCmd : public Tcl::TclCmd {
public:
  PixelCheckSumCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, "x y width height", 1, 5, true)
  {}

protected:
  virtual void invoke()
    {
      int x,y,w,h;
      if (objc() == 5) {
        x = getIntFromArg(1);
        y = getIntFromArg(2);
        w = getIntFromArg(3);
        h = getIntFromArg(4);
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
          returnVal(sum);
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
          returnVal(sum);
        }
    }
};

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

    // for glBegin
    this->linkVarCopy("GL_POINTS",         GL_POINTS);
    this->linkVarCopy("GL_LINES",          GL_LINES);
    this->linkVarCopy("GL_LINE_STRIP",     GL_LINE_STRIP);
    this->linkVarCopy("GL_LINE_LOOP",      GL_LINE_LOOP);
    this->linkVarCopy("GL_TRIANGLES",      GL_TRIANGLES);
    this->linkVarCopy("GL_TRIANGLE_STRIP", GL_TRIANGLE_STRIP);
    this->linkVarCopy("GL_TRIANGLE_FAN",   GL_TRIANGLE_FAN);
    this->linkVarCopy("GL_QUADS",          GL_QUADS);
    this->linkVarCopy("GL_QUAD_STRIP",     GL_QUAD_STRIP);
    this->linkVarCopy("GL_POLYGON",        GL_POLYGON);

    // for glBlendFunc
    this->linkVarCopy("GL_ZERO", GL_ZERO);
    this->linkVarCopy("GL_ONE", GL_ONE);
    this->linkVarCopy("GL_DST_COLOR", GL_DST_COLOR);
    this->linkVarCopy("GL_ONE_MINUS_DST_COLOR", GL_ONE_MINUS_DST_COLOR);
    this->linkVarCopy("GL_SRC_COLOR", GL_SRC_COLOR);
    this->linkVarCopy("GL_ONE_MINUS_SRC_COLOR", GL_ONE_MINUS_SRC_COLOR);
    this->linkVarCopy("GL_SRC_ALPHA", GL_SRC_ALPHA);
    this->linkVarCopy("GL_ONE_MINUS_SRC_ALPHA", GL_ONE_MINUS_SRC_ALPHA);
    this->linkVarCopy("GL_DST_ALPHA", GL_DST_ALPHA);
    this->linkVarCopy("GL_ONE_MINUS_DST_ALPHA", GL_ONE_MINUS_DST_ALPHA);
    this->linkVarCopy("GL_SRC_ALPHA_SATURATE", GL_SRC_ALPHA_SATURATE);

    // for glClear
    this->linkVarCopy("GL_COLOR_BUFFER_BIT", GL_COLOR_BUFFER_BIT);
    this->linkVarCopy("GL_DEPTH_BUFFER_BIT", GL_DEPTH_BUFFER_BIT);
    this->linkVarCopy("GL_ACCUM_BUFFER_BIT", GL_ACCUM_BUFFER_BIT);
    this->linkVarCopy("GL_STENCIL_BUFFER_BIT", GL_STENCIL_BUFFER_BIT);

    // for glDrawBuffer
    this->linkVarCopy("GL_NONE", GL_NONE);
    this->linkVarCopy("GL_FRONT_LEFT", GL_FRONT_LEFT);
    this->linkVarCopy("GL_FRONT_RIGHT", GL_FRONT_RIGHT);
    this->linkVarCopy("GL_BACK_LEFT", GL_BACK_LEFT);
    this->linkVarCopy("GL_BACK_RIGHT", GL_BACK_RIGHT);
    this->linkVarCopy("GL_FRONT", GL_FRONT);
    this->linkVarCopy("GL_BACK", GL_BACK);
    this->linkVarCopy("GL_LEFT", GL_LEFT);
    this->linkVarCopy("GL_RIGHT", GL_RIGHT);
    this->linkVarCopy("GL_FRONT_AND_BACK", GL_FRONT_AND_BACK);

    // for glEnable/glDisable

    this->linkVarCopy("GL_ALPHA_TEST",  GL_ALPHA_TEST);
    this->linkVarCopy("GL_BLEND",  GL_BLEND);
    this->linkVarCopy("GL_FOG",  GL_FOG);
    this->linkVarCopy("GL_LIGHTING",  GL_LIGHTING);
//     this->linkVarCopy("GL_TEXTURE_1D",  GL_TEXTURE_1D);
//     this->linkVarCopy("GL_TEXTURE_2D",  GL_TEXTURE_2D);
//     this->linkVarCopy("GL_TEXTURE_3D_EXT",  GL_TEXTURE_3D_EXT);
    this->linkVarCopy("GL_LINE_STIPPLE",  GL_LINE_STIPPLE);
    this->linkVarCopy("GL_POLYGON_STIPPLE",  GL_POLYGON_STIPPLE);
    this->linkVarCopy("GL_CULL_FACE",  GL_CULL_FACE);
    this->linkVarCopy("GL_INDEX_LOGIC_OP",  GL_INDEX_LOGIC_OP);
    this->linkVarCopy("GL_COLOR_LOGIC_OP",  GL_COLOR_LOGIC_OP);
    this->linkVarCopy("GL_DITHER",  GL_DITHER);
    this->linkVarCopy("GL_STENCIL_TEST",  GL_STENCIL_TEST);
    this->linkVarCopy("GL_DEPTH_TEST",  GL_DEPTH_TEST);
//     this->linkVarCopy("GL_CLIP_PLANE0",  GL_CLIP_PLANE0);
//     this->linkVarCopy("GL_CLIP_PLANE1",  GL_CLIP_PLANE1);
//     this->linkVarCopy("GL_CLIP_PLANE2",  GL_CLIP_PLANE2);
//     this->linkVarCopy("GL_CLIP_PLANE3",  GL_CLIP_PLANE3);
//     this->linkVarCopy("GL_CLIP_PLANE4",  GL_CLIP_PLANE4);
//     this->linkVarCopy("GL_CLIP_PLANE5",  GL_CLIP_PLANE5);
//     this->linkVarCopy("GL_LIGHT0",  GL_LIGHT0);
//     this->linkVarCopy("GL_LIGHT1",  GL_LIGHT1);
//     this->linkVarCopy("GL_LIGHT2",  GL_LIGHT2);
//     this->linkVarCopy("GL_LIGHT3",  GL_LIGHT3);
//     this->linkVarCopy("GL_LIGHT4",  GL_LIGHT4);
//     this->linkVarCopy("GL_LIGHT5",  GL_LIGHT5);
//     this->linkVarCopy("GL_LIGHT6",  GL_LIGHT6);
//     this->linkVarCopy("GL_LIGHT7",  GL_LIGHT7);
//     this->linkVarCopy("GL_TEXTURE_GEN_S",  GL_TEXTURE_GEN_S);
//     this->linkVarCopy("GL_TEXTURE_GEN_T",  GL_TEXTURE_GEN_T);
//     this->linkVarCopy("GL_TEXTURE_GEN_R",  GL_TEXTURE_GEN_R);
//     this->linkVarCopy("GL_TEXTURE_GEN_Q",  GL_TEXTURE_GEN_Q);
    this->linkVarCopy("GL_MAP1_VERTEX_3",  GL_MAP1_VERTEX_3);
    this->linkVarCopy("GL_MAP1_VERTEX_4",  GL_MAP1_VERTEX_4);
    this->linkVarCopy("GL_MAP1_COLOR_4",  GL_MAP1_COLOR_4);
    this->linkVarCopy("GL_MAP1_INDEX",  GL_MAP1_INDEX);
    this->linkVarCopy("GL_MAP1_NORMAL",  GL_MAP1_NORMAL);
//     this->linkVarCopy("GL_MAP1_TEXTURE_COORD_1",  GL_MAP1_TEXTURE_COORD_1);
//     this->linkVarCopy("GL_MAP1_TEXTURE_COORD_2",  GL_MAP1_TEXTURE_COORD_2);
//     this->linkVarCopy("GL_MAP1_TEXTURE_COORD_3",  GL_MAP1_TEXTURE_COORD_3);
//     this->linkVarCopy("GL_MAP1_TEXTURE_COORD_4",  GL_MAP1_TEXTURE_COORD_4);
//     this->linkVarCopy("GL_MAP2_VERTEX_3",  GL_MAP2_VERTEX_3);
//     this->linkVarCopy("GL_MAP2_VERTEX_4",  GL_MAP2_VERTEX_4);
//     this->linkVarCopy("GL_MAP2_COLOR_4",  GL_MAP2_COLOR_4);
//     this->linkVarCopy("GL_MAP2_INDEX",  GL_MAP2_INDEX);
//     this->linkVarCopy("GL_MAP2_NORMAL",  GL_MAP2_NORMAL);
//     this->linkVarCopy("GL_MAP2_TEXTURE_COORD_1",  GL_MAP2_TEXTURE_COORD_1);
//     this->linkVarCopy("GL_MAP2_TEXTURE_COORD_2",  GL_MAP2_TEXTURE_COORD_2);
//     this->linkVarCopy("GL_MAP2_TEXTURE_COORD_3",  GL_MAP2_TEXTURE_COORD_3);
//     this->linkVarCopy("GL_MAP2_TEXTURE_COORD_4",  GL_MAP2_TEXTURE_COORD_4);
    this->linkVarCopy("GL_POINT_SMOOTH",  GL_POINT_SMOOTH);
    this->linkVarCopy("GL_LINE_SMOOTH",  GL_LINE_SMOOTH);
    this->linkVarCopy("GL_POLYGON_SMOOTH",  GL_POLYGON_SMOOTH);
    this->linkVarCopy("GL_SCISSOR_TEST",  GL_SCISSOR_TEST);
    this->linkVarCopy("GL_COLOR_MATERIAL",  GL_COLOR_MATERIAL);
    this->linkVarCopy("GL_NORMALIZE",  GL_NORMALIZE);
    this->linkVarCopy("GL_AUTO_NORMAL",  GL_AUTO_NORMAL);
    this->linkVarCopy("GL_VERTEX_ARRAY",  GL_VERTEX_ARRAY);
    this->linkVarCopy("GL_NORMAL_ARRAY",  GL_NORMAL_ARRAY);
    this->linkVarCopy("GL_COLOR_ARRAY",  GL_COLOR_ARRAY);
    this->linkVarCopy("GL_INDEX_ARRAY",  GL_INDEX_ARRAY);
    this->linkVarCopy("GL_TEXTURE_COORD_ARRAY",  GL_TEXTURE_COORD_ARRAY);
    this->linkVarCopy("GL_EDGE_FLAG_ARRAY",  GL_EDGE_FLAG_ARRAY);
    this->linkVarCopy("GL_POLYGON_OFFSET_POINT",  GL_POLYGON_OFFSET_POINT);
    this->linkVarCopy("GL_POLYGON_OFFSET_LINE",  GL_POLYGON_OFFSET_LINE);
    this->linkVarCopy("GL_POLYGON_OFFSET_FILL",  GL_POLYGON_OFFSET_FILL);
    this->linkVarCopy("GL_RESCALE_NORMAL_EXT",  GL_RESCALE_NORMAL_EXT);

    // for glMatrixMode
    this->linkVarCopy("GL_MODELVIEW",  GL_MODELVIEW);
    this->linkVarCopy("GL_PROJECTION", GL_PROJECTION);
    this->linkVarCopy("GL_TEXTURE",    GL_TEXTURE);

    // for glNewList
    this->linkVarCopy("GL_COMPILE", GL_COMPILE);
    this->linkVarCopy("GL_COMPILE_AND_EXECUTE", GL_COMPILE_AND_EXECUTE);

    // for glPolygonMode
    this->linkVarCopy("GL_POINT", GL_POINT);
    this->linkVarCopy("GL_LINE", GL_LINE);
    this->linkVarCopy("GL_FILL", GL_FILL);

    using namespace Tcl;

    addCommand( makeCmd (interp, glBegin, "glBegin", "mode") );
    addCommand( makeCmd (interp, glBlendFunc, "glBlendFunc", "sfactor dfactor") );
    addCommand( makeCmd (interp, glCallList, "glCallList", "list") );
    addCommand( makeCmd (interp, glClear, "glClear", "mask_bits") );
    addCommand( makeCmd (interp, glClearColor, "glClearColor", "red green blue alpha") );
    addCommand( makeCmd (interp, glClearIndex, "glClearIndex", "index") );
    addCommand( makeCmd (interp, glColor4d, "glColor", "red green blue alpha") );
    addCommand( makeCmd (interp, glDeleteLists, "glDeleteLists", "list_id range") );
    addCommand( makeCmd (interp, glDisable, "glDisable", "capability") );
    addCommand( makeCmd (interp, glDrawBuffer, "glDrawBuffer", "mode") );
    addCommand( makeCmd (interp, glEnable, "glEnable", "capability") );
    addCommand( makeCmd (interp, glEnd, "glEnd", 0) );
    addCommand( makeCmd (interp, glEndList, "glEndList", 0) );
    addCommand( makeCmd (interp, glFlush, "glFlush", 0) );
    addCommand( makeCmd (interp, glGenLists, "glGenLists", "range") );
    addCommand( makeCmd (interp, checkGL, "glGetError", 0) );
    addCommand( makeCmd (interp, glIndexi, "glIndexi", "index") );
    addCommand( makeCmd (interp, glIsList, "glIsList", "list_id") );
    addCommand( makeCmd (interp, glLineWidth, "glLineWidth", "width") );
    addCommand( makeCmd (interp, glListBase, "glListBase", "base") );
    addCommand( makeCmd (interp, glLoadIdentity, "glLoadIdentity", 0) );
    addCommand( makeCmd (interp, glMatrixMode, "glMatrixMode", "mode") );
    addCommand( makeCmd (interp, glNewList, "glNewList", "list_id mode") );
    addCommand( makeCmd (interp, glPolygonMode, "glPolygonMode", "face mode") );
    addCommand( makeCmd (interp, glPopMatrix, "glPopMatrix", 0) );
    addCommand( makeCmd (interp, glPushMatrix, "glPushMatrix", 0) );
    addCommand( makeCmd (interp, glRotated, "glRotate", "angle_in_degrees x y z") );
    addCommand( makeCmd (interp, glScaled, "glScale", "x y z") );
    addCommand( makeCmd (interp, glTranslated, "glTranslate", "x y z") );
    addCommand( makeCmd (interp, glVertex2d, "glVertex2", "x y") );
    addCommand( makeCmd (interp, glVertex3d, "glVertex3", "x y z") );
    addCommand( makeCmd (interp, glVertex4d, "glVertex4", "x y z w") );
    addCommand( makeCmd (interp, gluPerspective, "gluPerspective",
                         "field_of_view_y aspect zNear zFar") );

    addCommand( new glFrustumCmd      (this, "glFrustum") );
    addCommand( new glGetTypeCmd<GLboolean>(this, "glGetBoolean") );
    addCommand( new glGetTypeCmd<GLdouble>(this, "glGetDouble") );
    addCommand( new glGetTypeCmd<GLint>(this, "glGetInteger") );
    addCommand( new glLoadMatrixCmd   (this, "glLoadMatrix") );
    addCommand( new glOrthoCmd        (this, "glOrtho") );
    addCommand( new gluLookAtCmd      (this, "gluLookAt") );

    addCommand( new AntialiasCmd      (interp, "antialias") );
    addCommand( new DrawOneLineCmd    (interp, "drawOneLine") );
    addCommand( new DrawThickLineCmd  (interp, "drawThickLine") );
    addCommand( new LineInfoCmd       (interp, "lineInfo") );
    addCommand( new PixelCheckSumCmd  (interp, "pixelCheckSum") );
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

#ifdef ACC_COMPILER
//   typeid(out_of_range);
//   typeid(length_error);
#endif

  return TCL_OK;
}

static const char vcid_gltcl_cc[] = "$Header$";
#endif // !GLTCL_CC_DEFINED
