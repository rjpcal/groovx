///////////////////////////////////////////////////////////////////////
//
// dlisttcl.cc
//
// Copyright (c) 1998-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Dec  1 08:00:00 1998
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

#ifndef DLISTTCL_CC_DEFINED
#define DLISTTCL_CC_DEFINED

// This file implements additional Tcl list manipulation functions

#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "util/algo.h"
#include "util/arrays.h"
#include "util/error.h"
#include "util/rand.h"
#include "util/randutils.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER;

#include <algorithm>

namespace DlistTcl
{

  //--------------------------------------------------------------------
  //
  // This command takes two lists as arguments, and uses the integers in
  // the second (index) list to return a permutation of the elements in
  // the first (source) list
  //
  // Example:
  //      dlist_choose { 3 5 7 } { 2 0 1 }
  // returns
  //      7 3 5
  //
  //--------------------------------------------------------------------

  Tcl::List choose(Tcl::List source_list, Tcl::List index_list)
  {
    Tcl::List result;

    for (unsigned int i = 0; i < index_list.length(); ++i)
      {
        unsigned int index = index_list.get<unsigned int>(i);

        // use that int as an index into source list, getting the
        // corresponding list element and appending it to the output list
        result.append(source_list.at(index));
      }

    Assert(result.length() == index_list.length());

    return result;
  }

  //---------------------------------------------------------------------
  //
  // Cyclically shift the elements of the list leftward by n steps.
  //
  //---------------------------------------------------------------------

  Tcl::List cycle_left(Tcl::List source_list, unsigned int n)
  {
    n = n % source_list.length();

    if (n == 0)
      return source_list;

    Tcl::List result;

    for (unsigned int i = n; i < source_list.length(); ++i)
      {
        result.append(source_list.at(i));
      }

    for (unsigned int i = 0; i < n; ++i)
      {
        result.append(source_list.at(i));
      }

    return result;
  }

  //---------------------------------------------------------------------
  //
  // Cyclically shift the elements of the list rightward by n steps.
  //
  //---------------------------------------------------------------------

  Tcl::List cycle_right(Tcl::List source_list, unsigned int n)
  {
    n = n % source_list.length();

    if (n == 0)
      return source_list;

    return cycle_left(source_list, source_list.length() - n);
  }

  //---------------------------------------------------------------------
  //
  // Returns the n'th element of the list; generates an error if n is
  // out of range.
  //
  //---------------------------------------------------------------------

  Tcl::ObjPtr index(Tcl::List source_list, unsigned int n)
  {
    return source_list.at(n);
  }

  //--------------------------------------------------------------------
  //
  // This command takes as its argument a single list containing only
  // integers, and returns a list in which each element is the logical
  // negation of its corresponding element in the source list.
  //
  //--------------------------------------------------------------------

  Tcl::List not_(Tcl::List source_list)
  {
    Tcl::ObjPtr one = Tcl::toTcl(int(1));
    Tcl::ObjPtr zero = Tcl::toTcl(int(0));
    Tcl::List result;

    for (unsigned int i = 0; i < source_list.length(); ++i)
      {
        if ( source_list.get<int>(i) == 0 )
          result.append(one);
        else
          result.append(zero);
      }

    Assert(result.length() == source_list.length());

    return result;
  }

  //--------------------------------------------------------------------
  //
  // this command produces a list of ones of the length specified by its
  // lone argument
  //
  //--------------------------------------------------------------------

  Tcl::List ones(unsigned int num_ones)
  {
    Tcl::List result;
    result.append(1, num_ones);

    return result;
  }

  //--------------------------------------------------------------------
  //
  // This commmand returns a single element chosen at random
  // from the source list
  //
  //--------------------------------------------------------------------

  Tcl::ObjPtr pickone(Tcl::List source_list)
  {
    if (source_list.length() == 0)
      {
        throw Util::Error("source_list is empty");
      }

    return source_list.at(Util::randRange(0u, source_list.length()));
  }

  //--------------------------------------------------------------------
  //
  // this command produces an ordered list of all integers between begin
  // and end, inclusive.
  //
  //--------------------------------------------------------------------

  Tcl::List range(int begin, int end, int step)
  {
    Tcl::List result;

    if (step == 0)
      {
        // special case: if step is 0, we return an empty list
      }
    else if (step > 0)
      {
        for (int i = begin; i <= end; i += step)
          {
            result.append(i);
          }
      }
    else // step < 0
      {
        for (int i = begin; i >= end; i += step)
          {
            result.append(i);
          }
      }

    return result;
  }

  //--------------------------------------------------------------------
  //
  // Make a series of linearly spaced values between (and including) two
  // endpoints
  //
  //--------------------------------------------------------------------

  Tcl::List linspace(double begin, double end, unsigned int npts)
  {
    Tcl::List result;

    if (npts < 2)
      {
        throw Util::Error("npts must be at least 2");
      }

    const double skip = (end - begin) / (npts - 1);

    bool integer_mode = (skip == int(skip) && begin == int(begin));

    if (integer_mode)
      for (unsigned int i = 0; i < npts; ++i)
        {
          result.append(int(begin + i*skip));
        }
    else
      for (unsigned int i = 0; i < npts; ++i)
        {
          result.append(begin + i*skip);
        }

    return result;
  }

  //--------------------------------------------------------------------
  //
  // This command taks two lists as arguments. Each element from the
  // first (source) list is appended to the result multiple times; the
  // number of times is determined by the corresponding integer found in
  // the second (times) list.
  //
  // For example:
  //      dlist_repeat { 4 5 6 } { 1 2 3 }
  // returns
  //      4 5 5 6 6 6
  //
  //--------------------------------------------------------------------

  Tcl::List repeat(Tcl::List source_list, Tcl::List times_list)
  {
    // find the minimum of the two lists' lengths
    unsigned int min_len = Util::min(source_list.length(), times_list.length());

    Tcl::List result;

    for (unsigned int t = 0; t < min_len; ++t)
      {
        result.append(source_list.at(t),
                      times_list.get<unsigned int>(t));
      }

    return result;
  }

  //--------------------------------------------------------------------
  //
  // This command takes two lists as arguments, using the binary flags
  // in the second (flags) list to choose which elements from the first
  // (source) list should be appended to the output list
  //
  //--------------------------------------------------------------------

  Tcl::List select(Tcl::List source_list, Tcl::List flags_list)
  {
    unsigned int src_len = source_list.length();
    unsigned int flg_len = flags_list.length();

    if (flg_len < src_len)
      {
        throw Util::Error("flags list must be as long as source_list");
      }

    Tcl::List result;

    for (unsigned int i = 0; i < src_len; ++i)
      {
        // if the flag is true, add the corresponding source_list
        // element to the result list
        if ( flags_list.get<int>(i) )
          {
            result.append(source_list.at(i));
          }
      }

    return result;
  }

  //---------------------------------------------------------------------
  //
  // dlist::shuffle
  //
  //---------------------------------------------------------------------

  Tcl::List shuffle(Tcl::List src, int seed)
  {
    fixed_block<Tcl::ObjPtr> objs(src.begin<Tcl::ObjPtr>(),
                                  src.end<Tcl::ObjPtr>());

    Util::Urand generator(seed);

    std::random_shuffle(objs.begin(), objs.end(), generator);

    Tcl::List result;

    std::copy(objs.begin(), objs.end(), result.appender());

    return result;
  }

  //---------------------------------------------------------------------
  //
  // dlist::slice
  //
  //---------------------------------------------------------------------

  Tcl::List slice(Tcl::List src, unsigned int slice)
  {
    Tcl::List result;

    for (unsigned int i = 0, end = src.length(); i < end; ++i)
      {
        Tcl::List sub(src.at(i));
        result.append(sub.at(slice));
      }

    Assert(result.length() == src.length());

    return result;
  }

  //--------------------------------------------------------------------
  //
  // this command sums the numbers in a list, trying to return an int
  // result if possible, but returning a double result if any doubles
  // are found in the source list
  //
  //--------------------------------------------------------------------

  Tcl::ObjPtr sum(Tcl::List source_list)
  {
    int isum=0;
    double dsum=0.0;
    bool seen_double=false;

    for (unsigned int i = 0; i < source_list.length(); /* incr in loop body*/)
      {
        if ( !seen_double )
          {
            try
              {
                isum += source_list.get<int>(i);
              }
            catch(Util::Error&)
              {
                seen_double = true;
                dsum = isum;
                continue; // skip the increment
              }
          }
        else
          {
            dsum += source_list.get<double>(i);
          }

        ++i; // here's the increment
      }

    if ( !seen_double )
      return Tcl::toTcl(isum);
    else
      return Tcl::toTcl(dsum);
  }

  //--------------------------------------------------------------------
  //
  // this command produces a list of zeros of the length specified by its
  // lone argument
  //
  //--------------------------------------------------------------------

  Tcl::List zeros(unsigned int num_zeros)
  {
    Tcl::List result;
    result.append(0, num_zeros);

    return result;
  }

} // end namespace


extern "C"
int Dlist_Init(Tcl_Interp* interp)
{
DOTRACE("Dlist_Init");

  PKG_CREATE(interp, "dlist", "$Revision$");

  pkg->def( "choose", "source_list index_list", &DlistTcl::choose );
  pkg->def( "cycle_left", "source_list n", &DlistTcl::cycle_left );
  pkg->def( "cycle_right", "source_list n", &DlistTcl::cycle_right );
  pkg->def( "index", "list index", &DlistTcl::index );
  pkg->def( "not", "source_list", &DlistTcl::not_ );
  pkg->def( "ones", "num_ones", &DlistTcl::ones );
  pkg->def( "linspace", "begin end npts", &DlistTcl::linspace );
  pkg->def( "pickone", "source_list", &DlistTcl::pickone );
  pkg->def( "range", "begin end ?step=1?", &DlistTcl::range );
  pkg->def( "range", "begin end", Util::bindLast(&DlistTcl::range, 1) );
  pkg->def( "repeat", "source_list times_list", &DlistTcl::repeat );
  pkg->def( "select", "source_list flags_list", &DlistTcl::select );
  pkg->def( "shuffle", "source_list ?seed=0?", &DlistTcl::shuffle );
  pkg->def( "shuffle", "source_list", Util::bindLast(&DlistTcl::shuffle, 0) );
  pkg->def( "slice", "list n", &DlistTcl::slice );
  pkg->def( "sum", "source_list", &DlistTcl::sum );
  pkg->def( "zeros", "num_zeros", &DlistTcl::zeros );

  PKG_RETURN;
}

static const char vcid_dlisttcl_cc[] = "$Header$";
#endif // !DLISTTCL_CC_DEFINED
