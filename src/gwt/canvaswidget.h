///////////////////////////////////////////////////////////////////////
//
// canvaswidget.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 25 17:26:14 2002
// written: Wed Sep 25 18:53:57 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef CANVASWIDGET_H_DEFINED
#define CANVASWIDGET_H_DEFINED

#include "gwt/widget.h"

namespace GWT
{
  class CanvasWidget;
}

/// A widget for drawing objects on a GWT::Canvas.
class GWT::CanvasWidget : public GWT::Widget
{
public:
  /// Default constructor.
  CanvasWidget();

  /// Virtual destuctor.
  virtual ~CanvasWidget();
};

static const char vcid_canvaswidget_h[] = "$Header$";
#endif // !CANVASWIDGET_H_DEFINED
