///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:09:12 1999
// written: Wed Sep 27 11:36:15 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KBDRESPONSEHDLR_CC_DEFINED
#define KBDRESPONSEHDLR_CC_DEFINED

#include "kbdresponsehdlr.h"

#include "io/iolegacy.h"
#include "util/strings.h"

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

void KbdResponseHdlr::legacySrlz(IO::Writer* writer) const {
DOTRACE("KbdResponseHdlr::legacySrlz");
  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 ostream& os = lwriter->output();

	 if (lwriter->flags() & IO::TYPENAME) { os << ioTag << IO::SEP; }

	 oldLegacySrlz(writer);

	 if (os.fail()) throw IO::OutputError(ioTag.c_str());
  }
}

void KbdResponseHdlr::legacyDesrlz(IO::Reader* reader) {
DOTRACE("KbdResponseHdlr::legacyDesrlz");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 istream& is = lreader->input();

	 if (lreader->flags() & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag.c_str()); }

	 oldLegacyDesrlz(reader);

	 setEventSequence("<KeyPress");
	 setBindingSubstitution("%K");

	 if (is.fail()) throw IO::InputError(ioTag.c_str());
  }
}

static const char vcid_kbdresponsehdlr_cc[] = "$Header$";
#endif // !KBDRESPONSEHDLR_CC_DEFINED
