///////////////////////////////////////////////////////////////////////
//
// cppdeps.cc
//
// Copyright (c) 2003-2004 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 16 15:47:10 2003
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef CPPDEPS_CC_DEFINED
#define CPPDEPS_CC_DEFINED

#include <cassert>
#include <cctype>
#include <cstring>     // for strerror()
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <dirent.h>    // for readdir()
#include <errno.h>     // for errno
#include <fcntl.h>     // for open(), O_RDONLY
#include <sys/mman.h>  // for mmap()
#include <sys/stat.h>  // for stat()
#include <sys/types.h>
#include <time.h>      // for time()
#include <unistd.h>

using std::vector;
using std::map;
using std::set;
using std::string;
using std::cerr;

namespace
{
  // helper functions

  string string_time(const time_t t)
  {
    string s(ctime(&t));
    while (s.length() > 0 && isspace(s[s.length()-1]))
      {
        s.erase(s.length()-1, 1);
      }
    return s;
  }

  bool file_exists(const char* fname)
  {
    struct stat statbuf;
    return (stat(fname, &statbuf) == 0);
  }

  bool is_directory(const char* fname)
  {
    struct stat statbuf;
    errno = 0;
    if (stat(fname, &statbuf) != 0)
      {
        cerr << strerror(errno) << "\n";
        cerr << "stat failed " << fname << "\n";
        return false;
      }

    return S_ISDIR(statbuf.st_mode);
  }

  bool should_ignore_file(const char* fname)
  {
    return (fname[0] == '.' ||
            strcmp(fname, "RCS") == 0 ||
            strcmp(fname, "CVS") == 0);
  }

  // get the name of the directory containing fname -- the result will NOT
  // have a trailing slash!
  string get_dirname_of(const string& fname)
  {
    const string::size_type pos = fname.find_last_of('/');
    if (pos == string::npos)
      {
        // no '/' was found, so the directory is the current directory
        return string(".");
      }
    return fname.substr(0, pos);
  }

  string join_filename(const string& dir, const string& fname)
  {
    string result(dir);
    result.reserve(dir.length() + 1 + fname.length());
    result += '/';
    result += fname;
    return result;
  }

  string trim_trailing_slashes(const string& inp)
  {
    string result = inp;

    while (result.length() > 1 && result[result.length()-1] == '/')
      {
        result.erase(result.length()-1,1);
      }

    return result;
  }

  // make a normalized pathname from the given input, by making the
  // following transformations:
  //   (1) "/./"          --> "/"
  //   (2) "somedir/../"  --> "/"
  //   (3) "//"           --> "/"
  string make_normpath(const string& path)
  {
    vector<string> result;

    string::size_type p = 0;

    for (string::size_type i = 0; i < path.size(); ++i)
      {
        if (path[i] == '/' || i+1 == path.size())
          {
            if (i == 0) result.push_back("");
            else
              {
                string::size_type i2 = i;
                if (path[i] != '/' && i+1 == path.size())
                  i2 = path.size();
                string part(path.begin()+p, path.begin()+i2);
                if (part == "")
                  {
                    // do nothing
                  }
                else if (part == ".")
                  {
                    // do nothing
                  }
                else if (part == "..")
                  {
                    if (result.size() == 0)
                      {
                        cerr << "ERROR: used '../' at beginning of path\n";
                        exit(1);
                      }
                    else if (result.size() == 1 && result.back() == "")
                      {
                        // do nothing, here we have '/../' at beginning of path
                      }
                    else
                      {
                        result.pop_back();
                      }
                  }
                else
                  {
                    result.push_back(part);
                  }
              }
            p = i+1;
          }
      }

    string newpath;

    for (unsigned int i = 0; i < result.size(); ++i)
      {
        newpath += result[i];
        if (i+1 < result.size())
          newpath += '/';
      }

    return newpath;
  }

  class mapped_file
  {
  public:
    mapped_file(const char* filename)
      :
      m_statbuf(),
      m_fileno(0),
      m_mem(0)
    {
      errno = 0;

      if (stat(filename, &m_statbuf) == -1)
        {
          cerr << "stat() failed for file " << filename << ":\n";
          cerr << strerror(errno) << "\n";
          exit(1);
        }

      m_fileno = open(filename, O_RDONLY);
      if (m_fileno == -1)
        {
          cerr << "open() failed for file " << filename << ":\n";
          cerr << strerror(errno) << "\n";
          exit(1);
        }

      m_mem = mmap(0, m_statbuf.st_size,
                   PROT_READ, MAP_PRIVATE, m_fileno, 0);

      if (m_mem == (void*)-1)
        {
          cerr << "mmap() failed for file " << filename << ":\n";
          cerr << strerror(errno) << "\n";
          exit(1);
        }
    }

    ~mapped_file()
    {
      munmap(m_mem, m_statbuf.st_size);
      close(m_fileno);
    }

    const void* memory() const { return m_mem; }

    off_t length() const { return m_statbuf.st_size; }

    time_t mtime() const { return m_statbuf.st_mtime; }

  private:
    mapped_file(const mapped_file&);
    mapped_file& operator=(const mapped_file&);

    struct stat m_statbuf;
    int         m_fileno;
    void*       m_mem;

  }; // end class mapped_file

} // end unnamed namespace

struct dependency
{
  dependency(const string& t, const string& s = string(),
             bool l = false)
    : target(make_normpath(t)), source(s), literal(l)
  {}

  string target;
  string source;
  bool literal; // if true, then we don't try to look up nested includes
};

bool operator<(const dependency& i1, const dependency& i2)
{
  return i1.target < i2.target;
}

/// A class for doing fast include-file dependency analysis.
/** Several shortcuts (er, hacks...) are taken to make the parsing
    extremely fast and cheap, but at worst this makes the computed
    dependencies be unnecessarily pessimistic. For example, a #include
    that occurs inside a comment will still be treated as a regular
    #include. */
class cppdeps
{
private:
  // Typedefs and enums
  typedef vector<dependency>      dep_list_t;
  typedef map<string, dep_list_t> dep_map_t;

  enum parse_state
    {
      NOT_STARTED = 0,
      IN_PROGRESS = 1,
      COMPLETE = 2
    };

  enum output_mode
    {
      MAKEFILE_DEPS,
      DIRECT_INCLUDE_TREE,
      NESTED_INCLUDE_TREE
    };

  // Member variables
  vector<string>           m_user_ipath;
  vector<string>           m_sys_ipath;
  vector<string>           m_literal_exts;
  vector<string>           m_src_files;
  vector<string>           m_obj_exts;
  string                   m_objdir;
  bool                     m_check_sys_includes;
  bool                     m_quiet;
  output_mode              m_output_mode;

  dep_map_t                m_nested_includes;
  dep_map_t                m_direct_includes;

  map<string, parse_state> m_parse_states;

  string                   m_strip_prefix;

  const time_t             m_start_time;  // so we can check to see if
                                          // any source files have
                                          // timestamps in the future

public:
  cppdeps(int argc, char** argv);

  void inspect(char** arg0, char** argn);

  // Return value serves two purposes... (1) if it is zero, then the
  // fname is not recognized as a c++ file, and (2) otherwise, the
  // return value gives the length of the c++ filename extension,
  // including the '.' (i.e. '.C' gives 2, '.cc' gives 3, '.cpp' gives
  // 4).
  int is_cc_filename(const char* fname);

  // Like is_cc_filename(), but also checks for c++ header-file
  // extensions (like '.h', '.hh', '.hpp').
  int is_cc_or_h_filename(const char* fname);

  static bool resolve_one(const string& include_name,
                          const string& src_fname,
                          const string& dirname,
                          const vector<string>& ipath,
                          const vector<string>& literal,
                          dep_list_t& vec);

  const dep_list_t& get_direct_includes(const string& src_fname);
  const dep_list_t& get_nested_includes(const string& src_fname);

  void print_makefile_dep(const string& current_file);
  void print_include_tree(const string& current_file);

  typedef void (cppdeps::* output_func)(const string&);

  void traverse_sources(output_func handler);

  void batch_build();
};

cppdeps::cppdeps(const int argc, char** const argv) :
  m_check_sys_includes(false),
  m_quiet(false),
  m_output_mode(MAKEFILE_DEPS),
  m_start_time(time((time_t*) 0))
{
  bool debug = false;

  if (argc == 1)
    {
      printf
        ("usage: %s [options] srcdir...\n"
         "\n"
         "options:\n"
         "    --includedir [dir]    specify a directory to be searched when resolving\n"
         "                          #include \"...\" directives\n"
         "    --I[dir]              same as --includedir [dir]\n"
         "    --sysincludedir [dir] specify a directory to be searched when resolving\n"
         "                          #include <...> directives\n"
         "    --objdir [dir]        specify a path prefix indicating where the object\n"
         "                          (.o) files should be placed (the default\n"
         "                          is no prefix, or just './')\n"
         "    --objext [.ext]       make .ext be one of the suffixes used in target\n"
         "                          rules in the makefile; there may be more than one\n"
         "                          such extension, in which case each rule emitted\n"
         "                          will have more than one target; the default is for\n"
         "                          the list of extensions to include just '.o'\n"
         "    --checksys            force tracking of dependencies in #include <...>\n"
         "                          directives (default is to not record <...> files\n"
         "                          as dependencies)\n"
         "    --literal [.ext]      treat files ending in \".ext\" as literal #include\'s\n"
         "    --quiet               suppress warnings\n"
         "    --debug               show contents of internal variables\n"
         "\n"
         "any unrecognized command-line arguments are treated as source directories,\n"
         "which will be searched recursively for files with C/C++ filename extensions\n"
         "(including .c, .C, .cc, .cpp, .h, .H, .hh, .hpp)\n"
         "\n"
         "example:\n"
         "\n"
         "    %s --includedir ~/local/include/ --objdir project/obj/ ./\n"
         "\n"
         "    builds dependencies for all source files found recursively within the\n"
         "    current directory (./), using ~/local/include to resolve #include's,\n"
         "    and putting .o files into project/obj/.\n",
         argv[0],
         argv[0]);
    }

  m_sys_ipath.push_back("/usr/include");
  m_sys_ipath.push_back("/usr/include/linux");
  m_sys_ipath.push_back("/usr/local/matlab/extern/include");

  char** arg = argv+1; // skip to first command-line arg

  for ( ; *arg != 0; ++arg)
    {
      if (strcmp(*arg, "--includedir") == 0)
        {
          m_user_ipath.push_back(*++arg);
        }
      else if (strncmp(*arg, "-I", 2) == 0)
        {
          m_user_ipath.push_back((*arg) + 2);
        }
      else if (strcmp(*arg, "--sysincludedir") == 0)
        {
          m_sys_ipath.push_back(*++arg);
        }
      else if (strcmp(*arg, "--objdir") == 0)
        {
          m_objdir = trim_trailing_slashes(*++arg);
        }
      else if (strcmp(*arg, "--checksys") == 0)
        {
          m_check_sys_includes = true;
        }
      else if (strcmp(*arg, "--quiet") == 0)
        {
          m_quiet = true;
        }
      else if (strcmp(*arg, "--output-direct-includes") == 0)
        {
          m_output_mode = DIRECT_INCLUDE_TREE;
        }
      else if (strcmp(*arg, "--output-nested-includes") == 0)
        {
          m_output_mode = NESTED_INCLUDE_TREE;
        }
      else if (strcmp(*arg, "--literal") == 0)
        {
          m_literal_exts.push_back(*++arg);
        }
      else if (strcmp(*arg, "--objext") == 0)
        {
          m_obj_exts.push_back(*++arg);
        }
      else if (strcmp(*arg, "--debug") == 0)
        {
          debug = true;
        }
      // treat any unrecognized arguments as src files
      else
        {
          const string fname = trim_trailing_slashes(*arg);
          if (!file_exists(fname.c_str()))
            {
              cerr << "ERROR: no such source file: '" << fname << "'\n";
              exit(1);
            }
          m_src_files.push_back(fname);
          if (is_directory(fname.c_str()))
            {
              m_user_ipath.push_back(fname);
              m_strip_prefix = fname;
            }
        }

      if (debug)
        {
          inspect(argv, arg);
        }
    }

  // If the user didn't specify any object-filename extensions, then
  // we just use the default '.o'.
  if (m_obj_exts.size() == 0)
    m_obj_exts.push_back(".o");

  if (debug)
    {
      inspect(argv, arg);
    }
}

namespace
{
  void print_stringvec(std::ostream& out, const vector<string>& v)
  {
    out << "[";
    for (unsigned int i = 0; i < v.size(); ++i)
      {
        out << "'" << v[i] << "'";

        if (i+1 < v.size()) out << ", ";
      }
    out << "]";
  }
}

void cppdeps::inspect(char** arg0, char** argn)
{
  cerr << "command line: ";
  while (arg0 <= argn)
    {
      cerr << *arg0 << " ";
      ++arg0;
    }
  cerr << "\n";

  cerr << "user_ipath: ";
  print_stringvec(cerr, m_user_ipath);
  cerr << "\n";

  cerr << "sys_ipath: ";
  print_stringvec(cerr, m_sys_ipath);
  cerr << "\n";

  cerr << "sources: ";
  print_stringvec(cerr, m_src_files);
  cerr << "\n";

  cerr << "literal_exts: ";
  print_stringvec(cerr, m_literal_exts);
  cerr << "\n";

  cerr << "obj_exts: ";
  print_stringvec(cerr, m_obj_exts);
  cerr << "\n";

  cerr << "objdir: '" << m_objdir << "'\n\n";
}

int cppdeps::is_cc_filename(const char* fname)
{
  const unsigned int len = strlen(fname);
  if (len > 3)
    {
      if (strcmp(".c", fname+len-2) == 0) return 2;
      if (strcmp(".C", fname+len-2) == 0) return 2;
    }
  if (len > 4)
    {
      if (strcmp(".cc", fname+len-3) == 0) return 3;
    }
  if (len > 5)
    {
      if (strcmp(".cpp", fname+len-3) == 0) return 4;
    }
  return 0;
}

int cppdeps::is_cc_or_h_filename(const char* fname)
{
  int n = is_cc_filename(fname);
  if (n > 0) return n;

  const unsigned int len = strlen(fname);
  if (len > 3)
    {
      if (strcmp(".h", fname+len-2) == 0) return 2;
      if (strcmp(".H", fname+len-2) == 0) return 2;
    }
  if (len > 4)
    {
      if (strcmp(".hh", fname+len-3) == 0) return 3;
    }
  if (len > 5)
    {
      if (strcmp(".hpp", fname+len-4) == 0) return 4;
    }
  return 0;
}

bool cppdeps::resolve_one(const string& include_name,
                          const string& src_fname,
                          const string& dirname_without_slash,
                          const vector<string>& ipath,
                          const vector<string>& literal,
                          cppdeps::dep_list_t& vec)
{
  for (unsigned int i = 0; i < ipath.size(); ++i)
    {
      const string fullpath = join_filename(ipath[i], include_name);

      if (file_exists(fullpath.c_str()))
        {
          vec.push_back(dependency(fullpath, src_fname));
          return true;
        }
    }

  assert(dirname_without_slash.length() > 0); // must be at least '.'
  assert(dirname_without_slash[dirname_without_slash.length()-1] != '/');

  // Try resolving the include by using the directory containing the
  // source file currently being examined.
  const string fullpath =
    join_filename(dirname_without_slash, include_name);

  if (file_exists(fullpath.c_str()))
    {
      vec.push_back(dependency(fullpath, src_fname));
      return true;
    }

  // Try resolving the include by using the current working directory
  // from which this program was invoked.
  if (file_exists(include_name.c_str()))
    {
      if (src_fname != include_name)
        vec.push_back(dependency(include_name, src_fname));
      return true;
    }

  // If all else fails, try to see if we can consider the included file as
  // a literal file:
  for (unsigned int i = 0; i < literal.size(); ++i)
    {
      const char* extension =
        include_name.c_str() + include_name.length() - literal[i].length();

      if (strncmp(extension, literal[i].c_str(),
                  literal[i].length()) == 0)
        {
          vec.push_back(dependency(include_name, "(literal)", true));
          return true;
        }
    }

  return false;
}

const cppdeps::dep_list_t&
cppdeps::get_direct_includes(const string& src_fname)
{
  {
    dep_map_t::iterator itr = m_direct_includes.find(src_fname);
    if (itr != m_direct_includes.end())
      return (*itr).second;
  }

  const string dirname_without_slash = get_dirname_of(src_fname);

  dep_list_t& vec = m_direct_includes[src_fname];

  mapped_file f(src_fname.c_str());

  if (f.mtime() > m_start_time && !m_quiet)
    {
      cerr << "WARNING: for file " << src_fname << ":\n"
           << "\tmodification time (" << string_time(f.mtime()) << ") is in the future\n"
           << "\tvs. current time  (" << string_time(m_start_time) << ")\n";
    }

  const char* fptr = static_cast<const char*>(f.memory());
  const char* const stop = fptr + f.length();

  bool firsttime = true;

  while (fptr < stop)
    {
      if (!firsttime)
        {
          while (fptr < stop && *fptr != '\n')
            ++fptr;

          assert(!(fptr > stop));

          if (fptr == stop)
            break;

          assert(*fptr == '\n');
          ++fptr;
        }

      firsttime = false;

      if (fptr >= stop)
        break;

      // OK, at this point we are guaranteed to be at the beginning of
      // a line (either we're at the beginning of the file, or else
      // we've just skipped over a line terminator).

      if (*fptr != '#')
        continue;

      ++fptr;

      while (isspace(*fptr) && fptr < stop)
        ++fptr;

      if (*fptr++ != 'i') continue;
      if (*fptr++ != 'n') continue;
      if (*fptr++ != 'c') continue;
      if (*fptr++ != 'l') continue;
      if (*fptr++ != 'u') continue;
      if (*fptr++ != 'd') continue;
      if (*fptr++ != 'e') continue;

      while (isspace(*fptr) && fptr < stop)
        ++fptr;

      const char delimiter = *fptr++;

      const bool is_valid_delimiter =
        (delimiter == '\"') ||
        (delimiter == '<' && m_check_sys_includes == true);

      if (!is_valid_delimiter)
        continue;

      const char* const include_start = fptr;

      switch (delimiter)
        {
        case '\"':
          while (*fptr != '\"' && fptr < stop)
            ++fptr;
          break;
        case '<':
          while (*fptr != '>' && fptr < stop)
            ++fptr;
          break;
        default:
          cerr << "unknown delimiter '" << delimiter << "'\n";
          exit(1);
          break;
        }

      if (fptr >= stop)
        {
          cerr << "premature end-of-file; runaway #include directive?\n";
          exit(1);
        }

      // include_start and include_length together specify the piece
      // of text inside the #include "..." or #include <...> -- we
      // need to keep track of include_length because include_start is
      // not null-terminated, since it's just pointing into the middle
      // of some mmap'ed file

      const int include_length = fptr - include_start;
      const string include_name(include_start, include_length);

      if (resolve_one(include_name, src_fname,
                      dirname_without_slash, m_user_ipath,
                      m_literal_exts, vec))
        continue;

      if (m_check_sys_includes &&
          resolve_one(include_name, src_fname,
                      dirname_without_slash, m_sys_ipath,
                      m_literal_exts, vec))
        continue;

      if (!m_quiet)
        {
          cerr << "WARNING: in " << src_fname
               << ": couldn\'t resolve #include \""
               << include_name << "\"\n";

          cerr << "\twith search path: ";
          print_stringvec(cerr, m_user_ipath);
          cerr << "\n";

          if (m_check_sys_includes)
            {
              cerr << "\and system search path: [";
              for (unsigned int i = 0; i < m_user_ipath.size(); ++i)
                {
                  cerr << "'" << m_user_ipath[i] << "'";
                  if (i+1 < m_user_ipath.size())
                    cerr << ", ";
                }
              cerr << "]\n";
            }
        }
    }

  return vec;
}

const cppdeps::dep_list_t&
cppdeps::get_nested_includes(const string& src_fname)
{
  {
    dep_map_t::iterator itr = m_nested_includes.find(src_fname);
    if (itr != m_nested_includes.end())
      return (*itr).second;
  }

  if (m_parse_states[src_fname] == IN_PROGRESS)
    {
      cerr << "ERROR: in " << src_fname
           << ": untrapped nested #include recursion\n";
      exit(1);
    }

  m_parse_states[src_fname] = IN_PROGRESS;

  // use a set to build up the list of #includes, so that
  //   (1) we automatically avoid duplicates
  //   (2) we get sorting for free
  //
  // this turns out to be cheaper than building up the list in a
  // std::vector and then doing a big std::sort, std::unique(), and
  // vec.erase() at the end
  std::set<dependency> includes_set;

  includes_set.insert(src_fname);

  const dep_list_t& direct = get_direct_includes(src_fname);

  for (dep_list_t::const_iterator
         i = direct.begin(),
         istop = direct.end();
       i != istop;
       ++i)
    {
      // Check for self-inclusion to avoid infinite recursion.
      if ((*i).target == src_fname)
        continue;

      // Check if the included file is to be treated as a 'literal' file,
      // meaning that we don't look for nested includes, and thus don't
      // require the file to currently exist. This is useful for handling
      // files that are generated by an intermediate rule in some makefile.
      if ((*i).literal == true)
        {
          includes_set.insert((*i).target);
          continue;
        }

      // Check for other recursion cycles
      if (m_parse_states[(*i).target] == IN_PROGRESS)
        {
          if (!m_quiet)
            cerr << "WARNING: in " << src_fname
                 << ": recursive #include cycle with " << (*i).target << "\n";
          continue;
        }

      const dep_list_t& indirect = get_nested_includes((*i).target);
      includes_set.insert(indirect.begin(), indirect.end());
    }

  dep_list_t& result = m_nested_includes[src_fname];
  assert(result.empty());
  result.assign(includes_set.begin(), includes_set.end());

  m_parse_states[src_fname] = COMPLETE;

  return result;
}

void cppdeps::print_makefile_dep(const string& fname)
{
  const int ext_len = is_cc_filename(fname.c_str());

  if (ext_len == 0)
    // it's not a c++ file, so just do nothing
    return;

  // otherwise, we do have a c++ file...

  // Remove the trailing filename suffix (i.e. '.C', '.cc', ...)
  string fname_stem = fname.substr(0, fname.length()-ext_len);

  // Remove a leading directory prefix if necessary
  if (fname_stem.compare(0, m_strip_prefix.length(),
                         m_strip_prefix) == 0)
    {
      fname_stem.erase(0, m_strip_prefix.length() + 1);
    }

  // Make sure that m_objdir ends with a slash if it is non-empty, so
  // that we can join it to fname_stem and make a proper
  // pathname.
  if (m_objdir.length() > 0
      && m_objdir[m_objdir.length()-1] != '/')
    {
      m_objdir += '/';
    }

  assert(m_obj_exts.size() > 0);

  // Use C-style stdio here since it came out running quite a bit
  // faster than iostreams, at least under g++-3.2.
  printf("%s%s%s",
         m_objdir.c_str(),
         fname_stem.c_str(),
         m_obj_exts[0].c_str());

  for (unsigned int i = 1; i < m_obj_exts.size(); ++i)
    {
      printf(" %s%s%s",
             m_objdir.c_str(),
             fname_stem.c_str(),
             m_obj_exts[i].c_str());
    }

  printf(": ");

  const dep_list_t& includes = get_nested_includes(fname);

  for (dep_list_t::const_iterator
         itr = includes.begin(),
         stop = includes.end();
       itr != stop;
       ++itr)
    {
      printf(" %s", (*itr).target.c_str());
    }

  printf("\n");
}

void cppdeps::print_include_tree(const string& fname)
{
  const int ext_len = is_cc_or_h_filename(fname.c_str());

  if (ext_len == 0)
    // it's not a c++ file, so just do nothing
    return;

  // otherwise, we do have a c++ file

  const dep_list_t& includes =
    m_output_mode == DIRECT_INCLUDE_TREE
    ? get_direct_includes(fname)
    : get_nested_includes(fname);

  printf("%s:: ", fname.c_str());

  for (dep_list_t::const_iterator
         itr = includes.begin(),
         stop = includes.end();
       itr != stop;
       ++itr)
    {
      printf(" %s:%s",
             (*itr).target.c_str(),
             (*itr).source.c_str());
    }

  printf("\n");
}

void cppdeps::traverse_sources(output_func handler)
{
  // start off with a copy of m_src_files
  vector<string> files (m_src_files);

  while (!files.empty())
    {
      const string current_file = files.back();
      files.pop_back();

      if (is_directory(current_file.c_str()))
        {
          errno = 0;
          DIR* d = opendir(current_file.c_str());
          if (d == 0)
            {
              cerr << strerror(errno) << "\n"
                   << "ERROR: couldn't read directory: "
                   << current_file.c_str() << "\n";
              exit(1);
            }

          for (dirent* e = readdir(d); e != 0; e = readdir(d))
            {
              if (should_ignore_file(e->d_name))
                continue;

              files.push_back(join_filename(current_file, e->d_name));
            }

          closedir(d);
        }
      else
        {
          (this->*handler)(current_file);
        }
    }
}

void cppdeps::batch_build()
{
  switch (m_output_mode)
    {
    case MAKEFILE_DEPS:
      {
        printf("%s\n",
               "# Do not edit this file! "
               "It is automatically generated. "
               "Changes will be lost.");
        traverse_sources(&cppdeps::print_makefile_dep);
      }
      break;

    case DIRECT_INCLUDE_TREE:
    case NESTED_INCLUDE_TREE:
      {
        traverse_sources(&cppdeps::print_include_tree);
      }
      break;
    }
}

int main(int argc, char** argv)
{
  cppdeps dc(argc, argv);
  dc.batch_build();
  exit(0);
}

static const char vcid_cppdeps_cc[] = "$Header$";
#endif // !CPPDEPS_CC_DEFINED
