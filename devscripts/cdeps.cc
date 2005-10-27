///////////////////////////////////////////////////////////////////////
//
// cppdeps.cc
//
// Copyright (c) 2003-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
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
#include <cstdlib>     // for atoi()
#include <cstring>     // for strerror()
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
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

using std::cerr;
using std::map;
using std::ostream;
using std::set;
using std::string;
using std::vector;

namespace
{
  //----------------------------------------------------------
  //
  // helper functions
  //
  //----------------------------------------------------------

  string string_time(const time_t t)
  {
    string s(ctime(&t));
    while (s.length() > 0 && isspace(s[s.length()-1]))
      {
        s.erase(s.length()-1, 1);
      }
    return s;
  }

  string file_tail(string fname)
  {
    if (fname.length() == 0)
      return fname;

    // kill any trailing slashes
    while (fname.length() > 0 && fname[fname.length()-1] == '/')
      fname.erase(fname.length()-1, 1);

    string::size_type slash = fname.find_last_of('/');
    if (slash == string::npos)
      {
        // no '/' was found, so file tail is the whole filename
        return fname;
      }
    return fname.substr(slash+1, string::npos);
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

  bool file_exists(const char* fname)
  {
    struct stat statbuf;
    return (stat(fname, &statbuf) == 0);
  }

  // assuming that stat(fname) has already failed, can we come up with
  // any more explanation for the failure... e.g., the file is a
  // broken link
  void print_stat_failure_info(const char* fname)
  {
    struct stat statbuf;
    if (lstat(fname, &statbuf) != 0)
      {
        cerr << "lstat failed for " << fname << '\n';
        // it's not a symlink, so give up
        return;
      }

    char buf[256];
    int nchars = readlink(fname, &buf[0], 255);

    if (nchars < 0)
      {
        cerr << "readlink failed for " << fname << '\n';
        // readlink() failed, so give up
        return;
      }

    buf[nchars] = '\0';

    string tail = file_tail(fname);
    string dirname = get_dirname_of(fname);

    if (tail.compare(0, 2, ".#") == 0)
      {
        tail = tail.substr(2, string::npos);

        cerr << "\t(make sure you have saved all your emacs buffers;\n"
             << "\t " << fname << " appears to be an emacs placeholder\n"
             << "\t for the unsaved file " << dirname << "/" << tail << " pointing to\n"
             << "\t " << buf << ")\n";
      }
  }

  bool is_directory(const char* fname)
  {
    struct stat statbuf;
    errno = 0;
    if (stat(fname, &statbuf) != 0)
      {
        cerr << "ERROR: in is_directory: stat failed " << fname << "\n";
        cerr << strerror(errno) << "\n";
        return false;
      }

    return S_ISDIR(statbuf.st_mode);
  }

  string join_filename(const string& dir, const string& fname)
  {
    string result(dir);
    result.reserve(dir.length() + 1 + fname.length());
    result += '/';
    result += fname;
    return result;
  }

  string join_filename(const string& dir1, const string& dir2,
                       const string& fname)
  {
    string result(dir1);
    result.reserve(dir1.length() + 1 + dir2.length() + 1 + fname.length());
    result += '/';
    result += dir2;
    result += '/';
    result += fname;
    return result;
  }

  string strip_leading_prefix(const string& s, const string& pfx)
  {
    string result = s;

    // Remove a leading directory prefix if necessary
    if (pfx.length() > 0)
      {
        if (result.compare(0, pfx.length(), pfx) == 0)
          {
            result.erase(0, pfx.length());

            // Now we've stripped off the non-zero-length prefix, we
            // might also need to strip a '/' component
            if (result.length() > 0 && result[0] == '/')
              result.erase(0, 1);
          }
      }

    return result;
  }

  void strip_whitespace(string& s)
  {
    const string::size_type p1 = s.find_first_not_of(" \t\n");
    const string::size_type p2 = s.find_last_not_of(" \t\n");

    if (p2 != string::npos && (p2+1 < s.length()))
      s.erase(p2+1, string::npos);

    if (p1 != 0)
      s.erase(0, p1);
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

  string::size_type get_last_of(const string& s, char c)
  {
    string::size_type p = s.find_last_of(c);
    if (p == string::npos) p = s.length();
    return p;
  }

  void print_stringvec(ostream& out, const vector<string>& v)
  {
    out << "[";
    for (unsigned int i = 0; i < v.size(); ++i)
      {
        out << "'" << v[i] << "'";

        if (i+1 < v.size()) out << ", ";
      }
    out << "]";
  }

  // make a normalized pathname from the given input, by making the
  // following transformations:
  //   (1) "/./"          --> "/"
  //   (2) "somedir/../"  --> "/"
  //   (3) "//"           --> "/"
  void make_normpath(const string& path, string& normpath)
  {
    typedef std::pair<string::size_type, string::size_type> subspec;

    vector<subspec> subspecs;

    string::size_type p = 0;

    string::size_type newlen = 0;

    for (string::size_type i = 0; i < path.size(); ++i)
      {
        if (path[i] == '/' || i+1 == path.size())
          {
            if (i == 0) subspecs.push_back(subspec(0,0));
            else
              {
                string::size_type i2 = i;

                if (path[i] != '/' && i+1 == path.size())
                  i2 = path.size();

                const string::size_type n = (i2-p);

                if (n == 0)
                  {
                    // do nothing, we have a double-slash '//' in the path
                  }
                else if (n == 1 && path.compare(p, n, ".") == 0)
                  {
                    // do nothing
                  }
                else if (n == 2 && path.compare(p, n, "..") == 0)
                  {
                    if (subspecs.size() == 0)
                      {
                        cerr << "ERROR: used '../' at beginning of path\n";
                        exit(1);
                      }
                    else if (subspecs.size() == 1 && subspecs.back().second == 0)
                      {
                        // do nothing, here we have '/../' at beginning of path
                      }
                    else
                      {
                        newlen -= subspecs.back().second;
                        subspecs.pop_back();
                      }
                  }
                else
                  {
                    subspecs.push_back(subspec(p, n));
                    newlen += n;
                  }
              }
            p = i+1;
          }
      }

    normpath.clear();
    normpath.reserve(newlen + subspecs.size() + 1);

    for (unsigned int i = 0; i < subspecs.size(); ++i)
      {
        normpath.append(path, subspecs[i].first, subspecs[i].second);
        if (i+1 < subspecs.size())
          normpath += '/';
      }
  }

  //----------------------------------------------------------
  //
  // shared_ptr class
  //
  //----------------------------------------------------------

  template<class T>
  class shared_ptr
  {
  public:
    typedef T element_type;

    /// Construct with pointer to given object (or null).
    explicit shared_ptr(T* p =0) :
      px(p), pn(0)
    {
      try { pn = new long(1); }  // fix: prevent leak if new throws
      catch (...) { delete p; throw; }
    }

    /// Copy construct.
    shared_ptr(const shared_ptr& r) throw() :
      px(r.px), pn(r.pn)
    {
      ++(*pn);
    }

    /// Destructor.
    ~shared_ptr() { dispose(); }

    /// Assignment operator.
    shared_ptr& operator=(const shared_ptr& r)
    {
      share(r.px,r.pn);
      return *this;
    }

    /// Copy constructor from pointer to related type.
    template<class TT>
    shared_ptr(const shared_ptr<TT>& r) throw() :
      px(r.px), pn(r.pn)
    {
      ++(*pn);
    }

    /// Assignment operator from pointer to related type.
    template<class TT>
    shared_ptr& operator=(const shared_ptr<TT>& r)
    {
      share(r.px,r.pn);
      return *this;
    }

    /// Reset with a pointer to a different object (or null).
    void reset(T* p=0)
    {
      if ( px == p ) return;  // fix: self-assignment safe
      if (--*pn == 0) { delete px; }
      else { // allocate new reference counter
        try { pn = new long; }  // fix: prevent leak if new throws
        catch (...)
          {
            ++*pn;  // undo effect of --*pn above to meet effects guarantee
            delete p;
            throw;
          } // catch
      } // allocate new reference counter
      *pn = 1;
      px = p;
    }

    /// Dereference.
    T& operator*() const throw()
    { assert(px != 0); return *px; }

    /// Dereference for member access.
    T* operator->() const throw()
    { assert(px != 0); return px; }

    /// Get a pointer to the referred-to object.
    T* get() const throw()
    { return px; }

    /// Get the current reference count.
    long use_count() const throw()
    { return *pn; }

    /// Query whether the pointee is unique (i.e. refcount == 1).
    bool unique() const throw()
    { return *pn == 1; }

    /// Swap pointees with another shared_ptr.
    void swap(shared_ptr<T>& other) throw()
    {
      std::swap(px, other.px);
      std::swap(pn, other.pn);
    }

  private:

    T*     px;     // contained pointer
    long*  pn;     // ptr to reference counter

    template<class TT> friend class shared_ptr;

    void dispose() { if (--*pn == 0) { delete px; delete pn; } }

    void share(T* rpx, long* rpn)
    {
      if (pn != rpn)
        {
          dispose();
          px = rpx;
          ++*(pn = rpn);
        }
    }
  };

  /// Equality for shared_ptr
  template<class T, class U>
  inline bool operator==(const shared_ptr<T>& a, const shared_ptr<U>& b)
  {
    return a.get() == b.get();
  }

  /// Inequality for shared_ptr
  template<class T, class U>
  inline bool operator!=(const shared_ptr<T>& a, const shared_ptr<U>& b)
  {
    return a.get() != b.get();
  }

  /// Less-than for shared_ptr
  template<class T, class U>
  inline bool operator<(const shared_ptr<T>& a, const shared_ptr<U>& b)
  {
    return a.get() < b.get();
  }

  //----------------------------------------------------------
  //
  // mapped_file class
  //
  //----------------------------------------------------------

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
          cerr << "in mapped_file(): stat() failed for file "
               << filename << ":\n"
               << strerror(errno) << "\n";
          print_stat_failure_info(filename);
          exit(1);
        }

      m_fileno = open(filename, O_RDONLY);
      if (m_fileno == -1)
        {
          cerr << "in mapped_file(): open() failed for file "
               << filename << ":\n"
               << strerror(errno) << "\n";
          exit(1);
        }

      m_mem = mmap(0, m_statbuf.st_size,
                   PROT_READ, MAP_PRIVATE, m_fileno, 0);

      if (m_mem == (void*)-1)
        {
          cerr << "in mapped_file(): mmap() failed for file "
               << filename << ":\n"
               << strerror(errno) << "\n";
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

  };

  //----------------------------------------------------------
  //
  // link_pattern class
  //
  //----------------------------------------------------------

  class link_pattern
  {
  public:
    link_pattern(const string& pat)
      :
      m_pattern(pat),
      m_wildcard_pos(pat.find_first_of('*'))
    {}

    string            m_pattern;
    string::size_type m_wildcard_pos;

    string transform(const string& stem) const
    {
      string result = m_pattern;
      result.replace(m_wildcard_pos, 1, stem);
      return result;
    }
  };

  //----------------------------------------------------------
  //
  // formatter class
  //
  //----------------------------------------------------------

  class formatter
  {
  private:
    string               m_group;
    string               m_prefix;
    vector<link_pattern> m_patterns;
    string               m_full_pattern;
    mutable bool         m_ever_matched;

  public:
    // Try to parse the input string as follows:
    //
    // (group,)? prefix : link_pattern
    //
    // The link pattern may contain a single asterisk ('*'). If there
    // is an asterisk, then when this formatter is matched, the
    // asterisk will be replaced with the input string, EXCLUDING that
    // portion that matched the prefix, and EXCLUDING the portion of
    // the input that counts as a filename extension (e.g., '.cc' or
    // '.h').
    //
    // E.g.: Suppose the format_spec is given as
    //
    //     src/:objdir/*.o
    //
    // Then this formatter object would do the following: for an input
    // string 'src/myfile.cc', transform it to an output string
    // 'objdir/myfile.o'.
    formatter(string format_spec) :
      m_ever_matched(false)
    {
      // Find the 'group', if any
      const string::size_type comma = format_spec.find_first_of(',');
      if (comma != string::npos)
        {
          m_group = format_spec.substr(0, comma);
          strip_whitespace(m_group);
          format_spec.erase(0, comma+1);
        }

      // Find the 'prefix' (everything after the group, up to the
      // first colon)
      const string::size_type colon = format_spec.find_first_of(':');
      if (colon == string::npos)
        {
          cerr << "ERROR: invalid format (missing colon): '"
               << format_spec << "'\n";
          exit(1);
        }

      m_prefix = format_spec.substr(0, colon);
      strip_whitespace(m_prefix);

      // Find the 'link_pattern' (everything after the first colon)
      m_full_pattern = format_spec.substr(colon+1);
      strip_whitespace(m_full_pattern);
      std::stringstream strm(m_full_pattern);
      while (strm)
        {
          string s; strm >> s;
          if (s.length() > 0)
            m_patterns.push_back(s);
        }
    }

    void warn_if_never_matched(const char* setname) const
    {
      if (!m_ever_matched)
        {
          cerr << "WARNING: " << setname << " pattern was never matched: "
               << m_prefix << ':';
          for (unsigned int i = 0; i < m_patterns.size(); ++i)
            cerr << m_patterns[i].m_pattern << ' ';
          cerr << '\n';
        }
    }

    bool matches(const string& srcfile) const
    {
      const bool result = strncmp(srcfile.c_str(),
                                  m_prefix.c_str(),
                                  m_prefix.length()) == 0;

      if (result == true)
        m_ever_matched = true;

      return result;
    }

    string transform(const string& srcfile) const
    {
      bool needtransform = false;
      for (unsigned int i = 0; i < m_patterns.size(); ++i)
        if (m_patterns[i].m_wildcard_pos != string::npos)
          { needtransform = true; break; }

      if (!needtransform)
        return m_full_pattern;

      // else...

      // here we know it's safe to chop off the prefix from srcfile,
      // because somebody has already called matches() and verified
      // that srcfile begins with m_prefix:
      string stem = srcfile.substr(m_prefix.length(), string::npos);
      const string::size_type suff = stem.find_last_of('.');
      if (suff != string::npos)
        stem.erase(suff, string::npos);

      // now let's build the result up by transforming each one of the
      // patterns in our list
      string result;
      for (unsigned int i = 0; i < m_patterns.size(); ++i)
        {
          if (m_patterns[i].m_wildcard_pos == string::npos)
            {
              result += m_patterns[i].m_pattern;
            }
          else
            {
              result += m_patterns[i].transform(stem);
            }

          if (i+1 < m_patterns.size())
            result += ' ';
        }

      return result;
    }

    bool has_group() const { return m_group.length() > 0; }

    const string& group() const { return m_group; }
  };

  //----------------------------------------------------------
  //
  // format_set class
  //
  //----------------------------------------------------------

  class format_set
  {
    vector<formatter> m_links;
    string            m_setname;

  public:
    format_set(const char* name) : m_links(), m_setname(name) {}

    void add_format(const string& format_spec)
    {
      m_links.push_back(formatter(format_spec));
    }

    /// Try to find a pattern that matches srcfile, and return its transformation.
    /** If no pattern matches, it is considered a fatal error. */
    string transform_strict(const string& srcfile) const
    {
      for (unsigned int i = m_links.size(); i > 0; --i)
        {
          if (m_links[i-1].matches(srcfile))
            return m_links[i-1].transform(srcfile);
        }
      cerr << "ERROR: no " << m_setname
           << " patterns matched source file: " << srcfile << '\n';
      exit(1);
      return string(); // can't happen, but placate compiler
    }

    /// Try to find a pattern that matches srcfile, and return its transformation.
    /** If no pattern matches, return an empty string. */
    string transform(const string& srcfile) const
    {
      for (unsigned int i = m_links.size(); i > 0; --i)
        {
          if (m_links[i-1].matches(srcfile))
            return m_links[i-1].transform(srcfile);
        }
      return string(); // can't happen, but placate compiler
    }

    /// Try to find a pattern that matches srcfile, and return its transformation.
    /** If no pattern matches, return an empty string. Returns the
        group name, if any, for the matching pattern. */
    string transform(const string& srcfile, string& group) const
    {
      group.clear();
      for (unsigned int i = m_links.size(); i > 0; --i)
        {
          if (m_links[i-1].matches(srcfile))
            {
              if (m_links[i-1].has_group())
                group = m_links[i-1].group();

              return m_links[i-1].transform(srcfile);
            }
        }
      return string(); // can't happen, but placate compiler
    }

    void give_warnings() const
    {
      for (unsigned int i = 0; i < m_links.size(); ++i)
        {
          m_links[i].warn_if_never_matched(m_setname.c_str());
        }
    }
  };

  //----------------------------------------------------------
  //
  // config class
  //
  //----------------------------------------------------------

  enum verbosity_level
    {
      SILENT = -1,
      QUIET = 0,
      NORMAL = 1,
      VERBOSE = 2,
      NOISY = 3
    };

  struct dep_config
  {
    dep_config() :
      exe_formats("--exeformat"),
      link_formats("--linkformat"),
      phantom_link_formats("--phantomlinkformat"),
      check_sys_deps(false),
      phantom_sys_deps(true),
      verbosity(NORMAL),
      output_mode(0),
      start_time(time((time_t*) 0)),
      nest_level(0),
      output_comment_character("#"),
      ldep_raw_mode(false)
    {}

    ostream& info()
    {
      if (this->verbosity >= NOISY)
        for (int i = 0; i < this->nest_level; ++i) cerr << '\t';
      return cerr;
    }

    ostream& warning()
    {
      if (this->verbosity >= NOISY)
        for (int i = 0; i < this->nest_level; ++i) cerr << '\t';
      cerr << "WARNING: ";
      return cerr;
    }

    vector<string>  user_ipath;
    vector<string>  sys_ipath;
    vector<string>  literal_exts;
    vector<string>  source_exts;
    vector<string>  header_exts;
    vector<string>  obj_exts;
    string          obj_prefix;
    format_set      exe_formats;
    format_set      link_formats;
    format_set      phantom_link_formats;
    bool            check_sys_deps;
    bool            phantom_sys_deps;
    verbosity_level verbosity;
    int             output_mode;
    vector<string>  prune_exts;
    vector<string>  prune_dirs;
    string          strip_prefix;
    const time_t    start_time;  // so we can check to see if any
                                 // source files have timestamps in
                                 // the future
    int             nest_level;
    const char*     output_comment_character;

    bool            ldep_raw_mode;
  };

  dep_config cfg;

  //----------------------------------------------------------
  //
  // file_info class
  //
  //----------------------------------------------------------

  class file_info;

  // Typedefs and enums
  typedef vector<file_info*>      dep_list_t;

  enum parse_state
    {
      NOT_STARTED = 0,
      IN_PROGRESS = 1,
      COMPLETE = 2
    };

  struct string_cmp
  {
    bool operator()(const char* p1, const char* p2)
    {
      return (strcmp(p1, p2) < 0);
    }
  };

  class ldep_group
  {
    ldep_group(const ldep_group&);
    ldep_group& operator=(const ldep_group&);

  public:
    ldep_group()             : m_members(), m_level(-1) { }
    ldep_group(file_info* f) : m_members(), m_level(-1) { m_members.push_back(f); }

    string bigname() const;

    string abbrevname() const;

    void get_all_nested_ldeps(dep_list_t& result, bool prune) const;

    int level();

    vector<file_info*>  m_members;
    int                 m_level;
    unsigned int        m_id;
  };

  struct ldep_group_level_cmp
  {
    bool operator()(const shared_ptr<ldep_group>& g1,
                    const shared_ptr<ldep_group>& g2)
    {
      return g1->level() > g2->level();
    }
  };

  class file_info
  {
  public:
    typedef map<const char*, file_info*, string_cmp> info_map_t;

  private:
    static info_map_t s_info_map;

    file_info(const string& t);

    bool should_prune() const;

    file_info(const file_info&); // not implemented
    file_info& operator=(const file_info&); // not implemented

  public:
    static file_info* get(const string& fname);

    // Returns true if we are a header file that has no .c/.cc/.C/.cpp
    // source-file counterpart.
    bool is_header_only() const { return this->m_is_header_only; }

    // Returns true if m_fname has a c++ source file extension, and
    // assigns the stem (without the extension) to 'stem'.
    bool is_cc_file() const;

    // Like is_cc_fname(), but also checks for header-file
    // extensions (e.g., '.h').
    bool is_cc_or_h_fname() const;

    // Find the source file that corresponds to the given header file
    file_info* find_source_for_header();

    bool resolve_include(const string& include_name,
                         const vector<string>& ipath,
                         const vector<string>& literal);

    const dep_list_t& get_direct_cdeps();
    const dep_list_t& get_nested_cdeps();
    const dep_list_t& get_direct_ldeps();
    const dep_list_t& get_nested_ldeps();

    bool is_phantom() const { return m_phantom; }
    bool is_pruned() const { return m_pruned; }

    const string& name() const { return m_fname; }
    const string& stripped_name() const { return m_stripped_name; }

    static void merge_ldep_groups(file_info* f1,
                                  file_info* f2)
    {
      assert(f1->m_ldep_group.get() != 0);
      assert(f2->m_ldep_group.get() != 0);

      const ldep_group& g1 = *f1->m_ldep_group;
      const ldep_group& g2 = *f2->m_ldep_group;

      set<file_info*> new_members;
      new_members.insert(g1.m_members.begin(), g1.m_members.end());
      new_members.insert(g2.m_members.begin(), g2.m_members.end());

      shared_ptr<ldep_group> new_group(new ldep_group);

      new_group->m_members.assign(new_members.begin(), new_members.end());

      for (unsigned int i = 0;
           i < new_group->m_members.size();
           ++i)
        {
          new_group->m_members[i]->m_ldep_group = new_group;
        }
    }

    static void dump()
    {
      for (info_map_t::const_iterator
             itr = s_info_map.begin(),
             stop = s_info_map.end();
           itr != stop;
           ++itr)
        {
          cerr << (*itr).second->m_nested_ldeps_done
               << ' ' << (*itr).second->m_fname << '\n';;
        }
    }

    static void find_ldep_groups(set<shared_ptr<ldep_group> >& result)
    {
      result.clear();

      for (info_map_t::const_iterator
             itr = s_info_map.begin(),
             stop = s_info_map.end();
           itr != stop;
           ++itr)
        {
          file_info* finfo = (*itr).second;

          if (!finfo->is_cc_file() && !finfo->is_header_only())
            continue;

          result.insert(finfo->m_ldep_group);
        }
    }

    static void find_ldep_groups_sorted
                          (vector<shared_ptr<ldep_group> >& result)
    {
      set<shared_ptr<ldep_group> > temp;
      find_ldep_groups(temp);

      result.assign(temp.begin(), temp.end());

      std::sort(result.begin(), result.end(), ldep_group_level_cmp());
    }

    static void dump_ldep_groups()
    {
      typedef set<shared_ptr<ldep_group> > all_groups_t;
      all_groups_t all_groups;

      find_ldep_groups(all_groups);

      for (all_groups_t::const_iterator
             itr = all_groups.begin(),
             stop = all_groups.end();
           itr != stop;
           ++itr)
        {
          std::cout << std::setw(4) << (*itr)->m_members.size() << "  "
                    << (*itr)->bigname() << '\n';;
        }
    }

    static void dump_ldep_adjacency()
    {
      typedef vector<shared_ptr<ldep_group> > all_groups_t;
      all_groups_t all_groups;

      find_ldep_groups_sorted(all_groups);

      unsigned int id = 0;

      std::cout << "filenames = {\n";

      // Assign id's and write filenames
      for (all_groups_t::const_iterator
             itr = all_groups.begin(),
             stop = all_groups.end();
           itr != stop;
           ++itr)
        {
          (*itr)->m_id = id++;
          std::cout << "\t'"
                    << (*itr)->abbrevname()
                    << " [level=" << (*itr)->level() << "]"
                    << "'\n";
        }

      std::cout << "}; % end filenames\n\n\n";

      const unsigned int N = id;

      std::cout << "adjacency = [\n";

      int maxlevel = 0;

      for (all_groups_t::const_iterator
             itr = all_groups.begin(),
             stop = all_groups.end();
           itr != stop;
           ++itr)
        {
          dep_list_t deps;
          (*itr)->get_all_nested_ldeps(deps, true);

          vector<int> adj(N, 0);

          for (unsigned int i = 0; i < deps.size(); ++i)
            {
              adj[deps[i]->m_ldep_group->m_id]
                = deps[i]->m_ldep_group->level();

              if (deps[i]->m_ldep_group->level() > maxlevel)
                maxlevel = deps[i]->m_ldep_group->level();
            }

          std::cout << "\t";

          for (unsigned int i = 0; i < N; ++i)
            {
              std::cout << adj[i] << ' ';
            }

          std::cout << ";\n";
        }

      std::cout << "]; % end adjacency\n\n\n";
    }

    // We use this function so that if we pipe the output of
    // dump_ldep_levels() through /bin/sort, all the lines still end
    // up in the right order.
    static void format_sort_key(std::ostream& out,
                                int level,
                                void* addr,
                                char linetype,
                                int linelevel,
                                const char* other)
    {
      out << "[[[#" << std::setw(2) << std::setfill('0') << level
          << "-" << std::setw(8) << std::hex << (long int)(addr) << std::dec
          << "-" << linetype
          << "-" << other
          << "-" << std::setw(2) << linelevel
          << "]]]";
    }

    static void dump_ldep_levels(bool verbose)
    {
      typedef set<shared_ptr<ldep_group> > all_groups_t;
      all_groups_t all_groups;

      find_ldep_groups(all_groups);

      for (all_groups_t::const_iterator
             itr = all_groups.begin(),
             stop = all_groups.end();
           itr != stop;
           ++itr)
        {
          format_sort_key(std::cout, (*itr)->level(), (*itr).get(),
                          'a', 0, "");
          std::cout << "\n";

          format_sort_key(std::cout, (*itr)->level(), (*itr).get(),
                          'a', 1, "");
          std::cout << "==============================================\n";

          if ((*itr)->m_members.size() > 1)
            {
              format_sort_key(std::cout, (*itr)->level(), (*itr).get(),
                              'a', 2, "");
              std::cout << "WARNING: CYCLIC LINK DEPENDENCY GROUP:\n";
            }


          for (unsigned int i = 0; i < (*itr)->m_members.size(); ++i)
            {
              format_sort_key(std::cout, (*itr)->level(), (*itr).get(),
                              'b', (*itr)->level(), "");

              std::cout << ">>>> module: "
                        << (*itr)->m_members[i]->name()
                        << '[' << (*itr)->level() << ']'
                        << '\n';
            }

          format_sort_key(std::cout, (*itr)->level(), (*itr).get(),
                          'c', 0, "");
          std::cout << "\n";

          if (verbose)
            {
              dep_list_t deps;
              (*itr)->get_all_nested_ldeps(deps, true);

              for (unsigned int i = 0; i < deps.size(); ++i)
                {
                  format_sort_key(std::cout, (*itr)->level(), (*itr).get(),
                                  'd', deps[i]->m_ldep_group->level(),
                                  get_dirname_of(deps[i]->name()).c_str());

                  std::cout << "             depends on:  "
                            << deps[i]->name()
                            << '[' << deps[i]->m_ldep_group->level() << ']'
                            << '\n';
                }
            }
        }
    }

    static void dump_ldep_raw()
    {
      typedef set<shared_ptr<ldep_group> > all_groups_t;
      all_groups_t all_groups;

      find_ldep_groups(all_groups);

      for (all_groups_t::const_iterator
             itr = all_groups.begin(),
             stop = all_groups.end();
           itr != stop;
           ++itr)
        {
          dep_list_t deps;
          (*itr)->get_all_nested_ldeps(deps, true);

          for (unsigned int i = 0; i < (*itr)->m_members.size(); ++i)
            {
              for (unsigned int j = 0; j < deps.size(); ++j)
                {
                  std::cout << std::setw(35) << std::left
                            << (*itr)->m_members[i]->name()
                            << " "
                            << deps[j]->name()
                            << "\n";
                }
            }
        }
    }

  private:
    const string            m_fname;
    const string::size_type m_dotpos; // position of the final "."
    const string            m_rootname; // filename without trailing .extension
    const string            m_stripped_name; // rootname without leading src root dir
    const string            m_extension; // trailing .extension, including the "."
    const string            m_dirname_without_slash;
    bool                    m_literal; // if true, then don't try to look up nested includes
    bool                    m_phantom; // if true, then only consider for link deps
    const bool              m_pruned;
    parse_state             m_cdep_parse_state;
    bool                    m_direct_cdeps_done;
    dep_list_t              m_direct_cdeps;
    bool                    m_nested_cdeps_done;
    dep_list_t              m_nested_cdeps;
    bool                    m_direct_ldeps_done;
    dep_list_t              m_direct_ldeps;
    bool                    m_nested_ldeps_done;
    dep_list_t              m_nested_ldeps;
  public:
    shared_ptr<ldep_group>  m_ldep_group;
  private:
    int                     m_epoch;
    bool                    m_is_header_only;
  };

  struct file_info_cmp
  {
    bool operator()(const file_info* f1, const file_info* f2)
    {
      return f1->name() < f2->name();
    }
  };

  //----------------------------------------------------------
  //
  // ldep_group member definitions
  //
  //----------------------------------------------------------

  string ldep_group::bigname() const
  {
    string result;
    for (unsigned int i = 0; i < m_members.size(); ++i)
      {
        result += m_members[i]->name();
        if (i+1 < m_members.size())
          result += " + ";
      }
    return result;
  }

  string ldep_group::abbrevname() const
  {
    if (m_members.size() <= 2)
      return this->bigname();

    std::ostringstream result;
    result << "group of "
           << m_members[0]->name()
           << " + " << m_members.size()-1 << " others";

    return result.str();
  }

  void ldep_group::get_all_nested_ldeps(dep_list_t& result,
                                        bool prune) const
  {
    typedef set<file_info*> all_deps_t;
    all_deps_t all_deps;

    for (unsigned int i = 0; i < m_members.size(); ++i)
      {
        const dep_list_t& d = m_members[i]->get_nested_ldeps();
        if (!prune)
          all_deps.insert(d.begin(), d.end());
        else
          {
            for (unsigned int j = 0; j < d.size(); ++j)
              if (!d[j]->is_phantom() &&
                  !d[j]->is_pruned())
                all_deps.insert(d[j]);
          }
      }

    result.assign(all_deps.begin(), all_deps.end());
  }

  int ldep_group::level()
  {
    if (m_level >= 0)
      return m_level;

    dep_list_t deps;
    get_all_nested_ldeps(deps, true);

    if (deps.size() == 0)
      {
        m_level = 0;
      }
    else
      {
        m_level = 1;

        for (unsigned int i = 0; i < deps.size(); ++i)
          {
            if (deps[i]->m_ldep_group.get() == this)
              continue;

            assert(deps[i]->m_ldep_group.get() != 0);

            const int lev = deps[i]->m_ldep_group->level();

            if (lev >= m_level)
              m_level = lev+1;
          }
      }

    return m_level;
  }

  //----------------------------------------------------------
  //
  // file_info member definitions
  //
  //----------------------------------------------------------

  file_info::info_map_t file_info::s_info_map;

  file_info::file_info(const string& t)
    :
    m_fname(t),
    m_dotpos(get_last_of(m_fname, '.')),
    m_rootname(m_fname.substr(0,m_dotpos)),
    m_stripped_name(strip_leading_prefix(m_rootname, cfg.strip_prefix)),
    m_extension(m_fname.substr(m_dotpos, string::npos)),
    m_dirname_without_slash(get_dirname_of(t)),
    m_literal(false),
    m_phantom(false),
    m_pruned(this->should_prune()),
    m_cdep_parse_state(NOT_STARTED),
    m_direct_cdeps_done(false),
    m_direct_cdeps(),
    m_nested_cdeps_done(false),
    m_nested_cdeps(),
    m_direct_ldeps_done(false),
    m_direct_ldeps(),
    m_nested_ldeps_done(false),
    m_nested_ldeps(),
    m_epoch(0),
    m_is_header_only(false)
  {
    assert(this->m_dirname_without_slash.length() > 0); // must be at least '.'
    assert(this->m_dirname_without_slash[this->m_dirname_without_slash.length()-1] != '/');
  }

  bool file_info::should_prune() const
  {
    for (unsigned int i = 0; i < cfg.prune_dirs.size(); ++i)
      {
        const string d = cfg.prune_dirs[i] + '/';
        if (this->m_fname.find(d) != string::npos)
          {
            return true;
          }
      }

    for (unsigned int i = 0; i < cfg.prune_exts.size(); ++i)
      {
        if (this->m_extension == cfg.prune_exts[i])
          {
            return true;
          }
      }

    return false;
  }

  file_info* file_info::get(const string& fname_orig)
  {
    string fname;
    make_normpath(fname_orig, fname);

    if (cfg.verbosity >= NOISY)
      cfg.info() << "file_info normpath is " << fname << '\n';

    info_map_t::iterator p = s_info_map.find(fname.c_str());
    if (p != s_info_map.end())
      return (*p).second;

    file_info* finfo = new file_info(fname);

    info_map_t::iterator i =
      s_info_map.insert(info_map_t::value_type
                        (finfo->m_fname.c_str(), finfo)).first;

    return (*i).second;
  }

  bool file_info::is_cc_file() const
  {
    for (unsigned int i = 0; i < cfg.source_exts.size(); ++i)
      {
        if (this->m_extension == cfg.source_exts[i])
          {
            return true;
          }
      }

    return false;
  }

  bool file_info::is_cc_or_h_fname() const
  {
    for (unsigned int i = 0; i < cfg.source_exts.size(); ++i)
      {
        if (this->m_extension == cfg.source_exts[i])
          {
            return true;
          }
      }

    for (unsigned int i = 0; i < cfg.header_exts.size(); ++i)
      {
        if (this->m_extension == cfg.header_exts[i])
          {
            return true;
          }
      }

    return false;
  }

  file_info* file_info::find_source_for_header()
  {
    if (this->m_phantom)
      return this;

    for (unsigned int i = 0; i < cfg.source_exts.size(); ++i)
      {
        const string result = this->m_rootname + cfg.source_exts[i];
        if (file_exists(result.c_str()))
          return file_info::get(result);
      }

    if (cfg.ldep_raw_mode)
      // if we're doing --output-ldep-raw, then we want to list .h
      // files as link dependencies if there is no corresponding
      // .c/.C/.cc/.cpp source file:
      { this->m_is_header_only = true; return this; }
    else
      // but in "normal" processing (e.g. building link dependency
      // rules for a makefile), then we don't want to consider .h
      // files as link dependencies (though .h files might inject
      // transitive link dependencies due to the headers #include'd
      // within the .h file):
      return 0;
  }

  bool file_info::resolve_include(const string& include_name,
                                  const vector<string>& ipath,
                                  const vector<string>& literal)
  {
    for (unsigned int i = 0; i < ipath.size(); ++i)
      {
        const string fullpath = join_filename(ipath[i], include_name);

        if (file_exists(fullpath.c_str()))
          {
            this->m_direct_cdeps.push_back(file_info::get(fullpath));
            return true;
          }
      }

    // Try resolving the include by using the directory containing the
    // source file currently being examined.
    const string fullpath =
      join_filename(this->m_dirname_without_slash, include_name);

    if (file_exists(fullpath.c_str()))
      {
        this->m_direct_cdeps.push_back(file_info::get(fullpath));
        return true;
      }

    // Try resolving the include by looking for directories in ipath,
    // relative to the directory containing the current source file.
    for (unsigned int i = 0; i < ipath.size(); ++i)
      {
        if (ipath[i].length() > 0 && ipath[i][0] == '/')
          {
            // it's an absolute path, so don't try to join it with the current dir
            continue;
          }

        const string fullpath = join_filename(this->m_dirname_without_slash,
                                              ipath[i], include_name);

        if (file_exists(fullpath.c_str()))
          {
            this->m_direct_cdeps.push_back(file_info::get(fullpath));
            return true;
          }
      }

    // Try resolving the include by using the current working directory
    // from which this program was invoked.
    if (file_exists(include_name.c_str()))
      {
        if (this->m_fname != include_name)
          {
            this->m_direct_cdeps.push_back(file_info::get(include_name));
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
            this->m_direct_cdeps.push_back(file_info::get(include_name));
            this->m_direct_cdeps.back()->m_literal = true;
            return true;
          }
      }

    return false;
  }

  const dep_list_t& file_info::get_direct_cdeps()
  {
    if (this->m_direct_cdeps_done)
      return this->m_direct_cdeps;

    if (cfg.verbosity >= NOISY)
      cfg.info() << "get_direct_cdeps for " << this->m_fname << '\n';

    ++cfg.nest_level;

    mapped_file f(this->m_fname.c_str());

    if (cfg.verbosity >= NOISY)
      cfg.info() << "mmap @ " << f.memory()
                 << ", length " << f.length() << '\n';

    if (f.mtime() > cfg.start_time && (cfg.verbosity >= NORMAL))
      {
        cfg.warning() << "for file " << this->m_fname << ":\n"
                      << "\tmodification time (" << string_time(f.mtime()) << ") is in the future\n"
                      << "\tvs. current time  (" << string_time(cfg.start_time) << ")\n";
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
          (delimiter == '<' &&
           (cfg.check_sys_deps == true || cfg.phantom_sys_deps == true));

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

        if (delimiter == '\"' &&
            this->resolve_include(include_name,
                                  cfg.user_ipath,
                                  cfg.literal_exts))
          continue;

        if (cfg.phantom_sys_deps && delimiter == '<')
          {
            this->m_direct_cdeps.push_back(file_info::get(include_name));
            this->m_direct_cdeps.back()->m_phantom = true;
            continue;
          }

        if (cfg.check_sys_deps &&
            this->resolve_include(include_name,
                                  cfg.sys_ipath,
                                  cfg.literal_exts))
          continue;

        if (cfg.verbosity >= NORMAL)
          {
            cfg.warning() << "in " << this->m_fname
                          << ": couldn\'t resolve #include \""
                          << include_name << "\"\n";

            cfg.info() << "\twith search path: ";
            print_stringvec(cerr, cfg.user_ipath);
            cerr << '\n';

            if (cfg.check_sys_deps)
              {
                cfg.info() << "\tand system search path: ";
                print_stringvec(cerr, cfg.sys_ipath);
                cerr << '\n';
              }
          }
      }

    this->m_direct_cdeps_done = true;

    --cfg.nest_level;

    return this->m_direct_cdeps;
  }

  const dep_list_t& file_info::get_nested_cdeps()
  {
    if (this->m_nested_cdeps_done)
      return this->m_nested_cdeps;

    if (this->m_cdep_parse_state == IN_PROGRESS)
      {
        cerr << "ERROR: in " << this->m_fname
             << ": untrapped nested #include recursion\n";
        exit(1);
      }

    if (this->m_phantom)
      {
        cerr << "ERROR: get_nested_cdeps() called for phantom file: "
             << this->m_fname << '\n';
        exit(1);
      }

    this->m_cdep_parse_state = IN_PROGRESS;

    // use a set to build up the list of #includes, so that
    //   (1) we automatically avoid duplicates
    //   (2) we get sorting for free
    //
    // this turns out to be cheaper than building up the list in a
    // std::vector and then doing a big std::sort, std::unique(), and
    // vec.erase() at the end
    set<file_info*, file_info_cmp> dep_set;

    dep_set.insert(this);

    const dep_list_t& direct = this->get_direct_cdeps();

    for (dep_list_t::const_iterator
           i = direct.begin(),
           istop = direct.end();
         i != istop;
         ++i)
      {
        // Check for self-inclusion to avoid infinite recursion.
        if (*i == this)
          continue;

        // Check if the included file is to be treated as a 'phantom'
        // file -- these would be e.g. system headers (#include'd with
        // angle brackets) that we don't to treat as compile
        // dependencies, but for which we would like to compute link
        // dependencies.
        if ((*i)->m_phantom == true)
          {
            dep_set.insert(*i);
            continue;
          }

        // Check if the included file is to be treated as a 'literal' file,
        // meaning that we don't look for nested includes, and thus don't
        // require the file to currently exist. This is useful for handling
        // files that are generated by an intermediate rule in some makefile.
        if ((*i)->m_literal == true)
          {
            dep_set.insert(*i);
            continue;
          }

        // Check for other recursion cycles
        if ((*i)->m_cdep_parse_state == IN_PROGRESS)
          {
            if (cfg.verbosity >= NORMAL)
              cfg.warning() << "in " << this->m_fname
                            << ": recursive #include cycle with "
                            << (*i)->m_fname << "\n";
            continue;
          }

        ++cfg.nest_level;
        const dep_list_t& indirect = (*i)->get_nested_cdeps();
        --cfg.nest_level;
        if (cfg.verbosity >= NOISY)
          {
            for (dep_list_t::const_iterator
                   itr = indirect.begin(), end = indirect.end();
                 itr != end;
                 ++itr)
              cfg.info() << (*i)->name() << " --> (direct) " << (*itr)->name() << '\n';
          }
        dep_set.insert(indirect.begin(), indirect.end());
      }

    assert(this->m_nested_cdeps.empty());
    this->m_nested_cdeps.assign(dep_set.begin(), dep_set.end());

    if (cfg.verbosity >= NOISY)
      {
        for (dep_list_t::const_iterator
               itr = this->m_nested_cdeps.begin(),
               end = this->m_nested_cdeps.end();
             itr != end;
             ++itr)
          cfg.info() << this->name() << " --> (nested) " << (*itr)->name() << '\n';
      }

    this->m_nested_cdeps_done = true;
    this->m_cdep_parse_state = COMPLETE;

    return this->m_nested_cdeps;
  }

  const dep_list_t& file_info::get_direct_ldeps()
  {
    if (this->m_direct_ldeps_done)
      return this->m_direct_ldeps;

    if (this->m_phantom)
      {
        assert(this->m_direct_ldeps.empty());
        this->m_direct_ldeps.push_back(this);
        this->m_direct_ldeps_done = true;
        return this->m_direct_ldeps;
      }

    set<file_info*> deps_set;

    ++cfg.nest_level;
    const dep_list_t& cdeps = this->get_nested_cdeps();
    --cfg.nest_level;

    for (dep_list_t::const_iterator
           i = cdeps.begin(),
           istop = cdeps.end();
         i != istop;
         ++i)
      {
        file_info* ccfile = (*i)->find_source_for_header();
        if (ccfile == 0)
          continue;

        if (ccfile == this)
          continue;

        deps_set.insert(ccfile);
      }

    assert(this->m_direct_ldeps.empty());
    this->m_direct_ldeps.assign(deps_set.begin(), deps_set.end());

    this->m_direct_ldeps_done = true;

    assert(this->m_ldep_group.get() == 0);
    this->m_ldep_group.reset(new ldep_group(this));

    return this->m_direct_ldeps;
  }

  const dep_list_t& file_info::get_nested_ldeps()
  {
    if (this->m_nested_ldeps_done)
      return this->m_nested_ldeps;

    // Enforce that this function is not safe for being called recursively.
    static bool computing_ldeps = false;
    assert(!computing_ldeps);
    computing_ldeps = true;

    static int epoch = 0;
    ++epoch;

    if (cfg.verbosity >= NOISY)
      {
        cfg.info() << "start ldeps for " << this->m_fname << '\n';
      }

    assert(this->m_nested_ldeps.empty());

    vector<file_info*> to_handle;

    this->m_epoch = epoch;
    to_handle.push_back(this);

    while (to_handle.size() > 0)
      {
        file_info* f = to_handle.back();
        to_handle.pop_back();

        assert(f->m_epoch == epoch);

        this->m_nested_ldeps.push_back(f);

        const dep_list_t& direct = f->get_direct_ldeps();

        for (dep_list_t::const_iterator
               itr = direct.begin(),
               stop = direct.end();
             itr != stop;
             ++itr)
          {
            if (*itr == this)
              {
                if (cfg.verbosity >= VERBOSE)
                  cfg.warning() << " in " << this->m_fname
                                << ": recursive link-dep cycle with "
                                << f->m_fname << "\n";

                file_info::merge_ldep_groups(this, f);
                continue;
              }

            if ((*itr)->m_epoch != epoch)
              {
                (*itr)->m_epoch = epoch;
                to_handle.push_back(*itr);
              }
          }
      }

    if (cfg.verbosity >= NOISY)
      {
        cfg.info() << "...end ldeps for " << this->m_fname << '\n';
      }

    this->m_nested_ldeps_done = true;
    computing_ldeps = false;

    return this->m_nested_ldeps;
  }

} // end unnamed namespace

//----------------------------------------------------------
//
// cppdeps class
//
//----------------------------------------------------------

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
  static const int MAKEFILE_LDEPS      = (1 << 1);
  static const int DIRECT_CDEPS        = (1 << 2);
  static const int LDEP_GROUPS         = (1 << 3);
  static const int LDEP_LEVELS         = (1 << 4);
  static const int LDEP_LEVELSV        = (1 << 5);
  static const int LDEP_ADJACENCY      = (1 << 6);
  static const int LDEP_RAW            = (1 << 7);

  // Member variables

  vector<string>           m_src_files;

  bool                     m_inspect;
  int                      m_argc;
  char**                   m_argv;

public:
  cppdeps(int argc, char** argv);

  bool handle_option(const char* option, const char* optarg);

  void load_options_file(const char* filename);

  void inspect(char** arg0, char** argn);

  bool should_prune_directory(const char* fname) const;

  void print_direct_cdeps(file_info* finfo);
  void print_makefile_dep(file_info* finfo);
  void print_link_deps(file_info* finfo);

  void traverse_sources();
};

cppdeps::cppdeps(const int argc, char** const argv) :
  m_inspect(false),
  m_argc(argc),
  m_argv(argv)
{
  if (argc == 1)
    {
      printf
        ("usage: %s [options] --srcdir [dir]...\n"
         "\n"
         "options:\n"
         "    --srcdir [dir]        specify a directory containing source files; this\n"
         "                          directory will be searched recursively for files\n"
         "                          with C/C++ filename extensions (including .c, .C,\n"
         "                          .cc, .cpp, .h, .H, .hh, .hpp, and any others given\n"
         "                          with --source-ext or --header-ext)\n"
         "    --includedir [dir]    specify a directory to be searched when resolving\n"
         "                          #include \"...\" directives\n"
         "    --I[dir]              same as --includedir [dir]\n"
         "    --sysincludedir [dir] specify a directory to be searched when resolving\n"
         "                          #include <...> directives\n"
         "    --checksys            force tracking of dependencies in #include <...>\n"
         "                          directives (default is to not record <...> files\n"
         "                          as dependencies)\n"
         "    --objdir [dir]        specify a path prefix indicating where the object\n"
         "                          (.o) files should be placed (the default\n"
         "                          is no prefix, or just './')\n"
         "    --objext [.ext]       make .ext be one of the suffixes used in target\n"
         "                          rules in the makefile; there may be more than one\n"
         "                          such extension, in which case each rule emitted\n"
         "                          will have more than one target; the default is for\n"
         "                          the list of extensions to include just '.o'\n"
         "    --source-ext [.ext]   treat filenames ending in .ext as c/c++ source files\n"
         "                          (i.e., just like .c, .cpp, .C, etc.)\n"
         "    --header-ext [.ext]   treat filenames ending in .ext as c/c++ header files\n"
         "                          (i.e., just like .h, .hpp, .H, etc.)\n"
         "    --options-file [file] read additional options from the named file; this\n"
         "                          file is expected to have one option (plus possible\n"
         "                          argument) per line\n"
         "    --output-compile-deps print makefile rules expressing compile-time\n"
         "                          dependencies (i.e., dependencies of object files on\n"
         "                          source and header files) -- this is the default\n"
         "                          output mode\n"
         "    --output-link-deps    print makefile rules expressing link-time\n"
         "                          dependencies (i.e., dependencies of executables on\n"
         "                          object files and static or dynamic libraries)\n"
         "    --literal [.ext]      treat files ending in \".ext\" as literal #include\'s\n"
         "    --exeformat [fmt]\n"
         "    --linkformat [fmt]\n"
         "    --phantomlinkformat [fmt]\n"
         "    --prune-dir [dir]     don't look for source files in the named directory;\n"
         "                          the <dirname> should not contain any slashes, since\n"
         "                          it refers to simple directory entry (default pruned\n"
         "                          directories are \".\", \"..\", \"RCS\", \"CVS\"\n"
         "    --prune-ext [.ext]    don't consider any source files whose names end with\n"
         "                          the given extension\n"
         "    --verbosity [level]   level -1: suppress warnings and error messages\n"
         "                                    (you'll still get the errors themselves :)\n"
         "                          level  0: suppress warnings\n"
         "                          level  1: [default] normal verbosity\n"
         "                          level  2: extra warnings\n"
         "                          level  3: lots of extra tracing statements\n"
         "    --inspect             show contents of internal variables while processing\n"
         "                          command-line arguments\n"
         "\n"
         "\n"
         "example:\n"
         "\n"
         "    %s --includedir ~/local/include/ --objdir project/obj/ --srcdir ./\n"
         "\n"
         "    builds dependencies for all source files found recursively within the\n"
         "    current directory (./), using ~/local/include to resolve #include's,\n"
         "    and putting .o files into project/obj/.\n",
         argv[0],
         argv[0]);
      exit(1);
    }

  cfg.sys_ipath.push_back("/usr/include");
  cfg.sys_ipath.push_back("/usr/include/linux");
  cfg.sys_ipath.push_back("/usr/local/matlab/extern/include");

  cfg.source_exts.push_back(".cc");
  cfg.source_exts.push_back(".C");
  cfg.source_exts.push_back(".c");
  cfg.source_exts.push_back(".cpp");

  cfg.header_exts.push_back(".h");
  cfg.header_exts.push_back(".H");
  cfg.header_exts.push_back(".hh");
  cfg.header_exts.push_back(".hpp");

  cfg.prune_dirs.clear();
  cfg.prune_dirs.push_back("RCS");
  cfg.prune_dirs.push_back("CVS");
  cfg.prune_dirs.push_back(".svn");

  char** arg = argv+1; // skip to first command-line arg

  for ( ; *arg != 0; ++arg)
    {
      if (handle_option(*arg, *(arg+1)))
        ++arg;

      if (m_inspect)
        {
          inspect(argv, arg);
        }
    }

  if (m_src_files.size() == 0)
    {
      cerr << "ERROR: no source directories specified (use --srcdir)\n";
      exit(1);
    }

  if (cfg.output_mode == 0)
    cfg.output_mode = MAKEFILE_CDEPS;

  // If the user didn't specify any object-filename extensions, then
  // we just use the default '.o'.
  if (cfg.obj_exts.size() == 0)
    cfg.obj_exts.push_back(".o");

  if (m_inspect)
    {
      inspect(argv, arg);
    }
}

bool cppdeps::handle_option(const char* option, const char* optarg)
{
  if (strcmp(option, "--srcdir") == 0)
    {
      // SPECIAL CASE: if the argument to --srcdir is an empty string,
      // then we treat it as if it were "." instead, i.e. the current
      // directory. Otherwise, we just trim any trailing slashes from
      // the non-empty argument.

      const string fname =
        (strlen(optarg) == 0)
        ? string(".")
        : trim_trailing_slashes(optarg);

      if (!file_exists(fname.c_str()))
        {
          cerr << "ERROR: no such source file: '" << fname << "'\n";
          exit(1);
        }
      m_src_files.push_back(fname);
      if (is_directory(fname.c_str()))
        {
          cfg.user_ipath.push_back(fname);
          make_normpath(fname, cfg.strip_prefix);
        }
      return true;
    }
  else if (strcmp(option, "--includedir") == 0)
    {
      cfg.user_ipath.push_back(optarg);
      return true;
    }
  else if (strncmp(option, "-I", 2) == 0)
    {
      cfg.user_ipath.push_back(option + 2);
      return false;
    }
  else if (strcmp(option, "--sysincludedir") == 0)
    {
      cfg.sys_ipath.push_back(optarg);
      return true;
    }
  else if (strcmp(option, "--checksys") == 0)
    {
      cfg.check_sys_deps = true;
      cfg.phantom_sys_deps = false;
      return false;
    }
  else if (strcmp(option, "--objdir") == 0)
    {
      cfg.obj_prefix = trim_trailing_slashes(optarg);
      return true;
    }
  else if (strcmp(option, "--objext") == 0)
    {
      cfg.obj_exts.push_back(optarg);
      return true;
    }
  else if (strcmp(option, "--source-ext") == 0)
    {
      cfg.source_exts.push_back(optarg);
      return true;
    }
  else if (strcmp(option, "--header-ext") == 0)
    {
      cfg.header_exts.push_back(optarg);
      return true;
    }
  else if (strcmp(option, "--options-file") == 0)
    {
      load_options_file(optarg);
      return true;
    }
  else if (strcmp(option, "--output-direct-cdeps") == 0)
    {
      cfg.output_mode |= DIRECT_CDEPS;
      return false;
    }
  else if (strcmp(option, "--output-compile-deps") == 0)
    {
      cfg.output_mode |= MAKEFILE_CDEPS;
      return false;
    }
  else if (strcmp(option, "--output-link-deps") == 0)
    {
      cfg.output_mode |= MAKEFILE_LDEPS;
      return false;
    }
  else if (strcmp(option, "--output-ldep-groups") == 0)
    {
      cfg.output_mode |= LDEP_GROUPS;
      return false;
    }
  else if (strcmp(option, "--output-ldep-levels") == 0)
    {
      cfg.output_mode |= LDEP_LEVELS;
      return false;
    }
  else if (strcmp(option, "--output-ldep-levelsv") == 0)
    {
      cfg.output_mode |= LDEP_LEVELSV;
      return false;
    }
  else if (strcmp(option, "--output-ldep-adjacency") == 0)
    {
      cfg.output_mode |= LDEP_ADJACENCY;
      cfg.output_comment_character = "%"; // for matlab
      return false;
    }
  else if (strcmp(option, "--output-ldep-raw") == 0)
    {
      cfg.output_mode |= LDEP_RAW;
      cfg.output_comment_character = NULL;
      cfg.ldep_raw_mode = true;
      return false;
    }
  else if (strcmp(option, "--literal") == 0)
    {
      cfg.literal_exts.push_back(optarg);
      return true;
    }
  else if (strcmp(option, "--exeformat") == 0)
    {
      cfg.exe_formats.add_format(optarg);
      return true;
    }
  else if (strcmp(option, "--linkformat") == 0)
    {
      cfg.link_formats.add_format(optarg);
      return true;
    }
  else if (strcmp(option, "--phantomlinkformat") == 0)
    {
      cfg.phantom_link_formats.add_format(optarg);
      return true;
    }
  else if (strcmp(option, "--prune-dir") == 0)
    {
      cfg.prune_dirs.push_back(optarg);
      return true;
    }
  else if (strcmp(option, "--prune-ext") == 0)
    {
      cfg.prune_exts.push_back(optarg);
      return true;
    }
  else if (strcmp(option, "--verbosity") == 0)
    {
      cfg.verbosity = verbosity_level(atoi(optarg));
      return true;
    }
  else if (strcmp(option, "--inspect") == 0)
    {
      m_inspect = true;
      return false;
    }
  else
    {
      cerr << "ERROR: unrecognized command-line option: "
           << option << '\n';
      exit(1);
    }
  return false; // can't happen
}

void cppdeps::load_options_file(const char* filename)
{
  if (!file_exists(filename))
    {
      cerr << "ERROR: couldn't open options file: " << filename << '\n';
      exit(1);
    }

  std::ifstream in(filename);

  if (!in.is_open())
    {
      cerr << "ERROR: couldn't open options file for reading: " << filename << '\n';
      exit(1);
    }

  string line;
  while (in)
    {
      std::getline(in, line);
      if (line.length() > 0 && line[0] != '#')
        {
          string::size_type s = line.find_first_of(' ');
          if (s == string::npos) s = line.length();
          string option = line.substr(0, s);
          string optarg = line.substr(s+1);
          handle_option(option.c_str(), optarg.c_str());
        }
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
  print_stringvec(cerr, cfg.user_ipath);
  cerr << "\n";

  cerr << "sys_ipath: ";
  print_stringvec(cerr, cfg.sys_ipath);
  cerr << "\n";

  cerr << "sources: ";
  print_stringvec(cerr, m_src_files);
  cerr << "\n";

  cerr << "literal_exts: ";
  print_stringvec(cerr, cfg.literal_exts);
  cerr << "\n";

  cerr << "obj_exts: ";
  print_stringvec(cerr, cfg.obj_exts);
  cerr << "\n";

  cerr << "obj_prefix: '" << cfg.obj_prefix << "'\n\n";
}

bool cppdeps::should_prune_directory(const char* fname) const
{
  if (strcmp(fname, ".") == 0)  return true;
  if (strcmp(fname, "..") == 0) return true;

  for (unsigned int i = 0; i < cfg.prune_dirs.size(); ++i)
    if (cfg.prune_dirs[i] == fname)
      return true;

  return false;
}

void cppdeps::print_direct_cdeps(file_info* finfo)
{
  if (!finfo->is_cc_or_h_fname())
    {
      if (cfg.verbosity >= NOISY)
        cfg.info() << finfo->name() << " is not a c++ file\n";
      return;
    }

  printf("%s -->", finfo->name().c_str());

  const dep_list_t& cdeps = finfo->get_direct_cdeps();

  for (dep_list_t::const_iterator
         itr = cdeps.begin(),
         stop = cdeps.end();
       itr != stop;
       ++itr)
    {
      if ((*itr)->is_phantom())
        continue;

      printf(" %s", (*itr)->name().c_str());
    }

  printf("\n");
}

void cppdeps::print_makefile_dep(file_info* finfo)
{
  if (!finfo->is_cc_file())
    {
      if (cfg.verbosity >= NOISY)
        cfg.info() << finfo->name() << " is not a c++ file\n";
      return;
    }

  const string& stem = finfo->stripped_name();

  // Make sure that cfg.obj_prefix ends with a slash if it is
  // non-empty, so that we can join it to stem and make a proper
  // pathname.
  if (cfg.obj_prefix.length() > 0
      && cfg.obj_prefix[cfg.obj_prefix.length()-1] != '/')
    {
      cfg.obj_prefix += '/';
    }

  assert(cfg.obj_exts.size() > 0);

  // Use C-style stdio here since it came out running quite a bit
  // faster than iostreams, at least under g++-3.2.
  printf("%s%s%s",
         cfg.obj_prefix.c_str(),
         stem.c_str(),
         cfg.obj_exts[0].c_str());

  for (unsigned int i = 1; i < cfg.obj_exts.size(); ++i)
    {
      printf(" %s%s%s",
             cfg.obj_prefix.c_str(),
             stem.c_str(),
             cfg.obj_exts[i].c_str());
    }

  printf(": ");

  const dep_list_t& cdeps = finfo->get_nested_cdeps();

  for (dep_list_t::const_iterator
         itr = cdeps.begin(),
         stop = cdeps.end();
       itr != stop;
       ++itr)
    {
      if ((*itr)->is_phantom())
        continue;

      printf(" %s", (*itr)->name().c_str());
    }

  printf("\n");
}

void cppdeps::print_link_deps(file_info* finfo)
{
  if (!finfo->is_cc_file())
    return;

  string group;
  const string exe = cfg.exe_formats.transform(finfo->name(), group);

  if (exe.empty())
    {
      // If the verbosity is high, then force the nested ldeps to be
      // computed for every source file... this way we have the
      // information needed to give warnings about recursive link-dep
      // cycles. If the verbosity is low, then we can skip these extra
      // computations and save execution time.
      if ((cfg.verbosity >= VERBOSE) && exe.empty())
        (void) finfo->get_nested_ldeps();

      return;
    }

  if (!group.empty())
    {
      printf("%s: %s\n", group.c_str(), exe.c_str());
    }

  printf("ALLEXECS += %s\n", exe.c_str());

  const dep_list_t& ldeps = finfo->get_nested_ldeps();

  set<string> links;

  for (dep_list_t::const_iterator
         itr = ldeps.begin(),
         stop = ldeps.end();
       itr != stop;
       ++itr)
    {
      if (cfg.verbosity >= NOISY)
        cfg.info() << "considering exec " << exe << " --> ldep " << (*itr)->name() << '\n';

      if (!(*itr)->is_phantom() && !(*itr)->is_pruned())
        {
          const string t = cfg.link_formats.transform_strict((*itr)->name());
          if (!t.empty())
            {
              links.insert(t);
            }

          if (cfg.verbosity >= NOISY)
            cfg.info() << "ldep " << (*itr)->name() << " --> link format '" << t << "'\n";
        }
      else
        if (cfg.verbosity >= NOISY)
          cfg.info() << "ldep " << (*itr)->name() << " is phantom or pruned\n";
    }

  for (set<string>::iterator
         itr = links.begin(),
         stop = links.end();
       itr != stop;
       ++itr)
    {
      printf("%s: %s\n", exe.c_str(), (*itr).c_str());
    }

  links.clear();

  for (dep_list_t::const_iterator
         itr = ldeps.begin(),
         stop = ldeps.end();
       itr != stop;
       ++itr)
    {
      if ((*itr)->is_phantom() && !(*itr)->is_pruned())
        {
          const string t = cfg.phantom_link_formats.transform((*itr)->name());
          if (!t.empty())
            links.insert(t);
        }
    }

  for (set<string>::iterator
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

  if (cfg.output_comment_character != 0)
    printf("%s %s\n",
           cfg.output_comment_character,
           "Do not edit this file! "
           "It is automatically generated. "
           "Changes will be lost.");

  while (!files.empty())
    {
      const string current_file = files.back();
      files.pop_back();

      if (is_directory(current_file.c_str()))
        {
          if (cfg.verbosity >= NOISY)
            cfg.info() << "considering directory:" << current_file << '\n';

          ++cfg.nest_level;

          errno = 0;
          DIR* d = opendir(current_file.c_str());
          if (d == 0)
            {
              cerr << "ERROR: couldn't read directory: "
                   << current_file.c_str() << "\n"
                   << strerror(errno) << "\n";
              exit(1);
            }

          for (dirent* e = readdir(d); e != 0; e = readdir(d))
            {
              if (should_prune_directory(e->d_name))
                {
                  if (cfg.verbosity >= NOISY)
                    cfg.info() << "pruning file:" << e->d_name << '\n';

                  continue;
                }

              if (cfg.verbosity >= NOISY)
                cfg.info() << "adding file:" << e->d_name << '\n';

              files.push_back(join_filename(current_file, e->d_name));
            }

          --cfg.nest_level;

          if (cfg.verbosity >= NOISY)
            cfg.info() << "finished directory:" << current_file << '\n';

          closedir(d);
        }
      else
        {
          if (cfg.verbosity >= NOISY)
            cfg.info() << "considering file:" << current_file << '\n';

          ++cfg.nest_level;

          file_info* finfo = file_info::get(current_file);

          if (cfg.verbosity >= NOISY)
            cfg.info() << "got finfo with fname " << finfo->name() << '\n';

          if (cfg.output_mode & DIRECT_CDEPS)
            {
              print_direct_cdeps(finfo);
            }

          if (cfg.output_mode & MAKEFILE_CDEPS)
            {
              print_makefile_dep(finfo);
            }

          if (cfg.output_mode & MAKEFILE_LDEPS)
            {
              print_link_deps(finfo);
            }

          if ((cfg.output_mode & LDEP_GROUPS) ||
              (cfg.output_mode & LDEP_LEVELS) ||
              (cfg.output_mode & LDEP_LEVELSV) ||
              (cfg.output_mode & LDEP_ADJACENCY) ||
              (cfg.output_mode & LDEP_RAW))
            {
              if (finfo->is_cc_file())
                (void) finfo->get_nested_ldeps();
            }

          --cfg.nest_level;

          if (cfg.verbosity >= NOISY)
            cfg.info() << "finished file:" << current_file << '\n';
        }
    }

  cfg.exe_formats.give_warnings();

  if (cfg.output_mode & LDEP_GROUPS)
    {
      file_info::dump_ldep_groups();
    }
  if (cfg.output_mode & LDEP_LEVELS)
    {
      file_info::dump_ldep_levels(false);
    }
  if (cfg.output_mode & LDEP_LEVELSV)
    {
      file_info::dump_ldep_levels(true);
    }
  if (cfg.output_mode & LDEP_ADJACENCY)
    {
      file_info::dump_ldep_adjacency();
    }
  if (cfg.output_mode & LDEP_RAW)
    {
      file_info::dump_ldep_raw();
    }
}

int main(int argc, char** argv)
{
  cppdeps dc(argc, argv);
  dc.traverse_sources();
//   file_info::dump();
  exit(0);
}

static const char vcid_cppdeps_cc[] = "$Id$ $HeadURL$";
#endif // !CPPDEPS_CC_DEFINED
