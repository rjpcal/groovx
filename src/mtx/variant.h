/** @file pkgs/mtx/variant.h dash::variant type provides a thin
    wrapper around matlab 'struct' objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue Dec 21 10:49:48 2004
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_PKGS_MTX_VARIANT_H_UTC20050626084022_DEFINED
#define GROOVX_PKGS_MTX_VARIANT_H_UTC20050626084022_DEFINED

namespace dash
{
  class variant;
}

namespace rutz
{
  class fstring;
}

class mtx;

class dash::variant
{
public:
  virtual ~variant();

  virtual mtx           as_mtx() const = 0;
  virtual rutz::fstring as_string() const = 0;
  virtual double        as_double() const = 0;
  virtual int           as_int() const = 0;

  virtual mtx           get_mtx_field(const char* field_name) const = 0;
  virtual rutz::fstring get_string_field(const char* field_name) const = 0;
  virtual double        get_double_field(const char* field_name) const = 0;
  virtual int           get_int_field(const char* field_name) const = 0;
  virtual bool          get_bool_field(const char* field_name) const = 0;

  virtual bool          has_field(const char* field_name) const = 0;

  virtual void          set_mtx_field(const char* field_name, const mtx& field) = 0;
  virtual void          set_string_field(const char* field_name, const char* field) = 0;
  virtual void          set_double_field(const char* field_name, double field) = 0;
  virtual void          set_int_field(const char* field_name, int field) = 0;
  virtual void          set_bool_field(const char* field_name, bool field) = 0;
};

#endif // !GROOVX_PKGS_MTX_VARIANT_H_UTC20050626084022_DEFINED
