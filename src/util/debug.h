///////////////////////////////////////////////////////////////////////
//
// debug.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jan  4 08:00:00 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef DEBUG_H_DEFINED
#define DEBUG_H_DEFINED

namespace rutz
{
  class fstring;
}

namespace rutz {namespace debug
{
  void eval (const char* what, int level, const char* where, int line_no, bool nl, bool expr) throw();
  void eval (const char* what, int level, const char* where, int line_no, bool nl, char expr) throw();
  void eval (const char* what, int level, const char* where, int line_no, bool nl, unsigned char expr) throw();
  void eval (const char* what, int level, const char* where, int line_no, bool nl, short expr) throw();
  void eval (const char* what, int level, const char* where, int line_no, bool nl, unsigned short expr) throw();
  void eval (const char* what, int level, const char* where, int line_no, bool nl, int expr) throw();
  void eval (const char* what, int level, const char* where, int line_no, bool nl, unsigned int expr) throw();
  void eval (const char* what, int level, const char* where, int line_no, bool nl, long expr) throw();
  void eval (const char* what, int level, const char* where, int line_no, bool nl, unsigned long expr) throw();
  void eval (const char* what, int level, const char* where, int line_no, bool nl, float expr) throw();
  void eval (const char* what, int level, const char* where, int line_no, bool nl, double expr) throw();
  void eval (const char* what, int level, const char* where, int line_no, bool nl, const char* expr) throw();
  void eval (const char* what, int level, const char* where, int line_no, bool nl, void* expr) throw();
  void eval (const char* what, int level, const char* where, int line_no, bool nl, rutz::fstring expr) throw();

  void dump (const char* what, int level, const char* where, int line_no) throw();

  void panic_aux         (const char* what, const char* where, int line_no) throw();
  void assert_aux        (const char* what, const char* where, int line_no) throw();
  void precondition_aux  (const char* what, const char* where, int line_no) throw();
  void postcondition_aux (const char* what, const char* where, int line_no) throw();
  void invariant_aux     (const char* what, const char* where, int line_no) throw();

  const int MAX_KEYS = 1024;

  extern unsigned char key_levels[MAX_KEYS];
  extern const char* key_filenames[MAX_KEYS];

  int create_key(const char* filename);

  /// Get the debug key associated with the given filename.
  /** Returns -1 if the filename is not registered. */
  int lookup_key(const char* filename);

  void set_global_level(int lev);
}}

#define PANIC(message) rutz::debug::panic_aux(message, __FILE__, __LINE__)

// Like ASSERT(), but can't ever be turned off.
#define ABORT_IF(expr) \
      do { if ( expr ) { rutz::debug::panic_aux(#expr, __FILE__, __LINE__); } } while (0)

#define CONCAT(x, y) x ## y

#define DO_DBG_REGISTER(ext)                                    \
static const int CONCAT(DEBUG_KEY, ext) =                       \
  rutz::debug::create_key(__FILE__);                            \
                                                                \
static inline int CONCAT(dbg_level_, ext) ()                    \
{                                                               \
  return rutz::debug::key_levels[CONCAT(DEBUG_KEY, ext)];       \
}

#define DBG_REGISTER DO_DBG_REGISTER(1)
#define GET_DBG_LEVEL dbg_level_1

static const char vcid_debug_h[] = "$Id$ $URL$";

#else // DEBUG_H_DEFINED

//
// Everything here gets processed on the second and subsequent times that
// this file is #include'ed.
//

#undef DBG_REGISTER
#undef GET_DBG_LEVEL

#if   !defined(DEBUG_H_2)
#      define  DEBUG_H_2
#      define  DBG_REGISTER DO_DBG_REGISTER(2)
#      define  GET_DBG_LEVEL dbg_level_2
#elif !defined(DEBUG_H_3)
#      define  DEBUG_H_3
#      define  DBG_REGISTER DO_DBG_REGISTER(3)
#      define  GET_DBG_LEVEL dbg_level_3
#elif !defined(DEBUG_H_4)
#      define  DEBUG_H_4
#      define  DBG_REGISTER DO_DBG_REGISTER(4)
#      define  GET_DBG_LEVEL dbg_level_4
#elif !defined(DEBUG_H_5)
#      define  DEBUG_H_5
#      define  DBG_REGISTER DO_DBG_REGISTER(5)
#      define  GET_DBG_LEVEL dbg_level_5
#elif !defined(DEBUG_H_6)
#      define  DEBUG_H_6
#      define  DBG_REGISTER DO_DBG_REGISTER(6)
#      define  GET_DBG_LEVEL dbg_level_6
#elif !defined(DEBUG_H_7)
#      define  DEBUG_H_7
#      define  DBG_REGISTER DO_DBG_REGISTER(7)
#      define  GET_DBG_LEVEL dbg_level_7
#elif !defined(DEBUG_H_8)
#      define  DEBUG_H_8
#      define  DBG_REGISTER DO_DBG_REGISTER(8)
#      define  GET_DBG_LEVEL dbg_level_8
#else
#      error "debug.h included too many times!"
#endif

#undef dbg_eval
#undef dbg_eval_nl
#undef dbg_print
#undef dbg_print_nl

#undef ASSERT
#undef INVARIANT
#undef PRECONDITION
#undef POSTCONDITION

#endif // DEBUG_H_DEFINED

//
// Everything here gets re-processed every time that this file is #include'ed.
//

#if !defined(NO_DEBUG)
#  define dbg_eval(lev, x)     do { if (GET_DBG_LEVEL() >= lev) rutz::debug::eval(#x, lev, __FILE__, __LINE__, false, x); } while (0)
#  define dbg_eval_nl(lev, x)  do { if (GET_DBG_LEVEL() >= lev) rutz::debug::eval(#x, lev, __FILE__, __LINE__, true, x); } while (0)
#  define dbg_print(lev, x)    do { if (GET_DBG_LEVEL() >= lev) rutz::debug::eval(0, lev, __FILE__, __LINE__, false, x); } while (0)
#  define dbg_print_nl(lev, x) do { if (GET_DBG_LEVEL() >= lev) rutz::debug::eval(0, lev, __FILE__, __LINE__, true, x); } while (0)
#  define dbg_dump(lev, x)     do { if (GET_DBG_LEVEL() >= lev) { rutz::debug::dump(#x, lev, __FILE__, __LINE__); (x).debug_dump(); } } while (0)

#  define ASSERT(expr)        do { if ( !(expr) ) rutz::debug::assert_aux(#expr, __FILE__, __LINE__); } while(0)
#  define INVARIANT(expr)     do { if ( !(expr) ) rutz::debug::invariant_aux(#expr, __FILE__, __LINE__); } while(0)
#  define PRECONDITION(expr)  do { if ( !(expr) ) rutz::debug::precondition_aux(#expr, __FILE__, __LINE__); } while(0)
#  define POSTCONDITION(expr) do { if ( !(expr) ) rutz::debug::postcondition_aux(#expr, __FILE__, __LINE__); } while(0)

#else // defined(NO_DEBUG)

#  define dbg_eval(lev, x)     do {} while (0)
#  define dbg_eval_nl(lev, x)  do {} while (0)
#  define dbg_print(lev, x)    do {} while (0)
#  define dbg_print_nl(lev, x) do {} while (0)
#  define dbg_dump(lev, expr)  do {} while (0)

#  define ASSERT(x)        do {} while (0)
#  define INVARIANT(x)     do {} while (0)
#  define PRECONDITION(x)  do {} while (0)
#  define POSTCONDITION(x) do {} while (0)

#endif
