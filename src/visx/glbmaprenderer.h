///////////////////////////////////////////////////////////////////////
//
// glbmaprenderer.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Dec  1 17:52:41 1999
// written: Wed Dec  1 17:55:37 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBMAPRENDERER_H_DEFINED
#define GLBMAPRENDERER_H_DEFINED

#ifndef BMAPRENDERER_H_DEFINED
#include "bmaprenderer.h"
#endif

class GLBmapRenderer : public BmapRenderer {
public:
  GLBmapRenderer();
  virtual ~GLBmapRenderer();

  virtual void doRender(unsigned char* bytes,
								double x_pos,
								double y_pos,
								int width,
								int height,
								int bits_per_pixel,
								int byte_alignment,
								double zoom_x,
								double zoom_y) const;
  
  bool getUsingGlBitmap() const { return itsUsingGlBitmap; }

  void setUsingGlBitmap(bool val) { itsUsingGlBitmap = val; }

private:
  bool itsUsingGlBitmap;
};

static const char vcid_glbmaprenderer_h[] = "$Header$";
#endif // !GLBMAPRENDERER_H_DEFINED
