///////////////////////////////////////////////////////////////////////
// fixpt.h
// Rob Peters
// created: Jan-99
// written: Tue Mar  9 21:33:48 1999
///////////////////////////////////////////////////////////////////////

#ifndef FIXPT_H_DEFINED
#define FIXPT_H_DEFINED

#ifndef IO_H_INCLUDED
#include "io.h"
#endif
#ifndef SCALEABLE_H_INCLUDED
#include "scaleable.h"
#endif

///////////////////////////////////////////////////////////////////////
// FixPt class declaration
///////////////////////////////////////////////////////////////////////

class FixPt : public Scaleable, public virtual IO {
public:
  FixPt (float len, int wid);
  FixPt (istream &is);
  virtual ~FixPt ();

  virtual IOResult serialize(ostream &os, IOFlag flag = NO_FLAGS) const;

  virtual void grRecompile() const;

  void setLength(float len) { grPostRecompile(); itsLength = len; }
  void setWidth(int wid) { grPostRecompile(); itsWidth = wid; }

  float getLength() const { return itsLength; }
  int getWidth() const { return itsWidth; }

private:
  float itsLength;               // length of crosshairs in GL coordinates
  int itsWidth;                  // width of crosshairs in pixels
};

#endif // !FIXPT_H_DEFINED
