///////////////////////////////////////////////////////////////////////
// fixpt.h
// Rob Peters
// created: Jan-99
// written: Fri Mar 12 10:56:05 1999
static const char vcid[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef FIXPT_H_DEFINED
#define FIXPT_H_DEFINED

#ifndef IO_H_INCLUDED
#include "grobj.h"
#endif

///////////////////////////////////////////////////////////////////////
// FixPt class declaration
///////////////////////////////////////////////////////////////////////

class FixPt : public GrObj {
public:
  FixPt (float len, int wid);
  FixPt (istream &is);
  virtual ~FixPt ();

  virtual IOResult serialize(ostream &os, IOFlag flag = NO_FLAGS) const;
  virtual IOResult deserialize(istream &is, IOFlag flag = NO_FLAGS);

  virtual void grRecompile() const;

  void setLength(float len) { grPostRecompile(); itsLength = len; }
  void setWidth(int wid) { grPostRecompile(); itsWidth = wid; }
  virtual void setCategory(int) {}

  float getLength() const { return itsLength; }
  int getWidth() const { return itsWidth; }
  virtual int getCategory() const { return -1; }

private:
  float itsLength;               // length of crosshairs in GL coordinates
  int itsWidth;                  // width of crosshairs in pixels
};

#endif // !FIXPT_H_DEFINED
