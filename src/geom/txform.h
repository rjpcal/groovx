///////////////////////////////////////////////////////////////////////
//
// txform.h
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jun 21 13:57:32 2002
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

#ifndef TXFORM_H_DEFINED
#define TXFORM_H_DEFINED

namespace geom
{
  template <class V> class vec2;
  template <class V> class vec3;

  /// Represents a 4x4 transformation matrix in homogenous coordinates.
  class txform
  {
  private:
    /// Default constructor; matrix entries are left uninitialized.
    /** The dummy bool parameter is there just as a reminder that this
        constructor leaves the matrix entries in an unspecified state;
        i.e. so that we don't carelessly use an apparently "default"
        constructor that doesn't set any special default. */
    txform(bool) {}

  public:
    txform(const vec3<double>& translation,
           const vec3<double>& scaling,
           const vec3<double>& rotation_axis,
           double rotation_angle);

    /// Factory function to make an identity matrix.
    static txform identity();

    /// Factory function to make a random matrix (uniform distribution in [0,1]).
    /** This is mainly intended for use in testing contexts. */
    static txform random();

    /// Factory function to make a matrix with uninitialized entries.
    static txform no_init() { return txform(true); }

    /// Get the inverse of this transformation.
    geom::txform inverted() const;

    void translate(const vec3<double>& t);
    void scale(const vec3<double>& s);
    void rotate(const vec3<double>& rotation_axis, double rotation_angle);

    /// Equivalent to this = this x other, where 'x' is matrix multiplication.
    void transform(const geom::txform& other);

    vec2<double> apply_to(const vec2<double>& input) const;
    vec3<double> apply_to(const vec3<double>& input) const;

    const double* col_major_data() const { return &m_mtx[0]; }

    void set_col_major_data(const double* data);

    double  operator[](unsigned int i) const { return m_mtx[i]; }
    double& operator[](unsigned int i)       { return m_mtx[i]; }

    void debug_dump() const;

  private:
    double m_mtx[16];
  };

} // end namespace geom

static const char vcid_txform_h[] = "$Header$";
#endif // !TXFORM_H_DEFINED
