///////////////////////////////////////////////////////////////////////
//
// widget.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  4 12:52:59 1999
// written: Mon Nov 25 19:04:11 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WIDGET_CC_DEFINED
#define WIDGET_CC_DEFINED

#include "gwt/widget.h"

#include "util/trace.h"

GWT::Widget::Widget()
{
DOTRACE("GWT::Widget::Widget");
}

GWT::Widget::~Widget()
{
DOTRACE("GWT::Widget::~Widget");
}

static const char vcid_widget_cc[] = "$Header$";
#endif // !WIDGET_CC_DEFINED
