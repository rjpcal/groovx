///////////////////////////////////////////////////////////////////////
//
// glxopts.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Aug  4 16:28:37 2002
// written: Tue Sep 17 22:27:11 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLXOPTS_H_DEFINED
#define GLXOPTS_H_DEFINED

class GlxAttribs;

template <class T> class shared_ptr;

struct GlxOpts
{
  GlxOpts() :
#ifndef NO_RGBA
    rgbaFlag( 1 ),
#else
    rgbaFlag( 0 ),
#endif
    rgbaRed( 1 ),
    rgbaGreen( 1 ),
    rgbaBlue( 1 ),
    colorIndexSize( 8 ),
#ifndef NO_DOUBLE_BUFFER
    doubleFlag( 1 ),
#else
    doubleFlag( 0 ),
#endif
    depthFlag( 1 ),
    depthSize( 8 ),
    accumFlag( 0 ),
    accumRed( 1 ),
    accumGreen( 1 ),
    accumBlue( 1 ),
    accumAlpha( 1 ),
    alphaFlag( 0 ),
    alphaSize( 1 ),
    stencilFlag( 0 ),
    stencilSize( 1 ),
    auxNumber( 0 ),
    indirect( 0 ),
    level( 0 ),
    transparent( 0 )
  {}

  int rgbaFlag;
  int rgbaRed;
  int rgbaGreen;
  int rgbaBlue;
  int colorIndexSize;
  int doubleFlag;
  int depthFlag;
  int depthSize;
  int accumFlag;
  int accumRed;
  int accumGreen;
  int accumBlue;
  int accumAlpha;
  int alphaFlag;
  int alphaSize;
  int stencilFlag;
  int stencilSize;
  int auxNumber;
  int indirect;
  int level;
  int transparent;

  shared_ptr<GlxAttribs> buildAttribList();
};

static const char vcid_glxopts_h[] = "$Header$";
#endif // !GLXOPTS_H_DEFINED
