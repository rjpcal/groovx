/** @file mtx/matlabinterface.h convert mtx objects to/from
    matlab mxArray objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue Dec 21 11:10:49 2004
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_PKGS_MTX_MATLABINTERFACE_H_UTC20050626084022_DEFINED
#define GROOVX_PKGS_MTX_MATLABINTERFACE_H_UTC20050626084022_DEFINED

#ifndef GVX_NO_MATLAB

#include <matrix.h>

#include "mtx/datablock.h"

class mtx;

mtx make_mtx(mxArray* a,
             mtx_policies::storage_policy s = storage_policy::COPY);

/** With a const mxArray*, only BORROW or COPY are allowed as storage
    policies, in order to preserve const-correctness. */
mtx make_mtx(const mxArray* a,
             mtx_policies::storage_policy s = storage_policy::COPY);

mxArray* make_mxarray(const mtx& m);

#endif // HAVE_MATLAB

#endif // !GROOVX_PKGS_MTX_MATLABINTERFACE_H_UTC20050626084022_DEFINED
