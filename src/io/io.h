///////////////////////////////////////////////////////////////////////
//
// io.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Jan-99
// written: Thu Aug  9 07:08:51 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IO_H_DEFINED
#define IO_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJECT_H_DEFINED)
#include "util/object.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IODECLS_H_DEFINED)
#include "io/iodecls.h"
#endif

class fstring;

///////////////////////////////////////////////////////////////////////
/**
 *
 * The IO::IoObject class defines the abstract interface for object
 * persistence. Classes which need these facilities should inherit
 * virtually from IO::IoObject. IO::IoObject is reference counted (by
 * subclassing Util::Object), providing automatic memory management
 * when IO::IoObject's are managed with a smart pointer that manages
 * the reference count, such as PtrHandle or Util::Ref.
 *
 **/
///////////////////////////////////////////////////////////////////////

class IO::IoObject : public virtual Util::Object {
private:
  IoObject(const IoObject&);
  IoObject& operator=(const IoObject&);

protected:
  /** Default constructor. */
  IoObject();

  /// Virtual destructor to ensure correct destruction of subclasses
  virtual ~IoObject() = 0;

public:
  /** Subclasses implement this method to save the object's state via
      the generic interface provided by \c IO::Reader. Parsing the
      format of the input is handled by the \c IO::Reader, so
      implementors of \c readFrom() of don't need to deal with
      formatting. */
  virtual void readFrom(IO::Reader* reader) = 0;

  /** Subclasses implement this method to restore the object's state
      via the generic interface provided by \c IO::Writer. Formatting
      the output is handled by the \c IO::Writer, so implementors of
      \c writeTo() of don't need to deal with formatting. */
  virtual void writeTo(IO::Writer* writer) const = 0;

  /** Returns the number of attributes that are written in the
      object's \c writeTo() function. The default implementation
      simply calls writeTo() with a dummy Writer and counts how many
      attributes are written. However, this is somewhat inefficient
      since it defers the counting to runtime when the number may in
      fact be known at compile time. Thus subclasses may wish to
      override this function to return a compile-time constant. */
  virtual unsigned int ioAttribCount() const;

  /** Returns a serialization version id for the class. The default
      implementation returns zero. Classes should override this when
      they make a change that requires a change to their serialization
      protocol. Overriding versions of this function should follow the
      convention that a higher id refers to a later version of the
      class. Implementations of \c IO::Reader and \c IO::Writer will
      provide a way for a class to store and retrieve the
      serialization version of an object. */
  virtual IO::VersionId serialVersionId() const;

  /** Returns the typename of the full object. The implementation
      provided by \c IO returns a demangled version of \c
      typeid(*this).name(), which should very closely resemble the way
      the object was declared in source code. */
  virtual fstring ioTypename() const;
};


/**
 * A subclass of Util::Error for invalid filenames or filenames
 * referring to inaccessible files.
 **/
class IO::FilenameError : public Util::Error {
public:
  /// Default constructor.
  FilenameError(const char* filename);

  /// Virtual destructor
  virtual ~FilenameError();
};

static const char vcid_io_h[] = "$Header$";
#endif // !IO_H_DEFINED
