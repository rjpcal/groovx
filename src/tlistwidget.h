///////////////////////////////////////////////////////////////////////
//
// tlistwidget.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Dec  3 14:45:34 1999
// written: Sat Dec  4 01:10:49 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTWIDGET_H_DEFINED
#define TLISTWIDGET_H_DEFINED

#ifndef TOGLCONFIG_H_DEFINED
#include "toglconfig.h"
#endif

class TlistWidget : public ToglConfig {
public:
  TlistWidget(Togl* togl, double dist, double unit_angle);

  virtual void display();
  virtual void clearscreen();
  virtual void refresh();
  virtual void undraw();

  // Change the global visibility, which determines whether anything
  // will be displayed by a "redraw" command, or by remap events sent
  // to the screen window.
  void setVisibility(bool vis) { itsVisibility = vis; }
  
  void setCurTrial(int trial);

private:
  int itsCurTrial;
  bool itsVisibility;
};

static const char vcid_tlistwidget_h[] = "$Header$";
#endif // !TLISTWIDGET_H_DEFINED
