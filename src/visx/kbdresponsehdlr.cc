///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:09:12 1999
// written: Tue Sep 26 19:16:36 2000
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

KbdResponseHdlr::~KbdResponseHdlr() {}

void KbdResponseHdlr::legacySrlz(IO::Writer* writer, STD_IO::ostream &os, IO::IOFlag flag) const {
DOTRACE("KbdResponseHdlr::legacySrlz");
  if (flag & IO::TYPENAME) { os << ioTag << IO::SEP; }

  oldLegacySrlz(writer, os, flag);

  if (os.fail()) throw IO::OutputError(ioTag.c_str());

  if (flag & IO::BASES) { /* no bases to legacySrlz */ }
}

void KbdResponseHdlr::legacyDesrlz(IO::Reader* reader, STD_IO::istream &is, IO::IOFlag flag) {
DOTRACE("KbdResponseHdlr::legacyDesrlz");

  if (flag & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag.c_str()); }

  oldLegacyDesrlz(reader, is, flag);

  setEventSequence("<KeyPress");
  setBindingSubstitution("%K");

  if (is.fail()) throw IO::InputError(ioTag.c_str());

  if (flag & IO::BASES) { /* no bases to legacyDesrlz */ }
}

int KbdResponseHdlr::legacyCharCount() const {
DOTRACE("KbdResponseHdlr::legacyCharCount");
  return ioTag.length() + 1 + oldCharCount();
}

static const char vcid_kbdresponsehdlr_cc[] = "$Header$";
#endif // !KBDRESPONSEHDLR_CC_DEFINED
