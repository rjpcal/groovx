///////////////////////////////////////////////////////////////////////
//
// block.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sat Jun 26 12:29:34 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef BLOCK_CC_DEFINED
#define BLOCK_CC_DEFINED

#include "visx/block.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/readutils.h"
#include "io/writer.h"
#include "io/writeutils.h"

#include "nub/log.h"
#include "nub/ref.h"

#include "util/fstring.h"

#define DYNAMIC_TRACE_EXPR Block::tracer.status()
#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

rutz::tracer Block::tracer;

namespace
{
  IO::VersionId BLOCK_SVID = 3;
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
  return BLOCK_SVID;
}

void Block::readFrom(IO::Reader& reader)
{
DOTRACE("Block::readFrom");

  reader.ensureReadVersionId("Block", 3,
                             "Try cvs tag xml_conversion_20040526",
                             SRC_POS);

  reader.readBaseClass("ElementContainer",
                       IO::makeProxy<ElementContainer>(this));
}

void Block::writeTo(IO::Writer& writer) const
{
DOTRACE("Block::writeTo");

  writer.ensureWriteVersionId("Block", BLOCK_SVID, 3,
                              "Try groovx0.8a7", SRC_POS);

  writer.writeBaseClass("ElementContainer",
                        IO::makeConstProxy<ElementContainer>(this));
}

///////////////////////////////////////////////////////////////////////
//
// Block's Element interface
//
///////////////////////////////////////////////////////////////////////

const Nub::SoftRef<Toglet>& Block::getWidget() const
{
DOTRACE("Block::getWidget");
  PRECONDITION( itsParent != 0 );
  return itsParent->getWidget();
}

void Block::vxRun(Element& e)
{
DOTRACE("Block::vxRun");

  if ( isComplete() ) return;

  PRECONDITION( &e != 0 );

  Nub::log( vxInfo() );

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
  ASSERT( p != 0 );
  p->vxReturn(CHILD_OK);
}

static const char vcid_block_cc[] = "$Id$ $URL$";
#endif // !BLOCK_CC_DEFINED
