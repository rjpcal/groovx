///////////////////////////////////////////////////////////////////////
//
// soundlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  8 11:44:40 1999
// written: Fri Oct 27 17:50:15 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUNDLIST_H_DEFINED
#define SOUNDLIST_H_DEFINED

class IoPtrList;

namespace SoundList {
  IoPtrList& theSoundList();
}

static const char vcid_soundlist_h[] = "$Header$";
#endif // !SOUNDLIST_H_DEFINED
