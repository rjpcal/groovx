///////////////////////////////////////////////////////////////////////
//
// maskhatch.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Sep 23 15:49:58 1999
// written: Mon Aug 13 12:15:35 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MASKHATCH_H_DEFINED
#define MASKHATCH_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GROBJ_H_DEFINED)
#include "grobj.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FIELDS_H_DEFINED)
#include "io/fields.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c MaskHatch is a subclass of \c GrObj for rendering a visual mask
 * consisting of hatched lines. The pattern will contain vertical,
 * horizontal, and diagonal lines at 45-degrees left, and 45-degrees
 * right.
 *
 **/
///////////////////////////////////////////////////////////////////////

class MaskHatch : public GrObj, public FieldContainer {
protected:
  /// Default constructor.
  MaskHatch();

public:
  /// Default creator.
  static MaskHatch* make();

  /// Virtual destructor.
  virtual ~MaskHatch();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  ////////////////
  // properties //
  ////////////////

  /// The number of lines that will be rendered in each direction.
  TBoundedField<int> numLines;

  /// The pixel-width of each line.
  TBoundedField<int> lineWidth;

  static const FieldMap& classFields();

  /// Overridden from GrObj.
  virtual void receiveStateChangeMsg(const Util::Observable* obj);

protected:
  virtual Gfx::Rect<double> grGetBoundingBox() const;

  virtual void grRender(Gfx::Canvas& canvas, DrawMode mode) const;
};

static const char vcid_maskhatch_h[] = "$Header$";
#endif // !MASKHATCH_H_DEFINED
