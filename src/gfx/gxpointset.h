///////////////////////////////////////////////////////////////////////
//
// gxpointset.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jul  5 12:43:20 2002
// written: Tue Nov 19 12:43:42 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXPOINTSET_H_DEFINED
#define GXPOINTSET_H_DEFINED

#include "gfx/gbvec.h"
#include "gfx/gxnode.h"

#include "io/fields.h"

#include "util/arrayvalue.h"

//  ###################################################################
//  ===================================================================

/// GxPointSet is a GxNode subclass for drawing sets pf points.

class GxPointSet : public GxNode, public FieldContainer
{
protected:
  /// Default constructor.
  GxPointSet();

public:
  /// Default creator.
  static GxPointSet* make();

  /// Virtual destructor.
  virtual ~GxPointSet();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  ////////////
  // fields //
  ////////////

  TArrayValue<GbVec3<double> > itsPoints;

private:
  int itsStyle;

public:
  /// Get GxPointSet's FieldMap.
  static const FieldMap& classFields();

  /////////////
  // actions //
  /////////////

  /// Get the bounding cube for the pointset.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  /// Draw the pointset.
  virtual void draw(Gfx::Canvas&) const;

private:
  GxPointSet(const GxPointSet&);
  GxPointSet& operator=(const GxPointSet&);
};

static const char vcid_gxpointset_h[] = "$Header$";
#endif // !GXPOINTSET_H_DEFINED
