///////////////////////////////////////////////////////////////////////
//
// tostring.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Aug  5 20:00:26 2001
// written: Sun Aug  5 20:09:28 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOSTRING_H_DEFINED
#define TOSTRING_H_DEFINED


namespace Util
{
  template <class T>
  struct Convert
  {
    static const char* toString(const T& x);
  };

  template <>
  struct Convert<char*>
  {
    static const char* toString(char* const& x) { return x; }
  };

  template <>
  struct Convert<const char*>
  {
    static const char* toString(const char* const& x) { return x; }
  };
}

static const char vcid_tostring_h[] = "$Header$";
#endif // !TOSTRING_H_DEFINED
