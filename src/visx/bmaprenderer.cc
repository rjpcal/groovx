///////////////////////////////////////////////////////////////////////
//
// bmaprenderer.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Dec  1 16:55:36 1999
// written: Mon Dec  6 22:01:20 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BMAPRENDERER_CC_DEFINED
#define BMAPRENDERER_CC_DEFINED

#include "bmaprenderer.h"

#include "trace.h"

BmapRenderer::~BmapRenderer()
{
DOTRACE("BmapRenderer::~BmapRenderer");
}

void BmapRenderer::doUndraw(Canvas& canvas,
					  int /*winRasterX*/, int /*winRasterY*/,
					  int /*winWidthX*/, int /*winHeightY*/) const
{
DOTRACE("BmapRenderer::doUndraw");
}

void BmapRenderer::notifyBytesChanged() const
{
DOTRACE("BmapRenderer::notifyBytesChanged");
}

static const char vcid_bmaprenderer_cc[] = "$Header$";
#endif // !BMAPRENDERER_CC_DEFINED
