///////////////////////////////////////////////////////////////////////
//
// tcldlist.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Dec-98
// written: Thu May 10 12:04:42 2001
// $Id$
//
// This package provides additional list manipulation functions using
// object-style (rather than string-style) Tcl commands
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLDLIST_CC_DEFINED
#define TCLDLIST_CC_DEFINED

#include "tcl/tcldlist.h"

#include <tcl.h>
#include <cstdlib>

#include "tcl/errmsg.h"

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// Tcldlist Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

namespace Tcldlist {
  Tcl_ObjCmdProc dlist_chooseCmd;
  Tcl_ObjCmdProc dlist_notCmd;
  Tcl_ObjCmdProc dlist_onesCmd;
  Tcl_ObjCmdProc dlist_pickoneCmd;
  Tcl_ObjCmdProc dlist_rangeCmd;
  Tcl_ObjCmdProc dlist_repeatCmd;
  Tcl_ObjCmdProc dlist_selectCmd;
  Tcl_ObjCmdProc dlist_sumCmd;
  Tcl_ObjCmdProc dlist_zerosCmd;
}

///////////////////////////////////////////////////////////////////////
//
// Tcldlist Tcl package definitions
//
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
//
// This command takes two lists as arguments, and uses the integers in
// the second (index) list to return a permutation of the elements in
// the first (source) list
//
// usage:
// dlist_choose source_list index_list
//
// example:
//      dlist_choose { 3 5 7 } { 2 0 1 }
// returns 
//      7 3 5
//
//--------------------------------------------------------------------

int Tcldlist::dlist_chooseCmd(ClientData, Tcl_Interp* interp,
                              int objc, Tcl_Obj* const objv[]) {
DOTRACE("Tcldlist::dlist_chooseCmd");
  if (objc != 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "source_list index_list"); 
    return TCL_ERROR;
  }
  
  int src_len, idx_len;
  if (Tcl_ListObjLength(interp, objv[1], &src_len) != TCL_OK) return TCL_ERROR;
  if (Tcl_ListObjLength(interp, objv[2], &idx_len) != TCL_OK) return TCL_ERROR;

  DebugEval(src_len);
  DebugEvalNL(idx_len);

  // make a new list to hold the result
  Tcl_Obj* list_out = Tcl_NewListObj(0, (Tcl_Obj **) NULL);

  int index;
  Tcl_Obj *idx_elem, *src_elem;

  // loop over elements in the index list
  for (int i = 0; i < idx_len; ++i) {
    // get the idx_list element into idx_elem
    if (Tcl_ListObjIndex(interp, objv[2], i, &idx_elem) != TCL_OK)
      return TCL_ERROR;
    // get an int from idx_elem
    if (Tcl_GetIntFromObj(interp, idx_elem, &index) != TCL_OK)
      return TCL_ERROR;

    DebugEvalNL(index);

	 // Check index for valid value
	 if (index < 0 || index >= src_len) {
		Tcl::err_message(interp, objv, "index out of range");
		return TCL_ERROR;
	 }
    // use that int as an index into source list, getting the
    // corresponding list element and appending it to the output list
	 if (Tcl_ListObjIndex(interp, objv[1], index, &src_elem) != TCL_OK)
		return TCL_ERROR;
	 if (Tcl_ListObjAppendElement(interp, list_out, src_elem) != TCL_OK)
		return TCL_ERROR;
  }
  // make the interpreters result point to the output list
  Tcl_SetObjResult(interp, list_out);
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// This command takes as its argument a single list containing only
// integers, and returns a list in which each element is the logical
// negation of its corresponding element in the source list.
// 
// usage:
// dlist_not source_list
//
//--------------------------------------------------------------------

int Tcldlist::dlist_notCmd(ClientData, Tcl_Interp* interp,
                           int objc, Tcl_Obj* const objv[]) {
DOTRACE("Tcldlist::dlist_notCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "source_list");
    return TCL_ERROR;
  }

  int src_len;
  if (Tcl_ListObjLength(interp, objv[1], &src_len) != TCL_OK) return TCL_ERROR;

  DebugEvalNL(src_len);

  // make a new Tcl list to store the result
  Tcl_Obj* list_out = Tcl_NewListObj(0, (Tcl_Obj **) NULL);
  Tcl_Obj* one_elem = Tcl_NewIntObj(1);  // Tcl object holding (int) 1
  Tcl_Obj* zero_elem = Tcl_NewIntObj(0); // Tcl object holding (int) 0

  int element;
  Tcl_Obj* src_elem;

  // loop over the source list
  for (int i = 0; i < src_len; ++i) {
    // get the next list element and get an int from it
    if (Tcl_ListObjIndex(interp, objv[1], i, &src_elem) != TCL_OK)
      return TCL_ERROR;
    if (Tcl_GetIntFromObj(interp, src_elem, &element) != TCL_OK)
      return TCL_ERROR;
    // if the element is true
    if ( (!element) == 0 )
      Tcl_ListObjAppendElement(interp, list_out, zero_elem); // append 0
    // if the element is false
    else
      Tcl_ListObjAppendElement(interp, list_out, one_elem); // append 1
  }
  // make the interpreter's result point to the output list
  Tcl_SetObjResult(interp, list_out);
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// this command produces a list of ones of the length specified by its
// lone argument
//
// usage:
// dlist_ones num_ones
//
//--------------------------------------------------------------------

int Tcldlist::dlist_onesCmd(ClientData, Tcl_Interp* interp,
                            int objc, Tcl_Obj* const objv[]) {
DOTRACE("Tcldlist::dlist_onesCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "num_ones"); 
    return TCL_ERROR;
  }

  int num_ones;
  if (Tcl_GetIntFromObj(interp, objv[1], &num_ones) != TCL_OK) return TCL_ERROR;

  Tcl_Obj* list_out = Tcl_NewListObj(0, (Tcl_Obj **) NULL);
  Tcl_Obj* one_obj = Tcl_NewIntObj(1);
  for (int i = 0; i < num_ones; ++i) {
    if (Tcl_ListObjAppendElement(interp, list_out, one_obj) != TCL_OK)
      return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, list_out);
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// This commmand returns a single element chosen at random
// from the source list
//
// usage:
// dlist_pickone source_list
//
//--------------------------------------------------------------------

int Tcldlist::dlist_pickoneCmd(ClientData, Tcl_Interp* interp,
                               int objc, Tcl_Obj* const objv[]) {
DOTRACE("Tcldlist::dlist_pickoneCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "source_list"); 
    return TCL_ERROR;
  }

  int src_len;
  if (Tcl_ListObjLength(interp, objv[1], &src_len) != TCL_OK) return TCL_ERROR;
  if (src_len == 0) {
    Tcl::err_message(interp, objv, "source_list is empty");
    return TCL_ERROR;
  }

  // get a random int between 0 and (src_len-1), inclusive
  int randnum = int( double(rand())/(double(RAND_MAX)+1.0) * src_len );

  DebugEval(src_len);
  DebugEvalNL(randnum);

  // get the appropriate list element...
  Tcl_Obj* chosen;
  if (Tcl_ListObjIndex(interp, objv[1], randnum, &chosen) != TCL_OK)
    return TCL_ERROR;

  // and make the interpreter's result point to it
  Tcl_SetObjResult(interp, chosen);
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// this command produces an ordered list of all integers between begin
// and end, inclusive.
//
// usage:
// dlist_range begin end
//
//--------------------------------------------------------------------

int Tcldlist::dlist_rangeCmd(ClientData, Tcl_Interp* interp,
                             int objc, Tcl_Obj* const objv[]) {
DOTRACE("Tcldlist::dlist_rangeCmd");
  if (objc != 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "begin end"); 
    return TCL_ERROR;
  }

  int begin, end;
  if (Tcl_GetIntFromObj(interp, objv[1], &begin) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetIntFromObj(interp, objv[2], &end) != TCL_OK) return TCL_ERROR;

  Tcl_Obj* list_out = Tcl_NewListObj(0, (Tcl_Obj **) NULL);
  for (int i = begin; i <= end; ++i) {
    if (Tcl_ListObjAppendElement(interp, list_out, Tcl_NewIntObj(i)) != TCL_OK)
      return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, list_out);
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// This command taks two lists as arguments. Each element from the
// first (source) list is appended to the result multiple times; the
// number of times is determined by the corresponding integer found in
// the second (times) list.
//
// usage:
// dlist_repeat source_list times_list
//
// For example: 
//      dlist_repeat { 4 5 6 } { 1 2 3 }
// returns
//      4 5 5 6 6 6
//
// Note that times_list cannot be longer than source_list
//
//--------------------------------------------------------------------

int Tcldlist::dlist_repeatCmd(ClientData, Tcl_Interp* interp,
                              int objc, Tcl_Obj* const objv[]) {
DOTRACE("Tcldlist::dlist_repeatCmd");
  if (objc != 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "source_list times_list"); 
    return TCL_ERROR;
  }

  int src_len, tim_len;
  if (Tcl_ListObjLength(interp, objv[1], &src_len) != TCL_OK) return TCL_ERROR;
  if (Tcl_ListObjLength(interp, objv[2], &tim_len) != TCL_OK) return TCL_ERROR;
  
  // find the minimum of the two lists' lengths
  int min_len = (src_len < tim_len) ? src_len : tim_len;

  DebugEval(src_len);
  DebugEval(tim_len);
  DebugEvalNL(min_len);

  Tcl_Obj* list_out = Tcl_NewListObj(0, (Tcl_Obj **) NULL);

  int times;
  Tcl_Obj *src_elem, *tim_elem;
  for (int t = 0; t < min_len; ++t) { // loop over times_list
    // get the next element from times_list into tim_elem
    if (Tcl_ListObjIndex(interp, objv[2], t, &tim_elem) != TCL_OK)
      return TCL_ERROR;
    // get an int from tim_elem to be used as the number of times to repeat
    if (Tcl_GetIntFromObj(interp, tim_elem, &times) != TCL_OK) return TCL_ERROR;
    // get the next element from source_list into src_elem
    if (Tcl_ListObjIndex(interp, objv[1], t, &src_elem) != TCL_OK)
      return TCL_ERROR;
    // append src_elem to the output list
    for (int r = 0; r < times; ++r)
      Tcl_ListObjAppendElement(interp, list_out, src_elem);
  }
  // make the interpreter's result point to the output list
  Tcl_SetObjResult(interp, list_out);
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// This command takes two lists as arguments, using the binary flags
// in the second (flags) list to choose which elements from the first
// (source) list should be appended to the output list
//
// usage:
// dlist_select source_list flags_list
//
//--------------------------------------------------------------------

int Tcldlist::dlist_selectCmd(ClientData, Tcl_Interp* interp,
                              int objc, Tcl_Obj* const objv[]) {
DOTRACE("Tcldlist::dlist_selectCmd");
  if (objc != 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "source_list flags_list"); 
    return TCL_ERROR;
  }

  int src_len, flg_len;
  if (Tcl_ListObjLength(interp, objv[1], &src_len) != TCL_OK) return TCL_ERROR;
  if (Tcl_ListObjLength(interp, objv[2], &flg_len) != TCL_OK) return TCL_ERROR;

  if (flg_len < src_len) {
    Tcl::err_message(interp, objv, "flags list must be as long as source_list");
    return TCL_ERROR;
  }

  DebugEval(src_len);
  DebugEvalNL(flg_len);

  // make a new Tcl list to hold the result
  Tcl_Obj* list_out = Tcl_NewListObj(0, (Tcl_Obj **) NULL);

  int flag;
  Tcl_Obj *src_elem, *flg_elem;

  // loop over the source list
  for (int i = 0; i < src_len; ++i) {
    // get the next element in flag_list and get an int from it
    if (Tcl_ListObjIndex(interp, objv[2], i, &flg_elem) != TCL_OK) 
      return TCL_ERROR;
    if (Tcl_GetIntFromObj(interp, flg_elem, &flag) != TCL_OK) return TCL_ERROR;

    // if the flag is not false, add the corresponding source_list
    // element to the result list
    if ( flag != 0 ) {
      if (Tcl_ListObjIndex(interp, objv[1], i, &src_elem) != TCL_OK)
        return TCL_ERROR;
      if (Tcl_ListObjAppendElement(interp, list_out, src_elem) != TCL_OK)
        return TCL_ERROR;
    }
  }
  // make the interpreters result point to the output list
  Tcl_SetObjResult(interp, list_out);
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// this command sums the numbers in a list, trying to return an int
// result if possible, but returning a double result if any doubles
// are found in the source list
//
// usage:
// dlist_sum source_list
//
//--------------------------------------------------------------------

int Tcldlist::dlist_sumCmd(ClientData, Tcl_Interp* interp,
                           int objc, Tcl_Obj* const objv[]) {
DOTRACE("Tcldlist::dlist_sumCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "source_list");
    return TCL_ERROR;
  }

  // get length of list
  int src_len;
  if (Tcl_ListObjLength(interp, objv[1], &src_len) != TCL_OK) return TCL_ERROR;

  double sum=0.0;
  double next_d=0.0;
  int next_i=0;
  int is_double=0;              // true if any list elements are floating-pt
  Tcl_Obj* src_elem;

  // loop over objects in list
  for (int i = 0; i < src_len; ++i) {
    // put next list element at src_elem
    if (Tcl_ListObjIndex(interp, objv[1], i, &src_elem) != TCL_OK)
      return TCL_ERROR;
    if ( is_double == 0 ) {     // we haven't seen any doubles yet
      // try to get an int
      if (Tcl_GetIntFromObj(interp, src_elem, &next_i) == TCL_OK)
        sum += next_i;
      // if getting an int fails, try to get a double
      else {
        if (Tcl_GetDoubleFromObj(interp, src_elem, &next_d) != TCL_OK)
          return TCL_ERROR;
        is_double = 1;
        sum += next_d;
      }
    }
    else {                      // we've already seen a double
      if (Tcl_GetDoubleFromObj(interp, src_elem, &next_d) != TCL_OK)
        return TCL_ERROR;
      sum += next_d;
    }

    DebugEvalNL(sum);

  }
  if ( is_double == 0 )         // we have an int result
    Tcl_SetObjResult(interp, Tcl_NewIntObj( (int) sum ));
  else                          // we have a doulbe result
    Tcl_SetObjResult(interp, Tcl_NewDoubleObj( sum ));
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// this command produces a list of zeros of the length specified by its
// lone argument
//
// usage:
// dlist_zeros num_zeros
//
//--------------------------------------------------------------------

int Tcldlist::dlist_zerosCmd(ClientData, Tcl_Interp* interp,
                             int objc, Tcl_Obj* const objv[]) {
DOTRACE("Tcldlist::dlist_zerosCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "num_zeros"); 
    return TCL_ERROR;
  }

  int num_ones;
  if (Tcl_GetIntFromObj(interp, objv[1], &num_ones) != TCL_OK) return TCL_ERROR;

  Tcl_Obj* list_out = Tcl_NewListObj(0, (Tcl_Obj **) NULL);
  Tcl_Obj* zero_obj = Tcl_NewIntObj(0);
  for (int i = 0; i < num_ones; ++i) {
    if (Tcl_ListObjAppendElement(interp, list_out, zero_obj) != TCL_OK)
      return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, list_out);
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// package initialization procedure
//
//--------------------------------------------------------------------

int Tcldlist_Init(Tcl_Interp* interp) {
DOTRACE("Tcldlist_Init");
  Tcl_CreateObjCommand(interp, "dlist_choose", Tcldlist::dlist_chooseCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "dlist_ones", Tcldlist::dlist_onesCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "dlist_zeros", Tcldlist::dlist_zerosCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "dlist_sum", Tcldlist::dlist_sumCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "dlist_range", Tcldlist::dlist_rangeCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "dlist_repeat", Tcldlist::dlist_repeatCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "dlist_select", Tcldlist::dlist_selectCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "dlist_not", Tcldlist::dlist_notCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "dlist_pickone", Tcldlist::dlist_pickoneCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  
  Tcl_PkgProvide(interp, "Tcldlist", "1.6");
  return TCL_OK;
}

static const char vcid_tcldlist_cc[] = "$Header$";
#endif // !TCLDLIST_CC_DEFINED
