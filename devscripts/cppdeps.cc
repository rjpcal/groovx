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
#include <fstream>
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
    return (strcmp(fname, ".") == 0 ||
            strcmp(fname, "..") == 0 ||
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

class file_info;

// Typedefs and enums
typedef vector<file_info*>      dep_list_t;
typedef map<string, dep_list_t> dep_map_t;

enum parse_state
  {
    NOT_STARTED = 0,
    IN_PROGRESS = 1,
    COMPLETE = 2
  };

class file_info
{
public:
  static file_info* get(const string& fname)
  {
    info_map_t::iterator p = s_info_map.find(fname);
    if (p != s_info_map.end())
      return (*p).second;

    info_map_t::iterator i =
      s_info_map.insert(info_map_t::value_type
                        (fname, new file_info(fname))).first;

    return (*i).second;
  }

  typedef map<string, file_info*> info_map_t;

  const string                fname;
  const string::size_type     dotpos; // position of the final "."
  const string                rootname; // filename without trailing .extension
  const string                extension; // trailing .extension, including the "."
  const string                dirname_without_slash;
  const file_info*            source;
  bool                        literal; // if true, then don't try to look up nested includes
  parse_state                 cdep_parse_state;
  parse_state                 ldep_parse_state;
  bool                        direct_cdeps_done;
  dep_list_t                  direct_cdeps;
  bool                        nested_cdeps_done;
  dep_list_t                  nested_cdeps;
  bool                        direct_ldeps_done;
  dep_list_t                  direct_ldeps;
  bool                        nested_ldeps_done;
  dep_list_t                  nested_ldeps;

private:
  file_info(const string& t)
    :
    fname(make_normpath(t)),
    dotpos(get_last_of(fname, '.')),
    rootname(fname.substr(0,dotpos)),
    extension(fname.substr(dotpos, string::npos)),
    dirname_without_slash(get_dirname_of(t)),
    source(0),
    literal(false),
    cdep_parse_state(NOT_STARTED),
    ldep_parse_state(NOT_STARTED),
    direct_cdeps_done(false),
    direct_cdeps(),
    nested_cdeps_done(false),
    nested_cdeps(),
    direct_ldeps_done(false),
    direct_ldeps(),
    nested_ldeps_done(false),
    nested_ldeps()
  {}

  string::size_type get_last_of(const string& s, char c)
  {
    string::size_type p = s.find_last_of(c);
    if (p == string::npos) p = s.length();
    return p;
  }

  file_info(const file_info&); // not implemented
  file_info& operator=(const file_info&); // not implemented

  static info_map_t s_info_map;
};

file_info::info_map_t file_info::s_info_map;

struct file_info_cmp
{
  bool operator()(const file_info* f1, const file_info* f2)
  {
    return f1->fname < f2->fname;
  }
};

class formatter
{
private:
  string            m_prefix;
  string            m_link_pattern;
  string::size_type m_wildcard_pos;

public:
  formatter(const string& src, const string& link) :
    m_prefix(src),
    m_link_pattern(link),
    m_wildcard_pos(link.find_first_of('*'))
  {}

  bool matches(const string& srcfile) const
  {
    return strncmp(srcfile.c_str(),
                   m_prefix.c_str(),
                   m_prefix.length()) == 0;
  }

  string transform(const string& srcfile) const
  {
    if (m_wildcard_pos == string::npos)
      return m_link_pattern;

    // else...
    string stem = srcfile.substr(m_prefix.length(), string::npos);
    const string::size_type suff = stem.find_last_of('.');
    if (suff != string::npos)
      stem.erase(suff, string::npos);
    string result = m_link_pattern;
    result.replace(m_wildcard_pos, 1, stem);
    return result;
  }
};

class format_set
{
  vector<formatter> m_links;

public:
  format_set() : m_links() {}

  void add_format(const string& src_colon_pattern)
  {
    string::size_type colon = src_colon_pattern.find_first_of(':');
    if (colon == string::npos)
      {
        cerr << "ERROR: invalid format (missing colon): '"
             << src_colon_pattern << "'\n";
        exit(1);
      }
    string src = src_colon_pattern.substr(0, colon);
    string pattern = src_colon_pattern.substr(colon+1);
    m_links.push_back(formatter(src, pattern));
  }

  void load_file(const char* filename)
  {
    if (!file_exists(filename))
      {
        cerr << "ERROR: couldn't open format file: " << filename << '\n';
        exit(1);
      }

    std::ifstream in(filename);

    if (!in.is_open())
      {
        cerr << "ERROR: couldn't open format file for reading: " << filename << '\n';
        exit(1);
      }

    string line;
    while (in)
      {
        std::getline(in, line);
        if (line.length() > 0 && line[0] != '#')
          {
            add_format(line);
          }
      }
  }

  string transform(const string& srcfile) const
  {
    for (unsigned int i = m_links.size(); i > 0; --i)
      {
        if (m_links[i-1].matches(srcfile))
          return m_links[i-1].transform(srcfile);
      }
    cerr << "ERROR: no patterns matched source file: " << srcfile << '\n';
    exit(1);
    return string(); // can't happen, but placate compiler
  }
};

/// A class for doing fast dependency analysis.
/** Several shortcuts (er, hacks...) are taken to make the parsing
    extremely fast and cheap, but at worst this makes the computed
    dependencies be unnecessarily pessimistic. For example, a #include
    that occurs inside a comment will still be treated as a regular
    #include. */
class cppdeps
{
private:

  static const int MAKEFILE_CDEPS      = (1 << 0);
  static const int DIRECT_INCLUDE_TREE = (1 << 1);
  static const int NESTED_INCLUDE_TREE = (1 << 2);
  static const int MAKEFILE_LDEPS      = (1 << 3);

  // Member variables
  vector<string>           m_cfg_user_ipath;
  vector<string>           m_cfg_sys_ipath;
  vector<string>           m_cfg_literal_exts;
  vector<string>           m_cfg_source_exts;
  vector<string>           m_cfg_header_exts;
  vector<string>           m_cfg_obj_exts;
  string                   m_cfg_obj_prefix;
  format_set               m_cfg_exe_formats;
  format_set               m_cfg_link_formats;
  bool                     m_cfg_check_sys_deps;
  bool                     m_cfg_quiet;
  bool                     m_cfg_verbose;
  int                      m_cfg_output_mode;

  vector<string>           m_src_files;

  string                   m_strip_prefix;

  int                      m_nest_level;

  const time_t             m_start_time;  // so we can check to see if
                                          // any source files have
                                          // timestamps in the future

  std::ostream& warning();

  std::ostream& info();

public:
  cppdeps(int argc, char** argv);

  void inspect(char** arg0, char** argn);

  // Returns true if fname has a c++ source file extension, and
  // assigns the stem (without the extension) to 'stem'.
  bool is_cc_fname(const file_info* finfo) const;

  // Like is_cc_fname(), but also checks for header-file
  // extensions (e.g., '.h').
  bool is_cc_or_h_fname(const file_info* finfo) const;

  // Find the source file that corresponds to the given header file
  file_info* find_source_for_header(file_info* header) const;

  static bool resolve_one(const string& include_name,
                          file_info* finfo,
                          const vector<string>& ipath,
                          const vector<string>& literal,
                          dep_list_t& vec);

  const dep_list_t& get_direct_cdeps(file_info* finfo);
  const dep_list_t& get_nested_cdeps(file_info* finfo);
  const dep_list_t& get_direct_ldeps(file_info* finfo);
  const dep_list_t& get_nested_ldeps(file_info* finfo);

  void print_makefile_dep(file_info* finfo);
  void print_include_tree(file_info* finfo);
  void print_link_deps(file_info* finfo);

  void traverse_sources();
};

std::ostream& cppdeps::warning()
{
  if (m_cfg_verbose)
    for (int i = 0; i < m_nest_level; ++i) cerr << '\t';
  cerr << "WARNING: ";
  return cerr;
}

std::ostream& cppdeps::info()
{
  if (m_cfg_verbose)
    for (int i = 0; i < m_nest_level; ++i) cerr << '\t';
  return cerr;
}

cppdeps::cppdeps(const int argc, char** const argv) :
  m_cfg_check_sys_deps(false),
  m_cfg_quiet(false),
  m_cfg_verbose(false),
  m_cfg_output_mode(0),
  m_nest_level(0),
  m_start_time(time((time_t*) 0))
{
  bool debug = false;

  if (argc == 1)
    {
      printf
        ("usage: %s [options] --srcdir [dir]...\n"
         "\n"
         "options:\n"
         "    --srcdir [dir]        specify a directory containing source files\n"
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
         "    --exeformat\n"
         "    --exeformat-file\n"
         "    --linkformat\n"
         "    --checksys            force tracking of dependencies in #include <...>\n"
         "                          directives (default is to not record <...> files\n"
         "                          as dependencies)\n"
         "    --literal [.ext]      treat files ending in \".ext\" as literal #include\'s\n"
         "    --quiet               suppress warnings\n"
         "    --debug               show contents of internal variables\n"
         "    --verbose             trace the progress of the search algorithms\n"
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

  m_cfg_sys_ipath.push_back("/usr/include");
  m_cfg_sys_ipath.push_back("/usr/include/linux");
  m_cfg_sys_ipath.push_back("/usr/local/matlab/extern/include");

  m_cfg_source_exts.push_back(".cc");
  m_cfg_source_exts.push_back(".C");
  m_cfg_source_exts.push_back(".c");
  m_cfg_source_exts.push_back(".cpp");

  m_cfg_header_exts.push_back(".h");
  m_cfg_header_exts.push_back(".H");
  m_cfg_header_exts.push_back(".hh");
  m_cfg_header_exts.push_back(".hpp");

  m_cfg_exe_formats.add_format(":");

  char** arg = argv+1; // skip to first command-line arg

  for ( ; *arg != 0; ++arg)
    {
      if (strcmp(*arg, "--includedir") == 0)
        {
          m_cfg_user_ipath.push_back(*++arg);
        }
      else if (strncmp(*arg, "-I", 2) == 0)
        {
          m_cfg_user_ipath.push_back((*arg) + 2);
        }
      else if (strcmp(*arg, "--sysincludedir") == 0)
        {
          m_cfg_sys_ipath.push_back(*++arg);
        }
      else if (strcmp(*arg, "--objdir") == 0)
        {
          m_cfg_obj_prefix = trim_trailing_slashes(*++arg);
        }
      else if (strcmp(*arg, "--checksys") == 0)
        {
          m_cfg_check_sys_deps = true;
        }
      else if (strcmp(*arg, "--quiet") == 0)
        {
          m_cfg_quiet = true;
        }
      else if (strcmp(*arg, "--output-direct-includes") == 0)
        {
          m_cfg_output_mode |= DIRECT_INCLUDE_TREE;
        }
      else if (strcmp(*arg, "--output-nested-includes") == 0)
        {
          m_cfg_output_mode |= NESTED_INCLUDE_TREE;
        }
      else if (strcmp(*arg, "--output-compile-deps") == 0)
        {
          m_cfg_output_mode |= MAKEFILE_CDEPS;
        }
      else if (strcmp(*arg, "--output-link-deps") == 0)
        {
          m_cfg_output_mode |= MAKEFILE_LDEPS;
        }
      else if (strcmp(*arg, "--literal") == 0)
        {
          m_cfg_literal_exts.push_back(*++arg);
        }
      else if (strcmp(*arg, "--objext") == 0)
        {
          m_cfg_obj_exts.push_back(*++arg);
        }
      else if (strcmp(*arg, "--exeformat") == 0)
        {
          m_cfg_exe_formats.add_format(*++arg);
        }
      else if (strcmp(*arg, "--exeformat-file") == 0)
        {
          m_cfg_exe_formats.load_file(*++arg);
        }
      else if (strcmp(*arg, "--linkformat") == 0)
        {
          m_cfg_link_formats.add_format(*++arg);
        }
      else if (strcmp(*arg, "--debug") == 0)
        {
          debug = true;
        }
      else if (strcmp(*arg, "--verbose") == 0)
        {
          m_cfg_verbose = true;
        }
      // treat any unrecognized arguments as src files
      else if (strcmp(*arg, "--srcdir") == 0)
        {
          const string fname = trim_trailing_slashes(*++arg);
          if (!file_exists(fname.c_str()))
            {
              cerr << "ERROR: no such source file: '" << fname << "'\n";
              exit(1);
            }
          m_src_files.push_back(fname);
          if (is_directory(fname.c_str()))
            {
              m_cfg_user_ipath.push_back(fname);
              m_strip_prefix = fname;
            }
        }
      else
        {
          cerr << "ERROR: unrecognized command-line option: "
               << *arg << '\n';
          exit(1);
        }

      if (debug)
        {
          inspect(argv, arg);
        }
    }

  if (m_src_files.size() == 0)
    {
      cerr << "ERROR: no source directories specified (use --srcdir)\n";
      exit(1);
    }

  if (m_cfg_output_mode == 0)
    m_cfg_output_mode = MAKEFILE_CDEPS;

  // If the user didn't specify any object-filename extensions, then
  // we just use the default '.o'.
  if (m_cfg_obj_exts.size() == 0)
    m_cfg_obj_exts.push_back(".o");

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
  print_stringvec(cerr, m_cfg_user_ipath);
  cerr << "\n";

  cerr << "sys_ipath: ";
  print_stringvec(cerr, m_cfg_sys_ipath);
  cerr << "\n";

  cerr << "sources: ";
  print_stringvec(cerr, m_src_files);
  cerr << "\n";

  cerr << "literal_exts: ";
  print_stringvec(cerr, m_cfg_literal_exts);
  cerr << "\n";

  cerr << "obj_exts: ";
  print_stringvec(cerr, m_cfg_obj_exts);
  cerr << "\n";

  cerr << "obj_prefix: '" << m_cfg_obj_prefix << "'\n\n";
}

bool cppdeps::is_cc_fname(const file_info* finfo) const
{
  for (unsigned int i = 0; i < m_cfg_source_exts.size(); ++i)
    {
      if (finfo->extension == m_cfg_source_exts[i])
        {
          return true;
        }
    }

  return false;
}

bool cppdeps::is_cc_or_h_fname(const file_info* finfo) const
{
  for (unsigned int i = 0; i < m_cfg_source_exts.size(); ++i)
    {
      if (finfo->extension == m_cfg_source_exts[i])
        {
          return true;
        }
    }

  for (unsigned int i = 0; i < m_cfg_header_exts.size(); ++i)
    {
      if (finfo->extension == m_cfg_header_exts[i])
        {
          return true;
        }
    }

  return false;
}

file_info* cppdeps::find_source_for_header(file_info* header) const
{
  for (unsigned int i = 0; i < m_cfg_source_exts.size(); ++i)
    {
      const string result = header->rootname + m_cfg_source_exts[i];
      if (file_exists(result.c_str()))
        return file_info::get(result);
    }

  return 0;
}

bool cppdeps::resolve_one(const string& include_name,
                          file_info* finfo,
                          const vector<string>& ipath,
                          const vector<string>& literal,
                          dep_list_t& vec)
{
  for (unsigned int i = 0; i < ipath.size(); ++i)
    {
      const string fullpath = join_filename(ipath[i], include_name);

      if (file_exists(fullpath.c_str()))
        {
          vec.push_back(file_info::get(fullpath));
          vec.back()->source = finfo;
          return true;
        }
    }

  assert(finfo->dirname_without_slash.length() > 0); // must be at least '.'
  assert(finfo->dirname_without_slash[finfo->dirname_without_slash.length()-1] != '/');

  // Try resolving the include by using the directory containing the
  // source file currently being examined.
  const string fullpath =
    join_filename(finfo->dirname_without_slash, include_name);

  if (file_exists(fullpath.c_str()))
    {
      vec.push_back(file_info::get(fullpath));
      vec.back()->source = finfo;
      return true;
    }

  // Try resolving the include by using the current working directory
  // from which this program was invoked.
  if (file_exists(include_name.c_str()))
    {
      if (finfo->fname != include_name)
        {
          vec.push_back(file_info::get(include_name));
          vec.back()->source = finfo;
        }
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
          vec.push_back(file_info::get(include_name));
          vec.back()->literal = true;
          return true;
        }
    }

  return false;
}

const dep_list_t& cppdeps::get_direct_cdeps(file_info* finfo)
{
  if (finfo->direct_cdeps_done)
    return finfo->direct_cdeps;

  dep_list_t& vec = finfo->direct_cdeps;

  mapped_file f(finfo->fname.c_str());

  if (f.mtime() > m_start_time && !m_cfg_quiet)
    {
      warning() << "for file " << finfo->fname << ":\n"
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
        (delimiter == '<' && m_cfg_check_sys_deps == true);

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

      if (resolve_one(include_name, finfo,
                      m_cfg_user_ipath,
                      m_cfg_literal_exts, vec))
        continue;

      if (m_cfg_check_sys_deps &&
          resolve_one(include_name, finfo,
                      m_cfg_sys_ipath,
                      m_cfg_literal_exts, vec))
        continue;

      if (!m_cfg_quiet)
        {
          warning() << "in " << finfo->fname
                    << ": couldn\'t resolve #include \""
                    << include_name << "\"\n";

          info() << "\twith search path: ";
          print_stringvec(cerr, m_cfg_user_ipath);
          cerr << '\n';

          if (m_cfg_check_sys_deps)
            {
              info() << "\tand system search path: ";
              print_stringvec(cerr, m_cfg_sys_ipath);
              cerr << '\n';
            }
        }
    }

  finfo->direct_cdeps_done = true;

  return vec;
}

const dep_list_t& cppdeps::get_nested_cdeps(file_info* finfo)
{
  if (finfo->nested_cdeps_done)
    return finfo->nested_cdeps;

  if (finfo->cdep_parse_state == IN_PROGRESS)
    {
      cerr << "ERROR: in " << finfo->fname
           << ": untrapped nested #include recursion\n";
      exit(1);
    }

  finfo->cdep_parse_state = IN_PROGRESS;

  // use a set to build up the list of #includes, so that
  //   (1) we automatically avoid duplicates
  //   (2) we get sorting for free
  //
  // this turns out to be cheaper than building up the list in a
  // std::vector and then doing a big std::sort, std::unique(), and
  // vec.erase() at the end
  std::set<file_info*, file_info_cmp> dep_set;

  dep_set.insert(finfo);

  const dep_list_t& direct = get_direct_cdeps(finfo);

  for (dep_list_t::const_iterator
         i = direct.begin(),
         istop = direct.end();
       i != istop;
       ++i)
    {
      // Check for self-inclusion to avoid infinite recursion.
      if (*i == finfo)
        continue;

      // Check if the included file is to be treated as a 'literal' file,
      // meaning that we don't look for nested includes, and thus don't
      // require the file to currently exist. This is useful for handling
      // files that are generated by an intermediate rule in some makefile.
      if ((*i)->literal == true)
        {
          dep_set.insert(*i);
          continue;
        }

      // Check for other recursion cycles
      if ((*i)->cdep_parse_state == IN_PROGRESS)
        {
          if (!m_cfg_quiet)
            warning() << "in " << finfo->fname
                      << ": recursive #include cycle with "
                      << (*i)->fname << "\n";
          continue;
        }

      ++m_nest_level;
      const dep_list_t& indirect = get_nested_cdeps(*i);
      --m_nest_level;
      dep_set.insert(indirect.begin(), indirect.end());
    }

  assert(finfo->nested_cdeps.empty());
  finfo->nested_cdeps.assign(dep_set.begin(), dep_set.end());

  finfo->nested_cdeps_done = true;
  finfo->cdep_parse_state = COMPLETE;

  return finfo->nested_cdeps;
}

const dep_list_t& cppdeps::get_direct_ldeps(file_info* finfo)
{
  if (finfo->direct_ldeps_done)
    return finfo->direct_ldeps;

  std::set<file_info*, file_info_cmp> deps_set;

  ++m_nest_level;
  const dep_list_t& cdeps = get_nested_cdeps(finfo);
  --m_nest_level;

  for (dep_list_t::const_iterator
         i = cdeps.begin(),
         istop = cdeps.end();
       i != istop;
       ++i)
    {
      file_info* ccfile = find_source_for_header(*i);
      if (ccfile == 0)
        continue;

      if (ccfile == finfo)
        continue;

      deps_set.insert(ccfile);
    }

  assert(finfo->direct_ldeps.empty());
  finfo->direct_ldeps.assign(deps_set.begin(), deps_set.end());

  finfo->direct_ldeps_done = true;

  return finfo->direct_ldeps;
}

const dep_list_t& cppdeps::get_nested_ldeps(file_info* finfo)
{
  if (finfo->nested_ldeps_done)
    return finfo->nested_ldeps;

  if (finfo->ldep_parse_state == IN_PROGRESS)
    {
      cerr << "ERROR: in " << finfo->fname
           << ": untrapped nested link-dep recursion\n";
      exit(1);
    }

  finfo->ldep_parse_state = IN_PROGRESS;

  if (m_cfg_verbose)
    {
      info() << "start ldeps for " << finfo->fname << '\n';
    }

  std::set<file_info*, file_info_cmp> deps_set;

  vector<file_info*> to_handle;

  to_handle.push_back(finfo);

  while (to_handle.size() > 0)
    {
      file_info* f = to_handle.back();
      to_handle.pop_back();

      if (deps_set.find(f) != deps_set.end())
        continue;

      deps_set.insert(f);

      const dep_list_t& direct = get_direct_ldeps(f);

      for (dep_list_t::const_iterator
             itr = direct.begin(),
             stop = direct.end();
           itr != stop;
           ++itr)
        {
          if (*itr == finfo)
            {
              if (!m_cfg_quiet)
                warning() << " in " << finfo->fname
                          << ": recursive link-dep cycle with "
                          << (*itr)->fname << "\n";
              continue;
            }

          to_handle.push_back(*itr);
        }
    }

  if (m_cfg_verbose)
    {
      info() << "...end ldeps for " << finfo->fname << '\n';
    }

  assert(finfo->nested_ldeps.empty());
  finfo->nested_ldeps.assign(deps_set.begin(), deps_set.end());

  finfo->ldep_parse_state = COMPLETE;
  finfo->nested_ldeps_done = true;

  return finfo->nested_ldeps;
}

void cppdeps::print_makefile_dep(file_info* finfo)
{
  if (!is_cc_fname(finfo))
    return;

  string stem = finfo->rootname;

  // Remove a leading directory prefix if necessary
  if (stem.compare(0, m_strip_prefix.length(),
                   m_strip_prefix) == 0)
    {
      stem.erase(0, m_strip_prefix.length() + 1);
    }

  // Make sure that m_cfg_obj_prefix ends with a slash if it is
  // non-empty, so that we can join it to stem and make a proper
  // pathname.
  if (m_cfg_obj_prefix.length() > 0
      && m_cfg_obj_prefix[m_cfg_obj_prefix.length()-1] != '/')
    {
      m_cfg_obj_prefix += '/';
    }

  assert(m_cfg_obj_exts.size() > 0);

  // Use C-style stdio here since it came out running quite a bit
  // faster than iostreams, at least under g++-3.2.
  printf("%s%s%s",
         m_cfg_obj_prefix.c_str(),
         stem.c_str(),
         m_cfg_obj_exts[0].c_str());

  for (unsigned int i = 1; i < m_cfg_obj_exts.size(); ++i)
    {
      printf(" %s%s%s",
             m_cfg_obj_prefix.c_str(),
             stem.c_str(),
             m_cfg_obj_exts[i].c_str());
    }

  printf(": ");

  const dep_list_t& cdeps = get_nested_cdeps(finfo);

  for (dep_list_t::const_iterator
         itr = cdeps.begin(),
         stop = cdeps.end();
       itr != stop;
       ++itr)
    {
      printf(" %s", (*itr)->fname.c_str());
    }

  printf("\n");
}

void cppdeps::print_include_tree(file_info* finfo)
{
  if (!is_cc_or_h_fname(finfo))
    return;

  const dep_list_t& cdeps =
    (m_cfg_output_mode & DIRECT_INCLUDE_TREE)
    ? get_direct_cdeps(finfo)
    : get_nested_cdeps(finfo);

  printf("%s:: ", finfo->fname.c_str());

  for (dep_list_t::const_iterator
         itr = cdeps.begin(),
         stop = cdeps.end();
       itr != stop;
       ++itr)
    {
      printf(" %s:%s",
             (*itr)->fname.c_str(),
             (*itr)->source ? (*itr)->source->fname.c_str() : "(nil)");
    }

  printf("\n");
}

void cppdeps::print_link_deps(file_info* finfo)
{
  const string exe = m_cfg_exe_formats.transform(finfo->fname);

  if (exe.empty())
    return;

  const dep_list_t& ldeps = get_nested_ldeps(finfo);

  // FIXME just store pointers here
  std::set<string> links;

  for (dep_list_t::const_iterator
         itr = ldeps.begin(),
         stop = ldeps.end();
       itr != stop;
       ++itr)
    {
      const string t = m_cfg_link_formats.transform((*itr)->fname);
      if (!t.empty())
        links.insert(t);
    }

  for (std::set<string>::iterator
         itr = links.begin(),
         stop = links.end();
       itr != stop;
       ++itr)
    {
      printf("%s: %s\n", exe.c_str(), (*itr).c_str());
    }
}

void cppdeps::traverse_sources()
{
  // start off with a copy of m_src_files
  vector<string> files (m_src_files);

  printf("%s\n",
         "# Do not edit this file! "
         "It is automatically generated. "
         "Changes will be lost.");

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
          file_info* finfo = file_info::get(current_file);

          if (m_cfg_output_mode & MAKEFILE_CDEPS)
            {
              print_makefile_dep(finfo);
            }

          if ((m_cfg_output_mode & DIRECT_INCLUDE_TREE) ||
              (m_cfg_output_mode & NESTED_INCLUDE_TREE) )
            {
              print_include_tree(finfo);
            }

          if (m_cfg_output_mode & MAKEFILE_LDEPS)
            {
              print_link_deps(finfo);
            }
        }
    }
}

int main(int argc, char** argv)
{
  cppdeps dc(argc, argv);
  dc.traverse_sources();
  exit(0);
}

static const char vcid_cppdeps_cc[] = "$Header$";
#endif // !CPPDEPS_CC_DEFINED
