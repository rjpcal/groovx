///////////////////////////////////////////////////////////////////////
//
// tclcmd.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 14:50:58 1999
// written: Tue Jun 19 15:14:13 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCMD_CC_DEFINED
#define TCLCMD_CC_DEFINED

#include "tcl/tclcmd.h"

#include "system/demangle.h"

#include "tcl/errmsg.h"

#include "util/strings.h"

#include <tcl.h>
#include <exception>
#include <typeinfo>

#define TRACE_USE_COUNT

#ifdef TRACE_USE_COUNT
#  include <fstream.h>
#endif

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

extern Tcl_ObjType   tclBooleanType;
extern Tcl_ObjType   tclDoubleType;
extern Tcl_ObjType   tclIntType;
extern Tcl_ObjType   tclListType;
extern Tcl_ObjType   tclStringType;

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

class HelpCmd : public Tcl::TclCmd {
public:
  HelpCmd(Tcl_Interp* interp) :
    Tcl::TclCmd(interp, "?", "commandName", 2, 2) {}

protected:
  virtual void invoke()
  {
    fixed_string cmd_name(getCstringFromArg(1));
    Tcl_CmdInfo cmd_info;
    int result = Tcl_GetCommandInfo(interp(), cmd_name.data(), &cmd_info);
    if (result != 1)
      throw ErrorWithMsg("no such command");

    Tcl::TclCmd* cmd = dynamic_cast<Tcl::TclCmd*>(
                        static_cast<Tcl::TclCmd*>(cmd_info.objClientData));

    if (!cmd)
      throw ErrorWithMsg("no such TclCmd");

    dynamic_string cmd_usage(cmd_name.c_str());
    cmd_usage.append(" ").append(cmd->usage());
    returnCstring(cmd_usage.c_str());
  }
};

namespace {
  Tcl_Obj* nullObject () {
    static Tcl_Obj* obj = 0;
    if (obj == 0) {
      obj = Tcl_NewObj();
      Tcl_IncrRefCount(obj);
    }
    return obj;
  }

  void dummyDeleteProc(ClientData clientData) {
    Tcl::TclCmd* cmd = static_cast<Tcl::TclCmd*>(clientData);
    delete cmd;
  }

  inline Tcl_Obj* checkSharing(Tcl_Obj* obj, Tcl_ObjType* target_type)
    {
      if ( (obj->typePtr != target_type) && Tcl_IsShared(obj) )
        return Tcl_DuplicateObj(obj);
      return obj;
    }

#ifdef TRACE_USE_COUNT
  STD_IO::ofstream* USE_COUNT_STREAM = new STD_IO::ofstream("tclprof.out");
#endif

  HelpCmd* helpCmd = 0;
  bool firstTime = true;
}

///////////////////////////////////////////////////////////////////////
//
// TclCmd::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class Tcl::TclCmd::Impl {
public:
  Impl(const char* cmd_name) : itsCmdName(cmd_name), itsUseCount(0) {}
  ~Impl()
    {
#ifdef TRACE_USE_COUNT
      if (USE_COUNT_STREAM->good())
        {
          *USE_COUNT_STREAM << itsCmdName << " "
                            << itsUseCount << STD_IO::endl;
        }
#endif
    }

  fixed_string itsCmdName;
  int itsUseCount;
};

///////////////////////////////////////////////////////////////////////
//
// TclCmd member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::TclCmd::~TclCmd() {
DOTRACE("Tcl::TclCmd::~TclCmd");
  delete itsImpl;
}

Tcl::TclCmd::TclCmd(Tcl_Interp* interp, const char* cmd_name, const char* usage,
               int objc_min, int objc_max, bool exact_objc) :
  itsUsage(usage),
  itsObjcMin(objc_min),
  itsObjcMax( (objc_max > 0) ? objc_max : objc_min),
  itsExactObjc(exact_objc),
  itsInterp(0),
  itsObjc(0),
  itsObjv(0),
  itsImpl(new Impl(cmd_name)),
  itsResult(TCL_OK)
{
DOTRACE("Tcl::TclCmd::TclCmd");
  Tcl_CreateObjCommand(interp,
                       const_cast<char *>(cmd_name),
                       invokeCallback,
                       static_cast<ClientData> (this),
                       (Tcl_CmdDeleteProc*) NULL);

  if (firstTime)
    {
      firstTime = false;
      helpCmd = new HelpCmd(interp);
    }
}

const char* Tcl::TclCmd::usage() {
DOTRACE("Tcl::TclCmd::usage");
  return itsUsage;
}

void Tcl::TclCmd::errorMessage(const char* msg) {
DOTRACE("Tcl::TclCmd::errorMessage");
  Tcl::err_message(itsInterp, itsObjv, msg);
}


Tcl::TclValue Tcl::TclCmd::arg(int argn) {
DOTRACE("Tcl::TclCmd::arg");
  if (argn < 0 || argn > itsObjc) {
    throw TclError("argument number out of range");
  }

  return TclValue(itsInterp, itsObjv[argn]);
}

// Extracting int's

template <>
int Tcl::TclCmd::getValFromObj<int>(
  Tcl_Interp*, Tcl_Obj* obj, int* /*dummy*/
) {
DOTRACE("Tcl::TclCmd::getValFromObj<int>");
  int val;

  obj = checkSharing(obj, &tclIntType);

  if ( Tcl_GetIntFromObj(0, obj, &val) != TCL_OK )
    {
      TclError err("expected integer but got ");
      err.appendMsg("\"", Tcl_GetString(obj), "\"");
      throw err;
    }
  return val;
}

int Tcl::TclCmd::getIntFromArg(int argn) {
  return getValFromObj(itsInterp, itsObjv[argn], (int*)0);
}

template <>
unsigned int Tcl::TclCmd::getValFromObj<unsigned int>(
  Tcl_Interp*, Tcl_Obj* obj, unsigned int* /*dummy*/
) {
  return (unsigned int)getValFromObj(0,obj,(int*)0);
}

// Extracting long's

template <>
long Tcl::TclCmd::getValFromObj<long>(
  Tcl_Interp*, Tcl_Obj* obj, long* /*dummy*/
) {
DOTRACE("Tcl::TclCmd::getValFromObj<long>");
  long val;

  obj = checkSharing(obj, &tclIntType);

  if ( Tcl_GetLongFromObj(0, obj, &val) != TCL_OK )
    {
      TclError err("expected long value but got ");
      err.appendMsg("\"", Tcl_GetString(obj), "\"");
      throw err;
    }
  return val;
}

long Tcl::TclCmd::getLongFromArg(int argn) {
  return getValFromObj(itsInterp, itsObjv[argn], (long*)0);
}

template <>
unsigned long Tcl::TclCmd::getValFromObj<unsigned long>(
  Tcl_Interp*, Tcl_Obj* obj, unsigned long* /*dummy*/
) {
  return (unsigned long)getValFromObj(0,obj,(long*)0);
}

// Extracting bool's

template <>
bool Tcl::TclCmd::getValFromObj<bool>(
  Tcl_Interp*, Tcl_Obj* obj, bool* /*dummy*/
) {
DOTRACE("Tcl::TclCmd::getValFromObj<bool>");
  int int_val;

  obj = checkSharing(obj, &tclBooleanType);

  if ( Tcl_GetBooleanFromObj(0, obj, &int_val) != TCL_OK )
    {
      TclError err("expected boolean value but got ");
      err.appendMsg("\"", Tcl_GetString(obj), "\"");
      throw err;
    }
  return bool(int_val);
}

bool Tcl::TclCmd::getBoolFromArg(int argn) {
  return getValFromObj(itsInterp, itsObjv[argn], (bool*)0);
}

// Extracting double's

template <>
double Tcl::TclCmd::getValFromObj<double>(
  Tcl_Interp*, Tcl_Obj* obj, double* /*dummy*/
) {
DOTRACE("Tcl::TclCmd::getValFromObj<double>");
  double val;

  obj = checkSharing(obj, &tclDoubleType);

  if ( Tcl_GetDoubleFromObj(0, obj, &val) != TCL_OK )
    {
      TclError err("expected floating-point number but got ");
      err.appendMsg("\"", Tcl_GetString(obj), "\"");
      throw err;
    }
  return val;
}

template <>
float Tcl::TclCmd::getValFromObj<float>(
  Tcl_Interp*, Tcl_Obj* obj, float* /*dummy*/
) {
  return float(getValFromObj(0, obj, (double*)0));
}

double Tcl::TclCmd::getDoubleFromArg(int argn) {
  return getValFromObj(itsInterp, itsObjv[argn], (double*)0);
}

// Extracting const char*'s

template <>
const char* Tcl::TclCmd::getValFromObj<const char*>(
  Tcl_Interp*, Tcl_Obj* obj, const char** /*dummy*/
) {
DOTRACE("Tcl::TclCmd::getValFromObj<const char*>");
  return Tcl_GetString(obj);
}

const char* Tcl::TclCmd::getCstringFromArg(int argn) {
DOTRACE("Tcl::TclCmd::getCstringFromArg");
  return Tcl_GetString(itsObjv[argn]);
}

// Extracting TclValue's

template <>
Tcl::TclValue Tcl::TclCmd::getValFromObj<Tcl::TclValue>(
  Tcl_Interp* interp, Tcl_Obj* obj, TclValue* /*dummy*/
) {
DOTRACE("Tcl::TclCmd::getValFromObj<TclValue>");
  return TclValue(interp, obj);
}

Tcl_Obj* Tcl::TclCmd::getTclObjFromArg(int argn) {
DOTRACE("Tcl::TclCmd::getTclObjFromArg");
  return itsObjv[argn];
}

void Tcl::TclCmd::safeSplitList(Tcl_Obj* obj, int* count_return,
                                Tcl_Obj*** elements_return) {
DOTRACE("Tcl::TclCmd::safeSplitList");
  if ( Tcl_ListObjGetElements(0, obj, count_return, elements_return)
       != TCL_OK)
    {
      throw TclError("couldn't split Tcl list");
    }
}

unsigned int Tcl::TclCmd::safeListLength(Tcl_Obj* obj) {
DOTRACE("Tcl::TclCmd::safeListLength");
  int length;
  if ( Tcl_ListObjLength(0, obj, &length) != TCL_OK)
    {
      throw TclError("couldn't get length of Tcl list");
    }
  Postcondition(length >= 0);
  return (unsigned int) length;
}

void Tcl::TclCmd::returnVoid() {
DOTRACE("Tcl::TclCmd::returnVoid");
  Tcl_ResetResult(itsInterp);
  itsResult = TCL_OK;
}

void Tcl::TclCmd::returnError() {
DOTRACE("Tcl::TclCmd::returnError");
  itsResult = TCL_ERROR;
}

void Tcl::TclCmd::returnInt(int val) {
DOTRACE("Tcl::TclCmd::returnInt");
  Tcl_SetObjResult(itsInterp, Tcl_NewIntObj(val));
  itsResult = TCL_OK;
}

void Tcl::TclCmd::returnLong(long val) {
DOTRACE("Tcl::TclCmd::returnLong");
  Tcl_SetObjResult(itsInterp, Tcl_NewLongObj(val));
  itsResult = TCL_OK;
}

void Tcl::TclCmd::returnBool(bool val) {
DOTRACE("Tcl::TclCmd::returnBool");
  Tcl_SetObjResult(itsInterp, Tcl_NewBooleanObj(val));
  itsResult = TCL_OK;
}

void Tcl::TclCmd::returnDouble(double val) {
DOTRACE("Tcl::TclCmd::returnDouble");
  Tcl_SetObjResult(itsInterp, Tcl_NewDoubleObj(val));
  itsResult = TCL_OK;
}

void Tcl::TclCmd::returnCstring(const char* val) {
DOTRACE("Tcl::TclCmd::returnCstring");
  Tcl_SetObjResult(itsInterp, Tcl_NewStringObj(val, -1));
  itsResult = TCL_OK;
}

void Tcl::TclCmd::returnValue(const Value& val) {
DOTRACE("Tcl::TclCmd::returnValue");
  TclValue return_val(itsInterp, val);
  Tcl_SetObjResult(itsInterp, return_val.getObj());
  itsResult = TCL_OK;
};

void Tcl::TclCmd::returnTclObj(Tcl_Obj* obj) {
DOTRACE("Tcl::TclCmd::returnTclObj");
  Tcl_SetObjResult(itsInterp, obj);
  itsResult = TCL_OK;
}

void Tcl::TclCmd::lappendValue(const Value& val) {
DOTRACE("Tcl::TclCmd::lappendValue");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp);
  TclValue list_element(itsInterp, val);
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result,
                                            list_element.getObj());
  if (cmd_result != TCL_OK) throw TclError();
}

void Tcl::TclCmd::lappendInt(int val) {
DOTRACE("Tcl::TclCmd::lappendInt");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp);
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result,
                                            Tcl_NewIntObj(val));
  if (cmd_result != TCL_OK) throw TclError();
}

void Tcl::TclCmd::lappendLong(long val) {
DOTRACE("Tcl::TclCmd::lappendLong");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp);
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result,
                                            Tcl_NewLongObj(val));
  if (cmd_result != TCL_OK) throw TclError();
}

void Tcl::TclCmd::lappendBool(bool val) {
DOTRACE("Tcl::TclCmd::lappendBool");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp);
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result,
                                            Tcl_NewBooleanObj(val));
  if (cmd_result != TCL_OK) throw TclError();
}

void Tcl::TclCmd::lappendDouble(double val) {
DOTRACE("Tcl::TclCmd::lappendDouble");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp);
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result,
                                            Tcl_NewDoubleObj(val));
  if (cmd_result != TCL_OK) throw TclError();
}

void Tcl::TclCmd::lappendCstring(const char* val) {
DOTRACE("Tcl::TclCmd::lappendCstring");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp);
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result,
                                            Tcl_NewStringObj(val, -1));
  if (cmd_result != TCL_OK) throw TclError();
}

int Tcl::TclCmd::invokeCallback(ClientData clientData, Tcl_Interp* interp,
                                int objc, Tcl_Obj *const objv[]) {
DOTRACE("Tcl::TclCmd::invokeCallback");

  TclCmd* theCmd = static_cast<TclCmd*>(clientData);

  DebugEval((void *) theCmd);
  DebugEvalNL(typeid(*theCmd).name());

  ++(theCmd->itsImpl->itsUseCount);

  theCmd->itsInterp = interp;
  theCmd->itsObjc = objc;
  theCmd->itsObjv = objv;
  theCmd->itsResult = TCL_OK;

  DebugEval(objc);
  DebugEval(theCmd->itsObjcMin);
  DebugEvalNL(theCmd->itsObjcMax);

  // Check for bad argument count, if so abort the command and return
  // TCL_ERROR...
  if ( ((theCmd->itsExactObjc == true) &&
        (objc != theCmd->itsObjcMin && objc != theCmd->itsObjcMax))
       ||
       ((theCmd->itsExactObjc == false) &&
        (objc < theCmd->itsObjcMin || objc > theCmd->itsObjcMax)) ) {

    Tcl_WrongNumArgs(theCmd->itsInterp, 1, theCmd->itsObjv,
                     const_cast<char*>(theCmd->itsUsage));
    theCmd->itsResult = TCL_ERROR;
    return TCL_ERROR;
  }
  // ...otherwise if the argument count is OK, try the command and
  // catch all possible exceptions
  try {
    theCmd->invoke();
  }
  catch (TclError& err) {
    DebugPrintNL("catch (TclError&)");
    if ( !string_literal(err.msg_cstr()).empty() ) {
      Tcl::err_message(interp, theCmd->itsObjv, err.msg_cstr());
    }
    theCmd->itsResult = TCL_ERROR;
  }
  catch (ErrorWithMsg& err) {
    DebugPrintNL("catch (ErrorWithMsg&)");
    if ( !string_literal(err.msg_cstr()).empty() ) {
      Tcl::err_message(interp, theCmd->itsObjv, err.msg_cstr());
    }
    else {
      dynamic_string msg = "an error of type ";
      msg += demangle_cstr(typeid(err).name());
      msg += " occurred";
      Tcl::err_message(interp, theCmd->itsObjv, msg.c_str());
    }
    theCmd->itsResult = TCL_ERROR;
  }
  catch (Error& err) {
    DebugPrintNL("catch (Error&)");
    dynamic_string msg = "an error of type ";
    msg += demangle_cstr(typeid(err).name());
    msg += " occurred";
    Tcl::err_message(interp, theCmd->itsObjv, msg.c_str());
    theCmd->itsResult = TCL_ERROR;
  }
  catch (std::exception& err) {
    dynamic_string msg = "an error of type ";
    msg += demangle_cstr(typeid(err).name());
    msg += " occurred: ";
    msg += err.what();
    Tcl::err_message(interp, theCmd->itsObjv, msg.c_str());
    theCmd->itsResult = TCL_ERROR;
  }
  catch (const char* text) {
    dynamic_string msg = "an error occurred: ";
    msg += text;
    Tcl::err_message(interp, theCmd->itsObjv, msg.c_str());
    theCmd->itsResult = TCL_ERROR;
  }
  catch (...) {
    DebugPrintNL("catch (...)");
    Tcl::err_message(interp, theCmd->itsObjv, "an error of unknown type occurred");
    theCmd->itsResult = TCL_ERROR;
  }


  // cleanup
  theCmd->itsObjc = 0;

  DebugEvalNL(theCmd->itsResult == TCL_OK);
  return theCmd->itsResult;
}


///////////////////////////////////////////////////////////////////////
//
// Tcl::ListIterator member definitions
//
///////////////////////////////////////////////////////////////////////


Tcl::ListIteratorBase::ListIteratorBase(Tcl_Interp* interp,
                                        Tcl_Obj* aList, Pos pos) :
  itsInterp(interp),
  itsList(aList),
  itsListElements(0),
  itsElementCount(0),
  itsIndex(0)
{
DOTRACE("Tcl::ListIteratorBase::ListIteratorBase");
  if (itsList == 0)
    throw TclError("attempted to construct ListIterator with null Tcl_Obj*");

  Tcl_IncrRefCount(itsList);
  int count;
  TclCmd::safeSplitList(itsList, &count, &itsListElements);

  Assert(count >= 0);
  itsElementCount = (unsigned int) count;

  if (pos == END)
    itsIndex = itsElementCount;
}

Tcl::ListIteratorBase::ListIteratorBase(const ListIteratorBase& other) :
  itsInterp(other.itsInterp),
  itsList(other.itsList),
  itsListElements(other.itsListElements),
  itsElementCount(other.itsElementCount),
  itsIndex(other.itsIndex)
{
DOTRACE("Tcl::ListIteratorBase::ListIteratorBase(copy ctor)");
  Tcl_IncrRefCount(itsList);
}

Tcl::ListIteratorBase::~ListIteratorBase()
{
DOTRACE("Tcl::ListIteratorBase::~ListIteratorBase");
  Tcl_DecrRefCount(itsList);
}

Tcl::ListIteratorBase& Tcl::ListIteratorBase::operator=(
  const ListIteratorBase& other)
{
DOTRACE("Tcl::ListIterator::operator=");
  ListIteratorBase other_copy(other);
  this->swap(other_copy);
  return *this;
}

namespace {
  template <class T>
    inline void local_swap(T& a, T& b)
    {
      T b_copy = b;
      b = a;
      a = b_copy;
    }
}

void Tcl::ListIteratorBase::swap(ListIteratorBase& other)
{
DOTRACE("Tcl::ListIteratorBase::swap");
  local_swap(itsInterp, other.itsInterp);
  local_swap(itsList, other.itsList);
  local_swap(itsListElements, other.itsListElements);
  local_swap(itsElementCount, other.itsElementCount);
  local_swap(itsIndex, other.itsIndex);
}

template <class T>
T Tcl::ListIterator<T>::operator*() const
{
DOTRACE("Tcl::ListIterator::operator*");
  if (itsIndex >= itsElementCount)
    throw TclError("index is too larg");

  DebugEval((void*)itsInterp); DebugEval(itsIndex);
  DebugEvalNL(itsListElements[itsIndex]);

  return TclCmd::getValFromObj(itsInterp, itsListElements[itsIndex], (T*)0);
}

// Explicit instantiation requests
namespace Tcl {
  template class ListIterator<bool>;
  template class ListIterator<int>;
  template class ListIterator<unsigned int>;
  template class ListIterator<long>;
  template class ListIterator<unsigned long>;
  template class ListIterator<double>;
  template class ListIterator<const char*>;
  template class ListIterator<TclValue>;
}

static const char vcid_tclcmd_cc[] = "$Header$";
#endif // !TCLCMD_CC_DEFINED
