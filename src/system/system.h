///////////////////////////////////////////////////////////////////////
//
// system.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Nov 17 15:05:41 1999
// written: Wed Nov 17 17:40:49 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SYSTEM_H_DEFINED
#define SYSTEM_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

///
class System {
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

		 RETURN VALUE
       On  success,  zero is returned.  On error, -1 is returned,
       and errno is set appropriately.

		 ERRORS
       Depending  on  the  file  system,  other  errors  can   be
       returned.   The  more  general errors for chmod are listed
       below:

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
  int chmod(const char* path, mode_t mode);


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

		 RETURN VALUE
       On success, zero is returned.  On error, -1  is  returned,
       and errno is set appropriately. */
  int rename(const char* oldpath, const char* newpath);


  /// Get the absolute pathname of the current working directory
  string getcwd();
};

static const char vcid_system_h[] = "$Header$";
#endif // !SYSTEM_H_DEFINED
