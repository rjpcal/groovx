///////////////////////////////////////////////////////////////////////
//
// block.cc
//
// Copyright (c) 1999-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sat Jun 26 12:29:34 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCK_CC_DEFINED
#define BLOCK_CC_DEFINED

#include "visx/block.h"

#include "io/reader.h"
#include "io/readutils.h"
#include "io/writer.h"
#include "io/writeutils.h"

#include "util/log.h"
#include "util/ref.h"
#include "util/strings.h"

#define DYNAMIC_TRACE_EXPR Block::tracer.status()
#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

Util::Tracer Block::tracer;

namespace
{
  IO::VersionId BLOCK_SERIAL_VERSION_ID = 2;
}

///////////////////////////////////////////////////////////////////////
//
// Block creators
//
///////////////////////////////////////////////////////////////////////

Block* Block::make()
{
DOTRACE("Block::make");
  return new Block;
}

Block::Block() :
  itsParent( 0 )
{
DOTRACE("Block::Block");
}

Block::~Block() throw()
{}

IO::VersionId Block::serialVersionId() const
{
DOTRACE("Block::serialVersionId");
  return BLOCK_SERIAL_VERSION_ID;
}

void Block::readFrom(IO::Reader& reader)
{
DOTRACE("Block::readFrom");

  int svid = reader.ensureReadVersionId("Block", 1, "Try grsh0.8a3");

  ElementContainer::legacyReadElements(reader, "trialSeq");

  reader.readValue("randSeed", iolegacyRandSeed());
  reader.readValue("curTrialSeqdx", iolegacySequencePos());
  if (numCompleted() > numElements())
    {
      throw Util::Error("Block");
    }

  if (svid < 2)
    {
      bool dummy;
      reader.readValue("verbose", dummy);
    }
}

void Block::writeTo(IO::Writer& writer) const
{
DOTRACE("Block::writeTo");

  writer.ensureWriteVersionId("Block", BLOCK_SERIAL_VERSION_ID, 2,
                              "Try grsh0.8a7");

  ElementContainer::legacyWriteElements(writer, "trialSeq");

  writer.writeValue("randSeed", iolegacyRandSeed());
  writer.writeValue("curTrialSeqdx", iolegacySequencePos());
}

///////////////////////////////////////////////////////////////////////
//
// Block's Element interface
//
///////////////////////////////////////////////////////////////////////

const Util::SoftRef<Toglet>& Block::getWidget() const
{
DOTRACE("Block::getWidget");
  Precondition( itsParent != 0 );
  return itsParent->getWidget();
}

void Block::vxRun(Element& e)
{
DOTRACE("Block::vxRun");

  if ( isComplete() ) return;

  Precondition( &e != 0 );

  Util::log( vxInfo() );

  itsParent = &e;

  currentElement()->vxRun(*this);
}

void Block::vxEndTrialHook()
{
DOTRACE("Block::vxEndTrialHook");

  if (itsParent != 0)
    itsParent->vxEndTrialHook();
}

void Block::vxAllChildrenFinished()
{
DOTRACE("Block::vxAllChildrenFinished");

  // Release our current parent, then pass control onto it.
  Element* p = itsParent;
  itsParent = 0;
  Assert( p != 0 );
  p->vxReturn(CHILD_OK);
}

static const char vcid_block_cc[] = "$Header$";
#endif // !BLOCK_CC_DEFINED
