///////////////////////////////////////////////////////////////////////
//
// observable.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 25 18:29:04 1999
// written: Wed May 26 11:07:34 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBSERVABLE_H_DEFINED
#define OBSERVABLE_H_DEFINED

class Observer;
class ObservableImpl;

///////////////////////////////////////////////////////////////////////
//
// Observable interface
//
///////////////////////////////////////////////////////////////////////

class Observable {
public:
  virtual ~Observable();

  void attach(Observer* obs);
  void detach(Observer* obs);

protected:
  Observable();

  void sendStateChangeMsg() const;

private:
  void sendDestroyMsg();

  ObservableImpl& itsImpl;
};

static const char vcid_observable_h[] = "$Header$";
#endif // !OBSERVABLE_H_DEFINED
