///////////////////////////////////////////////////////////////////////
//
// stringfwd.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Mar  3 12:32:58 2000
// written: Tue Mar  7 13:28:16 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGFWD_H_DEFINED
#define STRINGFWD_H_DEFINED

#if defined(ACC_COMPILER)

template< class charT, class traits, class Allocator > class basic_string;

class allocator;
template<class charT> class string_char_traits;
typedef basic_string<char, string_char_traits<char>, allocator> string;

#elif defined(GCC_COMPILER)

template <class charT, class traits, class Allocator> class basic_string;

// template <int __inst> class __malloc_alloc_template;
// typedef __malloc_alloc_template<0> malloc_alloc;
// typedef malloc_alloc alloc;

template <bool threads, int inst> class __default_alloc_template;
typedef __default_alloc_template<false, 0> alloc;

template<class charT> class string_char_traits;

typedef basic_string<char, string_char_traits<char>, alloc> string;


#else
#  include <string>
#endif

static const char vcid_stringfwd_h[] = "$Header$";
#endif // !STRINGFWD_H_DEFINED
