///////////////////////////////////////////////////////////////////////
//
// gxcolor.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Aug 22 17:42:48 2001
// written: Wed Jun 26 12:52:41 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXCOLOR_H_DEFINED
#define GXCOLOR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GBCOLOR_H_DEFINED)
#include "gfx/gbcolor.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GXNODE_H_DEFINED)
#include "gfx/gxnode.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FIELDS_H_DEFINED)
#include "io/fields.h"
#endif

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

  virtual void draw(Gfx::Canvas& canvas) const;
  virtual void undraw(Gfx::Canvas& canvas) const;
};

static const char vcid_gxcolor_h[] = "$Header$";
#endif // !GXCOLOR_H_DEFINED
