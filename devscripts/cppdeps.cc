///////////////////////////////////////////////////////////////////////
//
// cppdeps.cc
//
// Copyright (c) 2003-2003 Rob Peters rjpeters@klab.caltech.edu
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
using std::cout;

/// A class for doing fast include-file dependency analysis.
/** Several shortcuts (er, hacks...) are taken to make the parsing
    extremely fast and cheap, but at worst this makes the computed
    dependencies be unnecessarily pessimistic. For example, a #include that
    occurs inside a comment will still be treated as a regular #include.*/
class cppdeps
{
  vector<string> user_ipath;
  vector<string> sys_ipath;

  string srcdir;
  string objdir;

  typedef vector<string> include_list_t;

  typedef map<string, include_list_t> include_map_t;
  include_map_t nested_includes;
  include_map_t direct_includes;

public:
  void inspect();

  string trim_dirname(const string& inp);

  cppdeps(char** argv);

  int is_cc_filename(const char* fname);

  void resolve_one(const char* include_name,
                   int include_length,
                   const string& filename,
                   const string& dirname,
                   include_list_t& vec);

  const include_list_t& get_direct_includes(const string& filename);

  const include_list_t& get_nested_includes(const string& filename);

  void batch_build();
};

void cppdeps::inspect()
{
  cerr << "user_ipath:\n";
  for (unsigned int i = 0; i < user_ipath.size(); ++i)
    cerr << '\t' << user_ipath[i] << '\n';

  cerr << "\nsys_ipath:\n";
  for (unsigned int i = 0; i < sys_ipath.size(); ++i)
    cerr << '\t' << sys_ipath[i] << '\n';

  cerr << "\nsrcdir: " << srcdir << '\n';
  cerr << "\nobjdir: " << objdir << '\n';
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

cppdeps::cppdeps(char** argv)
{
  sys_ipath.push_back("/usr/include");
  sys_ipath.push_back("/usr/include/linux");
  sys_ipath.push_back("/usr/local/matlab/extern/include");

  while (*argv != 0)
    {
      if (strcmp(*argv, "--include") == 0)
	{
	  user_ipath.push_back(*++argv);
	}
      else if (strcmp(*argv, "--src") == 0)
	{
	  string dir = trim_dirname(*++argv);
	  srcdir = dir;
	  user_ipath.push_back(dir);
	}
      else if (strcmp(*argv, "--objdir") == 0)
	{
	  objdir = trim_dirname(*++argv);
	}
      ++argv;
    }
}

int cppdeps::is_cc_filename(const char* fname)
{
  unsigned int len = strlen(fname);
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

void cppdeps::resolve_one(const char* include_name,
			  int include_length,
			  const string& filename,
			  const string& dirname,
			  cppdeps::include_list_t& vec)
{
  for (unsigned int i = 0; i < user_ipath.size(); ++i)
    {
      string fullpath = user_ipath[i];
      fullpath += '/';
      fullpath.append(include_name, include_length);

      struct stat statbuf;
      if (stat(fullpath.c_str(), &statbuf) == 0)
	{
	  vec.push_back(fullpath);
	  return;
	}
    }

  // Try resolving the include by using the directory containing the
  // source file currently being examined.
  string fullpath = dirname;
  fullpath.append(include_name, include_length);

  struct stat statbuf;
  if (stat(fullpath.c_str(), &statbuf) == 0)
    {
      vec.push_back(fullpath);
      return;
    }

  string include_string(include_name, include_length);

  // Try resolving the include by using the current working directory
  // from which this program was invoked.
  if (stat(include_string.c_str(), &statbuf) == 0)
    {
      if (filename != include_string)
	vec.push_back(include_string);
      return;
    }

  cerr << "warning: couldn\'t resolve #include \""
	    << include_string << "\" for " << filename << "\n";
}

const cppdeps::include_list_t&
cppdeps::get_direct_includes(const string& filename)
{
  {
    include_map_t::iterator itr = direct_includes.find(filename);
    if (itr != direct_includes.end())
      return (*itr).second;
  }

  string dirname = filename.substr(0, filename.find_last_of('/'));
  dirname += "/";

  include_list_t& vec = direct_includes[filename];

  struct stat statbuf;
  if (stat(filename.c_str(), &statbuf) == -1)
    {
      cerr << "couldn't stat file " << filename << "\n";
      exit(1);
    }

  const off_t nbytes = statbuf.st_size;

  const int fd = open(filename.c_str(), O_RDONLY);
  if (fd == -1)
    {
      cerr << "couldn't open file " << filename << "\n";
      exit(1);
    }

  void* mem = mmap(0, nbytes, PROT_READ, MAP_PRIVATE, fd, 0);

  const char* fptr = static_cast<char*>(mem);
  const char* const stop = fptr + nbytes;

  for ( ; fptr < stop; ++fptr)
    {
      while (*fptr != '#' && fptr < stop)
	++fptr;

      if (fptr >= stop)
	break;

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

      if (*fptr++ != '\"') continue;

      const char* const include_name = fptr;

      while (*fptr != '\"' && fptr < stop)
	++fptr;

      if (*fptr != '\"')
	{
	  cerr << "found end-of-file before closing double-quote\n";
	  exit(1);
	}

      const int include_length = fptr - include_name;

      resolve_one(include_name, include_length, filename, dirname, vec);
    }

  munmap(mem, nbytes);
  close(fd);

  return vec;
}

const cppdeps::include_list_t&
cppdeps::get_nested_includes(const string& filename)
{
  {
    include_map_t::iterator itr = nested_includes.find(filename);
    if (itr != nested_includes.end())
      return (*itr).second;
  }

  include_list_t vec;

  vec.push_back(filename);

  const include_list_t& direct = get_direct_includes(filename);

  for (include_list_t::const_iterator
	 i = direct.begin(),
	 istop = direct.end();
       i != istop;
       ++i)
    {
      const include_list_t& indirect = get_nested_includes(*i);

      vec.insert(vec.end(), indirect.begin(), indirect.end());
    }

  include_list_t& result = nested_includes[filename];
  result.swap(vec);

  return result;
}

void cppdeps::batch_build()
{
  vector<string> dirs;

  dirs.push_back(srcdir);

  const unsigned int offset = srcdir.length() + 1;

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
	  if (e->d_name[0] == '.') continue;
	  if (strcmp(e->d_name, "RCS") == 0) continue;
	  if (strcmp(e->d_name, "CVS") == 0) continue;

	  string fullname = dirname;
	  fullname += "/";
	  fullname += e->d_name;

	  struct stat statbuf;
	  errno = 0;
	  if (stat(fullname.c_str(), &statbuf) != 0)
	    {
	      cerr << strerror(errno) << "\n";
	      cerr << "stat failed " << e->d_name << "\n";
	      exit(1);
	    }

	  if (S_ISDIR(statbuf.st_mode))
	    {
	      dirs.push_back(fullname);
	    }
	  else
	    {
	      const int ext_len = is_cc_filename(e->d_name);
	      if (ext_len > 0)
		{
		  const include_list_t& includes = get_nested_includes(fullname);

		  // This essentially does a unique sort for us.
		  const set<string> uniq(includes.begin(), includes.end());

		  // Use C-style stdio here since it came out running quite
		  // a bit faster than iostreams, at least under g++-3.2.
		  printf("%s/%s.o: ",
			 objdir.c_str(),
			 fullname.substr(offset, fullname.length()-ext_len-offset).c_str());

		  for (set<string>::const_iterator
			 itr = uniq.begin(),
			 stop = uniq.end();
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
