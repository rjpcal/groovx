///////////////////////////////////////////////////////////////////////
//
// tkutil.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Aug  4 15:09:49 2002
// written: Tue Sep 17 12:20:54 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TKUTIL_CC_DEFINED
#define TKUTIL_CC_DEFINED

#include "togl/tkutil.h"

#ifndef _TKPORT
#  define _TKPORT  // This eliminates need to include a bunch of Tk baggage
#endif
#include <tk.h>
#include <tkInt.h>

#include "util/trace.h"
#include "util/debug.h"

void TkUtil::createWindow(Tk_Window tkWin, XVisualInfo* visInfo,
                          int width, int height, Colormap cmap)
{
DOTRACE("TkUtil::createWindow");

  TkWindow* winPtr = reinterpret_cast<TkWindow*>(tkWin);

  // Make sure Tk knows to switch to the new colormap when the cursor is over
  // this window when running in color index mode.
  Tk_SetWindowVisual(tkWin, visInfo->visual, visInfo->depth, cmap);

  // Find parent of window (necessary for creation)
  Window parent = TkUtil::findParent(tkWin);

  DebugEvalNL(parent);

#define ALL_EVENTS_MASK \
KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask| \
EnterWindowMask|LeaveWindowMask|PointerMotionMask|ExposureMask|   \
VisibilityChangeMask|FocusChangeMask|PropertyChangeMask|ColormapChangeMask

  winPtr->atts.colormap = cmap;
  winPtr->atts.border_pixel = 0;
  winPtr->atts.event_mask = ALL_EVENTS_MASK;
  winPtr->window = XCreateWindow(winPtr->display,
                                 parent,
                                 0, 0, width, height,
                                 0, visInfo->depth,
                                 InputOutput, visInfo->visual,
                                 CWBorderPixel | CWColormap | CWEventMask,
                                 &winPtr->atts);

  DebugEvalNL(winPtr->window);

  int dummy_new_flag;
  Tcl_HashEntry* hPtr =
    Tcl_CreateHashEntry(&winPtr->dispPtr->winTable,
                        (char*) winPtr->window, &dummy_new_flag);
  Tcl_SetHashValue(hPtr, winPtr);

  winPtr->dirtyAtts = 0;
  winPtr->dirtyChanges = 0;
#ifdef TK_USE_INPUT_METHODS
  winPtr->inputContext = NULL;
#endif /* TK_USE_INPUT_METHODS */

  selectAllInput(tkWin);

  setupStackingOrder(tkWin);

  // Issue a ConfigureNotify event if there were deferred changes
  issueConfigureNotify(tkWin);
}

Window TkUtil::findParent(Tk_Window tkWin) throw()
{
DOTRACE("TkUtil::findParent");

  TkWindow* winPtr = reinterpret_cast<TkWindow*>(tkWin);

  if ((winPtr->parentPtr == NULL) || (winPtr->flags & TK_TOP_LEVEL))
    {
      return XRootWindow(winPtr->display, winPtr->screenNum);
    }
  // else...
  if (winPtr->parentPtr->window == None)
    {
      Tk_MakeWindowExist((Tk_Window) winPtr->parentPtr);
    }

  return winPtr->parentPtr->window;
}

void TkUtil::selectAllInput(Tk_Window tkWin) throw()
{
DOTRACE("TkUtil::selectAllInput");

#define ALL_EVENTS_MASK \
KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask| \
EnterWindowMask|LeaveWindowMask|PointerMotionMask|ExposureMask|   \
VisibilityChangeMask|FocusChangeMask|PropertyChangeMask|ColormapChangeMask

  TkWindow* winPtr = reinterpret_cast<TkWindow*>(tkWin);

  XSelectInput(winPtr->display, winPtr->window, ALL_EVENTS_MASK);
}

void TkUtil::setupStackingOrder(Tk_Window tkWin) throw()
{
DOTRACE("TkUtil::setupStackingOrder");

  TkWindow* winPtr = reinterpret_cast<TkWindow*>(tkWin);

  if (!(winPtr->flags & TK_TOP_LEVEL))
    {
      /*
       * If any siblings higher up in the stacking order have already
       * been created then move this window to its rightful position
       * in the stacking order.
       *
       * NOTE: this code ignores any changes anyone might have made
       * to the sibling and stack_mode field of the window's attributes,
       * so it really isn't safe for these to be manipulated except
       * by calling Tk_RestackWindow.
       */

      for (TkWindow* winPtr2 = winPtr->nextPtr; winPtr2 != NULL;
           winPtr2 = winPtr2->nextPtr)
        {
          if ((winPtr2->window != None) && !(winPtr2->flags & TK_TOP_LEVEL))
            {
              XWindowChanges changes;
              changes.sibling = winPtr2->window;
              changes.stack_mode = Below;
              XConfigureWindow(winPtr->display, winPtr->window,
                               CWSibling|CWStackMode, &changes);
              break;
            }
        }

      /*
       * If this window has a different colormap than its parent, add
       * the window to the WM_COLORMAP_WINDOWS property for its top-level.
       */

      if ((winPtr->parentPtr != NULL) &&
          (winPtr->atts.colormap != winPtr->parentPtr->atts.colormap))
        {
          TkWmAddToColormapWindows(winPtr);
        }
    }
}

void TkUtil::issueConfigureNotify(Tk_Window tkWin) throw()
{
DOTRACE("TkUtil::issueConfigureNotify");

  /*
   * Issue a ConfigureNotify event if there were deferred configuration
   * changes (but skip it if the window is being deleted;  the
   * ConfigureNotify event could cause problems if we're being called
   * from Tk_DestroyWindow under some conditions).
   */
  TkWindow* winPtr = reinterpret_cast<TkWindow*>(tkWin);

  if ((winPtr->flags & TK_NEED_CONFIG_NOTIFY)
      && !(winPtr->flags & TK_ALREADY_DEAD))
    {
      winPtr->flags &= ~TK_NEED_CONFIG_NOTIFY;

      XEvent event;

      event.type = ConfigureNotify;
      event.xconfigure.serial = LastKnownRequestProcessed(winPtr->display);
      event.xconfigure.send_event = False;
      event.xconfigure.display = winPtr->display;
      event.xconfigure.event = winPtr->window;
      event.xconfigure.window = winPtr->window;
      event.xconfigure.x = winPtr->changes.x;
      event.xconfigure.y = winPtr->changes.y;
      event.xconfigure.width = winPtr->changes.width;
      event.xconfigure.height = winPtr->changes.height;
      event.xconfigure.border_width = winPtr->changes.border_width;
      if (winPtr->changes.stack_mode == Above)
        {
          event.xconfigure.above = winPtr->changes.sibling;
        }
      else
        {
          event.xconfigure.above = None;
        }
      event.xconfigure.override_redirect = winPtr->atts.override_redirect;
      Tk_HandleEvent(&event);
    }
}

void TkUtil::destroyWindow(Tk_Window tkWin) throw()
{
DOTRACE("TkUtil::destroyWindow");

  TkWindow* winPtr = reinterpret_cast<TkWindow*>(tkWin);

  if (winPtr->window != None)
    {
      XDestroyWindow(winPtr->display, winPtr->window);
      winPtr->window = 0;
    }
}

static const char vcid_tkutil_cc[] = "$Header$";
#endif // !TKUTIL_CC_DEFINED
