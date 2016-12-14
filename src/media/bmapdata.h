/** @file media/bmapdata.h storage class for bitmap images, handling
    sizing and storage alignment */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jan 19 17:25:51 2000
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

#ifndef GROOVX_MEDIA_BMAPDATA_H_UTC20050626084018_DEFINED
#define GROOVX_MEDIA_BMAPDATA_H_UTC20050626084018_DEFINED

#include <cstddef> // size_t
#include <cstdint> // uint32_t
#include <memory>

namespace geom
{
  template <class V> class vec2;
}

namespace media
{
  ///////////////////////////////////////////////////////////
  /**
   *
   * \c media::bmap_data handles the low-level details of manipulating
   * bitmap data. It keeps track of the width, height, bits-per-pixel,
   * and storage alignment requirements of a chunk of image data.
   *
   **/
  ///////////////////////////////////////////////////////////

  class bmap_data
  {
  public:

    /// Specifies the order in which the rows are physically arranged in memory.
    enum class row_order
      {
        TOP_FIRST,
        BOTTOM_FIRST
      };

    //---------------------------------------------------------
    //
    // Creators
    //
    //---------------------------------------------------------

    /// Construct with empty image data.
    bmap_data();

    /// Construct with the given image data specifications.
    bmap_data(const geom::vec2<size_t>& dims,
              unsigned int bits_per_pixel,
              unsigned int byte_alignment);

    /// Copy constructor.
    explicit bmap_data(const bmap_data& that);

    /// Copy assignment operator
    bmap_data& operator=(const bmap_data& that) = delete;

    /// Move constructor
    bmap_data(bmap_data&& that) noexcept
      :
      rep(that.rep)
    {
      that.rep = nullptr;
    }

    /// Move assignment operator
    bmap_data& operator=(bmap_data&& that) noexcept
    {
      this->swap(that);
      return *this;
    }

    /// Destructor frees Impl*.
    ~bmap_data();

    //---------------------------------------------------------
    //
    // Accessors
    //
    //---------------------------------------------------------

    /// Returns a pointer to the raw image data.
    unsigned char* bytes_ptr() const;

    /// Returns a pointer to the beginning of the specified row.
    /** This takes into account whether the current row_order is
        TOP_FIRST or BOTTOM_FIRST. */
    unsigned char* row_ptr(size_t row) const;

    /// Returns a checksum of all the bytes in the image data.
    long int bytes_sum() const;

    /// Compute the BKDR hash across all the bytes in the image data.
    uint32_t bkdr_hash() const;

    /// Returns the bitmap's width in pixels.
    size_t width() const;

    /// Returns the bitmap's height in pixels.
    size_t height() const;

    /// Returns the bitmap's size (x-width, y-height) in pixels.
    geom::vec2<size_t> size() const;

    /// Returns the number of bits used per pixel in the bitmap.
    unsigned int bits_per_pixel() const;

    /// Returns the number of bits used per each pixel component.
    /** This differs from bits_per_pixel() if each pixel contains
        multiple components (e.g. as in an RGB image): the
        bits_per_component() might be 8 while the bits_per_pixel()
        would be 24. */
    unsigned int bits_per_component() const;

    /// Returns the byte alignment of the bitmap data.
    /** Each image row in the data will begin on a multiple of this
        number of bytes. */
    unsigned int byte_alignment() const;

    /// Returns the number of bytes used by the bitmap data.
    /** Some of these bytes may be 'filler bytes' needed to meet the
        storage alignment requirements. */
    size_t byte_count() const;

    /// Returns the number of bytes used per image row in the bitmap data.
    size_t bytes_per_row() const;

    /// Get the current row order.
    row_order get_row_order() const;

    //---------------------------------------------------------
    //
    // Manipulators
    //
    //---------------------------------------------------------

    /// Flips the image contrast of the bitmap data.
    void flip_contrast();

    /// Flips the image vertically.
    void flip_vertical();

    /// Clears the bitmap data, erasing the current image.
    void clear();

    /// Swaps the internal representation with that of \a other.
    void swap(bmap_data& other) noexcept;

    /// Set the row order, swapping rows around in memory if necessary.
    /** This is a logical "const" operation since the image being
        represented stays the same; only its representation changes. */
    void set_row_order(row_order order) const;

    /// Specify the row order but leave the actual memory untouched.
    void specify_row_order(row_order order) const;

    /// Generate a new image from scrambled subparts of the current image.
    bmap_data make_scrambled(unsigned int numsubcols, unsigned int numsubrows,
                             unsigned long seed,
                             bool allow_move_subparts = true,
                             bool allow_flip_left_right = true,
                             bool allow_flip_top_bottom = true) const;

  private:
    class impl;
    impl* rep;
  };

} // end namespace media

#endif // !GROOVX_MEDIA_BMAPDATA_H_UTC20050626084018_DEFINED
