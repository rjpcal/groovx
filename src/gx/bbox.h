///////////////////////////////////////////////////////////////////////
//
// bbox.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 19 12:37:16 2002
// written: Tue Nov 19 14:02:45 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BBOX_H_DEFINED
#define BBOX_H_DEFINED

#include "gx/box.h"

namespace Gfx
{
  class Bbox;
  class Canvas;
  template <class V> class Rect;
  template <class V> class Vec2;
  template <class V> class Vec3;
}

class Gfx::Bbox
{
public:
  Bbox(Gfx::Canvas& c) : canvas(c) {}

  Bbox push() const { return Bbox(canvas); }

  Gfx::Vec2<int> screenFromWorld(const Gfx::Vec2<double>& world_pos) const;

  Gfx::Vec2<double> worldFromScreen(const Gfx::Vec2<int>& screen_pos) const;

  Gfx::Rect<int> screenFromWorld(const Gfx::Rect<double>& world_pos) const;

  Gfx::Rect<double> worldFromScreen(const Gfx::Rect<int>& screen_pos) const;

  Gfx::Box<double> cube;

private:
  Gfx::Canvas& canvas;
};

static const char vcid_bbox_h[] = "$Header$";
#endif // !BBOX_H_DEFINED
