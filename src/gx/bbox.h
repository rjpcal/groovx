///////////////////////////////////////////////////////////////////////
//
// bbox.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 19 12:37:16 2002
// written: Tue Nov 19 12:41:39 2002
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
}

class Gfx::Bbox
{
public:
  Bbox(Gfx::Canvas& c) : canvas(c) {}

  Gfx::Box<double> cube;
  Gfx::Canvas& canvas;
};

static const char vcid_bbox_h[] = "$Header$";
#endif // !BBOX_H_DEFINED
