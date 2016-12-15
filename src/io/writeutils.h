/** @file io/writeutils.h helper functions for writing counted
    sequences of objects to an io::writer */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue Nov 16 14:18:36 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_IO_WRITEUTILS_H_UTC20050626084021_DEFINED
#define GROOVX_IO_WRITEUTILS_H_UTC20050626084021_DEFINED

#include "io/writer.h"

#include "nub/ref.h"

#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace io
{
/// Utilities for writing sequences to a \c io::writer.
namespace write_utils
{

  rutz::fstring make_element_name(const rutz::fstring& seq_name,
                                  int element_num);

  rutz::fstring make_seq_length_name(const rutz::fstring& seq_name);

  template <class Itr>
  int compute_seq_length(Itr begin, Itr end)
    {
      int count = 0;
      while (begin != end) { ++count; ++begin; }
      return count;
    }

  /** A generic interface for handling containers, sequences, etc. of
      value types. */
  template <class Itr>
  void write_value_seq(io::writer& writer, const rutz::fstring& name,
                       Itr begin, Itr end, bool skip_count=false)
    {
      if (!skip_count)
        {
          writer.write_value(make_seq_length_name(name).c_str(),
                             compute_seq_length(begin, end));
        }

      int count = 0;

      while (begin != end)
        {
          writer.write_value(make_element_name(name, count).c_str(),
                            *begin);
          ++begin;
          ++count;
        }
    }

  /** A generic interface for handling containers, sequences, etc. of
      objects of rutz::value subtypes */
  template <class Itr>
  void write_value_obj_seq(io::writer& writer, const rutz::fstring& name,
                           Itr begin, Itr end, bool skip_count=false)
    {
      if (!skip_count)
        {
          writer.write_value(make_seq_length_name(name).c_str(),
                             compute_seq_length(begin, end));
        }

      int count = 0;

      while (begin != end)
        {
          writer.write_value_obj(make_element_name(name, count).c_str(),
                               *begin);
          ++begin;
          ++count;
        }
    }

  /// A generic interface for handling containers, sequences, etc. of objects
  template <class Itr>
  void write_object_seq(io::writer& writer, const rutz::fstring& name,
                        Itr begin, Itr end, bool skip_count=false)
    {
      if (!skip_count)
        {
          writer.write_value(make_seq_length_name(name).c_str(),
                             compute_seq_length(begin, end));
        }

      int count = 0;

      while (begin != end)
        {
          dbg_eval(4, count); dbg_eval_nl(4, (*begin)->id());

          writer.write_object(make_element_name(name, count).c_str(),
                              nub::soft_ref<const io::serializable>(*begin));
          ++begin;
          ++count;
        }
    }

}} // end namespace io::write_utils

#endif // !GROOVX_IO_WRITEUTILS_H_UTC20050626084021_DEFINED
