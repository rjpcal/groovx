///////////////////////////////////////////////////////////////////////
//
// x11util.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 16:32:51 2002
// written: Tue Sep 17 21:16:30 2002
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
                        bool privateCmap);
}

static const char vcid_x11util_h[] = "$Header$";
#endif // !X11UTIL_H_DEFINED
