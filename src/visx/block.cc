/** @file visx/block.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat Jun 26 12:29:34 1999
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

#ifndef GROOVX_VISX_BLOCK_CC_UTC20050626084016_DEFINED
#define GROOVX_VISX_BLOCK_CC_UTC20050626084016_DEFINED

#include "visx/block.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/readutils.h"
#include "io/writer.h"
#include "io/writeutils.h"

#include "nub/log.h"
#include "nub/ref.h"

#include "rutz/fstring.h"

#define GVX_DYNAMIC_TRACE_EXPR Block::tracer.status()
#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

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
GVX_TRACE("Block::make");
  return new Block;
}

Block::Block() :
  itsParent( 0 )
{
GVX_TRACE("Block::Block");
}

Block::~Block() throw()
{}

IO::VersionId Block::serialVersionId() const
{
GVX_TRACE("Block::serialVersionId");
  return BLOCK_SVID;
}

void Block::readFrom(IO::Reader& reader)
{
GVX_TRACE("Block::readFrom");

  reader.ensureReadVersionId("Block", 3,
                             "Try cvs tag xml_conversion_20040526",
                             SRC_POS);

  reader.readBaseClass("ElementContainer",
                       IO::makeProxy<ElementContainer>(this));
}

void Block::writeTo(IO::Writer& writer) const
{
GVX_TRACE("Block::writeTo");

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

const nub::soft_ref<Toglet>& Block::getWidget() const
{
GVX_TRACE("Block::getWidget");
  GVX_PRECONDITION( itsParent != 0 );
  return itsParent->getWidget();
}

void Block::vxRun(Element& e)
{
GVX_TRACE("Block::vxRun");

  if ( isComplete() ) return;

  GVX_PRECONDITION( &e != 0 );

  nub::log( vxInfo() );

  itsParent = &e;

  currentElement()->vxRun(*this);
}

void Block::vxEndTrialHook()
{
GVX_TRACE("Block::vxEndTrialHook");

  if (itsParent != 0)
    itsParent->vxEndTrialHook();
}

void Block::vxAllChildrenFinished()
{
GVX_TRACE("Block::vxAllChildrenFinished");

  // Release our current parent, then pass control onto it.
  Element* p = itsParent;
  itsParent = 0;
  GVX_ASSERT( p != 0 );
  p->vxReturn(CHILD_OK);
}

static const char vcid_groovx_visx_block_cc_utc20050626084016[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_BLOCK_CC_UTC20050626084016_DEFINED
