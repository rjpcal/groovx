///////////////////////////////////////////////////////////////////////
//
// jitter.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Apr  7 13:46:40 1999
// written: Thu May 10 12:04:40 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JITTER_H_DEFINED
#define JITTER_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POSITION_H_DEFINED)
#include "position.h"
#endif

///////////////////////////////////////////////////////////////////////
//
// Jitter class definition
//
///////////////////////////////////////////////////////////////////////

class Jitter : public Position {
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

  /////////////
  // actions //
  /////////////

public:
  virtual void draw(GWT::Canvas&) const;
  virtual void undraw(GWT::Canvas&) const;
  // The draw() inherited is fine to reuse for Jitter, but undraw must
  // be redefined for Jitter so that we duplicate the previous
  // jittering, rather than do a new jittering.

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
