///////////////////////////////////////////////////////////////////////
//
// widget.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Dec  2 15:05:17 1999
// written: Mon Nov 25 19:03:48 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WIDGET_H_DEFINED
#define WIDGET_H_DEFINED

#include "util/object.h"
#include "util/signal.h"

namespace GWT
{
  class Widget;

  enum EventStatus { HANDLED, NOT_HANDLED };

  struct ButtonPressEvent
  {
    unsigned int button;
    int x;
    int y;
  };

  struct KeyPressEvent
  {
    const char* keys;
    int x;
    int y;
    bool controlPressed;
  };
}


///////////////////////////////////////////////////////////////////////
/**
 *
 * GWT::Widget is the base widget class.
 *
 **/
///////////////////////////////////////////////////////////////////////

class GWT::Widget : public virtual Util::Object
{
public:
  /// Default constructor
  Widget();

  /// Virtual destructor.
  virtual ~Widget();

  Util::Signal1<const ButtonPressEvent&> sigButtonPressed;
  Util::Signal1<const KeyPressEvent&> sigKeyPressed;

  virtual void takeFocus() = 0;

private:
  Widget(const Widget&);
  Widget& operator=(const Widget&);
};

static const char vcid_widget_h[] = "$Header$";
#endif // !WIDGET_H_DEFINED
