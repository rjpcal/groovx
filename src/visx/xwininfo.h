///////////////////////////////////////////////////////////////////////
//
// xwininfo.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Dec  2 13:32:16 1999
// written: Thu May 10 12:04:39 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XWININFO_H_DEFINED
#define XWININFO_H_DEFINED

#ifndef _XLIB_H_
struct _XDisplay;
typedef struct _XDisplay Display;
typedef unsigned long Window;
#endif

class XWinInfo {
public:
  XWinInfo(Display* dpy, int scr_number, Window win) :
	 itsDisplay(dpy),
	 itsScreenNumber(scr_number),
	 itsWindow(win)
	 {}

  Display* getXDisplay()      const { return itsDisplay; }
  int      getXScreenNumber() const { return itsScreenNumber; }
  Window   getXWindow()       const { return itsWindow; }

private:
  Display*  itsDisplay;
  int       itsScreenNumber;
  Window    itsWindow;
};

static const char vcid_xwininfo_h[] = "$Header$";
#endif // !XWININFO_H_DEFINED
