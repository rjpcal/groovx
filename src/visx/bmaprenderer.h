///////////////////////////////////////////////////////////////////////
//
// bmaprenderer.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Dec  1 16:51:34 1999
// written: Wed Mar 29 22:12:39 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BMAPRENDERER_H_DEFINED
#define BMAPRENDERER_H_DEFINED

namespace GWT {
  class Canvas;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c BmapRenderer defines an abstract interface for rendering raw
 * bitmap data to the screen.
 *
 **/
///////////////////////////////////////////////////////////////////////

class BmapRenderer {
public:
  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~BmapRenderer();

  /** This is the operation that subclasses must implement to do the
		actual rendering of the bitmap data. All of the information
		needed to do the rendering is passed as arguments to the
		function. */
  virtual void doRender(GWT::Canvas& canvas,
								unsigned char* /* bytes */,
								double /* x_pos */,
								double /* y_pos */,
								int /* width */,
								int /* height */,
								int /* bits_per_pixel */,
								int /* byte_alignment */,
								double /* zoom_x */,
								double /* zoom_y */) const = 0;
  
  /** This is the operation that subclasses should override if they
      need to provide a special unrendering method. A default no-op
      implementation is provided. */
  virtual void doUndraw(GWT::Canvas& canvas,
					  int /*winRasterX*/, int /*winRasterY*/,
					  int /*winWidthX*/, int /*winHeightY*/) const;

  /** This operation should be called by clients after every change to
      the bitmap data. The default implementation provided by Bitmap
      is a no-op, but subclasses may override if they need to update
      data structures when the bitmap data change. */
  virtual void notifyBytesChanged() const;
};

static const char vcid_bmaprenderer_h[] = "$Header$";
#endif // !BMAPRENDERER_H_DEFINED
