///////////////////////////////////////////////////////////////////////
//
// block.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sat Jun 26 12:29:33 1999
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

#ifndef BLOCK_H_DEFINED
#define BLOCK_H_DEFINED

#include "io/io.h"

#include "util/tracer.h"

#include "visx/elementcontainer.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * The Block class represents a block as a sequence of elements in an
 * experiment. It maintains a sequence of elements, as well as an index for
 * the current element. Block is responsible for computing response times,
 * and recording the response information into the elements.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Block : public ElementContainer
{
public:
  /** This tracer dynamically controls the tracing of Block member
      functions. */
  static Util::Tracer tracer;

  //////////////
  // creators //
  //////////////

protected:
  /// Default constructor.
  Block();

public:
  /// Default creator.
  static Block* make();

  /// Virtual destructor ensures correct destruction of subclasses.
  virtual ~Block() throw();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  //
  // Element interface
  //

  virtual const Util::SoftRef<Toglet>& getWidget() const;

  /// Starts the current element.
  /** This will be called by vxReturn() as needed. */
  virtual void vxRun(Element& parent);

  /// Prepares the Block to start the next element. */
  virtual void vxEndTrialHook();

protected:
  /// Pass control back to the parent since all child elements are finished.
  virtual void vxAllChildrenFinished();

private:
  Block(const Block&);
  Block& operator=(const Block&);

  Element* itsParent;
};

static const char vcid_block_h[] = "$Header$";
#endif // !BLOCK_H_DEFINED
