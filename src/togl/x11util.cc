///////////////////////////////////////////////////////////////////////
//
// x11util.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 16:32:56 2002
// written: Tue Sep 17 17:16:36 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef X11UTIL_CC_DEFINED
#define X11UTIL_CC_DEFINED

#include "togl/x11util.h"

#include <X11/Xatom.h>  /* for XA_RGB_DEFAULT_MAP atom */
#include <X11/Xmu/StdCmap.h>  /* for XmuLookupStandardColormap */

#include "util/debug.h"
#include "util/trace.h"

// Return an X colormap to use for OpenGL RGB-mode rendering.
// Input:  dpy - the X display
//         scrnum - the X screen number
//         visinfo - the XVisualInfo as returned by glXChooseVisual()
// Return:  an X Colormap or 0 if there's a _serious_ error.
Colormap X11Util::rgbColormap(Display *dpy, int scrnum,
                              const XVisualInfo *visinfo)
{
DOTRACE("X11Util::rgbColormap");
  Window root = RootWindow(dpy,scrnum);

  // First check if visinfo's visual matches the default/root visual.

  if (visinfo->visual==DefaultVisual(dpy,scrnum))
    {
      /* use the default/root colormap */
      Colormap cmap;
      cmap = DefaultColormap( dpy, scrnum );
      return cmap;
    }


  // Next, try to find a standard X colormap.

#ifndef SOLARIS_BUG
  Status status = XmuLookupStandardColormap( dpy, visinfo->screen,
                                             visinfo->visualid, visinfo->depth,
                                             XA_RGB_DEFAULT_MAP,
                                             /* replace */ False,
                                             /* retain */ True);
  if (status == 1)
    {
      int numCmaps;
      XStandardColormap *standardCmaps;
      Status status = XGetRGBColormaps( dpy, root, &standardCmaps,
                                        &numCmaps, XA_RGB_DEFAULT_MAP);
      if (status == 1)
        {
          for (int i = 0; i < numCmaps; i++)
            {
              if (standardCmaps[i].visualid == visinfo->visualid)
                {
                  Colormap cmap = standardCmaps[i].colormap;
                  XFree(standardCmaps);
                  return cmap;
                }
            }
          XFree(standardCmaps);
        }
    }
#endif

  // If we get here, give up and just allocate a new colormap.

  return XCreateColormap( dpy, root, visinfo->visual, AllocNone );
}

Colormap X11Util::findColormap(Display* dpy, XVisualInfo* visInfo,
                               bool rgba, bool privateCmap)
{
DOTRACE("X11Util::findColormap");

  int scrnum = DefaultScreen(dpy);
  Colormap cmap;
  if (rgba)
    {
      /* Colormap for RGB mode */
      cmap = rgbColormap( dpy, scrnum, visInfo );
    }
  else
    {
      /* Colormap for CI mode */
      if (privateCmap)
        {
          /* need read/write colormap so user can store own color entries */
          cmap = XCreateColormap(dpy,
                                 RootWindow(dpy, visInfo->screen),
                                 visInfo->visual, AllocAll);
        }
      else
        {
          if (visInfo->visual==DefaultVisual(dpy, scrnum))
            {
              /* share default/root colormap */
              cmap = DefaultColormap(dpy,scrnum);
            }
          else
            {
              /* make a new read-only colormap */
              cmap = XCreateColormap(dpy,
                                     RootWindow(dpy, visInfo->screen),
                                     visInfo->visual, AllocNone);
            }
        }
    }

  return cmap;
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
