///////////////////////////////////////////////////////////////////////
//
// dlisttcl.cc
//
// Copyright (c) 1998-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Dec  1 08:00:00 1998
// commit: $Id$
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

#ifndef DLISTTCL_CC_DEFINED
#define DLISTTCL_CC_DEFINED

// This file implements additional Tcl list manipulation functions

#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "util/algo.h"
#include "util/arrays.h"
#include "util/error.h"
#include "util/rand.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

#include <algorithm> // for std::random_shuffle
#include <cmath>

// Helper functions
namespace
{
  template <class Itr>
  double perm_distance_aux(Itr itr, Itr end)
  {
    int c = 0;
    double result = 0.0;
    while (itr != end)
      {
        result += fabs(double(*itr) - double(c));
        ++itr;
        ++c;
      }

    return result / double(c);
  }

  template <class Itr>
  double perm_distance2_aux(Itr itr, Itr end, double power)
  {
    int c = 0;
    double result = 0.0;
    while (itr != end)
      {
        result += pow(fabs(double(*itr) - double(c)), power);
        ++itr;
        ++c;
      }

    return pow(result, 1.0/power) / double(c);
  }
}

namespace Dlist
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

    ASSERT(result.length() == index_list.length());

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

    ASSERT(result.length() == source_list.length());

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
        throw Util::Error("source_list is empty", SRC_POS);
      }

    return source_list.at(rutz::rand_range(0u, source_list.length()));
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
        throw Util::Error("npts must be at least 2", SRC_POS);
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

  double perm_distance(Tcl::List src)
  {
    return perm_distance_aux(src.begin<unsigned int>(),
                             src.end<unsigned int>());
  }

  double perm_distance2(Tcl::List src, double power)
  {
    return perm_distance2_aux(src.begin<unsigned int>(),
                              src.end<unsigned int>(),
                              power);
  }

  //---------------------------------------------------------------------
  //
  // generate a complete/pure permutation of the numbers 0..N-1
  // the result is such that:
  //   result[i] != i         for all i
  //   sum(abs(result[i]-i))  is maximal
  //
  // WARNING: At first glance this might sound like it yields a nice random
  // list, but in fact simply reversing the order of elements gives a
  // result that satisfies the constraints of this algorithm, without being
  // random at all!
  //
  //---------------------------------------------------------------------

  Tcl::List permute_maximal(unsigned int N)
  {
    if (N < 2)
      throw Util::Error("N must be at least 2 to make a permutation", SRC_POS);

    double maxdist = double(N)/2.0;

    if (N%2)
      {
        const double half = double(N)/2.0;
        maxdist = half + 1.0/(2.0 + 1.0/half);
      }

    maxdist -= 0.0001;

    rutz::fixed_block<unsigned int> slots(N);

    for (unsigned int i = 0; i < slots.size()-1; ++i)
      slots[i] = i+1;

    slots[slots.size()-1] = 0;

    double dist = perm_distance_aux(slots.begin(), slots.end());

    for (int c = 0; c < 100000; ++c)
      {
        unsigned int i = rutz::rand_range(0u, N);
        unsigned int j = i;
        while (j == i)
          {
            j = rutz::rand_range(0u, N);
          }

        if (slots[j] != i && slots[i] != j)
          {
            const double origdist =
              fabs(double(i)-double(slots[i])) +
              fabs(double(j)-double(slots[j]));

            const double newdist =
              fabs(double(j)-double(slots[i])) +
              fabs(double(i)-double(slots[j]));

            if (newdist > origdist)
              {
                rutz::swap2(slots[i], slots[j]);
                dist += (newdist-origdist)/double(N);
              }
          }

        if (dist >= maxdist)
          {
            double distcheck = perm_distance_aux(slots.begin(), slots.end());
            if (distcheck < maxdist)
              {
                throw Util::Error("snafu in permutation distance computation", SRC_POS);
              }

            dbg_eval_nl(3, c);

            Tcl::List result;

            for (unsigned int i = 0; i < slots.size(); ++i)
              {
                result.append(slots[i]);
              }

            return result;
          }
      }

    throw Util::Error("permutation algorithm failed to converge", SRC_POS);
    return Tcl::List(); // can't happen, but placate compiler
  }

  //---------------------------------------------------------------------
  //
  // generate a random permutation of the numbers 0..N-1 such that:
  //   result[i] != i         for all i
  //
  //---------------------------------------------------------------------

  Tcl::List permute_moveall(unsigned int N)
  {
    if (N < 2)
      throw Util::Error("N must be at least 2 to make a permutation", SRC_POS);

    rutz::fixed_block<bool> used(N);
    for (unsigned int i = 0; i < N; ++i)
      used[i] = false;

    rutz::fixed_block<unsigned int> slots(N);

    // fill slots[0] ... slots[N-2]
    for (unsigned int i = 0; i < N-1; ++i)
      {
        unsigned int v = i;
        while (v == i || used[v])
          v = rutz::rand_range(0u, N);

        ASSERT(v < N);

        used[v] = true;
        slots[i] = v;
      }

    // figure out which is the last available slot
    unsigned int lastslot = N;
    for (unsigned int i = 0; i < N; ++i)
      if (!used[i])
        {
          lastslot = i;
          break;
        }

    ASSERT(lastslot != N);

    if (lastslot == N)
      {
        slots[N-1] = slots[N-2];
        slots[N-2] = lastslot;
      }
    else
      {
        slots[N-1] = lastslot;
      }

    Tcl::List result;

    for (unsigned int i = 0; i < slots.size(); ++i)
      {
        result.append(slots[i]);
      }

    return result;
  }

  //--------------------------------------------------------------------
  //
  // this command produces a list of random numbers each between min and
  // max, and of the given
  //
  //--------------------------------------------------------------------

  Tcl::List rand(double min, double max, unsigned int N)
  {
    Tcl::List result;

    static rutz::urand generator;

    for (unsigned int i = 0; i < N; ++i)
      {
        result.append(generator.fdraw_range(min, max));
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
    unsigned int min_len = rutz::min(source_list.length(), times_list.length());

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
  // Return a new list containing the elements of the source list in
  // reverse order.
  //
  //--------------------------------------------------------------------

  Tcl::List reverse(Tcl::List src)
  {
    if (src.length() < 2)
      return src;

    Tcl::List result;
    for (unsigned int i = 0; i < src.length(); ++i)
      result.append(src.at(src.length()-i-1));
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
        throw Util::Error("flags list must be as long as source_list", SRC_POS);
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
    rutz::fixed_block<Tcl::ObjPtr> objs(src.begin<Tcl::ObjPtr>(),
                                        src.end<Tcl::ObjPtr>());

    rutz::urand generator(seed);

    std::random_shuffle(objs.begin(), objs.end(), generator);

    Tcl::List result;

    for (unsigned int i = 0; i < objs.size(); ++i)
      {
        result.append(objs[i]);
      }

    return result;
  }

  //---------------------------------------------------------------------
  //
  // Shuffle an input list through a random permutation such that no
  // element remains in its initial position.
  //
  //---------------------------------------------------------------------

  Tcl::List shuffle_moveall(Tcl::List src)
  {
    Tcl::List permutation = permute_moveall(src.length());
    return Dlist::choose(src, permutation);
  }

  //---------------------------------------------------------------------
  //
  // Shuffle an input list through a maximal permutation.
  //
  //---------------------------------------------------------------------

  Tcl::List shuffle_maximal(Tcl::List src)
  {
    Tcl::List permutation = permute_maximal(src.length());
    return Dlist::choose(src, permutation);
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

    ASSERT(result.length() == src.length());

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

} // end namespace Dlist


extern "C"
int Dlist_Init(Tcl_Interp* interp)
{
DOTRACE("Dlist_Init");

  PKG_CREATE(interp, "dlist", "$Revision$");

  pkg->def( "choose", "source_list index_list", &Dlist::choose, SRC_POS );
  pkg->def( "cycle_left", "list n", &Dlist::cycle_left, SRC_POS );
  pkg->def( "cycle_right", "list n", &Dlist::cycle_right, SRC_POS );
  pkg->def( "index", "list index", &Dlist::index, SRC_POS );
  pkg->def( "not", "list", &Dlist::not_, SRC_POS );
  pkg->def( "ones", "num_ones", &Dlist::ones, SRC_POS );
  pkg->def( "linspace", "begin end npts", &Dlist::linspace, SRC_POS );
  pkg->def( "perm_distance", "list", &Dlist::perm_distance, SRC_POS );
  pkg->def( "perm_distance2", "list power", &Dlist::perm_distance2, SRC_POS );
  pkg->def( "permute_maximal", "N", &Dlist::permute_maximal, SRC_POS );
  pkg->def( "permute_moveall", "N", &Dlist::permute_moveall, SRC_POS );
  pkg->def( "pickone", "list", &Dlist::pickone, SRC_POS );
  pkg->def( "rand", "min max N", &Dlist::rand, SRC_POS );
  pkg->def( "range", "begin end ?step=1?", &Dlist::range, SRC_POS );
  pkg->def( "range", "begin end", Util::bindLast(&Dlist::range, 1), SRC_POS );
  pkg->def( "repeat", "source_list times_list", &Dlist::repeat, SRC_POS );
  pkg->def( "reverse", "list", &Dlist::reverse, SRC_POS );
  pkg->def( "select", "source_list flags_list", &Dlist::select, SRC_POS );
  pkg->def( "shuffle", "list ?seed=0?", &Dlist::shuffle, SRC_POS );
  pkg->def( "shuffle", "list", Util::bindLast(&Dlist::shuffle, 0), SRC_POS );
  pkg->def( "shuffle_maximal", "list", &Dlist::shuffle_maximal, SRC_POS );
  pkg->def( "shuffle_moveall", "list", &Dlist::shuffle_moveall, SRC_POS );
  pkg->def( "slice", "list n", &Dlist::slice, SRC_POS );
  pkg->def( "sum", "list", &Dlist::sum, SRC_POS );
  pkg->def( "zeros", "num_zeros", &Dlist::zeros, SRC_POS );

  PKG_RETURN;
}

static const char vcid_dlisttcl_cc[] = "$Header$";
#endif // !DLISTTCL_CC_DEFINED
