///////////////////////////////////////////////////////////////////////
// position.h
// Rob Peters
// created: Wed Mar 10 21:33:14 1999
// written: Mon Apr 26 21:31:13 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef POSITION_H_DEFINED
#define POSITION_H_DEFINED

#ifndef IO_H_DEFINED
#include "io.h"
#endif

class PositionImpl;

class Position : public virtual IO {
public:
  //////////////
  // creators //
  //////////////

  Position();
  Position(istream &is, IOFlag flag);
  virtual ~Position();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);

  ///////////////
  // accessors //
  ///////////////

  void getRotate(float &a, float &x, float &y, float &z) const;
  // Return the vector (x, y, z) specifying the rotational axis
  // through the reference parameters x, y, and z, and return the
  // angle of rotation (in degrees) about that axis through the
  // reference parameter a.

  void getScale(float &x, float &y, float &z) const;
  // Return the scaling vector (x, y, z) through the reference
  // parameters x, y, and z. A negative value indicates a reflection
  // about the axis.

  void getTranslate(float &x, float &y, float &z) const;
  // Return the translation vector (x, y, z) through the reference
  // parameters x, y, and z.

  //////////////////
  // manipulators //
  //////////////////

  void setAngle(float a);
  // Set the angle of rotation to 'a' degrees.

  void setRotate(float a, float x, float y, float z);
  // Set the angle of rotation to 'a' degrees, and set the axis of
  // rotation to the vector (x, y, z).

  void setScale(float x, float y, float z);
  // Set the scaling vector to (x, y, z), where negative values
  // indicate a reflection about that axis.

  void setTranslate(float x, float y, float z);
  // Set the translation vector to (x, y, z).

  /////////////
  // actions //
  /////////////

  virtual void translate() const;
  virtual void scale() const;
  virtual void rotate() const;
  // Apply the appropriate operations to the current model view
  // matrix.

  void go() const;
  // Translate, scale, and rotate.

private:
  bool check() const;
  // Check all invariants and return true if everything is OK.

  PositionImpl* const itsImpl;  // opaque pointer to implementation
};

const char vcid_position_h[] = "$Header$";
#endif // !POSITION_H_DEFINED
