///////////////////////////////////////////////////////////////////////
//
// block.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Jun 26 12:29:33 1999
// written: Mon Jan 13 11:08:25 2003
// $Id$
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
  virtual ~Block();

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
