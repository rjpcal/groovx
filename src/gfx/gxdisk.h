///////////////////////////////////////////////////////////////////////
//
// gxdisk.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 26 18:43:56 2002
// written: Tue Nov 26 18:45:43 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXDISK_H_DEFINED
#define GXDISK_H_DEFINED

#include "gfx/gxnode.h"

#include "io/fields.h"

//  ###################################################################
//  ===================================================================

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
  virtual ~GxDisk();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

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

static const char vcid_gxdisk_h[] = "$Header$";
#endif // !GXDISK_H_DEFINED
