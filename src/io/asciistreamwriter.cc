///////////////////////////////////////////////////////////////////////
//
// asciistreamwriter.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 13:05:57 1999
// written: Tue Oct 19 16:11:23 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ASCIISTREAMWRITER_CC_DEFINED
#define ASCIISTREAMWRITER_CC_DEFINED

#include "asciistreamwriter.h"

#include <iostream.h>
#include <strstream.h>
#include <typeinfo>
#include <cstring>
#include <vector>
#include <set>

#include "demangle.h"
#include "io.h"
#include "value.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

#if defined(IRIX6) || defined(HP9000S700)
#define AsciiStreamWriter ASW
#endif

class AsciiStreamWriter::Impl {
public:
  Impl(ostream& os) : itsBuf(os), itsToHandle(), itsHandled() {}

  ostream& itsBuf;
  vector<const IO *> itsToHandle;
  set<const IO *> itsHandled;
  vector<string> itsAttribs;
};

AsciiStreamWriter::AsciiStreamWriter (ostream& os) : 
  itsImpl( *(new Impl(os)) )
{
DOTRACE("AsciiStreamWriter::AsciiStreamWriter");
  // nothing
}

AsciiStreamWriter::~AsciiStreamWriter () {
DOTRACE("AsciiStreamWriter::~AsciiStreamWriter");
  delete &itsImpl; 
}

void AsciiStreamWriter::writeChar(const string& name, char val) {
DOTRACE("AsciiStreamWriter::writeChar");
  vector<char> buf(32 + name.length());
  ostrstream ost(&buf[0], 32 + name.length());
  ost << "char " << name << " := " << val << '\0' ;
  itsImpl.itsAttribs.push_back(&buf[0]);
}

void AsciiStreamWriter::writeInt(const string& name, int val) {
DOTRACE("AsciiStreamWriter::writeInt");
  vector<char> buf(32 + name.length());
  ostrstream ost(&buf[0], 32 + name.length());
  ost << "int " << name << " := " << val << '\0';
  itsImpl.itsAttribs.push_back(&buf[0]);
}

void AsciiStreamWriter::writeBool(const string& name, bool val) {
DOTRACE("AsciiStreamWriter::writeBool");
  vector<char> buf(32 + name.length());
  ostrstream ost(&buf[0], 32 + name.length());
  ost << "int " << name << " := " << val << '\0';
  itsImpl.itsAttribs.push_back(&buf[0]);
}

void AsciiStreamWriter::writeDouble(const string& name, double val) {
DOTRACE("AsciiStreamWriter::writeDouble");
  vector<char> buf(32 + name.length());
  ostrstream ost(&buf[0], 32 + name.length());
  ost << "double " << name << " := " << val << '\0';
  itsImpl.itsAttribs.push_back(&buf[0]);
}

void AsciiStreamWriter::writeString(const string& name, const string& val) {
DOTRACE("AsciiStreamWriter::writeString");
  vector<char> buf(32 + name.length() + val.length());
  ostrstream ost(&buf[0], 32 + name.length() + val.length());
  ost << "string " << name << " := " 
		<< val.length() << " " << val << '\0';
  itsImpl.itsAttribs.push_back(&buf[0]);
}

void AsciiStreamWriter::writeCstring(const string& name, const char* val) {
DOTRACE("AsciiStreamWriter::writeCstring");
  int val_len = strlen(val);
  vector<char> buf(32 + name.length() + val_len);
  ostrstream ost(&buf[0], 32 + name.length() + val_len);
  ost << "cstring " << name << " := " 
		<< val_len << " " << val << '\0';
  itsImpl.itsAttribs.push_back(&buf[0]);
}

void AsciiStreamWriter::writeValueObj(const string& name, const Value& value) {
DOTRACE("AsciiStreamWriter::writeValueObj");
  switch (value.getNativeType()) {
  case Value::INT:
	 writeInt(name, value.getInt());
	 break;
  case Value::LONG:
	 writeInt(name, value.getInt());
	 break;
  case Value::BOOL:
	 writeBool(name, value.getBool());
	 break;
  case Value::DOUBLE:
	 writeDouble(name, value.getDouble());
	 break;
  case Value::CSTRING:
	 writeCstring(name, value.getCstring());
	 break;
  case Value::STRING:
	 writeString(name, value.getString());
	 break;
  default:
	 throw WriteError("couldn't handle Value type");
	 break;
  }
}

void AsciiStreamWriter::writeObject(const string& name, const IO* obj) {
DOTRACE("AsciiStreamWriter::writeObject");
  vector<char> buf(32 + name.length());
  ostrstream ost(&buf[0], 32 + name.length());

  if (obj == 0) {
	 ost << "NULL " << name << " := 0" << '\0';
  }
  else {
	 ost << demangle(typeid(*obj).name())
		  << " " << name << " := " << obj->id() << '\0';
	 
	 if (itsImpl.itsHandled.find(obj) == itsImpl.itsHandled.end()) {
		itsImpl.itsToHandle.push_back(obj);
	 }
  }

  itsImpl.itsAttribs.push_back(&buf[0]);
}

void AsciiStreamWriter::writeRoot(const IO* root) {
DOTRACE("AsciiStreamWriter::writeRoot");
  itsImpl.itsToHandle.clear();
  itsImpl.itsHandled.clear();

  itsImpl.itsToHandle.push_back(root);

  while ( !itsImpl.itsToHandle.empty() ) {
	 const IO* obj = itsImpl.itsToHandle.back();
	 itsImpl.itsToHandle.pop_back();

	 itsImpl.itsBuf << demangle(typeid(*obj).name())
						 << " " << obj->id() << " := { ";

	 itsImpl.itsHandled.insert(obj);

	 obj->writeTo(this);
	 flushAttributes();

	 itsImpl.itsBuf << '}' << endl;
  }
}

void AsciiStreamWriter::flushAttributes() {
DOTRACE("AsciiStreamWriter::flushAttributes");

  itsImpl.itsBuf << itsImpl.itsAttribs.size() << endl;

  for (int i = 0; i < itsImpl.itsAttribs.size(); ++i) {
	 itsImpl.itsBuf << itsImpl.itsAttribs[i] << endl;
  }

  itsImpl.itsAttribs.clear();
}

#if defined(IRIX6) || defined(HP9000S700)
#undef AsciiStreamWriter
#endif

static const char vcid_asciistreamwriter_cc[] = "$Header$";
#endif // !ASCIISTREAMWRITER_CC_DEFINED
