///////////////////////////////////////////////////////////////////////
//
// system.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Nov 17 15:05:41 1999
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

#ifndef SYSTEM_H_DEFINED
#define SYSTEM_H_DEFINED

#include <sys/types.h>

namespace rutz
{
  class fstring;

  /// Wrappers of OS/system calls.
  namespace unixcall
  {
    /**  DESCRIPTION
         The mode of the file given by path is changed.

         Modes are specified by or'ing the following:

         ERRORS
         Throws a rutz::error on error if an error occurs. The more
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
         Throws a rutz::error on error if an error occurs.
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
         Throws a rutz::error on error if an error occurs.
    */
    void remove(const char* pathname);

    /// Get the absolute pathname of the current working directory.
    rutz::fstring getcwd();

  } // end namespace unixcall

} // end namespace rutz

static const char vcid_system_h[] = "$Header$";
#endif // !SYSTEM_H_DEFINED