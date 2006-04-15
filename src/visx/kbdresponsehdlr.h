/** @file visx/kbdresponsehdlr.h response handler for keypress events */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jun 21 18:09:11 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_KBDRESPONSEHDLR_H_UTC20050626084015_DEFINED
#define GROOVX_VISX_KBDRESPONSEHDLR_H_UTC20050626084015_DEFINED

#include "visx/eventresponsehdlr.h"


/// KbdResponseHdlr is just EventResponseHdlr with a default "keypress" event type.
class KbdResponseHdlr : public EventResponseHdlr
{
protected:
  KbdResponseHdlr();
public:
  static KbdResponseHdlr* make();

  virtual ~KbdResponseHdlr() throw();
};

static const char __attribute__((used)) vcid_groovx_visx_kbdresponsehdlr_h_utc20050626084015[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_KBDRESPONSEHDLR_H_UTC20050626084015_DEFINED
