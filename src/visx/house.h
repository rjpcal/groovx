/** @file visx/house.h graphic object for drawing parameterized house
    outlines */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Sep 13 12:43:15 1999
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

#ifndef GROOVX_VISX_HOUSE_H_UTC20050626084016_DEFINED
#define GROOVX_VISX_HOUSE_H_UTC20050626084016_DEFINED

#include "gfx/gxshapekit.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c House is a subclass of \c GxShapeKit that is able to draw simple
 * line-drawn houses. The appearance of the houses is controlled by
 * many parameters, such as the number of stories in the building, the
 * number of windows per story, the height of the roof, the shape of
 * the roof, etc.
 *
 **/
///////////////////////////////////////////////////////////////////////

class House : public GxShapeKit
{
protected:
  /// Default constructor.
  House();

public:
  /// Default creator.
  static House* make();

  /// Virtual destructor.
  virtual ~House() throw();

  virtual io::version_id class_version_id() const;
  virtual void read_from(io::reader& reader);
  virtual void write_to(io::writer& writer) const;

  ////////////////
  // properties //
  ////////////////

private:
  /// Width/height ratio of each story.
  double itsStoryAspectRatio;

  /// Number of stories in the building.
  int itsNumStories;

  /// Position of the door in range [0, itsNumWindows).
  int itsDoorPosition;

  /// Width of door as fraction of available parcel.
  double itsDoorWidth;

  /// Height of door as fraction of one story.
  double itsDoorHeight;

  /// Whether door handle is on left or right.
  bool itsDoorOrientation;

  /// Number of windows per story.
  int itsNumWindows;

  /// Width of windows as fraction of available parcel.
  double itsWindowWidth;

  /// Height of windows as fraction of one story.
  double itsWindowHeight;

  /// Number of vertical bars in each window.
  int itsWindowVertBars;

  /// Number of horizontal bars in each window.
  int itsWindowHorizBars;

  /// Enumerated mode for the roof shape.
  int itsRoofShape;

  /// Amount of roof overhang, as a fraction of story width.
  double itsRoofOverhang;

  /// Height of roof as fraction of the story height.
  double itsRoofHeight;

  /// Color of the roof: 0 -> black, 1 -> white.
  int itsRoofColor;

  /// X position of the chimney within the roof.
  double itsChimneyXPosition;

  /// Y position of the chimney within the roof.
  double itsChimneyYPosition;

  /// Width of the chimney, as fraction of the story width.
  double itsChimneyWidth;

  /// Height of the chimney, as fraction of the story height.
  double itsChimneyHeight;

public:
  static const FieldMap& classFields();

protected:
  virtual void grGetBoundingBox(Gfx::Bbox& bbox) const;

  virtual void grRender(Gfx::Canvas& canvas) const;

private:
  House(const House&);
  House& operator=(const House&);
};

static const char __attribute__((used)) vcid_groovx_visx_house_h_utc20050626084016[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_HOUSE_H_UTC20050626084016_DEFINED
