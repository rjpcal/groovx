///////////////////////////////////////////////////////////////////////
//
// ioptrlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Nov 21 00:26:29 1999
// written: Sun Oct  8 15:46:00 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPTRLIST_H_DEFINED
#define IOPTRLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VOIDPTRLIST_H_DEFINED)
#include "voidptrlist.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

class MasterIoPtr : public MasterPtrBase {
public:
  MasterIoPtr();
  virtual ~MasterIoPtr();

  virtual IO::IoObject* ioPtr() const = 0;

  virtual bool isValid() const = 0;

  virtual bool operator==(const MasterPtrBase& other) = 0;
};

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

class IoPtrList : public VoidPtrList, public virtual IO::IoObject {
public:
  /// Construct and reserve space for \a size objects.
  IoPtrList(int size);

  /// Virtual destructor.
  virtual ~IoPtrList();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

protected:
  /** May be overridden by subclasses to provide a list of alternative
      typenames suitable for use in \c IO::readTypename(). The default
      implementation here returns an empty string. */
  virtual const char* alternateIoTags() const;

  virtual fixed_string legacyValidTypenames() const;

  virtual MasterIoPtr* makeMasterIoPtr(IO::IoObject* obj) const = 0;

private:
  IoPtrList(const IoPtrList&);
  IoPtrList& operator=(const IoPtrList&);

  void legacySrlz(IO::LegacyWriter* writer) const;
  void legacyDesrlz(IO::LegacyReader* reader);
};

static const char vcid_ioptrlist_h[] = "$Header$";
#endif // !IOPTRLIST_H_DEFINED
