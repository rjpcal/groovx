/** @file gfx/gxdrawstyle.h GxNode subclass for manipulating the
    line/polygon drawing style */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Aug 29 10:16:30 2001
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_GFX_GXDRAWSTYLE_H_UTC20050626084025_DEFINED
#define GROOVX_GFX_GXDRAWSTYLE_H_UTC20050626084025_DEFINED

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
  virtual ~GxDrawStyle() noexcept;

  /// Factory function.
  static GxDrawStyle* make() { return new GxDrawStyle; }

private:
  bool polygonFill;
  double pointSize;
  double lineWidth;
  int linePattern;

public:
  virtual io::version_id class_version_id() const override;
  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  /// Get GxDrawStyle's fields.
  static const FieldMap& classFields();

  /// A no-op since GxDrawStyle doesn't actually draw anything onscreen.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const override;

  virtual void draw(Gfx::Canvas& canvas) const override;
};

#endif // !GROOVX_GFX_GXDRAWSTYLE_H_UTC20050626084025_DEFINED
