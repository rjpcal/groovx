///////////////////////////////////////////////////////////////////////
//
// gxdrawstyle.h
//
// Copyright (c) 2001-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Aug 29 10:16:30 2001
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
  virtual ~GxDrawStyle() throw();

  /// Factory function.
  static GxDrawStyle* make() { return new GxDrawStyle; }

private:
  bool polygonFill;
  double pointSize;
  double lineWidth;
  int linePattern;

public:
  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  /// Get GxDrawStyle's fields.
  static const FieldMap& classFields();

  /// A no-op since GxDrawStyle doesn't actually draw anything onscreen.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  virtual void draw(Gfx::Canvas& canvas) const;
};

static const char vcid_gxdrawstyle_h[] = "$Header$";
#endif // !GXDRAWSTYLE_H_DEFINED
