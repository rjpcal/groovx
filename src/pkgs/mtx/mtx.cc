///////////////////////////////////////////////////////////////////////
//
// mtx.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:39:12 2001
// written: Mon Mar 12 12:42:05 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MTX_CC_DEFINED
#define MTX_CC_DEFINED

#include "mtx.h"

#include "libmatlb.h"

Mtx::Mtx(mxArray* a) :
  mrows_(mxGetM(a)),
  ncols_(mxGetN(a)),
  data_(mxGetPr(a)),
  storage_(BORROWED)
{}

void Mtx::print() const
{
  mexPrintf("mrows = %d, ncols = %d\n", mrows_, ncols_);
  for(int i = 0; i < mrows_; ++i)
	 {
		for(int j = 0; j < ncols_; ++j)
		  mexPrintf("%7.4f   ", at(i,j));
		mexPrintf("\n");
	 }
  mexPrintf("\n");
}

static const char vcid_mtx_cc[] = "$Header$";
#endif // !MTX_CC_DEFINED
