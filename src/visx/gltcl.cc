///////////////////////////////////////////////////////////////////////
//
// gltcl.cc
//
// Copyright (c) 1998-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Nov  2 08:00:00 1998
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GLTCL_CC_DEFINED
#define GLTCL_CC_DEFINED

// This Tcl package provides some simple Tcl wrappers for C OpenGL
// functions. The function names, argument lists, and symbolic constants
// for the Tcl functions are identical to those in the analagous C
// functions.

#include "gfx/canvas.h"

#include "gx/bmapdata.h"
#include "gx/rect.h"

#include "tcl/tclerror.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "util/arrays.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>                // for sqrt() in drawThickLine
#include <map>

#include "util/trace.h"

///////////////////////////////////////////////////////////////////////
//
// TclGL Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

namespace GLTcl
{
  void loadMatrix(Tcl::List entries);
  void lookAt(Tcl::List args);
  void antialias(bool on_off);
  void drawOneLine(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
  void drawThickLine(GLdouble x1, GLdouble y1,
                     GLdouble x2, GLdouble y2, GLdouble thickness);
  Tcl::List lineInfo();
  long int pixelCheckSum(int x, int y, int w, int h);
  long int pixelCheckSumAll()
  {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    return pixelCheckSum(viewport[0], viewport[1], viewport[2], viewport[3]);
  }

  void checkGL() { Gfx::Canvas::current().throwIfError("checkGL"); }

  // Just converts to char from unsigned char
  const char* getString(GLenum name)
  {
    return (const char*) glGetString(name);
  }

#define NAMEVAL(x) #x, x

  // The point of this struct and the associated std::map is so that we can
  // figure out how many values we should try to retrieve from an OpenGL get()
  // call, for a given parameter which is specified by a GLenum (e.g. we
  // should try to get 4 values for the GL_VIEWPORT param)
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
        pkg->linkVarCopy(theAttribs[i].param_name, (int)theAttribs[i].param_tag);
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
      { NAMEVAL(GL_RESCALE_NORMAL_EXT) },

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
        pkg->linkVarCopy(theEnums[i].name, (int)theEnums[i].val);
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
#ifdef BROKEN_TEMPLATE_FUNCTIONS
                , T* /*dummy*/=0
#endif
                )
  {
    const AttribInfo* theInfo = theAttribMap[param_tag];
    if ( theInfo == 0 )
      {
        throw Tcl::TclError("invalid or unsupported enumerant");
      }

    fixed_block<T> theVals(theInfo->num_values);
    extractValues(theInfo->param_tag, &(theVals[0]));
    Tcl::List result;
    result.appendRange(theVals.begin(), theVals.end());
    return result;
  }

#ifdef BROKEN_TEMPLATE_FUNCTIONS
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

void GLTcl::loadMatrix(Tcl::List entries)
{
  fixed_block<GLdouble> matrix(entries.begin<GLdouble>(),
                               entries.end<GLdouble>());

  if (matrix.size() != 16)
    {
      throw Tcl::TclError("matrix must have 16 entries in column-major order");
    }

  glLoadMatrixd(&matrix[0]);

  Gfx::Canvas::current().throwIfError("loadMatrix");
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
DOTRACE("GLTcl::antialias");
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

void GLTcl::drawOneLine(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
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
// GLTcl::pixelCheckSum --
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

long int GLTcl::pixelCheckSum(int x, int y, int w, int h)
{
  Gfx::BmapData data;

  const Gfx::Rect<int> bounds = Gfx::Rect<int>().setXYWH(x, y, w, h);

  Gfx::Canvas::current().grabPixels(bounds, data);

  return data.checkSum();
}

//--------------------------------------------------------------------
//
// Gltcl_Init --
//
//--------------------------------------------------------------------

extern "C"
int Gltcl_Init(Tcl_Interp* interp)
{
DOTRACE("Gltcl_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Tclgl", "$Revision$");
  GLTcl::loadGet(pkg);
  GLTcl::loadEnums(pkg);

  using Util::bindFirst;

  pkg->def( "::glBegin", "mode", glBegin );
  pkg->def( "::glBlendFunc", "sfactor dfactor", glBlendFunc );
  pkg->def( "::glCallList", "list", glCallList );
  pkg->def( "::glClear", "mask_bits", glClear );
  pkg->def( "::glClearColor", "red green blue alpha", glClearColor );
  pkg->def( "::glClearIndex", "index", glClearIndex );
  pkg->def( "::glColor", "red green blue", glColor3d );
  pkg->def( "::glColor", "red green blue alpha", glColor4d );
  pkg->def( "::glDeleteLists", "list_id range", glDeleteLists );
  pkg->def( "::glDisable", "capability", glDisable );
  pkg->def( "::glDrawBuffer", "mode", glDrawBuffer );
  pkg->def( "::glEnable", "capability", glEnable );
  pkg->def( "::glEnd", 0, glEnd );
  pkg->def( "::glEndList", 0, glEndList );
  pkg->def( "::glExtensions", 0, bindFirst(GLTcl::getString, GL_EXTENSIONS));
  pkg->def( "::glFlush", 0, glFlush );
  pkg->def( "::glFrustum", "left right bottom top zNear zFar", glFrustum );
  pkg->def( "::glGenLists", "range", glGenLists );
  pkg->def( "::glGetError", 0, GLTcl::checkGL );
  pkg->def( "::glIndexi", "index", glIndexi );
  pkg->def( "::glIsList", "list_id", glIsList );
  pkg->def( "::glLineWidth", "width", glLineWidth );
  pkg->def( "::glListBase", "base", glListBase );
  pkg->def( "::glLoadIdentity", 0, glLoadIdentity );
  pkg->def( "::glLoadMatrix", "4x4_column_major_matrix", GLTcl::loadMatrix );
  pkg->def( "::glMatrixMode", "mode", glMatrixMode );
  pkg->def( "::glNewList", "list_id mode", glNewList );
  pkg->def( "::glOrtho", "left right bottom top zNear zFar", glOrtho );
  pkg->def( "::glPolygonMode", "face mode", glPolygonMode );
  pkg->def( "::glPopMatrix", 0, glPopMatrix );
  pkg->def( "::glPushMatrix", 0, glPushMatrix );
  pkg->def( "::glRasterPos2d", "x y", glRasterPos2d );
  pkg->def( "::glRenderer", 0, bindFirst(GLTcl::getString, GL_RENDERER));
  pkg->def( "::glRotate", "angle_in_degrees x y z", glRotated );
  pkg->def( "::glScale", "x y z", glScaled );
  pkg->def( "::glTranslate", "x y z", glTranslated );
  pkg->def( "::glVendor", 0, bindFirst(GLTcl::getString, GL_VENDOR));
  pkg->def( "::glVersion", 0, bindFirst(GLTcl::getString, GL_VERSION));
  pkg->def( "::glVertex2", "x y", glVertex2d );
  pkg->def( "::glVertex3", "x y z", glVertex3d );
  pkg->def( "::glVertex4", "x y z w", glVertex4d );
  pkg->def( "::gluLookAt", "eyeX eyeY eyeZ targX targY targZ upX upY upZ",
            GLTcl::lookAt );
  pkg->def( "::gluPerspective", "field_of_view_y aspect zNear zFar",
            gluPerspective );

#ifndef BROKEN_TEMPLATE_FUNCTIONS
  pkg->def( "::glGetBoolean", "param_name", GLTcl::get<GLboolean> );
  pkg->def( "::glGetDouble", "param_name", GLTcl::get<GLdouble> );
  pkg->def( "::glGetInteger", "param_name", GLTcl::get<GLint> );
#else
  pkg->def( "::glGetBoolean", "param_name", GLTcl::getBoolean );
  pkg->def( "::glGetDouble", "param_name", GLTcl::getDouble );
  pkg->def( "::glGetInteger", "param_name", GLTcl::getInt );
#endif

  pkg->def( "::antialias", "on_off", GLTcl::antialias );
  pkg->def( "::drawOneLine", "x1 y1 x2 y2", GLTcl::drawOneLine );
  pkg->def( "::drawThickLine", "x1 y1 x2 y2 thickness", GLTcl::drawThickLine );
  pkg->def( "::lineInfo", 0, GLTcl::lineInfo );
  pkg->def( "::pixelCheckSum", "x y w h", GLTcl::pixelCheckSum );
  pkg->def( "::pixelCheckSum", 0, GLTcl::pixelCheckSumAll );

  return pkg->initStatus();
}

static const char vcid_gltcl_cc[] = "$Header$";
#endif // !GLTCL_CC_DEFINED
