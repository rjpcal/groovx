///////////////////////////////////////////////////////////////////////
//
// ioptrlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Nov 21 00:26:29 1999
// written: Wed Feb 16 14:31:04 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPTRLIST_H_DEFINED
#define IOPTRLIST_H_DEFINED

#ifndef VOIDPTRLIST_H_DEFINED
#include "voidptrlist.h"
#endif

#ifndef IO_H_DEFINED
#include "io.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * IoPtrList extends VoidPtrList by adding and implementing the IO
 * interface. This class remains abstract, however, because the cast
 * from void* to IO* is delegated to subclasses by way of the pure
 * virtual function fromVoidToIO().
 *
 * @short A subclass of VoidPtrList that adds IO capabilities.
 **/
///////////////////////////////////////////////////////////////////////

class IoPtrList : public VoidPtrList, public virtual IO {
public:
  ///
  IoPtrList(int size);

  ///
  virtual void serialize(ostream &os, IOFlag flag) const;
  ///
  virtual void deserialize(istream &is, IOFlag flag);

  ///
  virtual int charCount() const;

  ///
  virtual void readFrom(Reader* reader);
  ///
  virtual void writeTo(Writer* writer) const;

protected:
  /// Subclasses must override this to cast the void* to an IO*.
  virtual IO* fromVoidToIO(void* ptr) const = 0;
  /// Subclasses must override this to cast the IO* to a void*.
  virtual void* fromIOToVoid(IO* ptr) const = 0;

  /** May be overridden by subclasses to provide a list of alternative
      typenames suitable for use in \c IO::readTypename(). The default
      implementation here returns an empty string. */
  virtual const string& alternateIoTags() const;
};

static const char vcid_ioptrlist_h[] = "$Header$";
#endif // !IOPTRLIST_H_DEFINED
