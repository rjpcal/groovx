/** @file geom/txform.h 4x4 matrix for homogenous coordinate
    transformations */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Jun 21 13:57:32 2002
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_GEOM_TXFORM_H_UTC20050626084022_DEFINED
#define GROOVX_GEOM_TXFORM_H_UTC20050626084022_DEFINED

namespace geom
{
  template <class V> class rect;
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

    /// Factory function to copy an existing array in column-major order.
    static txform copy_of(const double* data)
    { txform r(true); r.set_col_major_data(data); return r; }

    /// Factory function to create an orthographic transform.
    static txform orthographic(const geom::rect<double>& bounds,
                               double zNear, double zFar);

    /// Factory function to make a matrix with uninitialized entries.
    static txform no_init() { return txform(true); }

    /// Get the inverse of this transformation.
    geom::txform inverted() const;

    void translate(const vec3<double>& t);
    void scale(const vec3<double>& s);
    void rotate(const vec3<double>& rotation_axis, double rotation_angle);

    /// Result = this x other, where 'x' is matrix multiplication.
    txform mtx_mul(const txform& other) const;

    /// Equivalent to this = this->mtx_mul(other).
    void transform(const geom::txform& other);

    vec2<double> apply_to(const vec2<double>& input) const;
    vec3<double> apply_to(const vec3<double>& input) const;

    const double* col_major_data() const { return &m_mtx[0]; }

    void set_col_major_data(const double* data);

    double  operator[](unsigned int i) const { return m_mtx[i]; }
    double& operator[](unsigned int i)       { return m_mtx[i]; }

    void debug_dump() const;

    /// FOR TESTING/DEBUGGING: compute the sum-squared-error to a reference matrix.
    double debug_sse(const txform& ref) const;

  private:
    double m_mtx[16];
  };

} // end namespace geom

static const char __attribute__((used)) vcid_groovx_geom_txform_h_utc20050626084022[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GEOM_TXFORM_H_UTC20050626084022_DEFINED
