/** @file pkgs/mtx/mtxops.cc free functions for class mtx */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Mar  4 11:13:05 2002
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "mtxops.h"

#include "mtx/arithfunctor.h"
#include "mtx/mtx.h"

#include "rutz/error.h"
#include "rutz/rand.h"

#include "rutz/trace.h"

mtx rand_mtx(size_t mrows, size_t ncols)
{
GVX_TRACE("rand_mtx");

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
GVX_TRACE("squared");

  mtx result(src);
  // FIXME would be better if mtx_base had a transform() template
  result.apply(dash::square());

  return result;
}

mtx zeropad(const mtx& src, size_t new_mrows, size_t new_ncols,
            size_t* ppadtop, size_t* ppadleft)
{
  GVX_TRACE("zeropad");

  if (new_mrows < src.mrows() || new_ncols < src.ncols())
    throw rutz::error("zeropad(): new size must be >= old size",
                      SRC_POS);

  mtx result = mtx::zeros(new_mrows, new_ncols);

  const size_t padtop =  (new_mrows - src.mrows() + 1) / 2;
  const size_t padleft = (new_ncols - src.ncols() + 1) / 2;

  if (ppadtop) *ppadtop = padtop;
  if (ppadleft) *ppadleft = padleft;

  for (size_t col = 0; col < src.ncols(); ++col)
    {
      result.column(col + padleft)(range_n(padtop, src.mrows()))
        = src.column(col);
    }

  return result;
}
