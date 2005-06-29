///////////////////////////////////////////////////////////////////////
//
// multivalue.cc
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Aug 22 16:58:56 2001
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_RUTZ_MULTIVALUE_CC_UTC20050626084019_DEFINED
#define GROOVX_RUTZ_MULTIVALUE_CC_UTC20050626084019_DEFINED

#include "rutz/multivalue.h"

#include "rutz/error.h"

#include <iostream>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

template <class T>
rutz::multi_value<T>::multi_value(int num) : m_num_values(num) {}

template <class T>
rutz::multi_value<T>::~multi_value() {}

template <class T>
void rutz::multi_value<T>::print_to(std::ostream& os) const
{
GVX_TRACE("rutz::multi_value<T>::print_to");
  const T* dat = const_begin();
  const T* end = const_end();

  while (dat < end)
    {
      os << *dat;
      if (++dat < end)
        os << ' ';
    }
}

template <class T>
void rutz::multi_value<T>::scan_from(std::istream& is)
{
GVX_TRACE("rutz::multi_value<T>::scan_from");

  if (is.fail())
    throw rutz::error("istream started out in fail state", SRC_POS);
  if (is.eof())
    throw rutz::error("istream started out in eof state", SRC_POS);

  T* dat = mutable_begin();
  T* end = mutable_end();

  while (dat < end)
    {
      is >> *dat;
      dbg_eval_nl(3, *dat);
      ++dat;
      if (dat < end)
        {
          if (is.fail() || is.eof())
            throw rutz::error("stream underflow "
                              "while scanning a multi-value", SRC_POS);
        }
      else
        {
          if (is.fail() && !is.eof())
            throw rutz::error("stream error "
                              "while scanning a multi-value", SRC_POS);
        }
    }
}

template class rutz::multi_value<int>;
template class rutz::multi_value<double>;

static const char vcid_groovx_rutz_multivalue_cc_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_MULTIVALUE_CC_UTC20050626084019_DEFINED
