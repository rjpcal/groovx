///////////////////////////////////////////////////////////////////////
//
// system.h
//
// Copyright (c) 1999-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 17 15:05:41 1999
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

#ifndef SYSTEM_H_DEFINED
#define SYSTEM_H_DEFINED

class fstring;

/// Singleton class with wrappers of OS/system calls.
class System
{
protected:
  ///
  System();
public:

  ///
  virtual ~System();

  ///
  static System& theSystem();

  ///
  typedef unsigned long mode_t;

  /// read by owner
  static const mode_t IRUSR   = 00400;
  /// write by owner
  static const mode_t IWUSR   = 00200;
  /// execute/search by owner
  static const mode_t IXUSR   = 00100;
  /// read by group
  static const mode_t IRGRP   = 00040;
  /// write by group
  static const mode_t IWGRP   = 00020;
  /// execute/search by group
  static const mode_t IXGRP   = 00010;
  /// read by others
  static const mode_t IROTH   = 00004;
  /// write by others
  static const mode_t IWOTH   = 00002;
  /// execute/search by others
  static const mode_t IXOTH   = 00001;


  /**  DESCRIPTION
       The mode of the file given by path is changed.

       Modes are specified by or'ing the following:

       ERRORS
       Throws a Util::Error on error if an error occurs. The more
       general errors for chmod are listed below:

       EPERM   The effective UID does not match the owner of  the
               file, and is not zero.
       EROFS   The named file resides on a read-only file system.
       EFAULT  path points outside your accessible address space.
       ENAMETOOLONG
               path is too long.
       ENOENT  The file does not exist.
       ENOMEM  Insufficient kernel memory was available.
       ENOTDIR A component of the path prefix is not a directory.
       EACCES  Search permission is denied on a component of  the
               path prefix.
       ELOOP   Too   many  symbolic  links  were  encountered  in
               resolving path.
       EIO     An I/O error occurred. */
  void chmod(const char* path, mode_t mode);


  /**  DESCRIPTION
       rename  renames  a  file, moving it between directories if
       required.

       Any other hard links to the file (as created  using  link)
       are unaffected.

       If  newpath already exists it will be atomically overwrit-
       ten (subject to a few conditions - see ERRORS  below),  so
       that there is no point at which another process attempting
       to access newpath will find it missing.

       If newpath exists but the operation fails for some  reason
       or  the  system  crashes  rename  guarantees  to  leave an
       instance of newpath in place.

       However, when overwriting there will probably be a  window
       in  which both oldpath and newpath refer to the file being
       renamed.

       If oldpath refers to a symbolic link the link is  renamed;
       if  newpath  refers  to  a  symbolic link the link will be
       overwritten.

       ERRORS
       Throws a Util::Error on error if an error occurs.
  */
  void rename(const char* oldpath, const char* newpath);


  /**  DESCRIPTION
       remove  deletes  a  name from the filesystem. If that name
       was the last link to a file and no processes have the file
       open  the  file  is  deleted and the space it was using is
       made available for reuse.

       If the name was the last link to a file but any  processes
       still have the file open the file will remain in existence
       until the last file descriptor referring to it is  closed.

       If  the  name  referred  to  a  symbolic  link the link is
       removed.

       If the name referred to a socket, fifo or device the  name
       for it is removed but processes which have the object open
       may continue to use it.

       ERRORS
       Throws a Util::Error on error if an error occurs.
  */
  void remove(const char* pathname);

  /** Get the absolute pathname of the current working
      directory. Warning: the result of this function is only valid
      until the next call to the function. */
  const char* getcwd();

  /** getenv() searches the environment list for a string of the form
      name=value, and returns a pointer to the value in the current
      environment if such a string is present, otherwise a NULL
      pointer.  name can be either the desired name, null-terminated,
      or of the form name=value, in which case getenv() uses the
      portion to the left of the = as the search key.

      getenv() returns a pointer to static data which can be overwritten by
      subsequent calls. */
  const char* getenv(const char* environment_variable);

  /** Suspend execution of the calling process for the specified
      number of seconds. */
  void sleep(unsigned int seconds);
};

static const char vcid_system_h[] = "$Header$";
#endif // !SYSTEM_H_DEFINED
