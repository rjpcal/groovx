///////////////////////////////////////////////////////////////////////
//
// Bitmap
//
///////////////////////////////////////////////////////////////////////

void Bitmap::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("Bitmap::legacySrlz");

  itsImpl->writeTo(lwriter);

  IO::ConstIoProxy<GrObj> baseclass(this);
  lwriter->writeBaseClass("GrObj", &baseclass);
}

void Bitmap::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("Bitmap::legacyDesrlz");

  itsImpl->readFrom(lreader);

  IO::IoProxy<GrObj> baseclass(this);
  lreader->readBaseClass("GrObj", &baseclass);
}

///////////////////////////////////////////////////////////////////////
//
// BitmapRep
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void BitmapRep::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("BitmapRep::legacySrlz");

//    lwriter->setStringMode(IO::GETLINE_TAB);
  lwriter->writeValue("itsFilename", itsImpl->itsFilename);

  lwriter->writeValue("rasterX", itsImpl->itsRasterX);
  lwriter->writeValue("rasterY", itsImpl->itsRasterY);
  lwriter->writeValue("zoomX", itsImpl->itsZoomX);
  lwriter->writeValue("zoomY", itsImpl->itsZoomY);
  lwriter->writeValue("usingZoom", itsImpl->itsUsingZoom);
  lwriter->writeValue("contrastFlip", itsImpl->itsContrastFlip);
  lwriter->setFieldSeparator('\n');
  lwriter->writeValue("verticalFlip", itsImpl->itsVerticalFlip);
}

void BitmapRep::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("BitmapRep::legacyDesrlz");

//    lreader->setStringMode(IO::GETLINE_TAB);
  lreader->readValue("itsFilename", itsImpl->itsFilename);

  lreader->readValue("rasterX", itsImpl->itsRasterX);
  lreader->readValue("rasterY", itsImpl->itsRasterY);
  lreader->readValue("zoomX", itsImpl->itsZoomX);
  lreader->readValue("zoomY", itsImpl->itsZoomY);
  lreader->readValue("usingZoom", itsImpl->itsUsingZoom);
  lreader->readValue("contrastFlip", itsImpl->itsContrastFlip);
  lreader->readValue("verticalFlip", itsImpl->itsVerticalFlip);

  if ( itsImpl->itsFilename.empty() ) {
	 clearBytes();
  }
  else {
	 queuePbmFile(itsImpl->itsFilename.c_str());
  }
}
#endif

///////////////////////////////////////////////////////////////////////
//
// Block
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void Block::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("Block::legacySrlz");

  IO::WriteUtils::writeValueSeq(
       lwriter, "trialSeq",
		 itsImpl->itsTrialSequence.begin(), itsImpl->itsTrialSequence.end());

  lwriter->insertChar('\n');

  lwriter->setFieldSeparator('\n');
  lwriter->writeValue("randSeed", itsImpl->itsRandSeed);
  lwriter->writeValue("curTrialSeqdx", itsImpl->itsCurTrialSeqIdx);
  lwriter->writeValue("verbose", itsImpl->itsVerbose);
}

void Block::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("Block::legacyDesrlz");

  itsImpl->itsTrialSequence.clear();
  IO::ReadUtils::template readValueSeq<int>(
		 lreader, "trialSeq", std::back_inserter(itsImpl->itsTrialSequence));

  lreader->readValue("randSeed", itsImpl->itsRandSeed);

  lreader->readValue("curTrialSeqdx", itsImpl->itsCurTrialSeqIdx);
  if (itsImpl->itsCurTrialSeqIdx < 0 ||
		size_t(itsImpl->itsCurTrialSeqIdx) > itsImpl->itsTrialSequence.size()) {
	 throw IO::ValueError(ioTag.c_str());
  }
  itsImpl->updateCurrentTrial();

  lreader->readValue("verbose", itsImpl->itsVerbose);

  // XXX I think this is not necessary
  //  	 lreader->input().ignore(1, '\n');
}
#endif

///////////////////////////////////////////////////////////////////////
//
// CloneFace
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
// In legacySrlz/legacyDesrlz, the derived class (CloneFace) is handled
// before the base class (Face) since the first thing the virtual
// constructor sees must be the typename of the most fully derived
// class, in order to inovke the proper constructor.
void CloneFace::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("CloneFace::legacySrlz");

  IO::WriteUtils::writeValueSeq(lwriter, "ctrlPnts",
										  itsCtrlPnts, itsCtrlPnts+24, true);

  lwriter->writeValue("eyeAspect", itsEyeAspect);
  lwriter->writeValue("vertOffset", itsVertOffset);

  // Always legacySrlz Face, regardless of lwriter->flags()
  IO::LWFlagJanitor jtr_(*lwriter, lwriter->flags() | IO::BASES);
  IO::ConstIoProxy<Face> baseclass(this);
  lwriter->writeBaseClass("Face", &baseclass);
}

void CloneFace::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("CloneFace::legacyDesrlz");

  IO::ReadUtils::template readValueSeq<double>(lreader, "ctrlPnts",
															  itsCtrlPnts, 24);

  lreader->readValue("eyeAspect", itsEyeAspect);
  lreader->readValue("vertOffset", itsVertOffset);

  // Always legacyDesrlz Face, regardless of lreader->flags()
  IO::LRFlagJanitor jtr_(*lreader, lreader->flags() | IO::BASES);
  IO::IoProxy<Face> baseclass(this);
  DebugEvalNL(lreader->flags());
  lreader->readBaseClass("Face", &baseclass);
}
#endif

///////////////////////////////////////////////////////////////////////
//
// EventResponseHdlr
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void EventResponseHdlr::Impl::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("EventResponseHdlr::Impl::legacySrlz");

  oldLegacySrlz(lwriter);

  lwriter->writeValue("eventSequence", itsEventSequence);
  lwriter->writeValue("bindingSubstitution", itsBindingSubstitution);
}

void EventResponseHdlr::Impl::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("EventResponseHdlr::Impl::legacyDesrlz");

  oldLegacyDesrlz(lreader);

  lreader->readValue("eventSequence", itsEventSequence);
  lreader->readValue("bindingSubstitution", itsBindingSubstitution);
}

void EventResponseHdlr::Impl::oldLegacySrlz(IO::Writer* writer) const {
DOTRACE("EventResponseHdlr::Impl::oldLegacySrlz");

  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
//  	 lwriter->setStringMode(IO::GETLINE_NEWLINE);

	 writer->writeValue("inputResponseMap", itsInputResponseMap);
	 writer->writeValue("feedbackMap", itsFeedbackMap);

	 lwriter->setFieldSeparator('\n');
	 writer->writeValue("useFeedback", itsUseFeedback);
  }
}

void EventResponseHdlr::Impl::oldLegacyDesrlz(IO::Reader* reader) {
DOTRACE("EventResponseHdlr::Impl::oldLegacyDesrlz");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
//  	 lreader->setStringMode(IO::GETLINE_NEWLINE);
	 reader->readValue("inputResponseMap", itsInputResponseMap);
	 reader->readValue("feedbackMap", itsFeedbackMap);
	 reader->readValue("useFeedback", itsUseFeedback);

	 itsRegexpsAreDirty = true;
	 itsFeedbacksAreDirty = true;

#ifdef MIPSPRO_COMPILER
	 // The next character after itsUseFeedback had better be a
	 // newline, and we need to remove it from the stream. ... OK, now
	 // I've commented out the code that does this, since I believe
	 // it's no longer necessary.

//    	 int cc = lreader->getChar();
//    	 if ( cc != '\n' ) {
//    		DebugEvalNL(cc);
//    		throw IO::LogicError(ioTag.c_str());
//    	 }
#endif
  }
}

void EventResponseHdlr::oldLegacySrlz(IO::Writer* writer) const
  { itsImpl->oldLegacySrlz(writer); }

void EventResponseHdlr::oldLegacyDesrlz(IO::Reader* reader)
  { itsImpl->oldLegacyDesrlz(reader); }
#endif

///////////////////////////////////////////////////////////////////////
//
// ExptDriver
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void ExptDriver::Impl::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("ExptDriver::Impl::legacySrlz");

  lwriter->writeOwnedObject("theObjList", &ObjList::theObjList());
  lwriter->writeOwnedObject("thePosList", &PosList::thePosList());
  lwriter->writeOwnedObject("theTlist", &Tlist::theTlist());
  lwriter->writeOwnedObject("theRhList", &RhList::theRhList());
  lwriter->writeOwnedObject("theThList", &ThList::theThList());
  lwriter->writeOwnedObject("theBlockList", &BlockList::theBlockList());

  lwriter->setStringMode(IO::GETLINE_NEWLINE);
  lwriter->writeValue("hostname", itsHostname);
  lwriter->writeValue("subject", itsSubject);
  lwriter->writeValue("beginDate", itsBeginDate);
  lwriter->writeValue("endDate", itsEndDate);
  lwriter->writeValue("autosaveFile", itsAutosaveFile);

  lwriter->writeValue("blockId", itsBlockId);
  lwriter->writeValue("rhId", itsDummyRhId);
  lwriter->setFieldSeparator('\n');
  lwriter->writeValue("thId", itsDummyThId);

  updateDoUponCompletionBody();

  lwriter->setStringMode(IO::CHAR_COUNT);
  lwriter->writeValue("doUponCompletionScript", itsDoUponCompletionBody);
}

void ExptDriver::Impl::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("ExptDriver::Impl::legacyDesrlz");

  lreader->readOwnedObject("theObjList", &ObjList::theObjList());
  lreader->readOwnedObject("thePosList", &PosList::thePosList());
  lreader->readOwnedObject("theTlist", &Tlist::theTlist());
  lreader->readOwnedObject("theRhList", &RhList::theRhList());
  lreader->readOwnedObject("theThList", &ThList::theThList());
  lreader->readOwnedObject("theBlockList", &BlockList::theBlockList());

  lreader->setStringMode(IO::GETLINE_NEWLINE);
  lreader->readValue("hostname", itsHostname);
  lreader->readValue("subject", itsSubject);
  lreader->readValue("beginDate", itsBeginDate);
  lreader->readValue("endDate", itsEndDate);
  lreader->readValue("autosaveFile", itsAutosaveFile);

  lreader->readValue("blockId", itsBlockId);
  lreader->readValue("rhId", itsDummyRhId);
  lreader->readValue("thId", itsDummyThId);

  lreader->setStringMode(IO::CHAR_COUNT);
  lreader->readValue("doUponCompletionScript", itsDoUponCompletionBody);

  recreateDoUponCompletionProc();
}
#endif

#ifdef LEGACY_IO
void ExptDriver::Impl::read(const char* filename) {
DOTRACE("ExptDriver::Impl::read");
  STD_IO::ifstream ifs(filename);
  if (ifs.fail()) throw IO::FilenameError(filename);
  IO::LegacyReader reader(ifs, IO::BASES|IO::TYPENAME);
  reader.readRoot(itsOwner);
}

void ExptDriver::Impl::write(const char* filename) const {
DOTRACE("ExptDriver::Impl::write");
  cerr << "warning: this file format is deprecated, "
		 << "and may not properly store all attributes\n";

  STD_IO::ofstream ofs(filename);
  if (ofs.fail()) throw IO::FilenameError(filename);
  IO::LegacyWriter writer(ofs, IO::BASES|IO::TYPENAME);
  writer.writeRoot(itsOwner);
}
#endif

#ifdef LEGACY_IO
void ExptDriver::read(const char* filename)
  { itsImpl->read(filename); }

void ExptDriver::write(const char* filename) const
  { itsImpl->write(filename); }
#endif

///////////////////////////////////////////////////////////////////////
//
// ExptTcl
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
//--------------------------------------------------------------------
//
// ReadCmd --
//
// Restore the current Expt to a previously saved state by reading a
// file. Returns an error if the filename provided is invalid, or if
// the file is corrupt or contains invalid data. The current Expt and
// all of its components (Tlist, ObjList, PosList) are restored to the
// state described in the file.
//
//--------------------------------------------------------------------

class ExptTcl::ReadCmd : public Tcl::TclItemCmd<ExptDriver> {
public:
  ReadCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<ExptDriver>(pkg, cmd_name, "filename", 2, 2) {}
protected:
  virtual void invoke() {
  DOTRACE("ExptTcl::ReadCmd::invoke");
	 ExptDriver* ed = getItem();
	 const char* filename = getCstringFromArg(1);

	 ed->read(filename);
	 returnVoid();
  }
};
#endif

#ifdef LEGACY_IO
//---------------------------------------------------------------------
//
// WriteCmd --
//
//---------------------------------------------------------------------

class ExptTcl::WriteCmd : public Tcl::TclItemCmd<ExptDriver> {
public:
  WriteCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<ExptDriver>(pkg, cmd_name, "filename", 2, 2) {}
protected:
  virtual void invoke() {
  DOTRACE("ExptTcl::WriteCmd::invoke");
	 ExptDriver* ed = getItem();
    const char* filename = getCstringFromArg(1);

	 ed->write(filename);
	 returnVoid();
  }
};
#endif

///////////////////////////////////////////////////////////////////////
//
// Face
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
// Writes the object's state to an output stream. The output stream
// must already be open and connected to an appropriate file.
void Face::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("Face::legacySrlz");
  Invariant(check());

  for (size_t i = 0; i < NUM_PINFOS; ++i) {
	 lwriter->writeValueObj(PINFOS[i].name_cstr(), get(PINFOS[i].property()));
  }

  lwriter->insertChar('\n');

  IO::ConstIoProxy<GrObj> baseclass(this);
  lwriter->writeBaseClass("GrObj", &baseclass);
}

void Face::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("Face::legacyDesrlz");

//    int version = lreader->getLegacyVersionId();

//    if (version == 1) lreader->grabLeftBrace();

  for (size_t i = 0; i < NUM_PINFOS; ++i) {
	 lreader->readValueObj(PINFOS[i].name_cstr(),
								  const_cast<Value&>(get(PINFOS[i].property())));
  }

//    if (version == 1) lreader->grabRightBrace();

  Invariant(check());

  IO::IoProxy<GrObj> baseclass(this);
  lreader->readBaseClass("GrObj", &baseclass);

  sendStateChangeMsg();
}
#endif

///////////////////////////////////////////////////////////////////////
//
// Fish
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void Fish::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("Fish::legacySrlz");

  for (unsigned int i = 0; i < NUM_LEGACY_PINFOS; ++i) {
	 lwriter->writeValueObj(PINFOS[i].name_cstr(), get(PINFOS[i].property()));
  }

  IO::ConstIoProxy<GrObj> baseclass(this);
  lwriter->writeBaseClass("GrObj", &baseclass);
}

void Fish::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("Fish::legacyDesrlz");

  for (unsigned int i = 0; i < NUM_LEGACY_PINFOS; ++i) {
	 lreader->readValueObj(PINFOS[i].name_cstr(),
								  const_cast<Value&>(get(PINFOS[i].property())));
  }

  IO::IoProxy<GrObj> baseclass(this);
  lreader->readBaseClass("GrObj", &baseclass);

  sendStateChangeMsg();
}
#endif

///////////////////////////////////////////////////////////////////////
//
// FixPt
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void FixPt::legacySrlz(IO::LegacyWriter* lwriter) const {

  lwriter->writeDouble("length", length());
  lwriter->setFieldSeparator('\n');
  lwriter->writeInt("width", width());

  IO::ConstIoProxy<GrObj> baseclass(this);
  lwriter->writeBaseClass("GrObj", &baseclass);
}

void FixPt::legacyDesrlz(IO::LegacyReader* lreader) {

  lreader->readValue("length", length());
  lreader->readValue("width", width());

  IO::IoProxy<GrObj> baseclass(this);
  lreader->readBaseClass("GrObj", &baseclass);
}
#endif

///////////////////////////////////////////////////////////////////////
//
// Gabor
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void Gabor::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("Gabor::legacySrlz");
  IO::ConstIoProxy<GrObj> baseclass(this);
  lwriter->writeBaseClass("GrObj", &baseclass);
}

void Gabor::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("Gabor::legacyDesrlz");
  IO::IoProxy<GrObj> baseclass(this);
  lreader->readBaseClass("GrObj", &baseclass);

  sendStateChangeMsg();
}
#endif

///////////////////////////////////////////////////////////////////////
//
// GLBitmap
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void GLBitmap::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("GLBitmap::legacySrlz");

  lwriter->writeValue("usingGlBitmap", itsRenderer->getUsingGlBitmap());

  IO::ConstIoProxy<Bitmap> baseclass(this);
  lwriter->writeBaseClass("Bitmap", &baseclass);
}

void GLBitmap::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("GLBitmap::legacyDesrlz");

  bool val;
  lreader->readValue("usingGlBitmap", val); 
  itsRenderer->setUsingGlBitmap(val);

  IO::IoProxy<Bitmap> baseclass(this);
  lreader->readBaseClass("Bitmap", &baseclass);
}
#endif

///////////////////////////////////////////////////////////////////////
//
// GrObj::Impl
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void GrObj::Impl::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("GrObj::Impl::legacySrlz");
  lwriter->writeValue("GrObj::category", itsCategory);

  lwriter->writeValue("GrObj::renderMode", itsRenderer.getMode());

  lwriter->writeValue("GrObj::unRenderMode", itsUnRenderer.itsMode);

  lwriter->writeValue("GrObj::bbVisibility", itsBB.itsIsVisible);

  lwriter->writeValue("GrObj::scalingMode", itsScaler.getMode());
  lwriter->writeValue("GrObj::widthFactor", itsScaler.itsWidthFactor);
  lwriter->writeValue("GrObj::heightFactor", itsScaler.itsHeightFactor);

  lwriter->writeValue("GrObj::alignmentMode", itsAligner.itsMode);
  lwriter->writeValue("GrObj::centerX", itsAligner.itsCenterX);
  lwriter->setFieldSeparator('\n');
  lwriter->writeValue("GrObj::centerY", itsAligner.itsCenterY);
}

void GrObj::Impl::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("GrObj::Impl::legacyDesrlz");

  lreader->readValue("GrObj::category", itsCategory);

  int temp;
  lreader->readValue("GrObj::renderMode", temp);
  itsRenderer.setMode(temp, this);

  lreader->readValue("GrObj::unRenderMode", itsUnRenderer.itsMode);

  lreader->readValue("GrObj::bbVisibility", itsBB.itsIsVisible);

  lreader->readValue("GrObj::scalingMode", temp);
  itsScaler.setMode(temp, this);

  lreader->readValue("GrObj::widthFactor", itsScaler.itsWidthFactor);
  lreader->readValue("GrObj::heightFactor", itsScaler.itsHeightFactor);

  lreader->readValue("GrObj::alignmentMode", itsAligner.itsMode);
  lreader->readValue("GrObj::centerX", itsAligner.itsCenterX);
  lreader->readValue("GrObj::centerY", itsAligner.itsCenterY);

  invalidateCaches();
}
#endif

///////////////////////////////////////////////////////////////////////
//
// Gtext
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void Gtext::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("Gtext::legacySrlz");

//    lwriter->setStringMode(IO::GETLINE_NEWLINE);
  lwriter->writeValue("text", itsText);

  IO::ConstIoProxy<GrObj> baseclass(this);
  lwriter->writeBaseClass("GrObj", &baseclass);
}

void Gtext::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("Gtext::legacyDesrlz");

//    lreader->setStringMode(IO::GETLINE_NEWLINE);
  lreader->readValue("text", itsText);

  IO::IoProxy<GrObj> baseclass(this);
  lreader->readBaseClass("GrObj", &baseclass);
}
#endif

///////////////////////////////////////////////////////////////////////
//
// IoPtrList
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void IoPtrList::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("IoPtrList::legacySrlz");

  fixed_string ioTag = IO::IoObject::ioTypename();

  // itsVec: we will legacySrlz only the non-null T*'s in
  // itsVec. In order to correctly legacyDesrlz the object later, we
  // must write both the size of itsVec (in order to correctly
  // resize later), as well as the number of non-null objects that we
  // legacySrlz (so that legacyDesrlz knows when to stop reading).

  lwriter->writeValue("voidVecSize", baseVecSize());

  int num_non_null = PtrListBase::count();
  lwriter->setFieldSeparator('\n');
  lwriter->writeValue("num_non_null", num_non_null);
  lwriter->resetFieldSeparator();

  // Serialize all non-null ptr's.
  int c = 0;
  for (size_t i = 0, end = baseVecSize();
		 i < end;
		 ++i) {
	 if (getPtrBase(i)->isValid()) {
		lwriter->writeValue("i", i);

		// we must legacySrlz the typename since legacyDesrlz requires a
		// typename in order to call the virtual constructor
		MasterIoPtr* ioPtr = dynamic_cast<MasterIoPtr*>(getPtrBase(i));
		IO::IoObject* obj = ioPtr ? ioPtr->ioPtr() : 0;

		IO::LWFlagJanitor jtr_(*lwriter, lwriter->flags() | IO::TYPENAME);
		lwriter->writeObject("ptrListItem", obj);

		++c;
	 }
  }

  if (c != num_non_null) {
	 throw IO::LogicError(ioTag.c_str());
  }

  lwriter->setFieldSeparator('\n');
  lwriter->writeInt("itsFirstVacant", firstVacant());
}

void IoPtrList::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("IoPtrList::legacyDesrlz");

  fixed_string ioTag = IO::IoObject::ioTypename();

  int size, num_non_null;
  lreader->readValue("voidVecSize", size);
  lreader->readValue("num_non_null", num_non_null);

  if ( size < 0 || num_non_null < 0 || num_non_null > size ) {
	 throw IO::ValueError(ioTag.c_str());
  }

  PtrListBase::clear();
  baseVecResize(size);

  int ptrid;

  for (int i = 0; i < num_non_null; ++i) {
	 lreader->readValue("i", ptrid);

	 if (ptrid < 0 || ptrid >= size) {
		throw IO::ValueError(ioTag.c_str());
	 }

	 IO::IoObject* obj = lreader->readObject("ptrListItem");

	 insertPtrBaseAt(ptrid, makeMasterIoPtr(obj));
  }

  firstVacant() = lreader->readInt("itsFirstVacant");

  // The next character after itsFirstVacant had better be a newline,
  // and we need to remove it from the stream.
  lreader->grabNewline();
}
#endif

///////////////////////////////////////////////////////////////////////
//
// House
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void House::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("House::legacySrlz");

  for (unsigned int i = 0; i < NUM_PINFOS; ++i) {
	 lwriter->writeValueObj(PINFOS[i].name_cstr(),
									get(PINFOS[i].property()));
  }

  IO::ConstIoProxy<GrObj> baseclass(this);
  lwriter->writeBaseClass("GrObj", &baseclass);
}

void House::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("House::legacyDesrlz");

  for (unsigned int i = 0; i < NUM_PINFOS; ++i) {
	 lreader->readValueObj(PINFOS[i].name_cstr(),
								  const_cast<Value&>(get(PINFOS[i].property())));
  }

  IO::IoProxy<GrObj> baseclass(this);
  lreader->readBaseClass("GrObj", &baseclass);

  sendStateChangeMsg();
}
#endif

///////////////////////////////////////////////////////////////////////
//
// HpAudioSound
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void HpAudioSound::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("HpAudioSound::legacySrlz");
  lwriter->setStringMode(IO::GETLINE_NEWLINE);
  lwriter->writeValue("filename", itsFilename);
}

void HpAudioSound::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("HpAudioSound::legacyDesrlz");
  lreader->setStringMode(IO::GETLINE_NEWLINE);
  lreader->readValue("filename", itsFilename);

  setFile(itsFilename.c_str());
}
#endif

///////////////////////////////////////////////////////////////////////
//
// IrixAudioSound
//
///////////////////////////////////////////////////////////////////////

void IrixAudioSound::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("IrixAudioSound::legacySrlz");
  lwriter->setStringMode(IO::GETLINE_NEWLINE);
  lwriter->writeValue("filename", itsFilename);
}

void IrixAudioSound::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("IrixAudioSound::legacyDesrlz");
  lreader->setStringMode(IO::GETLINE_NEWLINE);
  lreader->readValue("filename", itsFilename);

  setFile(itsFilename.c_str());
}

///////////////////////////////////////////////////////////////////////
//
// Jitter
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
// In legacySrlz/legacyDesrlz, the derived class (Jitter) is handled
// before the base class (Position), since the first thing that the
// PosMgr virtual constructor sees must be the name of the most fully
// derived class, in order to invoke the proper constructor.
void Jitter::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("Jitter::legacySrlz");

  lwriter->writeValue("jitterX", itsXJitter);
  lwriter->writeValue("jitterY", itsYJitter);
  lwriter->setFieldSeparator('\n');
  lwriter->writeValue("jitterR", itsRJitter);

  // The base class (Position) is always legacySrlzd, regardless of flag.
  IO::LWFlagJanitor(*lwriter, lwriter->flags() | IO::BASES);
  IO::ConstIoProxy<Position> baseclass(this);
  lwriter->writeBaseClass("Position", &baseclass);
}

void Jitter::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("Jitter::legacyDesrlz");

  lreader->readValue("jitterX", itsXJitter);
  lreader->readValue("jitterY", itsYJitter);
  lreader->readValue("jitterR", itsRJitter);

  // The base class (Position) is always legacyDesrlzd, regardless of flag.
  IO::LRFlagJanitor(*lreader, lreader->flags() | IO::BASES);
  IO::IoProxy<Position> baseclass(this);
  lreader->readBaseClass("Position", &baseclass);
}
#endif

///////////////////////////////////////////////////////////////////////
//
// KbdResponseHdlr
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void KbdResponseHdlr::writeTo(IO::Writer* writer) const {
DOTRACE("KbdResponseHdlr::writeTo");
  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {

	 oldLegacySrlz(writer);
  }
  else {
	 EventResponseHdlr::writeTo(writer);
  }
}

void KbdResponseHdlr::readFrom(IO::Reader* reader) {
DOTRACE("KbdResponseHdlr::readFrom");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {

	 oldLegacyDesrlz(reader);

	 setEventSequence("<KeyPress");
	 setBindingSubstitution("%K");
  }
  else {
	 EventResponseHdlr::readFrom(reader);
  }
}
#endif

///////////////////////////////////////////////////////////////////////
//
// MaskHatch
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void MaskHatch::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("MaskHatch::legacySrlz");

  for (size_t i = 0; i < numPropertyInfos(); ++i) {
	 lwriter->writeValueObj(PINFOS[i].name_cstr(), get(PINFOS[i].property()));
  }

  IO::ConstIoProxy<GrObj> baseclass(this);
  lwriter->writeBaseClass("GrObj", &baseclass);
}

void MaskHatch::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("MaskHatch::legacyDesrlz");

  for (size_t i = 0; i < numPropertyInfos(); ++i) {
	 lreader->readValueObj(PINFOS[i].name_cstr(),
								  const_cast<Value&>(get(PINFOS[i].property())));
  }

  IO::IoProxy<GrObj> baseclass(this);
  lreader->readBaseClass("GrObj", &baseclass);

  sendStateChangeMsg();
}
#endif

///////////////////////////////////////////////////////////////////////
//
// MorphyFace
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
// Writes the object's state to an output stream. The output stream
// must already be open and connected to an appropriate file.
void MorphyFace::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("MorphyFace::legacySrlz");
  Invariant(check());

  for (unsigned int i = 0; i < NUM_PINFOS; ++i) {
	 lwriter->writeValueObj(PINFOS[i].name_cstr(), get(PINFOS[i].property()));
  }

  lwriter->insertChar('\n');

  IO::ConstIoProxy<GrObj> baseclass(this);
  lwriter->writeBaseClass("GrObj", &baseclass);
}

void MorphyFace::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("MorphyFace::legacyDesrlz");

//    int version = lreader->getLegacyVersionId();

//    if (version == 1) lreader->grabLeftBrace();

  for (unsigned int i = 0; i < NUM_PINFOS; ++i) {
	 lreader->readValueObj(PINFOS[i].name_cstr(),
								  const_cast<Value&>(get(PINFOS[i].property())));
  }

//    if (version == 1) lreader->grabRightBrace();

  Invariant(check());

  IO::IoProxy<GrObj> baseclass(this);
  lreader->readBaseClass("GrObj", &baseclass);

  sendStateChangeMsg();
}
#endif

///////////////////////////////////////////////////////////////////////
//
// Position
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void Position::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("Position::legacySrlz");

  Invariant(check());

  lwriter->writeValue("transX", itsImpl->tr_x);
  lwriter->writeValue("transY", itsImpl->tr_y);
  lwriter->writeValue("transZ", itsImpl->tr_z);

  lwriter->writeValue("scaleX", itsImpl->sc_x);
  lwriter->writeValue("scaleY", itsImpl->sc_y);
  lwriter->writeValue("scaleZ", itsImpl->sc_z);

  lwriter->writeValue("rotateX", itsImpl->rt_x);
  lwriter->writeValue("rotateY", itsImpl->rt_y);
  lwriter->writeValue("rotateZ", itsImpl->rt_z);
  lwriter->setFieldSeparator('\n');
  lwriter->writeValue("rotateAngle", itsImpl->rt_ang);
}

void Position::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("Position::legacyDesrlz");

  lreader->readValue("transX", itsImpl->tr_x);
  lreader->readValue("transY", itsImpl->tr_y);
  lreader->readValue("transZ", itsImpl->tr_z);

  lreader->readValue("scaleX", itsImpl->sc_x);
  lreader->readValue("scaleY", itsImpl->sc_y);
  lreader->readValue("scaleZ", itsImpl->sc_z);

  lreader->readValue("rotateX", itsImpl->rt_x);
  lreader->readValue("rotateY", itsImpl->rt_y);
  lreader->readValue("rotateZ", itsImpl->rt_z);
  lreader->readValue("rotateAngle", itsImpl->rt_ang);

  Invariant(check());
}
#endif

///////////////////////////////////////////////////////////////////////
//
// Subject
//
///////////////////////////////////////////////////////////////////////

void Subject::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("Subject::legacySrlz");

  lwriter->setStringMode(IO::GETLINE_NEWLINE);

  lwriter->writeValue("name", itsName);
  lwriter->writeValue("directory", itsDirectory);
}

void Subject::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("Subject::legacyDesrlz");

  lreader->setStringMode(IO::GETLINE_NEWLINE);

  lreader->readValue("name", itsName);
  lreader->readValue("directory", itsDirectory);
}

///////////////////////////////////////////////////////////////////////
//
// TimingHandler
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void TimingHandler::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("TimingHandler::legacySrlz");

  IO::LWFlagJanitor jtr_(*lwriter, lwriter->flags() | IO::BASES);
  IO::ConstIoProxy<TimingHdlr> baseclass(this);
  lwriter->writeBaseClass("TimingHdlr", &baseclass);
}

void TimingHandler::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("TimingHandler::legacyDesrlz");

  IO::LRFlagJanitor(*lreader, lreader->flags() | IO::BASES);
  IO::IoProxy<TimingHdlr> baseclass(this);
  lreader->readBaseClass("TimingHdlr", &baseclass);
}
#endif

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void TimingHdlr::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("TimingHdlr::legacySrlz");

  lwriter->writeValue("autosavePeriod", itsImpl->itsDummyAutosavePeriod);

  IO::WriteUtils::writeObjectSeq(lwriter, "immediateEvents",
     itsImpl->itsImmediateEvents.begin(), itsImpl->itsImmediateEvents.end());

  IO::WriteUtils::writeObjectSeq(lwriter, "startEvents",
     itsImpl->itsStartEvents.begin(), itsImpl->itsStartEvents.end());

  IO::WriteUtils::writeObjectSeq(lwriter, "responseEvents",
     itsImpl->itsResponseEvents.begin(), itsImpl->itsResponseEvents.end());

  IO::WriteUtils::writeObjectSeq(lwriter, "abortEvents",
     itsImpl->itsAbortEvents.begin(), itsImpl->itsAbortEvents.end());
}

void TimingHdlr::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("TimingHdlr::legacyDesrlz");

  lreader->readValue("autosavePeriod", itsImpl->itsDummyAutosavePeriod);

  itsImpl->deleteAll(itsImpl->itsImmediateEvents);
  IO::ReadUtils::template readObjectSeq<TrialEvent>(lreader, "immediateEvents",
                        std::back_inserter(itsImpl->itsImmediateEvents));

  itsImpl->deleteAll(itsImpl->itsStartEvents);
  IO::ReadUtils::template readObjectSeq<TrialEvent>(lreader, "startEvents",
                        std::back_inserter(itsImpl->itsStartEvents));

  itsImpl->deleteAll(itsImpl->itsResponseEvents);
  IO::ReadUtils::template readObjectSeq<TrialEvent>(lreader, "responseEvents",
                        std::back_inserter(itsImpl->itsResponseEvents));

  itsImpl->deleteAll(itsImpl->itsAbortEvents);
  IO::ReadUtils::template readObjectSeq<TrialEvent>(lreader, "abortEvents",
                        std::back_inserter(itsImpl->itsAbortEvents));
}
#endif

///////////////////////////////////////////////////////////////////////
//
// Tlist
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void Tlist::writeTo(IO::Writer* writer) const {
DOTRACE("Tlist::writeTo");
  // Always legacySrlz the PtrList base
  PtrList<TrialBase>::writeTo(writer);

  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {

	 lwriter->resetFieldSeparator();

	 // Here we are spoofing the obselete data members itsCurTrial and
	 // itsVisibility.
	 writer->writeValue("dummy1", int(0));
	 writer->writeValue("dummy2", bool(false));
  }
}

void Tlist::readFrom(IO::Reader* reader) {
DOTRACE("Tlist::readFrom");
  // Always legacyDesrlz its PtrList<TrialBase> base
  PtrList<TrialBase>::readFrom(reader);

  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {

	 // Here we are spoofing the obselete data members itsCurTrial and
	 // itsVisibility.
	 int dummy1;
	 bool dummy2;
	 reader->readValue("dummy1", dummy1);
	 reader->readValue("dummy2", dummy2);
  }
}
#endif

///////////////////////////////////////////////////////////////////////
//
// Trial
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void Trial::Impl::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("Trial::Impl::legacySrlz");

  IO::WriteUtils::writeValueObjSeq(lwriter, "idPairs",
										 itsIdPairs.begin(), itsIdPairs.end());

  IO::WriteUtils::writeValueObjSeq(lwriter, "responses",
										 itsResponses.begin(), itsResponses.end());

  lwriter->writeValue("type", itsType);

  lwriter->writeValue("rhId", itsRhId);
  lwriter->setFieldSeparator('\n');
  lwriter->writeValue("thId", itsThId);
}

void Trial::Impl::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("Trial::Impl::legacyDesrlz");
  itsIdPairs.clear();
  IO::ReadUtils::template readValueObjSeq<IdPair>(lreader, "idPairs",
									  std::back_inserter(itsIdPairs));

  itsResponses.clear();
  IO::ReadUtils::template readValueObjSeq<Response>(lreader, "responses",
									  std::back_inserter(itsResponses));

  lreader->readValue("type", itsType);

  lreader->readValue("rhId", itsRhId);
  lreader->readValue("thId", itsThId);
}
#endif

///////////////////////////////////////////////////////////////////////
//
// TrialEvent
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void TrialEvent::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("TrialEvent::legacySrlz");
  lwriter->setFieldSeparator('\n');
  lwriter->writeValue("requestedDelay", itsRequestedDelay);
}

void TrialEvent::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("TrialEvent::legacyDesrlz");

  cancel(); // cancel since the event is changing state

  lreader->readValue("requestedDelay", itsRequestedDelay);
}
#endif

///////////////////////////////////////////////////////////////////////
//
// XBitmap
//
///////////////////////////////////////////////////////////////////////

#ifdef LEGACY_IO
void XBitmap::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("XBitmap::legacySrlz");
  IO::ConstIoProxy<Bitmap> baseclass(this);
  lwriter->writeBaseClass("Bitmap", &baseclass);
}

void XBitmap::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("XBitmap::legacyDesrlz");
  IO::IoProxy<Bitmap> baseclass(this);
  lreader->readBaseClass("Bitmap", &baseclass);
}
#endif
