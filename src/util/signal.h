///////////////////////////////////////////////////////////////////////
//
// signal.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:29:04 1999
// written: Tue Aug 21 11:49:45 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SIGNAL_H_DEFINED
#define SIGNAL_H_DEFINED

namespace Util
{
  class Observer;
  class Signal;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * Along with Observer, implements the Observer design
 * pattern. Subclasses of Signal do not have to override any virtual
 * functions to become an Signal: all of the machinery is provided in
 * this base class. However, to conform to the expected behavior,
 * subclasses of Signal must call emitSignal() when any of
 * their non-mutable properties changes. This in turn will call
 * receiveSignal() on all of the Observer's that are observing
 * this Signal.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Util::Signal {
public:
  /// Default constructor.
  Signal();

  /// Virtual destructor.
  virtual ~Signal();

  /// Add an Observer to the list of those watching this Signal.
  void connect(Observer* sig);
  /// Remove an Observer from the list of those watching this Signal.
  void disconnect(Observer* sig);

  /** Informs all this object's Observers that this Signal's state
      has changed */
  void emitSignal() const;

private:
  Signal(const Signal&);
  Signal& operator=(const Signal&);

  class SigImpl;

  SigImpl* itsImpl;
};

static const char vcid_signal_h[] = "$Header$";
#endif // !SIGNAL_H_DEFINED
