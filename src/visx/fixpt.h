///////////////////////////////////////////////////////////////////////
//
// fixpt.h
// Rob Peters
// created: Jan-99
// written: Thu Sep 23 14:19:15 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPT_H_DEFINED
#define FIXPT_H_DEFINED

#ifndef GROBJ_H_DEFINED
#include "grobj.h"
#endif

///////////////////////////////////////////////////////////////////////
//
// FixPt class declaration
//
///////////////////////////////////////////////////////////////////////

class FixPt : public GrObj {
public:
  FixPt (double len=0.1, int wid=1);
  FixPt (istream &is, IOFlag flag);
  virtual ~FixPt ();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  void setLength(double len) { sendStateChangeMsg(); itsLength = len; }
  void setWidth(int wid) { sendStateChangeMsg(); itsWidth = wid; }
  virtual void setCategory(int) {}
  virtual int getCategory() const { return -1; }

  double getLength() const { return itsLength; }
  int getWidth() const { return itsWidth; }

protected:
  virtual bool grGetBoundingBox(double& left, double& top,
										  double& right, double& bottom,
										  int& border_pixels) const;

  virtual void grRender() const;

private:
  double itsLength;               // length of crosshairs in GL coordinates
  int itsWidth;                  // width of crosshairs in pixels
};

static const char vcid_fixpt_h[] = "$Header$";
#endif // !FIXPT_H_DEFINED
