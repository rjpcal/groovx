///////////////////////////////////////////////////////////////////////
//
// bbox.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 19 12:37:16 2002
// written: Tue Nov 19 17:58:39 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BBOX_H_DEFINED
#define BBOX_H_DEFINED

namespace Gfx
{
  class Bbox;
  class Canvas;
  class Txform;
  template <class V> class Box;
  template <class V> class Rect;
  template <class V> class Vec2;
  template <class V> class Vec3;
}

class Gfx::Bbox
{
public:
  Bbox(Gfx::Canvas& c);

  Bbox(const Bbox& that);

  ~Bbox();

  Gfx::Bbox peer() const;

  void push();
  void pop();

  Gfx::Vec2<int> screenFromWorld(const Gfx::Vec2<double>& world_pos) const;

  Gfx::Vec2<double> worldFromScreen(const Gfx::Vec2<int>& screen_pos) const;

  Gfx::Rect<int> screenFromWorld(const Gfx::Rect<double>& world_pos) const;

  Gfx::Rect<double> worldFromScreen(const Gfx::Rect<int>& screen_pos) const;

  void translate(const Gfx::Vec3<double>& v);
  void scale(const Gfx::Vec3<double>& v);
  void transform(const Gfx::Txform& m);

  void vertex2(const Gfx::Vec2<double>& v);
  void vertex3(const Gfx::Vec3<double>& v);

  void drawRect(const Gfx::Rect<double>& rect);

  void drawBox(const Gfx::Box<double>& box);

  Gfx::Box<double> cube() const;
  Gfx::Rect<double> rect() const;

private:
  Bbox& operator=(const Bbox&);

  struct Impl;
  Impl* const rep;
};

static const char vcid_bbox_h[] = "$Header$";
#endif // !BBOX_H_DEFINED
