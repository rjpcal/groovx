///////////////////////////////////////////////////////////////////////
//
// xbmaprenderer.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 17:19:23 1999
// written: Fri Aug 10 10:46:48 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBMAPRENDERER_H_DEFINED
#define XBMAPRENDERER_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(BMAPRENDERER_H_DEFINED)
#include "bmaprenderer.h"
#endif

struct _XImage;
typedef _XImage XImage;

struct Tk_Window_;
typedef Tk_Window_* Tk_Window;

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c XBmapRenderer is an implementation of \c BmapRenderer that
 * renders bitmap data using X11's drawing API's.
 *
 **/
///////////////////////////////////////////////////////////////////////

class XBmapRenderer : public BmapRenderer {
public:
  /** Initializes some X11 structures. Must be called before any \c
      XBmapRenderer objects are rendered. */
  static void initClass(Tk_Window win);

  /// Default constructor.
  XBmapRenderer();

  /// Virtual destructor.
  virtual ~XBmapRenderer();

  virtual void doRender(Gfx::Canvas& canvas,
                        const BmapData& data,
                        const Point<double>& world_pos,
                        const Point<double>& zoom) const;

  /** Reimplemented from \c BmapRenderer to keep the \c XImage
      up-to-date when the bitmap data change. */
  virtual void notifyBytesChanged() const;

private:
  void update(const BmapData& data) const;

  XBmapRenderer(const XBmapRenderer&);
  XBmapRenderer& operator=(const XBmapRenderer&);

  mutable bool itsIsCurrent;
  mutable XImage* itsImage;
};

static const char vcid_xbmaprenderer_h[] = "$Header$";
#endif // !XBMAPRENDERER_H_DEFINED
