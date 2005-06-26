///////////////////////////////////////////////////////////////////////
//
// mtxops.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Mar  4 11:13:05 2002
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef MTXOPS_CC_DEFINED
#define MTXOPS_CC_DEFINED

#include "mtxops.h"

#include "mtx/arithfunctor.h"
#include "mtx/mtx.h"

#include "rutz/error.h"
#include "rutz/rand.h"

#include "rutz/trace.h"

mtx rand_mtx(int mrows, int ncols)
{
DOTRACE("rand_mtx");

  static rutz::urand generator(rutz::default_rand_seed);

  mtx result = mtx::uninitialized(mrows, ncols);

  for (mtx::colmaj_iter
         itr = result.colmaj_begin_nc(),
         stop = result.colmaj_end_nc();
       itr != stop;
       ++itr)
    {
      *itr = generator.fdraw();
    }

  return result;
}

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
