///////////////////////////////////////////////////////////////////////
//
// asciistreamwriter.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 13:05:56 1999
// written: Wed Mar 29 14:07:39 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ASCIISTREAMWRITER_H_DEFINED
#define ASCIISTREAMWRITER_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(WRITER_H_DEFINED)
#include "writer.h"
#endif

class ostream;

// This is a hack to help shorten up names for assemblers on systems
// that need short identifier names. A typedef at the end of this file
// re-introduces AsciiStreamWriter as a typedef for ASW, so that
// client code should be able to use the typename AsciiStreamWriter in
// all cases.
#if defined(IRIX6) || defined(HP9000S700)
#define AsciiStreamWriter ASW
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c AsciiStreamWriter implements the \c Writer interface, using nice
 * human-readable ascii formatting. \c AsciiStreamWriter writes
 * objects to an output stream in a ascii format that is readable by
 * \AsciiStreamReader. With this data format, objects may read and
 * write their attributes in any order.
 *
 **/
///////////////////////////////////////////////////////////////////////

class AsciiStreamWriter : public Writer {
public:
  /// Construct with a connection to an open \c ostream.
  AsciiStreamWriter(ostream& os);

  /// Virtual destructor.
  virtual ~AsciiStreamWriter();

  virtual void writeChar(const char* name, char val);
  virtual void writeInt(const char* name, int val);
  virtual void writeBool(const char* name, bool val);
  virtual void writeDouble(const char* name, double val);
  virtual void writeCstring(const char* name, const char* val);
  virtual void writeValueObj(const char* name, const Value& value);

  virtual void writeObject(const char* name, const IO* obj);
  virtual void writeOwnedObject(const char* name, const IO* obj);
  virtual void writeBaseClass(const char* baseClassName, const IO* basePart);

  virtual void writeRoot(const IO* root);

private:
  class Impl;
  Impl& itsImpl;
};


// The second part of the hack to shorten mangled names.
#if defined(IRIX6) || defined(HP9000S700)
#undef AsciiStreamWriter
typedef ASW AsciiStreamWriter;
#endif

static const char vcid_asciistreamwriter_h[] = "$Header$";
#endif // !ASCIISTREAMWRITER_H_DEFINED
