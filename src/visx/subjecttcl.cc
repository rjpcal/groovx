///////////////////////////////////////////////////////////////////////
// subjecttcl.cc
// Rob Peters 
// created: Jan-99
// written: Sun Jun 20 18:11:58 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef SUBJECTTCL_CC_DEFINED
#define SUBJECTTCL_CC_DEFINED

#include "subjecttcl.h"

#include <tcl.h>
#include <cstring>

#include "errmsg.h"
#include "subject.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Subject Tcl package
///////////////////////////////////////////////////////////////////////

namespace SubjectTcl {
  // utility function to extract a subject from a hash table given a
  // key string stored in an object in a Tcl_Obj array
  int getSubjectFromObj(Tcl_HashTable* tblPtr, Tcl_Interp* interp,
                        int objnum, Tcl_Obj* const objv[],
                        Subject*& theSubject);

  // the main Tcl_CmdProc which calls the subcommands below
  Tcl_ObjCmdProc subjectCmd;

  // the subcommands called by subjectCmd
  typedef int (SubjectTcl_SubcmdProc) 
    (Tcl_HashTable* tblPtr, Tcl_Interp* interp, 
     int objc, Tcl_Obj* const objv[]);

  SubjectTcl_SubcmdProc newSubjectCmd;
  SubjectTcl_SubcmdProc subjectNameCmd;
  SubjectTcl_SubcmdProc subjectDirCmd;
  SubjectTcl_SubcmdProc listSubjectsCmd;
  SubjectTcl_SubcmdProc clearCmd;

  const char* const no_hash_msg = "no hash table exists";
  const char* const subject_exists_msg = "subject already exists";
  const char* const no_subject_msg = "no such subject exists";
  const char* const bad_command = "invalid command";

  const char* const default_dir = "./";
}

int SubjectTcl::getSubjectFromObj(Tcl_HashTable* tblPtr, Tcl_Interp* interp,
                                  int objnum, Tcl_Obj* const objv[],
                                  Subject*& theSubject) {
DOTRACE("SubjectTcl::getSubjectFromObj");
  char* subjectKey = Tcl_GetString(objv[objnum]);
  Assert(subjectKey);

  Tcl_HashEntry* theEntry = Tcl_FindHashEntry(tblPtr, subjectKey);
  if (theEntry == NULL) {
    err_message(interp, objv, no_subject_msg);
    return TCL_ERROR;
  }
  
  theSubject = static_cast<Subject *>(Tcl_GetHashValue(theEntry));
  if (theSubject == NULL) {
    err_message(interp, objv, no_subject_msg);
    return TCL_ERROR;
  }
  
  return TCL_OK;
}

int SubjectTcl::subjectCmd(ClientData clientData, Tcl_Interp* interp,
                           int objc, Tcl_Obj* const objv[]) {
DOTRACE("SubjectTcl::subjectCmd");
  Tcl_HashTable* tblPtr = static_cast<Tcl_HashTable *>(clientData);
  
  if (tblPtr == NULL) {
    err_message(interp, objv, no_hash_msg);
    return TCL_ERROR;
  }

  if (objc == 1) {
    Tcl_WrongNumArgs(interp, 1, objv, "subcommand ?args...?");
    return TCL_ERROR;
  }
  else if (objc >= 2) {         // subcommand
    char* cmd = Tcl_GetString(objv[1]);

    if ( strcmp(cmd, "new") == 0 ) {
      return newSubjectCmd(tblPtr, interp, objc, objv);
    }
    if ( strcmp(cmd, "name") == 0 ) {
      return subjectNameCmd(tblPtr, interp, objc, objv);
    }
    if ( strcmp(cmd, "dir") == 0 ) {
      return subjectDirCmd(tblPtr, interp, objc, objv);
    }
    if ( strcmp(cmd, "list") == 0 ) {
      return listSubjectsCmd(tblPtr, interp, objc, objv);
    }
    if ( strcmp(cmd, "clear") == 0 ) {
      return clearCmd(tblPtr, interp, objc, objv);
    }
    else {
      err_message(interp, objv, bad_command);
      return TCL_ERROR;
    }
  }
  return TCL_OK;
}

int SubjectTcl::newSubjectCmd(Tcl_HashTable* tblPtr, Tcl_Interp* interp,
                              int objc, Tcl_Obj* const objv[]) {
DOTRACE("SubjectTcl::newSubjectCmd");
  if (objc < 3 || objc > 5) {
    Tcl_WrongNumArgs(interp, 2, objv, "subjectKey ?subjectName? ?subjectDir?");
    return TCL_ERROR;
  }

  const char* subjectKey = Tcl_GetString(objv[2]);
  Assert(subjectKey);
  const char* subjectName = (objc > 2) ? Tcl_GetString(objv[3]) : subjectKey;
  const char* subjectDir =  (objc > 3) ? Tcl_GetString(objv[4]) : default_dir;

  DebugEval(tblPtr);
  DebugEvalNL(subjectKey);

  int isNew;
  Tcl_HashEntry* newEntry = Tcl_CreateHashEntry(tblPtr, subjectKey, &isNew);
  if (isNew == 0) {
    err_message(interp, objv, subject_exists_msg);
    return TCL_ERROR;
  }
  
  Subject* newSubject = new Subject(subjectName,subjectDir);
  Tcl_SetHashValue(newEntry, static_cast<ClientData>(newSubject));

  return TCL_OK;
}

int SubjectTcl::subjectNameCmd(Tcl_HashTable* tblPtr, Tcl_Interp* interp,
                               int objc, Tcl_Obj* const objv[]) {
DOTRACE("SubjectTcl::subjectNameCmd");
  if (objc < 3 || objc > 4) {
    Tcl_WrongNumArgs(interp, 2, objv, "subjectKey ?subjectName?");
    return TCL_ERROR;
  }

  Subject* theSubject=NULL;
  if (getSubjectFromObj(tblPtr, interp, 2, objv, theSubject) != TCL_OK)
    return TCL_ERROR;

  if (objc > 3) {               // set new name
	 const char* subjectName = Tcl_GetString(objv[3]);
	 Assert(subjectName);
    theSubject->setName(subjectName);
  }
  else {                        // print current name
    if (theSubject->getName() != NULL) 
      Tcl_SetResult(interp,
                    const_cast<char *>(theSubject->getName()),
                    TCL_VOLATILE);
  }

  return TCL_OK;
}

int SubjectTcl::subjectDirCmd(Tcl_HashTable* tblPtr, Tcl_Interp* interp,
                              int objc, Tcl_Obj* const objv[]) {
DOTRACE("SubjectTcl::subjectDirCmd");
  if (objc < 3 || objc > 4) {
    Tcl_WrongNumArgs(interp, 2, objv, "subjectKey ?subjectDir?");
    return TCL_ERROR;
  }

  Subject* theSubject=NULL;
  if (getSubjectFromObj(tblPtr, interp, 2, objv, theSubject) != TCL_OK)
    return TCL_ERROR;

  if (objc > 3) {               // set new directory
	 const char* subjectDir = Tcl_GetString(objv[3]);
	 Assert(subjectDir);
    theSubject->setDirectory(subjectDir);
  }
  else {                        // print current directory
    if (theSubject->getDirectory() != NULL) 
      Tcl_SetResult(interp,
                    const_cast<char *>(theSubject->getDirectory()),
                    TCL_VOLATILE);
  }

  return TCL_OK;
}

int SubjectTcl::listSubjectsCmd(Tcl_HashTable* tblPtr, Tcl_Interp* interp,
                                int objc, Tcl_Obj* const objv[]) {
DOTRACE("SubjectTcl::listSubjectsCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 2, objv, NULL);
    return TCL_ERROR;
  }

  Tcl_HashSearch search;

  for (Tcl_HashEntry* theEntry = Tcl_FirstHashEntry(tblPtr, &search); 
       theEntry != NULL;
       theEntry = Tcl_NextHashEntry(&search)) {
	 Subject* theSubject = (Subject *) Tcl_GetHashValue(theEntry);
	 const char* theKey = Tcl_GetHashKey(tblPtr, theEntry);
    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
                           theKey, "\t",
                           theSubject->getName(), "\t",
                           theSubject->getDirectory(), "\n", (char *) NULL);
  }

  return TCL_OK;
}

int SubjectTcl::clearCmd(Tcl_HashTable* tblPtr, Tcl_Interp* interp,
								 int objc, Tcl_Obj* const objv[]) {
DOTRACE("SubjectTcl::clearCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 2, objv, NULL);
    return TCL_ERROR;
  }

  Tcl_HashSearch search;

  for (Tcl_HashEntry* theEntry = Tcl_FirstHashEntry(tblPtr, &search); 
       theEntry != NULL;
       theEntry = Tcl_NextHashEntry(&search)) {
	 Subject* theSubject = (Subject *) Tcl_GetHashValue(theEntry);
	 delete theSubject;
	 Tcl_DeleteHashEntry(theEntry);
  }

  return TCL_OK;
}

int Subject_Init(Tcl_Interp* interp) {
DOTRACE("Subject_Init");

  using namespace SubjectTcl;

  Tcl_HashTable* subjectTablePtr = new Tcl_HashTable;
  Tcl_InitHashTable(subjectTablePtr, TCL_STRING_KEYS);

  DebugEvalNL(subjectTablePtr);

  Tcl_CreateObjCommand(interp, "subject", subjectCmd,
                       (ClientData) subjectTablePtr, (Tcl_CmdDeleteProc *) NULL);
  Tcl_PkgProvide(interp, "Subject", "1.6");
  return TCL_OK;
}

static const char vcid_subjecttcl_cc[] = "$Header$";
#endif // !SUBJECTTCL_CC_DEFINED
