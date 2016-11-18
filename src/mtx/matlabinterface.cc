/** @file mtx/matlabinterface.cc convert mtx objects to/from
    matlab mxArray objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Dec 21 11:10:52 2004
//
///////////////////////////////////////////////////////////////////////

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

    if (s != storage_policy::BORROW && s != storage_policy::COPY)
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
