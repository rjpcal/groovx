///////////////////////////////////////////////////////////////////////
// jitter.h
// Rob Peters
// created: Wed Apr  7 13:46:40 1999
// written: Mon Apr 26 21:21:44 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef JITTER_H_DEFINED
#define JITTER_H_DEFINED

#include "position.h"

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

  //////////////////
  // manipulators //
  //////////////////

  void setXJitter(float xj) { itsXJitter = xj; }
  void setYJitter(float yj) { itsYJitter = yj; }
  void setRJitter(float rj) { itsRJitter = rj; }

  /////////////
  // actions //
  /////////////

  virtual void translate() const;
  virtual void rotate() const;

private:
  // These specify the amount of jitter that will be effected by the
  // translate() or rotate() calls; the actual value used will be in
  // the range of the specified value +/- jitter value.
  float itsXJitter;
  float itsYJitter;
  float itsRJitter;
};

static const char vcid_jitter_h_[] = "$Header$";
#endif // !JITTER_H_DEFINED
