///////////////////////////////////////////////////////////////////////
//
// asciistreamwriter.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 13:05:56 1999
// written: Tue Oct 19 16:08:43 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ASCIISTREAMWRITER_H_DEFINED
#define ASCIISTREAMWRITER_H_DEFINED

#ifndef WRITER_H_DEFINED
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
//
// AsciiStreamWriter class definition
//
///////////////////////////////////////////////////////////////////////

class AsciiStreamWriter : public Writer {
public:
  AsciiStreamWriter(ostream& os);
  virtual ~AsciiStreamWriter();

  virtual void writeChar(const string& name, char val);
  virtual void writeInt(const string& name, int val);
  virtual void writeBool(const string& name, bool val);
  virtual void writeDouble(const string& name, double val);

  virtual void writeString(const string& name, const string& val);
  virtual void writeCstring(const string& name, const char* val);

  virtual void writeValueObj(const string& name, const Value& value);

  virtual void writeObject(const string& name, const IO* obj);
  virtual void writeRoot(const IO* root);

private:
  void flushAttributes();

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
