///////////////////////////////////////////////////////////////////////
//
// gaborset.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon May 12 11:15:29 2003
// written: Mon May 12 13:33:08 2003
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

#ifndef GABORSET_H_DEFINED
#define GABORSET_H_DEFINED

const int GABOR_MAX_ORIENT = 64;
const int GABOR_MAX_PHASE = 8;

class GaborPatch
{
public:
  GaborPatch(double sigma, double omega, double theta,
             double phi, double contrast);

  ~GaborPatch() { delete [] itsData; }

  int size() const { return itsSize; }

  double operator[](int i) const { return itsData[i]; }

  double at(int x, int y) const { return itsData[x + y*itsSize]; }

private:
  const int itsSize;
  const double itsSigma;
  const double itsOmega;
  const double itsTheta;
  const double itsPhi;
  const double itsContrast;
  double* const itsData;

  GaborPatch(const GaborPatch&);
  GaborPatch& operator=(const GaborPatch&);
};

class GaborSet
{
public:
  GaborSet(double period, double sigma);
  ~GaborSet();

  const GaborPatch& getPatch(double theta, double phi) const;

private:
  GaborPatch* Patch[GABOR_MAX_ORIENT][GABOR_MAX_PHASE];
};

static const char vcid_gaborset_h[] = "$Header$";
#endif // !GABORSET_H_DEFINED
