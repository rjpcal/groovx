///////////////////////////////////////////////////////////////////////
//
// jitter.h
// Rob Peters
// created: Wed Apr  7 13:46:40 1999
// written: Thu May 27 19:54:06 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JITTER_H_DEFINED
#define JITTER_H_DEFINED

#ifndef POSITION_H_DEFINED
#include "position.h"
#endif

///////////////////////////////////////////////////////////////////////
//
// Jitter class definition
//
///////////////////////////////////////////////////////////////////////

class Jitter : public Position, public virtual IO {
public:
  //////////////
  // creators //
  //////////////

  Jitter();
  Jitter(istream &is, IOFlag flag);
  virtual ~Jitter();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  //////////////////
  // manipulators //
  //////////////////

  void setXJitter(float xj) { itsXJitter = xj; }
  void setYJitter(float yj) { itsYJitter = yj; }
  void setRJitter(float rj) { itsRJitter = rj; }

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
  float itsXJitter;
  float itsYJitter;
  float itsRJitter;

  mutable float itsXShift;
  mutable float itsYShift;
  mutable float itsRShift;
};

static const char vcid_jitter_h_[] = "$Header$";
#endif // !JITTER_H_DEFINED
