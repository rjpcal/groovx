///////////////////////////////////////////////////////////////////////
// trial.h
// Rob Peters
// created: Mar-99
// written: Fri Mar 12 17:55:06 1999
///////////////////////////////////////////////////////////////////////

#ifndef TRIAL_H_DEFINED
#define TRIAL_H_DEFINED

#include <vector>

class ObjList;
class PosList;

class Trial {
public:
  Trial () : itsObjs(), itsResponses() {}
  ~Trial () {}

  typedef pair<int, int> ObjPos;
  typedef vector<ObjPos> ObjGrp;

  const ObjGrp& objs() { return itsObjs; }

  void add(int objid, int posid) { itsObjs.push_back(ObjPos(objid, posid)); }

  virtual int trialType() { return -1; } 
  // returns some info about relationship between objects in trial

  void action() const;

  void recordResponse(int resp) { itsResponses.push_back(resp); }
  int lastResponse() { return itsResponses.back(); }

  static void setObjlist(const ObjList *olist) { theirObjList = olist; }
  static void setPoslist(const PosList *plist) { theirPosList = plist; }

private:  
  ObjGrp itsObjs;
  vector<int> itsResponses;
  int itsType;

  static const ObjList* theirObjList;
  static const PosList* theirPosList;
};

class SingleTrial : public Trial {
};

class PairTrial : public Trial {
};

class TripleTrial : public Trial {
};

static const char vcid_trial_h[] = "$Id$";
#endif // !TRIAL_H_DEFINED
