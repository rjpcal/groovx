///////////////////////////////////////////////////////////////////////
//
// tlistutils.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sat Dec  4 02:58:20 1999
// written: Wed Mar 19 18:00:52 2003
// $Id$
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

#ifndef TLISTUTILS_H_DEFINED
#define TLISTUTILS_H_DEFINED

/// Utility functions for working with batches of Trial objects.
class TlistUtils
{
public:
  /** Write a file containing a summary of the responses to each Trial
      in the ObjDb. */
  static void writeResponses(const char* filename);

  static void writeIncidenceMatrix(const char* filename);

  /** Write a matlab-readable file with one line per Trial, where each
      line contains a list of the objid's in that Trial followed by
      the average response given to that Trial. */
  static void writeMatlab(const char* filename);
};

static const char vcid_tlistutils_h[] = "$Header$";
#endif // !TLISTUTILS_H_DEFINED
