///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:09:12 1999
// written: Fri Mar  3 15:06:05 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KBDRESPONSEHDLR_CC_DEFINED
#define KBDRESPONSEHDLR_CC_DEFINED

#include "kbdresponsehdlr.h"

#include <string>

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace {
  const string ioTag = "KbdResponseHdlr";
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

KbdResponseHdlr::KbdResponseHdlr(const string& key_resp_pairs) : 
  EventResponseHdlr(key_resp_pairs)
{
  setEventSequence("<KeyPress>");
  setBindingSubstitution("%K");
}

void KbdResponseHdlr::serialize(ostream &os, IOFlag flag) const {
DOTRACE("KbdResponseHdlr::serialize");
  if (flag & TYPENAME) { os << ioTag << IO::SEP; }

  oldSerialize(os, flag);

  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { /* no bases to serialize */ }
}

void KbdResponseHdlr::deserialize(istream &is, IOFlag flag) {
DOTRACE("KbdResponseHdlr::deserialize");

  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  oldDeserialize(is, flag);

  setEventSequence("<KeyPress");
  setBindingSubstitution("%K");

  if (is.fail()) throw InputError(ioTag);

  if (flag & BASES) { /* no bases to deserialize */ }
}

int KbdResponseHdlr::charCount() const {
DOTRACE("KbdResponseHdlr::charCount");
  return ioTag.length() + 1 + oldCharCount();
}

static const char vcid_kbdresponsehdlr_cc[] = "$Header$";
#endif // !KBDRESPONSEHDLR_CC_DEFINED
