///////////////////////////////////////////////////////////////////////
//
// position.h
// Rob Peters
// created: Wed Mar 10 21:33:14 1999
// written: Wed Nov 10 13:25:52 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSITION_H_DEFINED
#define POSITION_H_DEFINED

#ifndef IO_H_DEFINED
#include "io.h"
#endif

class PositionImpl;

///////////////////////////////////////////////////////////////////////
/**
 *
 * The Position class provides an interface for storing a particular
 * manipulation of the OpenGL modelview matrix. This manipulation is
 * decomposed into translation, scaling, and rotation.
 *
 * @memo Base class that manipulates the OpenGL modelview matrix.
 *
 */

class Position : public virtual IO {
public:
  //////////////
  // creators //
  //////////////

  /// Default constructor.
  Position();
  /// Stream constructor.
  Position(istream &is, IOFlag flag);
  ///
  virtual ~Position();

  ///
  virtual void serialize(ostream &os, IOFlag flag) const;
  ///
  virtual void deserialize(istream &is, IOFlag flag);
  ///
  virtual int charCount() const;

  ///
  virtual void readFrom(Reader* reader);
  ///
  virtual void writeTo(Writer* writer) const;

  ///////////////
  // accessors //
  ///////////////

  /** Return the vector (x, y, z) specifying the rotational axis
		through the reference parameters x, y, and z, and return the
		angle of rotation (in degrees) about that axis through the
		reference parameter a. */
  void getRotate(double &a, double &x, double &y, double &z) const;

  /** Return the scaling vector (x, y, z) through the reference
		parameters x, y, and z. A negative value indicates a reflection
		about the axis. */
  void getScale(double &x, double &y, double &z) const;

  /** Return the translation vector (x, y, z) through the reference
		parameters x, y, and z. */
  void getTranslate(double &x, double &y, double &z) const;

  //////////////////
  // manipulators //
  //////////////////

  /**  Set the angle of rotation to 'a' degrees. */
  void setAngle(double a);

  /** Set the angle of rotation to 'a' degrees, and set the axis of
		rotation to the vector (x, y, z). */
  void setRotate(double a, double x, double y, double z);

  /** Set the scaling vector to (x, y, z), where negative values
		indicate a reflection about that axis. */
  void setScale(double x, double y, double z);

  /** Set the translation vector to (x, y, z). */
  void setTranslate(double x, double y, double z);

  /////////////
  // actions //
  /////////////

protected:
  /// Apply the stored translation to the model view matrix
  virtual void translate() const;
  /// Apply the stored scaling to the model view matrix
  virtual void scale() const;
  /// Apply the stored rotation to the model view matrix
  virtual void rotate() const;

public:
  /// Translate, scale, and rotate.
  virtual void go() const;

  /** Redo most recent go(). (This is useful if the behavior of go()
      changes from one call to the next.) */
  virtual void rego() const;

private:
  /// Check all invariants and return true if everything is OK.
  bool check() const;

  /// opaque pointer to implementation
  PositionImpl* const itsImpl;
};

const char vcid_position_h[] = "$Header$";
#endif // !POSITION_H_DEFINED
