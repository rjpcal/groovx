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

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
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

/// A class for doing fast include-file dependency analysis.
/** Several shortcuts (er, hacks...) are taken to make the parsing
    extremely fast and cheap, but at worst this makes the computed
    dependencies be unnecessarily pessimistic. For example, a #include that
    occurs inside a comment will still be treated as a regular #include.*/
class cppdeps
{
  vector<string> m_user_ipath;
  vector<string> m_sys_ipath;

  string m_srcdir;
  string m_objdir;

  typedef vector<string> include_list_t;

  typedef map<string, include_list_t> include_map_t;
  include_map_t m_nested_includes;
  include_map_t m_direct_includes;

  enum ParseState
    {
      NOT_STARTED = 0,
      IN_PROGRESS = 1,
      COMPLETE = 2
    };

  map<string, ParseState> m_parse_states;

  bool m_check_sys_includes;
  bool m_quiet;

public:
  cppdeps(char** argv);

  void inspect();

  string trim_dirname(const string& inp);

  int is_cc_filename(const char* fname);

  static bool resolve_one(const char* include_name,
                          int include_length,
                          const string& filename,
                          const string& dirname,
                          const vector<string>& ipath,
                          include_list_t& vec);

  const include_list_t& get_direct_includes(const string& filename);

  const include_list_t& get_nested_includes(const string& filename);

  void batch_build();
};

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
        exit(1);
      }

    return S_ISDIR(statbuf.st_mode);
  }

  bool ignore_directory(const char* dirname)
  {
    return (dirname[0] == '.' ||
            strcmp(dirname, "RCS") == 0 ||
            strcmp(dirname, "CVS") == 0);
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
  };
}

cppdeps::cppdeps(char** argv) :
  m_check_sys_includes(false),
  m_quiet(false)
{
  m_sys_ipath.push_back("/usr/include");
  m_sys_ipath.push_back("/usr/include/linux");
  m_sys_ipath.push_back("/usr/local/matlab/extern/include");

  while (*argv != 0)
    {
      if (strcmp(*argv, "--include") == 0)
        {
          m_user_ipath.push_back(*++argv);
        }
      else if (strcmp(*argv, "--sysinclude") == 0)
        {
          m_sys_ipath.push_back(*++argv);
        }
      else if (strcmp(*argv, "--src") == 0)
        {
          m_srcdir = trim_dirname(*++argv);
          m_user_ipath.push_back(m_srcdir);
        }
      else if (strcmp(*argv, "--objdir") == 0)
        {
          m_objdir = trim_dirname(*++argv);
        }
      else if (strcmp(*argv, "--checksys") == 0)
        {
          m_check_sys_includes = true;
        }
      else if (strcmp(*argv, "--quiet") == 0)
        {
          m_quiet = true;
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

  cerr << "\nsrcdir: " << m_srcdir << '\n';
  cerr << "\nobjdir: " << m_objdir << '\n';
}

string cppdeps::trim_dirname(const string& inp)
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
  return 0;
}

bool cppdeps::resolve_one(const char* include_name,
                          int include_length,
                          const string& filename,
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
          vec.push_back(fullpath);
          return true;
        }
    }

  // Try resolving the include by using the directory containing the
  // source file currently being examined.
  string fullpath = dirname_with_slash;
  fullpath.append(include_name, include_length);

  if (file_exists(fullpath.c_str()))
    {
      vec.push_back(fullpath);
      return true;
    }

  string include_string(include_name, include_length);

  // Try resolving the include by using the current working directory
  // from which this program was invoked.
  if (file_exists(include_string.c_str()))
    {
      if (filename != include_string)
        vec.push_back(include_string);
      return true;
    }

  return false;
}

const cppdeps::include_list_t&
cppdeps::get_direct_includes(const string& filename)
{
  {
    include_map_t::iterator itr = m_direct_includes.find(filename);
    if (itr != m_direct_includes.end())
      return (*itr).second;
  }

  string dirname_with_slash = filename.substr(0, filename.find_last_of('/'));
  dirname_with_slash += '/';

  include_list_t& vec = m_direct_includes[filename];

  mapped_file f(filename.c_str());

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

      if (resolve_one(include_name, include_length, filename,
                      dirname_with_slash, m_user_ipath, vec))
        continue;

      if (m_check_sys_includes &&
          resolve_one(include_name, include_length, filename,
                      dirname_with_slash, m_sys_ipath, vec))
        continue;

      const string include_string(include_name, include_length);
      if (!m_quiet)
        cerr << "WARNING: in " << filename
             << ": couldn\'t resolve #include \""
             << include_string << "\"\n";
    }

  return vec;
}

const cppdeps::include_list_t&
cppdeps::get_nested_includes(const string& filename)
{
  {
    include_map_t::iterator itr = m_nested_includes.find(filename);
    if (itr != m_nested_includes.end())
      return (*itr).second;
  }

  if (m_parse_states[filename] == IN_PROGRESS)
    {
      cerr << "ERROR: in " << filename
           << ": untrapped nested #include recursion\n";
      exit(1);
    }

  m_parse_states[filename] = IN_PROGRESS;

  include_list_t vec;

  vec.push_back(filename);

  const include_list_t& direct = get_direct_includes(filename);

  for (include_list_t::const_iterator
         i = direct.begin(),
         istop = direct.end();
       i != istop;
       ++i)
    {
      // Check for self-inclusion to avoid infinite recursion.
      if (*i == filename)
        continue;

      // Check for other recursion cycles
      if (m_parse_states[*i] == IN_PROGRESS)
        {
          if (!m_quiet)
            cerr << "WARNING: in " << filename
                 << ": recursive #include cycle with " << *i << "\n";
        }
      else
        {
          const include_list_t& indirect = get_nested_includes(*i);
          vec.insert(vec.end(), indirect.begin(), indirect.end());
        }
    }

  // do a sort and uniquify before storing the result
  std::sort(vec.begin(), vec.end());
  include_list_t::iterator it = std::unique(vec.begin(), vec.end());
  vec.erase(it, vec.end());

  include_list_t& result = m_nested_includes[filename];
  result.swap(vec);

  m_parse_states[filename] = COMPLETE;

  return result;
}

void cppdeps::batch_build()
{
  vector<string> dirs;

  dirs.push_back(m_srcdir);

  const unsigned int offset = m_srcdir.length() + 1;

  while (!dirs.empty())
    {
      const string dirname = dirs.back();
      dirs.pop_back();

      errno = 0;
      DIR* d = opendir(dirname.c_str());
      if (d == 0)
        {
          cerr << strerror(errno) << "\n";
          cerr << "not a directory: " << dirname.c_str() << "\n";
          exit(1);
        }

      for (dirent* e = readdir(d); e != 0; e = readdir(d))
        {
          if (ignore_directory(e->d_name))
            continue;

          string fullname = dirname;
          fullname += '/';
          fullname += e->d_name;

          if (is_directory(fullname.c_str()))
            {
              dirs.push_back(fullname);
            }
          else
            {
              const int ext_len = is_cc_filename(e->d_name);
              if (ext_len > 0)
                {
                  const include_list_t& includes = get_nested_includes(fullname);

                  // Use C-style stdio here since it came out running quite
                  // a bit faster than iostreams, at least under g++-3.2.
                  printf("%s/%s.o: ",
                         m_objdir.c_str(),
                         fullname.substr(offset, fullname.length()-ext_len-offset).c_str());

                  for (include_list_t::const_iterator
                         itr = includes.begin(),
                         stop = includes.end();
                       itr != stop;
                       ++itr)
                    {
                      printf(" %s", (*itr).c_str());
                    }

                  printf("\n");
                }
            }
        }
      closedir(d);
    }
}

int main(int argc, char** argv)
{
  cppdeps dc(argv+1);
  dc.batch_build();
  exit(0);
}

static const char vcid_cppdeps_cc[] = "$Header$";
#endif // !CPPDEPS_CC_DEFINED
