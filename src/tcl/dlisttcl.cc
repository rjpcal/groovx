///////////////////////////////////////////////////////////////////////
//
// dlisttcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Dec-98
// written: Mon Jul 16 11:34:29 2001
// $Id$
//
// This package provides additional Tcl list manipulation functions
//
///////////////////////////////////////////////////////////////////////

#ifndef DLISTTCL_CC_DEFINED
#define DLISTTCL_CC_DEFINED

#include "tcl/objfunctor.h"
#include "tcl/tclcmd.h"
#include "tcl/tclerror.h"
#include "tcl/tclitempkg.h"

#include <cstdlib>

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// DlistTcl package declarations
//
///////////////////////////////////////////////////////////////////////

namespace DlistTcl
{
  Tcl::List choose(Tcl::List source_list, Tcl::List index_list);
  Tcl::List not_(Tcl::List source_list);
  Tcl::List ones(unsigned int num_ones);
  Tcl::ObjPtr pickone(Tcl::List source_list);
  Tcl::List range(int begin, int end);
  Tcl::List repeat(Tcl::List source_list, Tcl::List times_list);
  Tcl::List select(Tcl::List source_list, Tcl::List flags_list);
  Tcl::ObjPtr sum(Tcl::List source_list);
  Tcl::List zeros(unsigned int num_zeros);
}

///////////////////////////////////////////////////////////////////////
//
// DlistTcl package definitions
//
///////////////////////////////////////////////////////////////////////

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

Tcl::List DlistTcl::choose(Tcl::List source_list, Tcl::List index_list)
{
  Tcl::List result;

  for (unsigned int i = 0; i < index_list.length(); ++i)
    {
      unsigned int index = index_list.get<unsigned int>(i);

      // use that int as an index into source list, getting the
      // corresponding list element and appending it to the output list
      result.append(source_list.at(index));
    }

  return result;
}

//--------------------------------------------------------------------
//
// This command takes as its argument a single list containing only
// integers, and returns a list in which each element is the logical
// negation of its corresponding element in the source list.
//
//--------------------------------------------------------------------

Tcl::List DlistTcl::not_(Tcl::List source_list)
{
  Tcl::ObjPtr one(Tcl::Convert<int>::toTcl(1));
  Tcl::ObjPtr zero(Tcl::Convert<int>::toTcl(0));
  Tcl::List result;

  for (unsigned int i = 0; i < source_list.length(); ++i)
    {
      if ( source_list.get<int>(i) == 0 )
        result.append(one);
      else
        result.append(zero);
    }

  return result;
}

//--------------------------------------------------------------------
//
// this command produces a list of ones of the length specified by its
// lone argument
//
//--------------------------------------------------------------------

Tcl::List DlistTcl::ones(unsigned int num_ones)
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

Tcl::ObjPtr DlistTcl::pickone(Tcl::List source_list)
{
  if (source_list.length() == 0)
    {
      throw Tcl::TclError("source_list is empty");
    }

  // pick a random int between 0 and (src_len-1), inclusive
  unsigned int randnum = (unsigned int)
    ( double(rand())/(double(RAND_MAX)+1.0) * source_list.length() );

  return source_list.at(randnum);
}

//--------------------------------------------------------------------
//
// this command produces an ordered list of all integers between begin
// and end, inclusive.
//
//--------------------------------------------------------------------

Tcl::List DlistTcl::range(int begin, int end)
{
  Tcl::List result;

  for (int i = begin; i <= end; ++i)
    {
      result.append(i);
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

Tcl::List DlistTcl::repeat(Tcl::List source_list, Tcl::List times_list)
{
  unsigned int src_len = source_list.length();
  unsigned int tim_len = times_list.length();

  // find the minimum of the two lists' lengths
  unsigned int min_len = (src_len < tim_len) ? src_len : tim_len;

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

Tcl::List DlistTcl::select(Tcl::List source_list, Tcl::List flags_list)
{
  unsigned int src_len = source_list.length();
  unsigned int flg_len = flags_list.length();

  if (flg_len < src_len)
    {
      throw Tcl::TclError("flags list must be as long as source_list");
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

//--------------------------------------------------------------------
//
// this command sums the numbers in a list, trying to return an int
// result if possible, but returning a double result if any doubles
// are found in the source list
//
//--------------------------------------------------------------------

Tcl::ObjPtr DlistTcl::sum(Tcl::List source_list)
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
          catch(Tcl::TclError&)
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
    return isum;
  else
    return dsum;
}

//--------------------------------------------------------------------
//
// this command produces a list of zeros of the length specified by its
// lone argument
//
//--------------------------------------------------------------------

Tcl::List DlistTcl::zeros(unsigned int num_zeros)
{
  Tcl::List result;
  result.append(0, num_zeros);

  return result;
}

//--------------------------------------------------------------------
//
// package initialization procedure
//
//--------------------------------------------------------------------

extern "C"
int Dlist_Init(Tcl_Interp* interp)
{
DOTRACE("Dlist_Init");

  Tcl::TclItemPkg* pkg = new Tcl::TclItemPkg(interp, "Dlist", "$Revision$");

  pkg->def( &DlistTcl::choose, "::dlist_choose", "source_list index_list" );
  pkg->def( &DlistTcl::not_, "::dlist_not", "source_list" );
  pkg->def( &DlistTcl::ones, "::dlist_ones", "num_ones" );
  pkg->def( &DlistTcl::pickone, "::dlist_pickone", "source_list" );
  pkg->def( &DlistTcl::range, "::dlist_range", "begin end" );
  pkg->def( &DlistTcl::repeat, "::dlist_repeat", "source_list times_list" );
  pkg->def( &DlistTcl::select, "::dlist_select", "source_list flags_list" );
  pkg->def( &DlistTcl::sum, "::dlist_sum", "source_list" );
  pkg->def( &DlistTcl::zeros, "::dlist_zeros", "num_zeros" );

  return pkg->initStatus();
}

static const char vcid_dlisttcl_cc[] = "$Header$";
#endif // !DLISTTCL_CC_DEFINED
