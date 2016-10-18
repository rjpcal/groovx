/** @file visx/block.h sequence of trials in a psychophysics experiment */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat Jun 26 12:29:33 1999
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

#ifndef GROOVX_VISX_BLOCK_H_UTC20050626084015_DEFINED
#define GROOVX_VISX_BLOCK_H_UTC20050626084015_DEFINED

#include "io/io.h"

#include "rutz/tracer.h"

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
  static rutz::tracer tracer;

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
  virtual ~Block() noexcept;

  virtual io::version_id class_version_id() const override;
  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  //
  // Element interface
  //

  virtual const nub::soft_ref<Toglet>& getWidget() const override;

  /// Starts the current element.
  /** This will be called by vxReturn() as needed. */
  virtual void vxRun(Element& parent) override;

  /// Prepares the Block to start the next element. */
  virtual void vxEndTrialHook() override;

protected:
  /// Pass control back to the parent since all child elements are finished.
  virtual void vxAllChildrenFinished() override;

private:
  Block(const Block&);
  Block& operator=(const Block&);

  Element* itsParent;
};

#endif // !GROOVX_VISX_BLOCK_H_UTC20050626084015_DEFINED
