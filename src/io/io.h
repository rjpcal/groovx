///////////////////////////////////////////////////////////////////////
// io.h
// Rob Peters 
// created: Jan-99
// written: Tue Mar  9 21:36:51 1999
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

#endif // !IO_H_DEFINED
