///////////////////////////////////////////////////////////////////////
//
// datablock.cc
//
// Copyright (c) 2001-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Mar 12 18:04:40 2001
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef DATABLOCK_CC_DEFINED
#define DATABLOCK_CC_DEFINED

#include "datablock.h"

#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "util/debug.h"
DBG_REGISTER
#include "util/trace.h"

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
    DOTRACE("shared_data_block::shared_data_block");
    dbgEval(3, this); dbgEvalNL(3, m_storage);
  }

  shared_data_block::~shared_data_block()
  {
    DOTRACE("shared_data_block::~shared_data_block");
    dbgEval(3, this); dbgEvalNL(3, m_storage);
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
DOTRACE("data_block::operator new");

  Assert(bytes == sizeof(data_block));
  if (fs_free_list == 0)
    return ::operator new(bytes);
  FreeNode* node = fs_free_list;
  fs_free_list = fs_free_list->next;
  return (void*)node;
}

void data_block::operator delete(void* space)
{
DOTRACE("data_block::operator delete");

  ((FreeNode*)space)->next = fs_free_list;
  fs_free_list = (FreeNode*)space;
}

data_block::data_block(double* data, unsigned int len) :
  m_refcount(0),
  m_storage(data),
  m_length(len)
{
DOTRACE("data_block::data_block");
}

data_block::~data_block()
{
DOTRACE("data_block::~data_block");
}

data_block* data_block::get_empty_data_block()
{
DOTRACE("data_block::get_empty_data_block");
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
DOTRACE("data_block::make_data_copy");
  if (data == 0)
    return get_empty_data_block();

  data_block* p = new shared_data_block(data_length);

  memcpy(p->m_storage, data, data_length*sizeof(double));

  return p;
}

data_block* data_block::make_zeros(int length)
{
DOTRACE("data_block::make_zeros");
  if (length <= 0)
    return get_empty_data_block();

  data_block* p = new shared_data_block(length);
  memset(p->m_storage, 0, length*sizeof(double));

  return p;
}

data_block* data_block::make_uninitialized(int length)
{
DOTRACE("data_block::make_uninitialized");
  if (length <= 0)
    return get_empty_data_block();

  return new shared_data_block(length);
}

data_block* data_block::make_borrowed(double* data, int data_length)
{
DOTRACE("data_block::make_borrowed");
  return new borrowed_data_block(data, data_length);
}

data_block* data_block::make_referred(double* data, int data_length)
{
DOTRACE("data_block::make_referred");
  return new referred_data_block(data, data_length);
}

void data_block::make_unique(data_block*& rep)
{
  if (rep->is_unique()) return;

  {
    DOTRACE("data_block::make_unique");

    data_block* rep_copy = make_data_copy(rep->m_storage, rep->m_length);

    // do the swap

    rep->decr_refcount();
    rep_copy->incr_refcount();

    rep = rep_copy;

    Postcondition(rep->m_refcount == 1);
  }
}

static const char vcid_datablock_cc[] = "$Header$";
#endif // !DATABLOCK_CC_DEFINED
