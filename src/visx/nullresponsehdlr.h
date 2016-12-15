/** @file visx/nullresponsehdlr.h response handler that ignores all
    user responses */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Jun 21 18:54:35 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_VISX_NULLRESPONSEHDLR_H_UTC20050626084015_DEFINED
#define GROOVX_VISX_NULLRESPONSEHDLR_H_UTC20050626084015_DEFINED

#include "visx/responsehandler.h"

/// NullResponseHdlr implements ResponseHandler by ignoring all responses.
class NullResponseHdlr : public ResponseHandler
{
protected:
  NullResponseHdlr();
public:
  static NullResponseHdlr* make();

  virtual ~NullResponseHdlr() noexcept;

  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  // actions
  virtual void rhBeginTrial(nub::soft_ref<Toglet> widget, Trial& trial) const override;
  virtual void rhAbortTrial() const override;
  virtual void rhEndTrial() const override;
  virtual void rhHaltExpt() const override;
  virtual void rhAllowResponses(nub::soft_ref<Toglet> widget,
                                Trial& trial) const override;
  virtual void rhDenyResponses() const override;
};

#endif // !GROOVX_VISX_NULLRESPONSEHDLR_H_UTC20050626084015_DEFINED
