///////////////////////////////////////////////////////////////////////
//
// tkutil.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Aug  4 15:07:44 2002
// written: Sun Aug  4 15:33:34 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TKUTIL_H_DEFINED
#define TKUTIL_H_DEFINED

typedef struct Tk_Window_ *Tk_Window;

#include <X11/Xlib.h>

namespace TkUtil
{
  void selectAllInput(Tk_Window tkWin) throw();

  /// Request the X window to be displayed.
  void mapWindow(Tk_Window tkWin) throw();
  void destroyWindow(Tk_Window tkWin) throw();
  Window findParent(Tk_Window tkWin) throw();
  void setupStackingOrder(Tk_Window tkWin) throw();
  void issueConfigureNotify(Tk_Window tkWin) throw();
};

static const char vcid_tkutil_h[] = "$Header$";
#endif // !TKUTIL_H_DEFINED
