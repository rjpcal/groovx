///////////////////////////////////////////////////////////////////////
//
// glutil.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 17:25:24 2002
// written: Sat Aug  3 17:28:24 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLUTIL_H_DEFINED
#define GLUTIL_H_DEFINED

namespace GLUtil
{
  void* grabPixels(int inColor, unsigned int width, unsigned int height);

  int generateEPS(const char *filename, int inColor,
                  unsigned int width, unsigned int height);
}

static const char vcid_glutil_h[] = "$Header$";
#endif // !GLUTIL_H_DEFINED
