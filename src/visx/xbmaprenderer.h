///////////////////////////////////////////////////////////////////////
//
// xbmaprenderer.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 17:19:23 1999
// written: Fri Nov 10 17:03:51 2000
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

  virtual void doRender(GWT::Canvas& canvas,
								unsigned char* bytes,
								double x_pos,
								double y_pos,
								int width,
								int height,
								int bits_per_pixel,
								int byte_alignment,
								double zoom_x,
								double zoom_y) const;
  
  virtual void doUndraw(GWT::Canvas& canvas,
					  int winRasterX, int winRasterY,
					  int winWidthX, int winHeightY) const;

  /** Reimplemented from \c BmapRenderer to keep the \c XImage
      up-to-date when the bitmap data change. */
  virtual void notifyBytesChanged() const;

private:
  void update(unsigned char* theBytes,
				  int width,
				  int height,
				  int bits_per_pixel,
				  int byte_alignment) const;

  XBmapRenderer(const XBmapRenderer&);
  XBmapRenderer& operator=(const XBmapRenderer&);

  mutable bool itsIsCurrent;
  mutable XImage* itsImage;
};

static const char vcid_xbmaprenderer_h[] = "$Header$";
#endif // !XBMAPRENDERER_H_DEFINED
