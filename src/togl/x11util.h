///////////////////////////////////////////////////////////////////////
//
// x11util.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 16:32:51 2002
// written: Sun Aug  4 19:37:16 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef X11UTIL_H_DEFINED
#define X11UTIL_H_DEFINED

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace X11Util
{
  Colormap findColormap(Display* dpy, XVisualInfo* visInfo,
                        bool rgba, bool privateCmap);

  Colormap rgbColormap(Display *dpy, int scrnum,
                       const XVisualInfo *visinfo);

  void hackInstallColormap(Display* dpy, Window win, Colormap cmap);

  // Returns the index number of the allocated color
  unsigned long noFaultXAllocColor(Display* dpy, Colormap cmap, int cmapSize,
                                   float red, float green, float blue);
}

static const char vcid_x11util_h[] = "$Header$";
#endif // !X11UTIL_H_DEFINED
