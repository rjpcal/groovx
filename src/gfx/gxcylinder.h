///////////////////////////////////////////////////////////////////////
//
// gxcylinder.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jul  5 12:43:20 2002
// written: Fri Jul  5 15:17:53 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXCYLINDER_H_DEFINED
#define GXCYLINDER_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GXNODE_H_DEFINED)
#include "gfx/gxnode.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FIELDS_H_DEFINED)
#include "io/fields.h"
#endif

//  ###################################################################
//  ===================================================================

/// GxCylinder is a GxNode subclass for drawing cylinders (also cones).

class GxCylinder : public GxNode, public FieldContainer
{
protected:
  /// Default constructor.
  GxCylinder();

public:
  /// Default creator.
  static GxCylinder* make();

  /// Virtual destructor.
  virtual ~GxCylinder();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  ////////////
  // fields //
  ////////////

private:
  double itsBase;
  double itsTop;
  double itsHeight;
  int itsSlices;
  int itsStacks;
  bool itsFilled;

public:
  /// Get GxCylinder's FieldMap.
  static const FieldMap& classFields();

  /////////////
  // actions //
  /////////////

  /// Get the bounding cube for the cylinder.
  virtual void getBoundingCube(Gfx::Box<double>& cube,
                               Gfx::Canvas& canvas) const;

  /// Draw the cylinder.
  virtual void draw(Gfx::Canvas&) const;

private:
  GxCylinder(const GxCylinder&);
  GxCylinder& operator=(const GxCylinder&);
};

static const char vcid_gxcylinder_h[] = "$Header$";
#endif // !GXCYLINDER_H_DEFINED
