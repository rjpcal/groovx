///////////////////////////////////////////////////////////////////////
//
// bmapdata.h
//
// Copyright (c) 2000-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Jan 19 17:25:51 2000
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

#ifndef BMAPDATA_H_DEFINED
#define BMAPDATA_H_DEFINED

namespace rutz
{
  template <class T> class shared_ptr;
}

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
   * and storage alignemnt requirements of a chunk of image data. In
   * addition, \c media::bmap_data provides for lazy updates via the
   * \c update_func class. Clients may subclass \c update_func to
   * implement \c update_func::update(), then pass this type of object
   * to \c queue_update(). Then the \c update() function will be
   * called only when the bitmap data need to be accessed.
   *
   **/
  ///////////////////////////////////////////////////////////

  class bmap_data
  {
  public:
    /// Nested class in allows \c bmap_data objects to be updated lazily.
    class update_func
    {
    public:
      /// Virtual destructor ensures correct destruction of subclasses.
      virtual ~update_func();

      /** To be overridden by subclasses to provide a way to update the
          \c bmap_data. */
      virtual void update(bmap_data& update_me) = 0;
    };

    /// Specifies the order in which the rows are physically arranged in memory.
    enum row_order
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
    bmap_data(const geom::vec2<int>& dims,
              int bits_per_pixel,
              int byte_alignment);

    /// Copy constructor.
    bmap_data(const bmap_data& other);

    /// Assignment operator
    bmap_data& operator=(const bmap_data& other)
    {
      bmap_data copy(other);
      this->swap(copy);
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
    unsigned char* row_ptr(unsigned int row) const;

    /// Returns a checksum of all the bytes in the image data.
    long int bytes_sum() const;

    /// Returns the bitmap's width in pixels.
    int width() const;

    /// Returns the bitmap's height in pixels.
    int height() const;

    /// Returns the bitmap's size (x-width, y-height) in pixels.
    geom::vec2<int> size() const;

    /// Returns the number of bits used per pixel in the bitmap.
    int bits_per_pixel() const;

    /// Returns the number of bits used per each pixel component.
    /** This differs from bits_per_pixel() if each pixel contains
        multiple components (e.g. as in an RGB image): the
        bits_per_component() might be 8 while the bits_per_pixel()
        would be 24. */
    int bits_per_component() const;

    /// Returns the byte alignment of the bitmap data.
    /** Each image row in the data will begin on a multiple of this
        number of bytes. */
    int byte_alignment() const;

    /// Returns the number of bytes used by the bitmap data.
    /** Some of these bytes may be 'filler bytes' needed to meet the
        storage alignment requirements. */
    unsigned int byte_count() const;

    /// Returns the number of bytes used per image row in the bitmap data.
    unsigned int bytes_per_row() const;

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
    void swap(bmap_data& other);

    /// Queues the update given by \a updater.
    /** The \c update() function will be called only when the bitmap
        data must be accessed. */
    void queue_update(rutz::shared_ptr<update_func> updater) const;

    /// Forces any pending update to be called.
    void update_if_needed() const;

    /// Cancels any pending update.
    void clear_queued_update() const;

    /// Set the row order, swapping rows around in memory if necessary.
    /** This is a logical "const" operation since the image being
        represented stays the same; only its representation changes. */
    void set_row_order(row_order order) const;

    /// Specify the row order but leave the actual memory untouched.
    void specify_row_order(row_order order) const;

    /// Generate a new image from scrambled subparts of the current image.
    rutz::shared_ptr<bmap_data>
    make_scrambled(int numsubcols, int numsubrows,
                   int seed,
                   bool allow_move_subparts = true,
                   bool allow_flip_left_right = true,
                   bool allow_flip_top_bottom = true) const;

  private:
    class impl;
    impl* rep;
  };

} // end namespace media

static const char vcid_bmapdata_h[] = "$Header$";
#endif // !BMAPDATA_H_DEFINED
