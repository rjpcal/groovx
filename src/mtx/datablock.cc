/** @file pkgs/mtx/datablock.cc auxiliary storage-management class for
    mtx */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Mar 12 18:04:40 2001
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

#include "datablock.h"

#include "rutz/error.h"

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "rutz/debug.h"
GVX_DBG_REGISTER
#include "rutz/trace.h"

namespace
{
  struct FreeNode
  {
    FreeNode* next;
  };

  FreeNode* fs_free_list = 0;

  class shared_data_block : public data_block
  {
  public:
    shared_data_block(int length);
    virtual ~shared_data_block();

    virtual bool is_unique() const { return refcount() <= 1; }
  };

  shared_data_block::shared_data_block(int length) :
    data_block(new double[length], length)
  {
    GVX_TRACE("shared_data_block::shared_data_block");
    dbg_eval(3, this); dbg_eval_nl(3, m_storage);
  }

  shared_data_block::~shared_data_block()
  {
    GVX_TRACE("shared_data_block::~shared_data_block");
    dbg_eval(3, this); dbg_eval_nl(3, m_storage);
    delete [] m_storage;
  }

  class borrowed_data_block : public data_block
  {
  public:
    borrowed_data_block(double* borrowed_data, unsigned int length) :
      data_block(borrowed_data, length)
    {}

    virtual ~borrowed_data_block()
    { /* don't delete the data, since they are 'borrowed' */ }

    // Since the data are borrowed, we always return false here.
    virtual bool is_unique() const { return false; }
  };

  class referred_data_block : public data_block
  {
  public:
    referred_data_block(double* referred_data, unsigned int length) :
      data_block(referred_data, length)
    {}

    virtual ~referred_data_block()
    { /* don't delete the data, since they are 'borrowed' */ }

    virtual bool is_unique() const { return refcount() <= 1; }
  };
}

void* data_block::operator new(size_t bytes)
{
GVX_TRACE("data_block::operator new");

  GVX_ASSERT(bytes == sizeof(data_block));
  if (fs_free_list == 0)
    return ::operator new(bytes);
  FreeNode* node = fs_free_list;
  fs_free_list = fs_free_list->next;
  return (void*)node;
}

void data_block::operator delete(void* space)
{
GVX_TRACE("data_block::operator delete");

  ((FreeNode*)space)->next = fs_free_list;
  fs_free_list = (FreeNode*)space;
}

data_block::data_block(double* data, unsigned int len) :
  m_refcount(0),
  m_storage(data),
  m_length(len)
{
GVX_TRACE("data_block::data_block");
}

data_block::~data_block()
{
GVX_TRACE("data_block::~data_block");
}

data_block* data_block::get_empty_data_block()
{
GVX_TRACE("data_block::get_empty_data_block");
  static data_block* empty_rep = 0;
  if (empty_rep == 0)
    {
      empty_rep = new shared_data_block(0);
      empty_rep->incr_refcount();
    }

  return empty_rep;
}

data_block* data_block::make_data_copy(const double* data, int data_length)
{
GVX_TRACE("data_block::make_data_copy");
  if (data == 0)
    return get_empty_data_block();

  data_block* p = new shared_data_block(data_length);

  memcpy(p->m_storage, data, data_length*sizeof(double));

  return p;
}

data_block* data_block::make_zeros(int length)
{
GVX_TRACE("data_block::make_zeros");
  if (length <= 0)
    return get_empty_data_block();

  data_block* p = new shared_data_block(length);
  memset(p->m_storage, 0, length*sizeof(double));

  return p;
}

data_block* data_block::make_uninitialized(int length)
{
GVX_TRACE("data_block::make_uninitialized");
  if (length <= 0)
    return get_empty_data_block();

  return new shared_data_block(length);
}

data_block* data_block::make_borrowed(double* data, int data_length)
{
GVX_TRACE("data_block::make_borrowed");
  return new borrowed_data_block(data, data_length);
}

data_block* data_block::make_referred(double* data, int data_length)
{
GVX_TRACE("data_block::make_referred");
  return new referred_data_block(data, data_length);
}

data_block* data_block::make(double* data,
                             int mrows, int ncols,
                             mtx_policies::storage_policy s)
{
  switch (s)
    {
    case mtx_policies::BORROW:
      return data_block::make_borrowed(data, mrows*ncols);

    case mtx_policies::REFER:
      return data_block::make_referred(data, mrows*ncols);

    case mtx_policies::COPY:
    default:
      break;
    }

  return data_block::make_data_copy(data, mrows*ncols);
}

data_block* data_block::make(int mrows, int ncols,
                             mtx_policies::init_policy p)
{
  if (p == mtx_policies::ZEROS)
    return data_block::make_zeros(mrows*ncols);
  // else...
  return data_block::make_uninitialized(mrows*ncols);
}

void data_block::make_unique(data_block*& rep)
{
  if (rep->is_unique()) return;

  {
    GVX_TRACE("data_block::make_unique");

    data_block* rep_copy = make_data_copy(rep->m_storage, rep->m_length);

    // do the swap

    rep->decr_refcount();
    rep_copy->incr_refcount();

    rep = rep_copy;

    GVX_POSTCONDITION(rep->m_refcount == 1);
  }
}

///////////////////////////////////////////////////////////////////////
//
// data_holder member functions
//
///////////////////////////////////////////////////////////////////////

data_holder::data_holder(double* data, int mrows, int ncols, storage_policy s) :
  m_data(data_block::make(data, mrows, ncols, s))
{
  m_data->incr_refcount();
}

data_holder::data_holder(int mrows, int ncols, init_policy p) :
  m_data(data_block::make(mrows, ncols, p))
{
  m_data->incr_refcount();
}

data_holder::data_holder(data_block* d) :
  m_data(d)
{
  m_data->incr_refcount();
}

data_holder::data_holder(const data_holder& other) :
  m_data(other.m_data)
{
  m_data->incr_refcount();
}

data_holder::~data_holder()
{
  m_data->decr_refcount();
}

void data_holder::swap(data_holder& other)
{
  std::swap(m_data, other.m_data);
}

///////////////////////////////////////////////////////////////////////
//
// data_ref_holder definitions
//
///////////////////////////////////////////////////////////////////////

void data_ref_holder::swap(data_ref_holder& other)
{
  std::swap(m_ref, other.m_ref);
}
