///////////////////////////////////////////////////////////////////////
//
// dlisttcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Dec-98
// written: Thu Jul 12 13:23:43 2001
// $Id$
//
// This package provides additional Tcl list manipulation functions
//
///////////////////////////////////////////////////////////////////////

#ifndef DLISTTCL_CC_DEFINED
#define DLISTTCL_CC_DEFINED

#include "tcl/tclcmd.h"
#include "tcl/tclerror.h"
#include "tcl/tclpkg.h"

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
  class ChooseCmd;
  class NotCmd;
  class OnesCmd;
  class PickoneCmd;
  class RangeCmd;
  class RepeatCmd;
  class SelectCmd;
  class SumCmd;
  class ZerosCmd;
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

class DlistTcl::ChooseCmd : public Tcl::TclCmd {
public:
  ChooseCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, "source_list index_list", 3) {}

protected:
  virtual void invoke(Tcl::Context& ctx)
    {
      Tcl::List source_list = ctx.getValFromArg(1, (Tcl::List*)0);
      Tcl::List index_list = ctx.getValFromArg(2, (Tcl::List*)0);

      Tcl::List result;

      for (unsigned int i = 0; i < index_list.length(); ++i)
        {
          unsigned int index = index_list.get<unsigned int>(i);

          // use that int as an index into source list, getting the
          // corresponding list element and appending it to the output list
          result.append(source_list.at(index));
        }

      ctx.setResult(result);
    }
};

//--------------------------------------------------------------------
//
// This command takes as its argument a single list containing only
// integers, and returns a list in which each element is the logical
// negation of its corresponding element in the source list.
//
//--------------------------------------------------------------------

class DlistTcl::NotCmd : public Tcl::TclCmd {
public:
  NotCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, "source_list", 2) {}

protected:
  virtual void invoke(Tcl::Context& ctx)
    {
      Tcl::List source_list = ctx.getValFromArg(1, (Tcl::List*)0);

      Tcl::ObjPtr one(Tcl::toTcl<int>(1));
      Tcl::ObjPtr zero(Tcl::toTcl<int>(0));
      Tcl::List result;

      for (unsigned int i = 0; i < source_list.length(); ++i)
        {
          if ( source_list.get<int>(i) == 0 )
            result.append(one);
          else
            result.append(zero);
        }

      ctx.setResult(result);
    }
};

//--------------------------------------------------------------------
//
// this command produces a list of ones of the length specified by its
// lone argument
//
//--------------------------------------------------------------------

class DlistTcl::OnesCmd : public Tcl::TclCmd {
public:
  OnesCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, "num_ones", 2) {}

protected:
  virtual void invoke(Tcl::Context& ctx)
    {
      unsigned int num_ones = ctx.getValFromArg(1, (unsigned int*)0);

      Tcl::List result;
      result.append(1, num_ones);

      ctx.setResult(result);
    }
};

//--------------------------------------------------------------------
//
// This commmand returns a single element chosen at random
// from the source list
//
//--------------------------------------------------------------------

class DlistTcl::PickoneCmd : public Tcl::TclCmd {
public:
  PickoneCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, "source_list", 2) {}

protected:
  virtual void invoke(Tcl::Context& ctx)
    {
      Tcl::List source_list = ctx.getValFromArg(1, (Tcl::List*)0);

      if (source_list.length() == 0)
        {
          throw Tcl::TclError("source_list is empty");
        }

      // pick a random int between 0 and (src_len-1), inclusive
      unsigned int randnum = (unsigned int)
        ( double(rand())/(double(RAND_MAX)+1.0) * source_list.length() );

      ctx.setResult(source_list.at(randnum));
    }
};

//--------------------------------------------------------------------
//
// this command produces an ordered list of all integers between begin
// and end, inclusive.
//
//--------------------------------------------------------------------

class DlistTcl::RangeCmd : public Tcl::TclCmd {
public:
  RangeCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, "begin end", 3) {}

protected:
  virtual void invoke(Tcl::Context& ctx)
    {
      int begin = ctx.getValFromArg(1, (int*)0);
      int end = ctx.getValFromArg(2, (int*)0);

      Tcl::List result;

      for (int i = begin; i <= end; ++i)
        {
          result.append(i);
        }

      ctx.setResult(result);
    }
};

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

class DlistTcl::RepeatCmd : public Tcl::TclCmd {
public:
  RepeatCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, "source_list times_list", 3) {}

protected:
  virtual void invoke(Tcl::Context& ctx)
    {
      Tcl::List source_list = ctx.getValFromArg(1, (Tcl::List*)0);
      Tcl::List times_list = ctx.getValFromArg(2, (Tcl::List*)0);

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

      ctx.setResult(result);
    }
};

//--------------------------------------------------------------------
//
// This command takes two lists as arguments, using the binary flags
// in the second (flags) list to choose which elements from the first
// (source) list should be appended to the output list
//
//--------------------------------------------------------------------

class DlistTcl::SelectCmd : public Tcl::TclCmd {
public:
  SelectCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, "source_list flags_list", 3) {}

protected:
  virtual void invoke(Tcl::Context& ctx)
    {
      Tcl::List source_list = ctx.getValFromArg(1, (Tcl::List*)0);
      Tcl::List flags_list = ctx.getValFromArg(2, (Tcl::List*)0);
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

      ctx.setResult(result);
    }
};

//--------------------------------------------------------------------
//
// this command sums the numbers in a list, trying to return an int
// result if possible, but returning a double result if any doubles
// are found in the source list
//
//--------------------------------------------------------------------

class DlistTcl::SumCmd : public Tcl::TclCmd {
public:
  SumCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, "source_list", 2) {}

protected:
  virtual void invoke(Tcl::Context& ctx)
    {
      Tcl::List source_list = ctx.getValFromArg(1, (Tcl::List*)0);

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
        ctx.setResult( isum );
      else
        ctx.setResult( dsum );
    }
};

//--------------------------------------------------------------------
//
// this command produces a list of zeros of the length specified by its
// lone argument
//
//--------------------------------------------------------------------

class DlistTcl::ZerosCmd : public Tcl::TclCmd {
public:
  ZerosCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, "num_zeros", 2) {}

protected:
  virtual void invoke(Tcl::Context& ctx)
    {
      unsigned int num_zeros = ctx.getValFromArg(1, (unsigned int*)0);

      Tcl::List result;
      result.append(0, num_zeros);

      ctx.setResult(result);
    }
};

//--------------------------------------------------------------------
//
// package initialization procedure
//
//--------------------------------------------------------------------

extern "C"
int Dlist_Init(Tcl_Interp* interp) {
DOTRACE("Dlist_Init");

  Tcl::TclPkg* pkg = new Tcl::TclPkg(interp, "Dlist", "$Revision$");

  pkg->addCommand( new DlistTcl::ChooseCmd(interp, "dlist_choose") );
  pkg->addCommand( new DlistTcl::OnesCmd(interp, "dlist_ones") );
  pkg->addCommand( new DlistTcl::ZerosCmd(interp, "dlist_zeros") );
  pkg->addCommand( new DlistTcl::SumCmd(interp, "dlist_sum") );
  pkg->addCommand( new DlistTcl::RangeCmd(interp, "dlist_range") );
  pkg->addCommand( new DlistTcl::RepeatCmd(interp, "dlist_repeat") );
  pkg->addCommand( new DlistTcl::SelectCmd(interp, "dlist_select") );
  pkg->addCommand( new DlistTcl::NotCmd(interp, "dlist_not") );
  pkg->addCommand( new DlistTcl::PickoneCmd(interp, "dlist_pickone") );

  return pkg->initStatus();
}

static const char vcid_dlisttcl_cc[] = "$Header$";
#endif // !DLISTTCL_CC_DEFINED
