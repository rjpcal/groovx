///////////////////////////////////////////////////////////////////////
//
// widget.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Dec  2 15:05:17 1999
// written: Sat Dec  4 12:52:50 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WIDGET_H_DEFINED
#define WIDGET_H_DEFINED

class Widget {
public:
  virtual ~Widget();

  virtual void bind(const char* event_sequence, const char* script) = 0;
  virtual void takeFocus() = 0;
  virtual void swapBuffers() = 0;
  virtual void display() = 0;
  virtual void clearscreen() = 0;
  virtual void refresh() = 0;
  virtual void undraw() = 0;
};

static const char vcid_widget_h[] = "$Header$";
#endif // !WIDGET_H_DEFINED
