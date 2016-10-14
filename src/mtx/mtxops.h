/** @file pkgs/mtx/mtxops.h free functions for class mtx */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat Mar  2 18:34:12 2002
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_PKGS_MTX_MTXOPS_H_UTC20050626084022_DEFINED
#define GROOVX_PKGS_MTX_MTXOPS_H_UTC20050626084022_DEFINED

class mtx;

mtx rand_mtx(int mrows, int ncols);

mtx squared(const mtx& src);

mtx zeropad(const mtx& src, int new_mrows, int new_ncols,
            int* ppadtop=0, int* ppadleft=0);

#endif // !GROOVX_PKGS_MTX_MTXOPS_H_UTC20050626084022_DEFINED
