///////////////////////////////////////////////////////////////////////
//
// gxdrawstyle.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Aug 29 10:16:30 2001
// written: Tue Nov 19 12:43:42 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXDRAWSTYLE_H_DEFINED
#define GXDRAWSTYLE_H_DEFINED

#include "gfx/gxnode.h"

#include "io/fields.h"

/// GxDrawStyle is a graphics node class for line/polygon drawing styles.
class GxDrawStyle : public GxNode, public FieldContainer
{
private:
  GxDrawStyle(const GxDrawStyle&);
  GxDrawStyle& operator=(const GxDrawStyle&);

protected:
  /// Default constructor.
  GxDrawStyle();

public:
  virtual ~GxDrawStyle();

  static GxDrawStyle* make() { return new GxDrawStyle; }

private:
  bool polygonFill;
  double pointSize;
  double lineWidth;
  int linePattern;

public:
  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  static const FieldMap& classFields();

  /// A no-op since GxDrawStyle doesn't actually draw anything onscreen.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  virtual void draw(Gfx::Canvas& canvas) const;
};

static const char vcid_gxdrawstyle_h[] = "$Header$";
#endif // !GXDRAWSTYLE_H_DEFINED
