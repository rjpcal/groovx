///////////////////////////////////////////////////////////////////////
//
// ioptrlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Nov 21 00:26:29 1999
// written: Wed Mar 22 16:47:04 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPTRLIST_H_DEFINED
#define IOPTRLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VOIDPTRLIST_H)
#include "voidptrlist.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H)
#include "io.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c IoPtrList extends \c VoidPtrList by adding and implementing the
 * \c IO interface. This class remains abstract, however, because the
 * cast from \c void* to \c IO* is delegated to subclasses by way of
 * the pure virtual function \c fromVoidToIO().
 *
 **/
///////////////////////////////////////////////////////////////////////

class IoPtrList : public VoidPtrList, public virtual IO {
public:
  /// Construct and reserve space for \a size objects.
  IoPtrList(int size);

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

protected:
  /// Subclasses must override this to cast the void* to an IO*.
  virtual IO* fromVoidToIO(void* ptr) const = 0;
  /// Subclasses must override this to cast the IO* to a void*.
  virtual void* fromIOToVoid(IO* ptr) const = 0;

  /** May be overridden by subclasses to provide a list of alternative
      typenames suitable for use in \c IO::readTypename(). The default
      implementation here returns an empty string. */
  virtual const char* alternateIoTags() const;

private:
  IoPtrList(const IoPtrList&);
  IoPtrList& operator=(const IoPtrList&);
};

static const char vcid_ioptrlist_h[] = "$Header$";
#endif // !IOPTRLIST_H_DEFINED
