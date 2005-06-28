///////////////////////////////////////////////////////////////////////
//
// gltcl.cc
//
// Copyright (c) 1998-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Nov  2 08:00:00 1998
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_VISX_GLTCL_CC_UTC20050626084016_DEFINED
#define GROOVX_VISX_GLTCL_CC_UTC20050626084016_DEFINED

#include "visx/gltcl.h"

// This Tcl package provides some simple Tcl wrappers for C OpenGL
// functions. The function names, argument lists, and symbolic
// constants for the Tcl functions are identical to those in the
// analagous C functions.

#include "geom/rect.h"

#include "gfx/canvas.h"
#include "gfx/glcanvas.h"

#include "tcl/list.h"
#include "tcl/pkg.h"

#include "rutz/arrays.h"
#include "rutz/error.h"

#include <cmath>                // for sqrt() in drawThickLine
#include <map>

#if defined(GVX_GL_PLATFORM_GLX)
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glx.h>
#elif defined(GVX_GL_PLATFORM_AGL)
#  include <AGL/gl.h>
#  include <AGL/glu.h>
#endif

#include "rutz/trace.h"

///////////////////////////////////////////////////////////////////////
//
// TclGL Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

namespace GLTcl
{
  void loadMatrix(Nub::SoftRef<GLCanvas> canvas, Tcl::List entries);
  void lookAt(Tcl::List args);
  void antialias(bool on_off);
  void drawOneLine(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
  void drawThickLine(GLdouble x1, GLdouble y1,
                     GLdouble x2, GLdouble y2, GLdouble thickness);
  Tcl::List lineInfo();

  // Just converts to char from unsigned char
  const char* getString(GLenum name)
  {
    return reinterpret_cast<const char*>(glGetString(name));
  }

#define NAMEVAL(x) #x, x

  // The point of this struct and the associated std::map is so that
  // we can figure out how many values we should try to retrieve from
  // an OpenGL get() call, for a given parameter which is specified by
  // a GLenum (e.g. we should try to get 4 values for the GL_VIEWPORT
  // param)
  struct AttribInfo
  {
    const char* param_name;
    GLenum param_tag;
    int num_values;
  };

  static std::map<GLenum, const AttribInfo*> theAttribMap;

  void loadGet(Tcl::Pkg* pkg)
  {
    static const AttribInfo theAttribs[] =
    {
      { NAMEVAL(GL_ACCUM_CLEAR_VALUE), 4 },
      { NAMEVAL(GL_ACCUM_ALPHA_BITS), 1 },
      { NAMEVAL(GL_ACCUM_BLUE_BITS), 1 },
      { NAMEVAL(GL_ACCUM_GREEN_BITS), 1 },
      { NAMEVAL(GL_ACCUM_RED_BITS), 1 },
      { NAMEVAL(GL_ALPHA_BITS), 1 },
      { NAMEVAL(GL_BLUE_BITS), 1 },
      { NAMEVAL(GL_COLOR_CLEAR_VALUE), 4 },
      { NAMEVAL(GL_CURRENT_COLOR), 4 },
      { NAMEVAL(GL_CURRENT_INDEX), 1 },
      { NAMEVAL(GL_DEPTH_BITS), 1 },
      { NAMEVAL(GL_DEPTH_CLEAR_VALUE), 1 },
      { NAMEVAL(GL_INDEX_BITS), 1 },
      { NAMEVAL(GL_INDEX_CLEAR_VALUE), 1 },
      { NAMEVAL(GL_GREEN_BITS), 1 },
      { NAMEVAL(GL_LINE_WIDTH), 1 },
      { NAMEVAL(GL_LINE_WIDTH_GRANULARITY), 1 },
      { NAMEVAL(GL_LINE_WIDTH_RANGE), 2 },
      { NAMEVAL(GL_LIST_BASE), 1 },
      { NAMEVAL(GL_LIST_INDEX), 1 },
      { NAMEVAL(GL_MATRIX_MODE), 1 },
      { NAMEVAL(GL_MODELVIEW_MATRIX), 16 },
      { NAMEVAL(GL_POLYGON_MODE), 2 },
      { NAMEVAL(GL_PROJECTION_MATRIX), 16 },
      { NAMEVAL(GL_RED_BITS), 1 },
      { NAMEVAL(GL_RGBA_MODE), 1 },
      { NAMEVAL(GL_SUBPIXEL_BITS), 1 },
      { NAMEVAL(GL_STENCIL_BITS), 1 },
      { NAMEVAL(GL_STENCIL_CLEAR_VALUE), 1 },
      { NAMEVAL(GL_VIEWPORT), 4 },
    };

    int num_params = sizeof(theAttribs) / sizeof(AttribInfo);
    for (int i = 0; i < num_params; ++i)
      {
        pkg->linkVarCopy(theAttribs[i].param_name,
                         static_cast<int>(theAttribs[i].param_tag));
        theAttribMap[theAttribs[i].param_tag] = &(theAttribs[i]);
      }
  }

  struct NameVal
  {
    const char* name;
    int val;
  };

  void loadEnums(Tcl::Pkg* pkg)
  {
    NameVal theEnums [] =
    {
      // for glBegin
      { NAMEVAL(GL_POINTS) },
      { NAMEVAL(GL_LINES) },
      { NAMEVAL(GL_LINE_STRIP) },
      { NAMEVAL(GL_LINE_LOOP) },
      { NAMEVAL(GL_TRIANGLES) },
      { NAMEVAL(GL_TRIANGLE_STRIP) },
      { NAMEVAL(GL_TRIANGLE_FAN) },
      { NAMEVAL(GL_QUADS) },
      { NAMEVAL(GL_QUAD_STRIP) },
      { NAMEVAL(GL_POLYGON) },

      // for glBlendFunc
      { NAMEVAL(GL_ZERO) },
      { NAMEVAL(GL_ONE) },
      { NAMEVAL(GL_DST_COLOR) },
      { NAMEVAL(GL_ONE_MINUS_DST_COLOR) },
      { NAMEVAL(GL_SRC_COLOR) },
      { NAMEVAL(GL_ONE_MINUS_SRC_COLOR) },
      { NAMEVAL(GL_SRC_ALPHA) },
      { NAMEVAL(GL_ONE_MINUS_SRC_ALPHA) },
      { NAMEVAL(GL_DST_ALPHA) },
      { NAMEVAL(GL_ONE_MINUS_DST_ALPHA) },
      { NAMEVAL(GL_SRC_ALPHA_SATURATE) },

      // for glClear
      { NAMEVAL(GL_COLOR_BUFFER_BIT) },
      { NAMEVAL(GL_DEPTH_BUFFER_BIT) },
      { NAMEVAL(GL_ACCUM_BUFFER_BIT) },
      { NAMEVAL(GL_STENCIL_BUFFER_BIT) },

      // for glDrawBuffer
      { NAMEVAL(GL_NONE) },
      { NAMEVAL(GL_FRONT_LEFT) },
      { NAMEVAL(GL_FRONT_RIGHT) },
      { NAMEVAL(GL_BACK_LEFT) },
      { NAMEVAL(GL_BACK_RIGHT) },
      { NAMEVAL(GL_FRONT) },
      { NAMEVAL(GL_BACK) },
      { NAMEVAL(GL_LEFT) },
      { NAMEVAL(GL_RIGHT) },
      { NAMEVAL(GL_FRONT_AND_BACK) },

      // for glEnable/glDisable
      { NAMEVAL(GL_ALPHA_TEST) },
      { NAMEVAL(GL_BLEND) },
      { NAMEVAL(GL_FOG) },
      { NAMEVAL(GL_LIGHTING) },
      { NAMEVAL(GL_LINE_STIPPLE) },
      { NAMEVAL(GL_POLYGON_STIPPLE) },
      { NAMEVAL(GL_CULL_FACE) },
      { NAMEVAL(GL_INDEX_LOGIC_OP) },
      { NAMEVAL(GL_COLOR_LOGIC_OP) },
      { NAMEVAL(GL_DITHER) },
      { NAMEVAL(GL_STENCIL_TEST) },
      { NAMEVAL(GL_DEPTH_TEST) },
      { NAMEVAL(GL_MAP1_VERTEX_3) },
      { NAMEVAL(GL_MAP1_VERTEX_4) },
      { NAMEVAL(GL_MAP1_COLOR_4) },
      { NAMEVAL(GL_MAP1_INDEX) },
      { NAMEVAL(GL_MAP1_NORMAL) },
      { NAMEVAL(GL_POINT_SMOOTH) },
      { NAMEVAL(GL_LINE_SMOOTH) },
      { NAMEVAL(GL_POLYGON_SMOOTH) },
      { NAMEVAL(GL_SCISSOR_TEST) },
      { NAMEVAL(GL_COLOR_MATERIAL) },
      { NAMEVAL(GL_NORMALIZE) },
      { NAMEVAL(GL_AUTO_NORMAL) },
      { NAMEVAL(GL_VERTEX_ARRAY) },
      { NAMEVAL(GL_NORMAL_ARRAY) },
      { NAMEVAL(GL_COLOR_ARRAY) },
      { NAMEVAL(GL_INDEX_ARRAY) },
      { NAMEVAL(GL_TEXTURE_COORD_ARRAY) },
      { NAMEVAL(GL_EDGE_FLAG_ARRAY) },
      { NAMEVAL(GL_POLYGON_OFFSET_POINT) },
      { NAMEVAL(GL_POLYGON_OFFSET_LINE) },
      { NAMEVAL(GL_POLYGON_OFFSET_FILL) },
#ifdef GVX_GL_PLATFORM_GLX
      { NAMEVAL(GL_RESCALE_NORMAL_EXT) },
#endif

      // for glMatrixMode
      { NAMEVAL(GL_MODELVIEW) },
      { NAMEVAL(GL_PROJECTION) },
      { NAMEVAL(GL_TEXTURE) },

      // for glNewList
      { NAMEVAL(GL_COMPILE) },
      { NAMEVAL(GL_COMPILE_AND_EXECUTE) },

      // for glPolygonMode
      { NAMEVAL(GL_POINT) },
      { NAMEVAL(GL_LINE) },
      { NAMEVAL(GL_FILL) },
    };

    int num_enums = sizeof(theEnums) / sizeof(NameVal);
    for (int i = 0; i < num_enums; ++i)
      {
        pkg->linkVarCopy(theEnums[i].name, theEnums[i].val);
      }
  }

#undef NAMEVAL

  void extractValues(GLenum tag, GLboolean* vals_out)
  { glGetBooleanv(tag, vals_out); }

  void extractValues(GLenum tag, GLdouble* vals_out)
  { glGetDoublev(tag, vals_out); }

  void extractValues(GLenum tag, GLint* vals_out)
  { glGetIntegerv(tag, vals_out); }

  template <class T>
  Tcl::List get(GLenum param_tag
#ifdef GVX_BROKEN_TEMPLATE_FUNCTIONS
                , T* /*dummy*/=0
#endif
                )
  {
    const AttribInfo* theInfo = theAttribMap[param_tag];
    if ( theInfo == 0 )
      {
        throw rutz::error("invalid or unsupported enumerant", SRC_POS);
      }

    rutz::fixed_block<T> theVals(theInfo->num_values);
    extractValues(theInfo->param_tag, &(theVals[0]));
    Tcl::List result;
    result.appendRange(theVals.begin(), theVals.end());
    return result;
  }

#ifdef GVX_BROKEN_TEMPLATE_FUNCTIONS
  Tcl::List getBoolean(GLenum param_tag)
  { return get<GLboolean>(param_tag, (GLboolean*) 0); }

  Tcl::List getDouble(GLenum param_tag)
  { return get<GLdouble>(param_tag, (GLdouble*) 0); }

  Tcl::List getInt(GLenum param_tag)
  { return get<GLint>(param_tag, (GLint*) 0); }
#endif
}

//---------------------------------------------------------------------
//
// GLTcl::loadMatrix --
//
//---------------------------------------------------------------------

void GLTcl::loadMatrix(Nub::SoftRef<GLCanvas> canvas, Tcl::List entries)
{
  rutz::fixed_block<GLdouble> matrix(entries.begin<GLdouble>(),
                                     entries.end<GLdouble>());

  if (matrix.size() != 16)
    {
      throw rutz::error("matrix must have 16 entries "
                        "in column-major order", SRC_POS);
    }

  glLoadMatrixd(&matrix[0]);

  canvas->throwIfError("loadMatrix", SRC_POS);
}

//---------------------------------------------------------------------
//
// GLTcl::lookAt --
//
//---------------------------------------------------------------------

void GLTcl::lookAt(Tcl::List args)
{
  gluLookAt(args.get<GLdouble>(0),
            args.get<GLdouble>(1),
            args.get<GLdouble>(2),
            args.get<GLdouble>(3),
            args.get<GLdouble>(4),
            args.get<GLdouble>(5),
            args.get<GLdouble>(6),
            args.get<GLdouble>(7),
            args.get<GLdouble>(8));
}

//--------------------------------------------------------------------
//
// GLTcl::antialias --
//
// Turns on or off the OpenGL modes required for antialiasing lines
// and polygons. Antialiasing works best with RGBA mode, and not so
// well with color index mode.
//
//--------------------------------------------------------------------

void GLTcl::antialias(bool on_off)
{
GVX_TRACE("GLTcl::antialias");
  if (on_off) // turn antialiasing on
    {
      glEnable(GL_BLEND);
      glEnable(GL_POLYGON_SMOOTH);
      glEnable(GL_LINE_SMOOTH);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
  else // turn antialiasing off
    {
      glDisable(GL_BLEND);
      glDisable(GL_LINE_SMOOTH);
      glDisable(GL_POLYGON_SMOOTH);
    }
}

//--------------------------------------------------------------------
//
// GLTcl::drawOneLine --
//
// This command takes four arguments specifying the x and y
// coordinates of two points between which a line will be drawn.
//
//--------------------------------------------------------------------

void GLTcl::drawOneLine(GLdouble x1, GLdouble y1,
                        GLdouble x2, GLdouble y2)
{
  glBegin(GL_LINES);
  glVertex3d(x1, y1, 0.0);
  glVertex3d(x2, y2, 0.0);
  glEnd();
  glFlush();
}

//--------------------------------------------------------------------
//
// GLTcl::drawThickLine --
//
// This command takes five arguments specifying two points and a
// thickness. A thick "line" is drawn by drawing a filled rectangle.
//
//--------------------------------------------------------------------

void GLTcl::drawThickLine(GLdouble x1, GLdouble y1,
                          GLdouble x2, GLdouble y2, GLdouble thickness)
{
  // construct the normal vector
  double a, b, c, d, norm;
  a = x2 - x1;      // (a, b) is the given vector
  b = y2 - y1;

  norm = sqrt(a*a + b*b);       // (c, d) is the normal
  c = -b/norm*thickness/2;
  d = a/norm*thickness/2;

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBegin(GL_POLYGON);
  glVertex3d( x1+c, y1+d, 0.0);
  glVertex3d( x1-c, y1-d, 0.0);

  glVertex3d( x2-c, y2-d, 0.0);
  glVertex3d( x2+c, y2+d, 0.0);
  glEnd();
  glFlush();
}

//--------------------------------------------------------------------
//
// GLTcl::lineInfo --
//
// Returns a string describing the LINE_WIDTH_RANGE and
// LINE_WIDTH_GRANULARITY for the current OpenGL implementation.
//
//--------------------------------------------------------------------

Tcl::List GLTcl::lineInfo()
{
  GLdouble range[2] = {-1.0,-1.0};
  GLdouble gran=-1.0;
  glGetDoublev(GL_LINE_WIDTH_RANGE, &range[0]);
  glGetDoublev(GL_LINE_WIDTH_GRANULARITY, &gran);

  Tcl::List result;
  result.append("range");
  result.append(range[0]);
  result.append(range[1]);
  result.append("granularity");
  result.append(gran);
  return result;
}

//--------------------------------------------------------------------
//
// Gltcl_Init --
//
//--------------------------------------------------------------------

extern "C"
int Gl_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gl_Init");

  GVX_PKG_CREATE(pkg, interp, "GL", "4.$Revision$");
  GLTcl::loadGet(pkg);
  GLTcl::loadEnums(pkg);

  using rutz::bind_first;

  pkg->def( "::glBegin", "mode", glBegin, SRC_POS );
  pkg->def( "::glBlendFunc", "sfactor dfactor", glBlendFunc, SRC_POS );
  pkg->def( "::glCallList", "list", glCallList, SRC_POS );
  pkg->def( "::glClear", "mask_bits", glClear, SRC_POS );
  pkg->def( "::glClearColor", "red green blue alpha", glClearColor, SRC_POS );
  pkg->def( "::glClearIndex", "index", glClearIndex, SRC_POS );
  pkg->def( "::glColor", "red green blue", glColor3d, SRC_POS );
  pkg->def( "::glColor", "red green blue alpha", glColor4d, SRC_POS );
  pkg->def( "::glDeleteLists", "list_id range", glDeleteLists, SRC_POS );
  pkg->def( "::glDisable", "capability", glDisable, SRC_POS );
  pkg->def( "::glDrawBuffer", "mode", glDrawBuffer, SRC_POS );
  pkg->def( "::glEnable", "capability", glEnable, SRC_POS );
  pkg->def( "::glEnd", 0, glEnd, SRC_POS );
  pkg->def( "::glEndList", 0, glEndList, SRC_POS );
  pkg->def( "::glExtensions", 0, bind_first(GLTcl::getString, GL_EXTENSIONS), SRC_POS);
  pkg->def( "::glFinish", 0, glFinish, SRC_POS );
  pkg->def( "::glFlush", 0, glFlush, SRC_POS );
  pkg->def( "::glFrustum", "left right bottom top zNear zFar", glFrustum, SRC_POS );
  pkg->def( "::glGenLists", "range", glGenLists, SRC_POS );
  pkg->def( "::glIndexi", "index", glIndexi, SRC_POS );
  pkg->def( "::glIsList", "list_id", glIsList, SRC_POS );
  pkg->def( "::glLineWidth", "width", glLineWidth, SRC_POS );
  pkg->def( "::glListBase", "base", glListBase, SRC_POS );
  pkg->def( "::glLoadIdentity", 0, glLoadIdentity, SRC_POS );
  pkg->def( "::glLoadMatrix", "glcanvas 4x4_column_major_matrix", GLTcl::loadMatrix, SRC_POS );
  pkg->def( "::glMatrixMode", "mode", glMatrixMode, SRC_POS );
  pkg->def( "::glNewList", "list_id mode", glNewList, SRC_POS );
  pkg->def( "::glOrtho", "left right bottom top zNear zFar", glOrtho, SRC_POS );
  pkg->def( "::glPolygonMode", "face mode", glPolygonMode, SRC_POS );
  pkg->def( "::glPointSize", "size", glPointSize, SRC_POS );
  pkg->def( "::glPopMatrix", 0, glPopMatrix, SRC_POS );
  pkg->def( "::glPushMatrix", 0, glPushMatrix, SRC_POS );
  pkg->def( "::glRasterPos2d", "x y", glRasterPos2d, SRC_POS );
  pkg->def( "::glRenderer", 0, bind_first(GLTcl::getString, GL_RENDERER), SRC_POS);
  pkg->def( "::glRotate", "angle_in_degrees x y z", glRotated, SRC_POS );
  pkg->def( "::glScale", "x y z", glScaled, SRC_POS );
  pkg->def( "::glTranslate", "x y z", glTranslated, SRC_POS );
  pkg->def( "::glVendor", 0, bind_first(GLTcl::getString, GL_VENDOR), SRC_POS);
  pkg->def( "::glVersion", 0, bind_first(GLTcl::getString, GL_VERSION), SRC_POS);
  pkg->def( "::glVertex2", "x y", glVertex2d, SRC_POS );
  pkg->def( "::glVertex3", "x y z", glVertex3d, SRC_POS );
  pkg->def( "::glVertex4", "x y z w", glVertex4d, SRC_POS );
#if defined(GVX_GL_PLATFORM_GLX)
  pkg->def( "::glXWaitX", 0, glXWaitX, SRC_POS );
  pkg->def( "::glXWaitGL", 0, glXWaitGL, SRC_POS );
#endif
  pkg->def( "::gluLookAt", "eyeX eyeY eyeZ targX targY targZ upX upY upZ",
            GLTcl::lookAt, SRC_POS );
  pkg->def( "::gluPerspective", "field_of_view_y aspect zNear zFar",
            gluPerspective, SRC_POS );

#ifndef GVX_BROKEN_TEMPLATE_FUNCTIONS
  pkg->def( "::glGetBoolean", "param_name", GLTcl::get<GLboolean>, SRC_POS );
  pkg->def( "::glGetDouble", "param_name", GLTcl::get<GLdouble>, SRC_POS );
  pkg->def( "::glGetInteger", "param_name", GLTcl::get<GLint>, SRC_POS );
#else
  pkg->def( "::glGetBoolean", "param_name", GLTcl::getBoolean, SRC_POS );
  pkg->def( "::glGetDouble", "param_name", GLTcl::getDouble, SRC_POS );
  pkg->def( "::glGetInteger", "param_name", GLTcl::getInt, SRC_POS );
#endif

  pkg->def( "::antialias", "on_off", GLTcl::antialias, SRC_POS );
  pkg->def( "::drawOneLine", "x1 y1 x2 y2", GLTcl::drawOneLine, SRC_POS );
  pkg->def( "::drawThickLine", "x1 y1 x2 y2 thickness", GLTcl::drawThickLine, SRC_POS );
  pkg->def( "::lineInfo", 0, GLTcl::lineInfo, SRC_POS );

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_visx_gltcl_cc_utc20050626084016[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_GLTCL_CC_UTC20050626084016_DEFINED
