///////////////////////////////////////////////////////////////////////
//
// stringfwd.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Mar  3 12:32:58 2000
// written: Fri Mar  3 16:41:07 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGFWD_H_DEFINED
#define STRINGFWD_H_DEFINED

#if defined(GCC_COMPILER)

typedef basic_string <char> string;

#elif defined(ACC_COMPILER)

class allocator;
template<class charT> class string_char_traits;
template< class charT, class traits, class Allocator > class basic_string;
typedef basic_string<char, string_char_traits<char>, allocator> string;

#else
#  include <string>
#endif

static const char vcid_stringfwd_h[] = "$Header$";
#endif // !STRINGFWD_H_DEFINED
