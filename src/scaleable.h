///////////////////////////////////////////////////////////////////////
// scaleable.h
// Rob Peters
// created: Tue Mar  2 17:17:29 1999
// written: Fri Mar 12 12:54:43 1999
static const char vcid_scaleable_h[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef SCALEABLE_H_DEFINED
#define SCALEABLE_H_DEFINED

#ifndef IO_H_INCLUDED
#include "io.h"
#endif
#ifndef GROBJ_H_INCLUDED
#include "grobj.h"
#endif

class Scaleable : public GrObj, public virtual IO {
public:
  Scaleable ();
  Scaleable (istream &is);
  virtual ~Scaleable();

  virtual IOResult serialize(ostream &os, IOFlag flag = NO_FLAGS) const;

  virtual void grRecompile() const = 0;

  void setTranslate(float x, float y, float z);
  void getTranslate(float &x, float &y, float &z) const;
  void doTranslate() const;

  void setScale(float x, float y, float z);
  void getScale(float &x, float &y, float &z) const;
  void doScale() const;

  void setRotate(float a, float x, float y, float z);
  void getRotate(float &a, float &x, float &y, float &z) const;
  void setAngle(float a);
  void doRotate() const;

private:
  float itsTranslate_x, itsTranslate_y, itsTranslate_z; // x,y,z coord shift
  float itsScale_x, itsScale_y, itsScale_z; // x,y,z scaling
  float itsRotAxis_x, itsRotAxis_y, itsRotAxis_z; // vector of rotation axis
  float itsRotAngle;				  // angle of rotation around rotation axis
};

///////////////////////////////////////////////////////////////////////
// Scaleable inline member functions
///////////////////////////////////////////////////////////////////////

inline void Scaleable::setTranslate(float x, float y, float z) {
  grPostRecompile();
  itsTranslate_x = x;
  itsTranslate_y = y;
  itsTranslate_z = z;
}

inline void Scaleable::getTranslate(float &x, float &y, float &z) const {
  x = itsTranslate_x;
  y = itsTranslate_y;
  z = itsTranslate_z;
}

inline void Scaleable::setScale(float x, float y, float z) {
  grPostRecompile();
  itsScale_x = x;
  itsScale_y = y;
  itsScale_z = z;
}

inline void Scaleable::getScale(float &x, float &y, float &z) const {
  x = itsScale_x;
  y = itsScale_y;
  z = itsScale_z;
}

inline void Scaleable::setRotate(float a, float x, float y, float z) {
  grPostRecompile();
  itsRotAngle = a;
  itsRotAxis_x = x;
  itsRotAxis_y = y;
  itsRotAxis_z = z;
}

inline void Scaleable::getRotate(float &a, float &x, float &y, float &z) const {
  a = itsRotAngle;
  x = itsRotAxis_x;
  y = itsRotAxis_y;
  z = itsRotAxis_z;
}

inline void Scaleable::setAngle(float a) {
  grPostRecompile(); 
  itsRotAngle = a;
}

#endif // !SCALEABLE_H_DEFINED
