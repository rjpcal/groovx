///////////////////////////////////////////////////////////////////////
//
// mtxops.h
//
// Copyright (c) 2002-2004 Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sat Mar  2 18:34:12 2002
// commit: $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MTXOPS_H_DEFINED
#define MTXOPS_H_DEFINED

class Mtx;

Mtx squared(const Mtx& src);

Mtx zeropad(const Mtx& src, int new_mrows, int new_ncols,
            int* ppadtop=0, int* ppadleft=0);

static const char vcid_mtxops_h[] = "$Header$";
#endif // !MTXOPS_H_DEFINED
