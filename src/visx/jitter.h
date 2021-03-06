/** @file visx/jitter.h GxTransform subclass with spatial randomness */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Apr  7 13:46:40 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_VISX_JITTER_H_UTC20050626084016_DEFINED
#define GROOVX_VISX_JITTER_H_UTC20050626084016_DEFINED

#include "gfx/gxtransform.h"

/// Jitter is a GxTransform subclass that has randomness in its position.
class Jitter : public GxTransform
{
  //////////////
  // creators //
  //////////////

  /// Default constructor.
  Jitter();

public:
  /// Default creator.
  static Jitter* make();

  /// Virtual destructor.
  virtual ~Jitter() noexcept;

  virtual io::version_id class_version_id() const override;
  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  //////////////////
  // manipulators //
  //////////////////

  void setXJitter(double xj) { itsXJitter = xj; }
  void setYJitter(double yj) { itsYJitter = yj; }
  void setRJitter(double rj) { itsRJitter = rj; }

  void setJitter(double xj, double yj, double rj)
  {
    itsXJitter = xj;
    itsYJitter = yj;
    itsRJitter = rj;
  }


  /////////////
  // actions //
  /////////////

public:
  virtual void draw(Gfx::Canvas&) const override;

private:
  void rejitter() const;

  // These specify the amount of jitter that will be effected by the
  // translate() or rotate() calls; the actual value used will be in
  // the range of the specified value +/- jitter value.
  double itsXJitter;
  double itsYJitter;
  double itsRJitter;

  mutable double itsXShift;
  mutable double itsYShift;
  mutable double itsRShift;
};

#endif // !GROOVX_VISX_JITTER_H_UTC20050626084016_DEFINED
