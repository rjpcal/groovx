///////////////////////////////////////////////////////////////////////
//
// jitter.h
// Rob Peters
// created: Wed Apr  7 13:46:40 1999
// written: Tue Sep 26 19:06:38 2000
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

class Jitter : public Position, public virtual IO::IoObject {
public:
  //////////////
  // creators //
  //////////////

  Jitter();
#ifdef LEGACY
  Jitter(STD_IO::istream &is, IO::IOFlag flag);
#endif
  virtual ~Jitter();

  virtual void legacySrlz(IO::Writer* writer, STD_IO::ostream &os, IO::IOFlag flag) const;
  virtual void legacyDesrlz(IO::Reader* reader, STD_IO::istream &is, IO::IOFlag flag);
  virtual int legacyCharCount() const;

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

protected:
  virtual void translate() const;
  virtual void rotate() const;

public:
  virtual void go() const;
  virtual void rego() const;
  // The go() inherited is fine to reuse for Jitter, but rego must be
  // redefined for Jitter so that we duplicate the previous jittering,
  // rather than do a new jittering.

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
