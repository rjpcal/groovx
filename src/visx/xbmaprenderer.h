///////////////////////////////////////////////////////////////////////
//
// xbmaprenderer.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Dec  1 17:19:23 1999
// written: Wed Dec  1 17:49:24 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBMAPRENDERER_H_DEFINED
#define XBMAPRENDERER_H_DEFINED

#ifndef BMAPRENDERER_H_DEFINED
#include "bmaprenderer.h"
#endif

struct _XImage;
typedef _XImage XImage;

class ToglConfig;

class XBmapRenderer : public BmapRenderer {
public:
  static void initClass(const ToglConfig* config);

  XBmapRenderer();
  virtual ~XBmapRenderer();

  virtual void doRender(unsigned char* bytes,
								double x_pos,
								double y_pos,
								int width,
								int height,
								int bits_per_pixel,
								int byte_alignment,
								double zoom_x,
								double zoom_y) const;
  
  virtual void doUndraw(
					  int winRasterX, int winRasterY,
					  int winWidthX, int winHeightY) const;

  virtual void bytesChangeHook(unsigned char* theBytes,
										 int width,
										 int height,
										 int bits_per_pixel,
										 int byte_alignment);
private:
  XImage* itsImage;
};

static const char vcid_xbmaprenderer_h[] = "$Header$";
#endif // !XBMAPRENDERER_H_DEFINED
