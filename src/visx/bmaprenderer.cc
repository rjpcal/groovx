///////////////////////////////////////////////////////////////////////
//
// bmaprenderer.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Dec  1 16:55:36 1999
// written: Thu Dec  2 12:50:52 1999
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

void BmapRenderer::doUndraw(
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
