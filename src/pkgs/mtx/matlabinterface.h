///////////////////////////////////////////////////////////////////////
//
// matlabinterface.h
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Dec 21 11:10:49 2004
// commit: $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MATLABINTERFACE_H_DEFINED
#define MATLABINTERFACE_H_DEFINED

#ifdef HAVE_MATLAB

#include <matrix.h>

#include "mtx/datablock.h"

class mtx;

mtx make_mtx(mxArray* a,
             mtx_policies::storage_policy s = mtx_policies::COPY);

/** With a const mxArray*, only BORROW or COPY are allowed as storage
    policies, in order to preserve const-correctness. */
mtx make_mtx(const mxArray* a,
             mtx_policies::storage_policy s = mtx_policies::COPY);

mxArray* make_mxarray(const mtx& m);

#endif // HAVE_MATLAB

static const char vcid_matlabinterface_h[] = "$Header$";
#endif // !MATLABINTERFACE_H_DEFINED
