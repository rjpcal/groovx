///////////////////////////////////////////////////////////////////////
//
// jitter.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Apr  7 13:46:40 1999
// written: Wed Nov 20 15:36:06 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JITTER_H_DEFINED
#define JITTER_H_DEFINED

#include "gfx/gxtransform.h"

/// Jitter is a GxTransform subclass that has randomness in its position.
class Jitter : public GxTransform
{
  //////////////
  // creators //
  //////////////

  /// Default constructor.
  Jitter();

public:
  /// Default creator.
  static Jitter* make();

  /// Virtual destructor.
  virtual ~Jitter();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  //////////////////
  // manipulators //
  //////////////////

  void setXJitter(double xj) { itsXJitter = xj; }
  void setYJitter(double yj) { itsYJitter = yj; }
  void setRJitter(double rj) { itsRJitter = rj; }

  void setJitter(double xj, double yj, double rj)
  {
    itsXJitter = xj;
    itsYJitter = yj;
    itsRJitter = rj;
  }


  /////////////
  // actions //
  /////////////

public:
  virtual void draw(Gfx::Canvas&) const;

private:
  void rejitter() const;

  // These specify the amount of jitter that will be effected by the
  // translate() or rotate() calls; the actual value used will be in
  // the range of the specified value +/- jitter value.
  double itsXJitter;
  double itsYJitter;
  double itsRJitter;

  mutable double itsXShift;
  mutable double itsYShift;
  mutable double itsRShift;
};

static const char vcid_jitter_h_[] = "$Header$";
#endif // !JITTER_H_DEFINED
