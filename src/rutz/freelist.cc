///////////////////////////////////////////////////////////////////////
//
// freelist.cc
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jul 20 08:00:31 2001
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

#ifndef FREELIST_CC_DEFINED
#define FREELIST_CC_DEFINED

#include "rutz/freelist.h"

#include "rutz/debug.h"
DBG_REGISTER

rutz::free_list_base::free_list_base(std::size_t size_check) :
  m_node_list(0), m_size_check(size_check)
{
  ASSERT(m_size_check >= sizeof(node));
}

void* rutz::free_list_base::allocate(std::size_t bytes)
{
  ASSERT(bytes == m_size_check);
  if (m_node_list == 0)
    return ::operator new(bytes);
  node* node = m_node_list;
  m_node_list = m_node_list->next;
  return static_cast<void*>(node);
}

void rutz::free_list_base::deallocate(void* space)
{
  node* n = static_cast<node*>(space);
  n->next = m_node_list;
  m_node_list = n;
}

static const char vcid_freelist_cc[] = "$Id$ $URL$";
#endif // !FREELIST_CC_DEFINED
