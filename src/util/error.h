///////////////////////////////////////////////////////////////////////
//
// error.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 22 14:59:47 1999
// written: Wed Mar  8 11:15:31 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERROR_H_DEFINED
#define ERROR_H_DEFINED

class dynamic_string;

/**
 *
 * \c Error is the base class for all error classes.
 *
 **/

class Error {
public:
  /// Default constructor.
  Error();
  /// Virtual constructor ensures correct destruction of subclasses.
  virtual ~Error();
};



/**
 *
 * \c ErrorWithMsg is a simple error class that carries a string
 * message describing the error.
 *
 **/

class ErrorWithMsg : public virtual Error {
public:
  /// Default construct with an empty message string.
  ErrorWithMsg();

  /// Construct with an informative message \a errorMessage.
  ErrorWithMsg(const char* errorMessage);

  /// Copy constructor.
  ErrorWithMsg(const ErrorWithMsg& other);

  /// Virtual destructor.
  virtual ~ErrorWithMsg();

  /// Assignment operator.
  ErrorWithMsg& operator=(const ErrorWithMsg& other);

  /// Get a C-style string describing the error.
  virtual const char* msg_cstr() const;

  void appendMsg(const char* addMsg);

protected:
  /// Change the informative message to \a newMessage.
  virtual void setMsg(const char* newMessage);

private:
  dynamic_string* itsInfo;
};

static const char vcid_error_h[] = "$Header$";
#endif // !ERROR_H_DEFINED
