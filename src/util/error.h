///////////////////////////////////////////////////////////////////////
//
// error.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 22 14:59:47 1999
// written: Tue Jun 22 15:00:13 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERROR_H_DEFINED
#define ERROR_H_DEFINED

class Error {
public:
  Error();
  virtual ~Error();
};

static const char vcid_error_h[] = "$Header$";
#endif // !ERROR_H_DEFINED
