///////////////////////////////////////////////////////////////////////
//
// ioptrlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Nov 21 00:26:29 1999
// written: Tue Oct 24 16:09:28 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPTRLIST_H_DEFINED
#define IOPTRLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PTRLISTBASE_H_DEFINED)
#include "ptrlistbase.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c IoPtrList extends \c PtrListBase by adding and implementing the
 * \c IO interface. This class remains abstract, however, because the
 * cast from \c void* to \c IO* is delegated to subclasses by way of
 * the pure virtual function \c fromVoidToIO().
 *
 **/
///////////////////////////////////////////////////////////////////////

class IoPtrList : public PtrListBase, public virtual IO::IoObject {
public:
  /// Construct and reserve space for \a size objects.
  IoPtrList(int size);

  /// Virtual destructor.
  virtual ~IoPtrList();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

private:
  IoPtrList(const IoPtrList&);
  IoPtrList& operator=(const IoPtrList&);
};

static const char vcid_ioptrlist_h[] = "$Header$";
#endif // !IOPTRLIST_H_DEFINED
