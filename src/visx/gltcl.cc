///////////////////////////////////////////////////////////////////////
//
// gltcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Nov-98
// written: Wed Aug  8 12:45:19 2001
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

#include "tcl/tclerror.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "util/arrays.h"

#include <GL/gl.h>
#include <GL/glu.h>
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

namespace GLTcl
{
  class GLPkg;

  void loadMatrix(Tcl::List entries);
  void lookAt(Tcl::List args);
  void antialias(bool on_off);
  void drawOneLine(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
  void drawThickLine(GLdouble x1, GLdouble y1,
                     GLdouble x2, GLdouble y2, GLdouble thickness);
  Tcl::List lineInfo();
  GLdouble pixelCheckSum(int x, int y, int w, int h);
  GLdouble pixelCheckSumAll()
  {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    return pixelCheckSum(viewport[0], viewport[1], viewport[2], viewport[3]);
  }

  class glGetCmd;
}

///////////////////////////////////////////////////////////////////////
//
// GLTcl Tcl package definitions
//
///////////////////////////////////////////////////////////////////////

namespace GLTcl
{
  void checkGL();
}

void GLTcl::checkGL()
{
  GLenum status = glGetError();
  if (status != GL_NO_ERROR)
    {
      const char* msg = reinterpret_cast<const char*>(gluErrorString(status));
      throw Util::Error(msg);
    }
}

//---------------------------------------------------------------------
//
// GLTcl::glGetCmd --
//
//---------------------------------------------------------------------

namespace GLTcl
{
  struct AttribInfo
  {
    const char* param_name;
    GLenum param_tag;
    int num_values;
  };

  static std::map<GLenum, const AttribInfo*> theAttribMap;

  void initializeGet(Tcl::Pkg* pkg) {
    static bool inited = false;

    if ( inited ) return;

#define NAMEVAL(x) #x, x

    static const AttribInfo theAttribs[] = {
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

#undef STRVAL

    int num_params = sizeof(theAttribs) / sizeof(AttribInfo);
    for (int i = 0; i < num_params; ++i) {
      pkg->linkVarCopy(theAttribs[i].param_name, (int)theAttribs[i].param_tag);
      theAttribMap[theAttribs[i].param_tag] = &(theAttribs[i]);
    }

    inited = true;
  }
}

namespace GLTcl
{
  template <class T>
  void extractValues(GLenum tag, T* vals_out);

  template <class T>
  Tcl::List get(GLenum param_tag)
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

}

// Specializations of glGetTypeCmd::extractValues for basic types

template <>
void GLTcl::extractValues<GLboolean>(GLenum tag, GLboolean* vals_out)
{ glGetBooleanv(tag, vals_out); }

template <>
void GLTcl::extractValues<GLdouble>(GLenum tag, GLdouble* vals_out)
{ glGetDoublev(tag, vals_out); }

template <>
void GLTcl::extractValues<GLint>(GLenum tag, GLint* vals_out)
{ glGetIntegerv(tag, vals_out); }

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
  checkGL();
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

GLdouble GLTcl::pixelCheckSum(int x, int y, int w, int h)
{
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
      for (unsigned int i = 0; i < pixels.size(); ++i)
        {
          sum += pixels[i];
        }
      return sum;
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
      for (unsigned int i = 0; i < pixels.size(); ++i)
        {
          sum += pixels[i];
        }
      return sum;
    }
}

//---------------------------------------------------------------------
//
// GLPkg --
//
//---------------------------------------------------------------------

class GLTcl::GLPkg : public Tcl::Pkg {
public:
  GLPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "Tclgl", "$Revision$")
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

    def( "::glBegin", "mode", glBegin );
    def( "::glBlendFunc", "sfactor dfactor", glBlendFunc );
    def( "::glCallList", "list", glCallList );
    def( "::glClear", "mask_bits", glClear );
    def( "::glClearColor", "red green blue alpha", glClearColor );
    def( "::glClearIndex", "index", glClearIndex );
    def( "::glColor", "red green blue alpha", glColor4d );
    def( "::glDeleteLists", "list_id range", glDeleteLists );
    def( "::glDisable", "capability", glDisable );
    def( "::glDrawBuffer", "mode", glDrawBuffer );
    def( "::glEnable", "capability", glEnable );
    def( "::glEnd", 0, glEnd );
    def( "::glEndList", 0, glEndList );
    def( "::glFlush", 0, glFlush );
    def( "::glFrustum", "left right bottom top zNear zFar", glFrustum );
    def( "::glGenLists", "range", glGenLists );
    def( "::glGetError", 0, checkGL );
    def( "::glIndexi", "index", glIndexi );
    def( "::glIsList", "list_id", glIsList );
    def( "::glLineWidth", "width", glLineWidth );
    def( "::glListBase", "base", glListBase );
    def( "::glLoadIdentity", 0, glLoadIdentity );
    def( "::glLoadMatrix", "4x4_column_major_matrix", GLTcl::loadMatrix );
    def( "::glMatrixMode", "mode", glMatrixMode );
    def( "::glNewList", "list_id mode", glNewList );
    def( "::glOrtho", "left right bottom top zNear zFar", glOrtho );
    def( "::glPolygonMode", "face mode", glPolygonMode );
    def( "::glPopMatrix", 0, glPopMatrix );
    def( "::glPushMatrix", 0, glPushMatrix );
    def( "::glRotate", "angle_in_degrees x y z", glRotated );
    def( "::glScale", "x y z", glScaled );
    def( "::glTranslate", "x y z", glTranslated );
    def( "::glVertex2", "x y", glVertex2d );
    def( "::glVertex3", "x y z", glVertex3d );
    def( "::glVertex4", "x y z w", glVertex4d );
    def( "::gluLookAt", "eyeX eyeY eyeZ targX targY targZ upX upY upZ",
         GLTcl::lookAt );
    def( "::gluPerspective", "field_of_view_y aspect zNear zFar",
         gluPerspective );

    initializeGet(this);
    def( "::glGetBoolean", "param_name", GLTcl::get<GLboolean> );
    def( "::glGetDouble", "param_name", GLTcl::get<GLdouble> );
    def( "::glGetInteger", "param_name", GLTcl::get<GLint> );

    def( "::antialias", "on_off", GLTcl::antialias );
    def( "::drawOneLine", "x1 y1 x2 y2", GLTcl::drawOneLine );
    def( "::drawThickLine", "x1 y1 x2 y2 thickness", GLTcl::drawThickLine );
    def( "::lineInfo", 0, GLTcl::lineInfo );
    def( "::pixelCheckSum", "x y w h", GLTcl::pixelCheckSum );
    def( "::pixelCheckSum", 0, GLTcl::pixelCheckSumAll );
  }
};

//--------------------------------------------------------------------
//
// GLTcl::Tclgl_Init --
//
//--------------------------------------------------------------------

extern "C"
int Gltcl_Init(Tcl_Interp* interp)
{
DOTRACE("Gltcl_Init");

  Tcl::Pkg* pkg = new GLTcl::GLPkg(interp);

#ifdef ACC_COMPILER
//   typeid(out_of_range);
//   typeid(length_error);
#endif

  return pkg->initStatus();
}

static const char vcid_gltcl_cc[] = "$Header$";
#endif // !GLTCL_CC_DEFINED
