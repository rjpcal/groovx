///////////////////////////////////////////////////////////////////////
//
// x11util.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 16:32:56 2002
// written: Tue Sep 17 17:22:55 2002
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

void X11Util::hackInstallColormap(Display* dpy, Window win, Colormap cmap)
{
DOTRACE("X11Util::hackInstallColormap");

  // This is a hack to get the a window's colormap to be
  // visible... basically we install this colormap in all windows up the
  // window hierarchy up to (but not including) the root window. It should
  // be possible to get the window manager to install the colormap when the
  // Togl widget becomes the active window, but this has not worked yet.

  Window current = win;
  Window root = XRootWindow( dpy, DefaultScreen(dpy) );

  while (current != root)
    {
      DebugEval((void*)current); DebugEvalNL((void*)root);

      XSetWindowColormap(dpy, current, cmap);

      Window parent;
      Window* children;
      unsigned int nchildren;

      XQueryTree(dpy, current, &root, &parent, &children, &nchildren);
      XFree(children);

      current = parent;
    }
}

static const char vcid_x11util_cc[] = "$Header$";
#endif // !X11UTIL_CC_DEFINED
