///////////////////////////////////////////////////////////////////////
//
// tlistwidget.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Dec  3 14:45:34 1999
// written: Fri Dec  3 14:51:04 1999
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
};

static const char vcid_tlistwidget_h[] = "$Header$";
#endif // !TLISTWIDGET_H_DEFINED
