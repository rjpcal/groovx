///////////////////////////////////////////////////////////////////////
//
// widget.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Dec  2 15:05:17 1999
// written: Thu Dec  2 15:05:29 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WIDGET_H_DEFINED
#define WIDGET_H_DEFINED

class Widget {
public:
  virtual void bind(const char* event_sequence, const char* script) = 0;
  virtual void takeFocus() = 0;
  virtual void swapBuffers() = 0;
};

static const char vcid_widget_h[] = "$Header$";
#endif // !WIDGET_H_DEFINED
