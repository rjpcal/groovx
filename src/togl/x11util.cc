///////////////////////////////////////////////////////////////////////
//
// x11util.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 16:32:56 2002
// written: Tue Sep 17 21:16:36 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef X11UTIL_CC_DEFINED
#define X11UTIL_CC_DEFINED

#include "togl/x11util.h"

#include "util/debug.h"
#include "util/trace.h"

Colormap X11Util::findColormap(Display* dpy, XVisualInfo* visInfo,
                               bool privateCmap)
{
DOTRACE("X11Util::findColormap");

  int scrnum = DefaultScreen(dpy);

  // For RGB colormap or shared color-index colormap
  if (!privateCmap)
    {
      if (visInfo->visual == DefaultVisual(dpy, scrnum))
        {
          // Just share the default colormap
          return DefaultColormap(dpy, scrnum);
        }
      else
        {
          // Make a new read-only colormap
          return XCreateColormap(dpy,
                                 RootWindow(dpy, visInfo->screen),
                                 visInfo->visual, AllocNone);
        }
    }

  // For private color-index colormap
  else
    {
      // Use AllocAll to get a read/write colormap
      return XCreateColormap(dpy,
                             RootWindow(dpy, visInfo->screen),
                             visInfo->visual, AllocAll);
    }
}

static const char vcid_x11util_cc[] = "$Header$";
#endif // !X11UTIL_CC_DEFINED
