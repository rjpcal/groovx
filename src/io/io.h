///////////////////////////////////////////////////////////////////////
// io.h
// Rob Peters 
// created: Jan-99
// written: Fri Mar 12 13:07:19 1999
///////////////////////////////////////////////////////////////////////

#ifndef IO_H_DEFINED
#define IO_H_DEFINED

class istream; class ostream;

enum IOResult {IO_ERROR=0, IO_OK=1};

IOResult checkStream(ostream &os);
IOResult checkStream(istream &is);

///////////////////////////////////////////////////////////////////////
// IO class
///////////////////////////////////////////////////////////////////////

class IO {
public:
  enum IOFlag { NO_FLAGS        = 0,
                TYPENAME        = 1 << 0,
                BASES           = 1 << 1 };

  virtual ~IO() {}
  virtual IOResult serialize(ostream &os, IOFlag flag = NO_FLAGS) const=0;
  virtual IOResult deserialize(istream &is, IOFlag flag = NO_FLAGS);
};

static const char vcid_io_h[] = "$Id$";
#endif // !IO_H_DEFINED
