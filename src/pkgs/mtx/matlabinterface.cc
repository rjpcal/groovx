///////////////////////////////////////////////////////////////////////
//
// matlabinterface.cc
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Dec 21 11:10:52 2004
// commit: $Id$
// $HeadURL$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_PKGS_MTX_MATLABINTERFACE_CC_UTC20050626084022_DEFINED
#define GROOVX_PKGS_MTX_MATLABINTERFACE_CC_UTC20050626084022_DEFINED

#include "matlabinterface.h"

#ifndef GVX_NO_MATLAB

#include "mtx/mtx.h"

#include "rutz/error.h"

#include "rutz/trace.h"

namespace
{
  data_block* new_data_block(mxArray* a,
                             mtx_policies::storage_policy s)
  {
    if (!mxIsDouble(a))
      throw rutz::error("cannot construct a mtx "
                        "with a non-'double' mxArray", SRC_POS);

    return data_block::make(mxGetPr(a), mxGetM(a), mxGetN(a), s);
  }

  data_block* new_data_block(const mxArray* a,
                             mtx_policies::storage_policy s)
  {
    if (!mxIsDouble(a))
      throw rutz::error("cannot construct a mtx "
                        "with a non-'double' mxArray", SRC_POS);

    if (s != mtx_policies::BORROW && s != mtx_policies::COPY)
      throw rutz::error("cannot construct a mtx from a const mxArray* "
                        "unless the storage_policy is COPY or BORROW",
                        SRC_POS);

    return data_block::make(mxGetPr(a), mxGetM(a), mxGetN(a), s);
  }
}

mtx make_mtx(mxArray* a, mtx_policies::storage_policy s)
{
GVX_TRACE("make_mtx(mxArray*, storage_policy)");
  return mtx(mtx_specs(mxGetM(a), mxGetN(a)),
             data_holder(new_data_block(a, s)));
}

mtx make_mtx(const mxArray* a, mtx_policies::storage_policy s)
{
GVX_TRACE("make_mtx(const mxArray*, storage_policy)");
  return mtx(mtx_specs(mxGetM(a), mxGetN(a)),
             data_holder(new_data_block(a, s)));
}

mxArray* make_mxarray(const mtx& m)
{
GVX_TRACE("make_mxarray");
  mxArray* result_mx =
    mxCreateDoubleMatrix(m.mrows(), m.ncols(), mxREAL);

  std::copy(m.colmaj_begin(), m.colmaj_end(), mxGetPr(result_mx));

  return result_mx;
}

#endif // HAVE_MATLAB

static const char vcid_groovx_pkgs_mtx_matlabinterface_cc_utc20050626084022[] = "$Id$ $HeadURL$";
#endif // !GROOVX_PKGS_MTX_MATLABINTERFACE_CC_UTC20050626084022_DEFINED
