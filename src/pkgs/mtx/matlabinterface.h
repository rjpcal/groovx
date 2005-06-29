///////////////////////////////////////////////////////////////////////
//
// matlabinterface.h
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Dec 21 11:10:49 2004
// commit: $Id$
// $HeadURL$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_PKGS_MTX_MATLABINTERFACE_H_UTC20050626084022_DEFINED
#define GROOVX_PKGS_MTX_MATLABINTERFACE_H_UTC20050626084022_DEFINED

#ifndef GVX_NO_MATLAB

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

static const char vcid_groovx_pkgs_mtx_matlabinterface_h_utc20050626084022[] = "$Id$ $HeadURL$";
#endif // !GROOVX_PKGS_MTX_MATLABINTERFACE_H_UTC20050626084022_DEFINED
