///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:09:12 1999
// written: Wed Mar 15 10:17:28 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KBDRESPONSEHDLR_CC_DEFINED
#define KBDRESPONSEHDLR_CC_DEFINED

#include "kbdresponsehdlr.h"

#include "util/strings.h"

#include <iostream.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace {
  const string_literal ioTag("KbdResponseHdlr");
}

///////////////////////////////////////////////////////////////////////
//
// KbdResponseHdlr method definitions
//
///////////////////////////////////////////////////////////////////////

KbdResponseHdlr::KbdResponseHdlr() : 
  EventResponseHdlr("")
{
  setEventSequence("<KeyPress>");
  setBindingSubstitution("%K");
}

KbdResponseHdlr::KbdResponseHdlr(const char* key_resp_pairs) : 
  EventResponseHdlr(key_resp_pairs)
{
  setEventSequence("<KeyPress>");
  setBindingSubstitution("%K");
}

void KbdResponseHdlr::serialize(ostream &os, IOFlag flag) const {
DOTRACE("KbdResponseHdlr::serialize");
  if (flag & TYPENAME) { os << ioTag << IO::SEP; }

  oldSerialize(os, flag);

  if (os.fail()) throw OutputError(ioTag.c_str());

  if (flag & BASES) { /* no bases to serialize */ }
}

void KbdResponseHdlr::deserialize(istream &is, IOFlag flag) {
DOTRACE("KbdResponseHdlr::deserialize");

  if (flag & TYPENAME) { IO::readTypename(is, ioTag.c_str()); }

  oldDeserialize(is, flag);

  setEventSequence("<KeyPress");
  setBindingSubstitution("%K");

  if (is.fail()) throw InputError(ioTag.c_str());

  if (flag & BASES) { /* no bases to deserialize */ }
}

int KbdResponseHdlr::charCount() const {
DOTRACE("KbdResponseHdlr::charCount");
  return ioTag.length() + 1 + oldCharCount();
}

static const char vcid_kbdresponsehdlr_cc[] = "$Header$";
#endif // !KBDRESPONSEHDLR_CC_DEFINED
