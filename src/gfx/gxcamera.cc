///////////////////////////////////////////////////////////////////////
//
// gxcamera.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 21 15:22:25 2002
// written: Thu Nov 21 15:29:36 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXCAMERA_CC_DEFINED
#define GXCAMERA_CC_DEFINED

#include "gxcamera.h"

#include "util/error.h"

#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>

#include "util/debug.h"
#include "util/trace.h"

void GxCamera::setPixelsPerUnit(double s)
{
  if (s <= 0.0)
    throw Util::Error("invalid scaling factor");

  itsPolicy = GxCamera::FIXED_SCALE;
  itsPixelsPerUnit = s;
}

void GxCamera::setUnitAngle(double deg_per_unit, double screen_ppi)
{
  if (deg_per_unit <= 0.0)
    throw Util::Error("unit angle must be positive");

  static const double deg_to_rad = 3.141593/180.0;

  // tan(deg_per_unit) == screen_inches_per_unit/viewing_dist;
  // screen_inches_per_unit == 1.0 * itsPixelsPerUnit / screepPpi;
  const double screen_inches_per_unit =
    tan(deg_per_unit*deg_to_rad) * itsViewingDistance;

  itsPixelsPerUnit = int(screen_inches_per_unit * screen_ppi);
  itsPolicy = GxCamera::FIXED_SCALE;
}

void GxCamera::setViewingDistIn(double inches)
{
  if (inches <= 0.0)
    throw Util::Error("viewing distance must be positive (duh)");

  // according to similar triangles,
  //   new_dist / old_dist == new_scale / old_scale;
  const double factor = inches / itsViewingDistance;
  itsPixelsPerUnit *= factor;
  itsViewingDistance = inches;
}

namespace
{
  void setRect(Gfx::Rect<double>& rect,
               double L, double T, double R, double B)
  {
    // Test for valid rect: right > left && top > bottom. In
    // particular, we must not have right == left or top == bottom
    // since this collapses the space onto one dimension.
    if (R <= L || T <= B)
      {
        throw Util::Error("invalid rect");
      }

    rect.setRectLTRB(L,T,R,B);
  }
}

void GxCamera::setFixedRectLTRB(double L, double T, double R, double B)
{
  itsPolicy = GxCamera::FIXED_RECT;
  setRect(itsRect, L,T,R,B);
}

void GxCamera::setMinRectLTRB(double L, double T, double R, double B)
{
  itsPolicy = GxCamera::MIN_RECT;
  setRect(itsRect, L,T,R,B);
}

void GxCamera::scaleRect(double factor)
{
  if (factor <= 0.0)
    throw Util::Error("invalid scaling factor");

  itsRect.scaleX(factor);
  itsRect.scaleY(factor);
}

void GxCamera::setPerspective(double fovy, double zNear, double zFar)
{
DOTRACE("GxCamera::setPerspective");

  itsPolicy = PERSPECTIVE;

  itsPerspective.fovy = fovy;
  itsPerspective.zNear = zNear;
  itsPerspective.zFar = zFar;
}

void GxCamera::reconfigure(const int width, const int height)
{
DOTRACE("GxCamera::reconfigure");

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, width, height);

  switch (itsPolicy)
    {
    case FIXED_SCALE:
      {
        const double l = -1 * (width  / 2.0) / itsPixelsPerUnit;
        const double r =      (width  / 2.0) / itsPixelsPerUnit;
        const double b = -1 * (height / 2.0) / itsPixelsPerUnit;
        const double t =      (height / 2.0) / itsPixelsPerUnit;
        glOrtho(l, r, b, t, -10.0, 10.0);
      }
      break;
    case FIXED_RECT:
      {
        glOrtho(itsRect.left(), itsRect.right(),
                itsRect.bottom(), itsRect.top(), -10.0, 10.0);
      }
      break;
    case MIN_RECT:
      {
        // the actual rect that we'll build:
        Gfx::Rect<double> port(itsRect);

        // the desired conditions are as follows:
        //    (1) port contains itsRect
        //    (2) port.aspect() == getAspect()
        //    (3) port is the smallest rectangle that meets (1) and (2)

        const double window_aspect = double(width) / double(height);

        const double ratio_of_aspects = itsRect.aspect() / window_aspect;

        if ( ratio_of_aspects < 1 ) // the available space is too wide...
          {
            port.scaleX(1/ratio_of_aspects); // so use some extra width
          }
        else                        // the available space is too tall...
          {
            port.scaleY(ratio_of_aspects); // so use some extra height
          }

        glOrtho(port.left(), port.right(),
                port.bottom(), port.top(), -10.0, 10.0);
      }
      break;
    case PERSPECTIVE:
      {
        gluPerspective(itsPerspective.fovy,
                       double(width) / double(height),
                       itsPerspective.zNear,
                       itsPerspective.zFar);
      }
      break;
    default:
      Assert(0); // "can't happen"
    }
}

static const char vcid_gxcamera_cc[] = "$Header$";
#endif // !GXCAMERA_CC_DEFINED
