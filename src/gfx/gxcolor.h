///////////////////////////////////////////////////////////////////////
//
// gxcolor.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Aug 22 17:42:48 2001
// written: Wed Mar 19 12:46:02 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXCOLOR_H_DEFINED
#define GXCOLOR_H_DEFINED

#include "gfx/gbcolor.h"
#include "gfx/gxnode.h"

#include "io/fields.h"

/// GxColor is a graphics node class for RGBA colors.
class GxColor : public GxNode, public FieldContainer
{
private:
  GxColor(const GxColor&);
  GxColor& operator=(const GxColor&);

  GbColor rgbaColor;

protected:
  /// Default constructor.
  GxColor();

public:
  virtual ~GxColor();

  static GxColor* make() { return new GxColor; }

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  static const FieldMap& classFields();

  /// This is a no-op since GxColor doesn't actually draw anything onscreen.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  virtual void draw(Gfx::Canvas& canvas) const;
};

static const char vcid_gxcolor_h[] = "$Header$";
#endif // !GXCOLOR_H_DEFINED
