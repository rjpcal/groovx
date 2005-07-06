/** @file gfx/gxdisk.h */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Nov 26 18:43:56 2002
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_GFX_GXDISK_H_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXDISK_H_UTC20050626084023_DEFINED

#include "gfx/gxnode.h"

#include "io/fields.h"

//  #######################################################
//  =======================================================

/// GxDisk is a GxNode subclass for drawing circles/rings/disks.

class GxDisk : public GxNode, public FieldContainer
{
protected:
  /// Default constructor.
  GxDisk();

public:
  /// Default creator.
  static GxDisk* make();

  /// Virtual destructor.
  virtual ~GxDisk() throw();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  ////////////
  // fields //
  ////////////

private:
  double itsInnerRadius;
  double itsOuterRadius;
  unsigned int itsNumSlices;
  unsigned int itsNumLoops;
  bool itsFilled;

public:
  /// Get GxDisk's FieldMap.
  static const FieldMap& classFields();

  /////////////
  // actions //
  /////////////

  /// Get the bounding cube for the cylinder.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  /// Draw the cylinder.
  virtual void draw(Gfx::Canvas&) const;

private:
  GxDisk(const GxDisk&);
  GxDisk& operator=(const GxDisk&);
};

static const char vcid_groovx_gfx_gxdisk_h_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXDISK_H_UTC20050626084023_DEFINED
