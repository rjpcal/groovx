///////////////////////////////////////////////////////////////////////
//
// tlistwidget.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Dec  3 14:45:34 1999
// written: Fri Nov 10 17:03:51 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTWIDGET_H_DEFINED
#define TLISTWIDGET_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TOGLCONFIG_H_DEFINED)
#include "toglconfig.h"
#endif

template <class T> class MaybeIdItem;

class TrialBase;

class TlistWidget : public ToglConfig {
public:
  TlistWidget(Tcl_Interp* interp, const char* pathname,
				  int config_argc, char** config_argv,
				  double dist, double unit_angle);

  virtual ~TlistWidget();

  virtual void display();
  virtual void clearscreen();
  virtual void refresh();
  virtual void undraw();

  // Change the global visibility, which determines whether anything
  // will be displayed by a "redraw" command, or by remap events sent
  // to the screen window.
  void setVisibility(bool vis);
  
  void setCurTrial(const MaybeIdItem<TrialBase>& item);

  void setHold(bool hold_on);

private:
  TlistWidget(const TlistWidget&);
  TlistWidget& operator=(const TlistWidget&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_tlistwidget_h[] = "$Header$";
#endif // !TLISTWIDGET_H_DEFINED
