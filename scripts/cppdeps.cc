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
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

using std::vector;
using std::map;
using std::set;
using std::string;
using std::cerr;

namespace
{
  // helper functions

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

  class mapped_file
  {
  public:
    mapped_file(const char* filename)
      :
      itsFd(0),
      itsMem(0)
    {
      errno = 0;

      struct stat statbuf;
      if (stat(filename, &statbuf) == -1)
        {
          cerr << "stat() failed for file " << filename << ":\n";
          cerr << strerror(errno) << "\n";
          exit(1);
        }

      itsLength = statbuf.st_size;

      itsFd = open(filename, O_RDONLY);
      if (itsFd == -1)
        {
          cerr << "open() failed for file " << filename << ":\n";
          cerr << strerror(errno) << "\n";
          exit(1);
        }

      itsMem = mmap(0, itsLength, PROT_READ, MAP_PRIVATE, itsFd, 0);

      if (itsMem == (void*)-1)
        {
          cerr << "mmap() failed for file " << filename << ":\n";
          cerr << strerror(errno) << "\n";
          exit(1);
        }
    }

    ~mapped_file()
    {
      munmap(itsMem, itsLength);
      close(itsFd);
    }

    const void* memory() const { return itsMem; }

    const off_t length() const { return itsLength; }

  private:
    mapped_file(const mapped_file&);
    mapped_file& operator=(const mapped_file&);

    off_t itsLength;
    int   itsFd;
    void* itsMem;

  }; // end class mapped_file

} // end unnamed namespace

struct include_spec
{
  include_spec(const string& t, const string& s = string())
    : target(t), source(s)
  {}

  string target;
  string source;
};

bool operator<(const include_spec& i1, const include_spec& i2)
{
  return i1.target < i2.target;
}

/// A class for doing fast include-file dependency analysis.
/** Several shortcuts (er, hacks...) are taken to make the parsing
    extremely fast and cheap, but at worst this makes the computed
    dependencies be unnecessarily pessimistic. For example, a #include that
    occurs inside a comment will still be treated as a regular #include.*/
class cppdeps
{
  vector<string> m_user_ipath;
  vector<string> m_sys_ipath;

  vector<string> m_src_files;
  string m_objdir;

  typedef vector<include_spec> include_list_t;

  typedef map<string, include_list_t> include_map_t;
  include_map_t m_nested_includes;
  include_map_t m_direct_includes;

  enum parse_state
    {
      NOT_STARTED = 0,
      IN_PROGRESS = 1,
      COMPLETE = 2
    };

  map<string, parse_state> m_parse_states;

  bool m_check_sys_includes;
  bool m_quiet;

  enum output_mode
    {
      MAKEFILE_DEPS,
      DIRECT_INCLUDE_TREE,
      NESTED_INCLUDE_TREE
    };

  output_mode m_output_mode;

  string m_strip_prefix;

public:
  cppdeps(int argc, char** argv);

  void inspect();

  static string trim_filename(const string& inp);

  int is_cc_filename(const char* fname);

  int is_cc_or_h_filename(const char* fname);

  static bool resolve_one(const char* include_name,
                          int include_length,
                          const string& src_fname,
                          const string& dirname,
                          const vector<string>& ipath,
                          include_list_t& vec);

  const include_list_t& get_direct_includes(const string& src_fname);

  const include_list_t& get_nested_includes(const string& src_fname);

  void batch_build();
};

cppdeps::cppdeps(int argc, char** argv) :
  m_check_sys_includes(false),
  m_quiet(false),
  m_output_mode(MAKEFILE_DEPS)
{
  if (argc == 1)
    {
      printf
        ("usage: %s [options] srcdir...\n"
         "\n"
         "options:\n"
         "    --includedir [dir]    specify a directory to be searched when resolving\n"
         "                          #include \"...\" directives\n"
         "    --sysincludedir [dir] specify a directory to be searched when resolving\n"
         "                          #include <...> directives\n"
         "    --objdir [dir]        specify a path prefix indicating where the object\n"
         "                          (.o) files should be placed (the default\n"
         "                          is no prefix, or just './')\n"
         "    --checksys            force tracking of dependencies in #include <...>\n"
         "                          directives (default is to not record <...> files\n"
         "                          as dependencies)\n"
         "    --quiet               suppress warnings\n"
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

  ++argv; // skip to first command-line arg

  while (*argv != 0)
    {
      if (strcmp(*argv, "--includedir") == 0)
        {
          m_user_ipath.push_back(*++argv);
        }
      else if (strcmp(*argv, "--sysincludedir") == 0)
        {
          m_sys_ipath.push_back(*++argv);
        }
      else if (strcmp(*argv, "--objdir") == 0)
        {
          m_objdir = trim_filename(*++argv);
        }
      else if (strcmp(*argv, "--checksys") == 0)
        {
          m_check_sys_includes = true;
        }
      else if (strcmp(*argv, "--quiet") == 0)
        {
          m_quiet = true;
        }
      else if (strcmp(*argv, "--output-direct-includes") == 0)
        {
          m_output_mode = DIRECT_INCLUDE_TREE;
        }
      else if (strcmp(*argv, "--output-nested-includes") == 0)
        {
          m_output_mode = NESTED_INCLUDE_TREE;
        }
      // treat any unrecognized arguments as src files
      else
        {
          const string fname = trim_filename(*argv);
          if (!file_exists(fname.c_str()))
            {
              cerr << "ERROR: no such source file: " << fname << '\n';
              exit(1);
            }
          m_src_files.push_back(fname);
          if (is_directory(fname.c_str()))
            {
              m_user_ipath.push_back(fname);
              m_strip_prefix = fname;
            }
        }
      ++argv;
    }
}

void cppdeps::inspect()
{
  cerr << "user_ipath:\n";
  for (unsigned int i = 0; i < m_user_ipath.size(); ++i)
    cerr << '\t' << m_user_ipath[i] << '\n';

  cerr << "\nsys_ipath:\n";
  for (unsigned int i = 0; i < m_sys_ipath.size(); ++i)
    cerr << '\t' << m_sys_ipath[i] << '\n';

  cerr << "\nsources:\n";
  for (unsigned int i = 0; i < m_src_files.size(); ++i)
    cerr << '\t' << m_src_files[i] << '\n';

  cerr << "\nobjdir: " << m_objdir << '\n';
}

string cppdeps::trim_filename(const string& inp)
{
  string result = inp;
  if (result[0] == '.' && result[1] == '/')
    {
      result.erase(0,2);
    }

  while (result.length() > 1 && result[result.length()-1] == '/')
    {
      result.erase(result.length()-1,1);
    }

  return result;
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

bool cppdeps::resolve_one(const char* include_name,
                          int include_length,
                          const string& src_fname,
                          const string& dirname_with_slash,
                          const vector<string>& ipath,
                          cppdeps::include_list_t& vec)
{
  for (unsigned int i = 0; i < ipath.size(); ++i)
    {
      string fullpath = ipath[i];
      fullpath += '/';
      fullpath.append(include_name, include_length);

      if (file_exists(fullpath.c_str()))
        {
          vec.push_back(include_spec(fullpath, src_fname));
          return true;
        }
    }

  // Try resolving the include by using the directory containing the
  // source file currently being examined.
  string fullpath = dirname_with_slash;
  fullpath.append(include_name, include_length);

  if (file_exists(fullpath.c_str()))
    {
      vec.push_back(include_spec(fullpath, src_fname));
      return true;
    }

  string include_string(include_name, include_length);

  // Try resolving the include by using the current working directory
  // from which this program was invoked.
  if (file_exists(include_string.c_str()))
    {
      if (src_fname != include_string)
        vec.push_back(include_spec(include_string, src_fname));
      return true;
    }

  return false;
}

const cppdeps::include_list_t&
cppdeps::get_direct_includes(const string& src_fname)
{
  {
    include_map_t::iterator itr = m_direct_includes.find(src_fname);
    if (itr != m_direct_includes.end())
      return (*itr).second;
  }

  string dirname_with_slash =
    src_fname.substr(0, src_fname.find_last_of('/'));
  dirname_with_slash += '/';

  include_list_t& vec = m_direct_includes[src_fname];

  mapped_file f(src_fname.c_str());

  const char* fptr = static_cast<const char*>(f.memory());
  const char* const stop = fptr + f.length();

  // need to keep track of a separate location where we can backup in case
  // we see a partial but eventually failed match
  const char* mark = fptr;

  for ( ; fptr < stop; ++fptr)
    {
      fptr = mark; // backup to site of previously failed match

      while (fptr < stop && *fptr != '#')
        ++fptr;

      if (fptr >= stop)
        break;

      // assert(*fptr == '#');

      ++fptr;

      mark = fptr; // note point of current match for backup purposes

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

      const char* const include_name = fptr;

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

      const int include_length = fptr - include_name;

      if (resolve_one(include_name, include_length, src_fname,
                      dirname_with_slash, m_user_ipath, vec))
        continue;

      if (m_check_sys_includes &&
          resolve_one(include_name, include_length, src_fname,
                      dirname_with_slash, m_sys_ipath, vec))
        continue;

      const string include_string(include_name, include_length);
      if (!m_quiet)
        cerr << "WARNING: in " << src_fname
             << ": couldn\'t resolve #include \""
             << include_string << "\"\n";
    }

  return vec;
}

const cppdeps::include_list_t&
cppdeps::get_nested_includes(const string& src_fname)
{
  {
    include_map_t::iterator itr = m_nested_includes.find(src_fname);
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
  std::set<include_spec> includes_set;

  includes_set.insert(src_fname);

  const include_list_t& direct = get_direct_includes(src_fname);

  for (include_list_t::const_iterator
         i = direct.begin(),
         istop = direct.end();
       i != istop;
       ++i)
    {
      // Check for self-inclusion to avoid infinite recursion.
      if ((*i).target == src_fname)
        continue;

      // Check for other recursion cycles
      if (m_parse_states[(*i).target] == IN_PROGRESS)
        {
          if (!m_quiet)
            cerr << "WARNING: in " << src_fname
                 << ": recursive #include cycle with " << (*i).target << "\n";
        }
      else
        {
          const include_list_t& indirect = get_nested_includes((*i).target);
          includes_set.insert(indirect.begin(), indirect.end());
        }
    }

  include_list_t& result = m_nested_includes[src_fname];
  assert(result.empty());
  result.assign(includes_set.begin(), includes_set.end());

  m_parse_states[src_fname] = COMPLETE;

  return result;
}

void cppdeps::batch_build()
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

              string pathname = current_file;
              pathname += '/';
              pathname += e->d_name;
              files.push_back(pathname);
            }

          closedir(d);
        }
      else
        {
          switch (m_output_mode)
            {
            case MAKEFILE_DEPS:
              {
                const int ext_len = is_cc_filename(current_file.c_str());
                if (ext_len > 0)
                  {
                    string stripped_filename = current_file;

                    if (strncmp(current_file.c_str(),
                                m_strip_prefix.c_str(),
                                m_strip_prefix.length()) == 0)
                      {
                        const unsigned int offset = m_strip_prefix.length() + 1;

                        stripped_filename =
                          current_file.substr(offset,
                                              current_file.length()-offset-ext_len);
                      }

                    // Make sure that m_objdir ends with a slash if it is
                    // non-empty, so that we can join it to
                    // stripped_filename and make a proper pathname.
                    if (m_objdir.length() > 0
                        && m_objdir[m_objdir.length()-1] != '/')
                      {
                        m_objdir += '/';
                      }

                    // Use C-style stdio here since it came out running
                    // quite a bit faster than iostreams, at least under
                    // g++-3.2.
                    printf("%s%s.o: ",
                           m_objdir.c_str(),
                           stripped_filename.c_str());

                    const include_list_t& includes =
                      get_nested_includes(current_file);

                    for (include_list_t::const_iterator
                           itr = includes.begin(),
                           stop = includes.end();
                         itr != stop;
                         ++itr)
                      {
                        printf(" %s", (*itr).target.c_str());
                      }

                    printf("\n");
                  }
              }
              break;
            case DIRECT_INCLUDE_TREE:
            case NESTED_INCLUDE_TREE:
              {
                const int ext_len = is_cc_or_h_filename(current_file.c_str());
                if (ext_len > 0)
                  {
                    const include_list_t& includes =
                      m_output_mode == DIRECT_INCLUDE_TREE
                      ? get_direct_includes(current_file)
                      : get_nested_includes(current_file);

                    printf("%s:: ", current_file.c_str());

                    for (include_list_t::const_iterator
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
              }
              break;
            }
        }
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
