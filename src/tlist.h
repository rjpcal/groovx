///////////////////////////////////////////////////////////////////////
// tlist.h
// Rob Peters
// created: Fri Mar 12 13:23:02 1999
// written: Sat Mar 13 12:30:46 1999
///////////////////////////////////////////////////////////////////////

#ifndef TLIST_H_DEFINED
#define TLIST_H_DEFINED

#include <vector>

class Trial;
class ObjList;
class PosList;

class Tlist {
private:
  const ObjList& itsObjList;
  const PosList& itsPosList;
  vector<Trial *> itsTrials;
  int itsCurTrial;
  bool itsVisibility;
public:
  Tlist (ObjList &olist, PosList &plist) : 
	 itsObjList(olist), itsPosList(plist) {}
  ~Tlist() {}

  // trials
  bool isValidTrial(int trial) const;
  void drawTrial(int trial);
  void drawCurTrial() const;
  void setVisibile(bool vis);
  void setCurTrial(int trial);
  void addToTrial(int trial, int objid, int posid);
};

class Expt {
  Tlist& itsTlist;
  vector<int> itsTrialSequence; // ordered list of indexes into itsTlist
										    // (may contain repeats)
  int itsRandSeed;
  int itsCurTrial;        // index into itsTrialList
  
public:
  Expt(Tlist& tlist) : itsTlist(tlist) {}

  void beginNextTrial();
  void recordResponse(int resp);
  int lastResponse();
  //	 return itsTrials[itsTrialList[itsCurstim-1]]->lastResponse();

};

static const char vcid_tlist_h[] = "$Id$";
#endif // !TLIST_H_DEFINED
