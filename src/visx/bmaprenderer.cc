///////////////////////////////////////////////////////////////////////
//
// bmaprenderer.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Dec  1 16:55:36 1999
// written: Wed Dec  1 17:49:47 1999
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

void BmapRenderer::bytesChangeHook(unsigned char* /* theBytes */,
											  int /* width */,
											  int /* height */,
											  int /* bits_per_pixel */,
											  int /* byte_alignment */)
{
DOTRACE("BmapRenderer::bytesChangeHook");
}

static const char vcid_bmaprenderer_cc[] = "$Header$";
#endif // !BMAPRENDERER_CC_DEFINED
