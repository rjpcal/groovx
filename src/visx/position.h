///////////////////////////////////////////////////////////////////////
// position.h
// Rob Peters
// created: Wed Mar 10 21:33:14 1999
// written: Wed Mar 10 21:55:19 1999
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
  virtual ~Position();

  IOResult serialize(ostream &os, IOFlag flag = NO_FLAGS) const;
  IOResult deserialize(istream &is, IOFlag flag = NO_FLAGS);

  ///////////////
  // accessors //
  ///////////////

  void getRotate(float &a, float &x, float &y, float &z) const;
  void getScale(float &x, float &y, float &z) const;
  void getTranslate(float &x, float &y, float &z) const;

  //////////////////
  // manipulators //
  //////////////////

  void setAngle(float a);
  void setRotate(float a, float x, float y, float z);
  void setScale(float x, float y, float z);
  void setTranslate(float x, float y, float z);

  /////////////
  // actions //
  /////////////

  void translate() const;
  void scale() const;
  void rotate() const;

private:
  PositionImpl *itsImpl;
};

#endif // !POSITION_H_DEFINED
