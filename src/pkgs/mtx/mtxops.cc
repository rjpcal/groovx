///////////////////////////////////////////////////////////////////////
//
// mtxops.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Mar  4 11:13:05 2002
// commit: $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MTXOPS_CC_DEFINED
#define MTXOPS_CC_DEFINED

#include "mtxops.h"

#include "mtx/arithfunctor.h"
#include "mtx/mtx.h"

#include "util/error.h"

#include "util/trace.h"

mtx squared(const mtx& src)
{
DOTRACE("squared");

  mtx result(src);
  // FIXME would be better if mtx_base had a transform() template
  result.apply(dash::square());

  return result;
}

mtx zeropad(const mtx& src, int new_mrows, int new_ncols,
            int* ppadtop, int* ppadleft)
{
  DOTRACE("zeropad");

  if (new_mrows < src.mrows() || new_ncols < src.ncols())
    throw rutz::error("zeropad(): new size must be >= old size",
                      SRC_POS);

  mtx result = mtx::zeros(new_mrows, new_ncols);

  const int padtop =  (new_mrows - src.mrows() + 1) / 2;
  const int padleft = (new_ncols - src.ncols() + 1) / 2;

  if (ppadtop) *ppadtop = padtop;
  if (ppadleft) *ppadleft = padleft;

  for (int col = 0; col < src.ncols(); ++col)
    {
      result.column(col + padleft)(range_n(padtop, src.mrows()))
        = src.column(col);
    }

  return result;
}

static const char vcid_mtxops_cc[] = "$Id$ $URL$";
#endif // !MTXOPS_CC_DEFINED
