///////////////////////////////////////////////////////////////////////
//
// block.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Jun 26 12:29:34 1999
// written: Thu Dec 19 18:26:00 2002
// $Id$
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
#include "util/minivec.h"
#include "util/ref.h"
#include "util/strings.h"

#define DYNAMIC_TRACE_EXPR Block::tracer.status()
#include "util/trace.h"
#include "util/debug.h"

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

Block::~Block()
{}

IO::VersionId Block::serialVersionId() const
{
DOTRACE("Block::serialVersionId");
  return BLOCK_SERIAL_VERSION_ID;
}

void Block::readFrom(IO::Reader* reader)
{
DOTRACE("Block::readFrom");

  int svid = reader->ensureReadVersionId("Block", 1, "Try grsh0.8a3");

  IO::ReadUtils::readObjectSeq<Element>
    (reader, "trialSeq", std::back_inserter(iolegacyElements()));

  reader->readValue("randSeed", iolegacyRandSeed());
  reader->readValue("curTrialSeqdx", iolegacySequencePos());
  if (numCompleted() > numElements())
    {
      throw IO::ReadError("Block");
    }

  if (svid < 2)
    {
      bool dummy;
      reader->readValue("verbose", dummy);
    }
}

void Block::writeTo(IO::Writer* writer) const
{
DOTRACE("Block::writeTo");

  writer->ensureWriteVersionId("Block", BLOCK_SERIAL_VERSION_ID, 2,
                               "Try grsh0.8a7");

  IO::WriteUtils::writeObjectSeq(writer, "trialSeq",
                                 iolegacyElements().begin(),
                                 iolegacyElements().end());

  writer->writeValue("randSeed", iolegacyRandSeed());
  writer->writeValue("curTrialSeqdx", iolegacySequencePos());
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

  Util::log( status() );

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
