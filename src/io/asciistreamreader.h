///////////////////////////////////////////////////////////////////////
//
// asciistreamreader.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:54:54 1999
// written: Tue Oct 19 16:04:14 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ASCIISTREAMREADER_H_DEFINED
#define ASCIISTREAMREADER_H_DEFINED

#ifndef READER_H_DEFINED
#include "reader.h"
#endif

class istream;

// This is a hack to help shorten up names for assemblers on systems
// that need short identifier names. A typedef at the end of this file
// re-introduces AsciiStreamReader as a typedef for ASR, so that
// client code should be able to use the typename AsciiStreamReader in
// all cases.
#if defined(IRIX6) || defined(HP9000S700)
#define AsciiStreamReader ASR
#endif

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamReader class definition
//
///////////////////////////////////////////////////////////////////////

class AsciiStreamReader : public Reader {
public:
  AsciiStreamReader(istream& is);
  virtual ~AsciiStreamReader();

  virtual char readChar(const string& name);
  virtual int readInt(const string& name);
  virtual bool readBool(const string& name);
  virtual double readDouble(const string& name);

  virtual string readString(const string& name);
  virtual char* readCstring(const string& name);

  virtual void readValueObj(const string& name, Value& value);

  virtual IO* readObject(const string& name);
  virtual IO* readRoot(IO* root=0);

protected:
  // This function removes all whitespace characters from the istream
  // buffer, until the first non-whitespace character is seen. It
  // returns the number of whitespace characters that were removed
  // from the stream.
  int eatWhitespace();

private:
  void initAttributes();

  class Impl;
  Impl& itsImpl;
};

// The second part of the hack to shorten mangled names.
#if defined(IRIX6) || defined(HP9000S700)
#undef AsciiStreamReader
typedef ASR AsciiStreamReader;
#endif

static const char vcid_asciistreamreader_h[] = "$Header$";
#endif // !ASCIISTREAMREADER_H_DEFINED
