///////////////////////////////////////////////////////////////////////
// trial.h
// Rob Peters
// created: Mar-99
// written: Fri Mar 12 11:07:18 1999
static const char vcid[] = "$Id$";
///////////////////////////////////////////////////////////////////////

class Tlist {
private:
  vector<Position *> itsPositions;
  vector<GrObj *> itsObjs;
  vector<Trial *> itsTrials;
  int itsCurGroup;
  bool itsVisibility;
public:
  void showTrial(int trial) const;
  void drawCurTrial() const;
  void setVisibile(bool vis);
  void setCurGroup(int group);
  void addToTrial(int trial, int objid, int posid);
}


class Expt {
  vec<Trial *> itsTrials; // each is unique
  vec<int> itsTrialSequence; // randomly ordered list of indexes into itsTrials
										    // (may contain repeats)
  int itsRandSeed;
  int itsCurstim;        // index into itsTrialList
  
public:
  void beginNextTrial();
  void recordResponse(int resp);
  int lastResponse() { 
	 return itsTrials[itsTrialList[itsCurstim-1]]->lastResponse();
  }
}

class Trial {
  typedef vec<ScaledObj *> ObjGrp;
  ObjGrp itsObjs;
  list<int> itsResponses;
  int itsType;
  
  class ScaledObj : public GrObj {
	 GrObj *itsObj;
	 Scaleable *itsScale;
  public:
	 ScaledObj(int objid, int posid) : itsObj(obj), itsScaleable(scl) {}
	 ~ScaledObj() {} // someone else owns the GrObj and Scaleable
	 void render() {
		glPushMatrix(GL_MODELVIEW);
		itsScale->translate();
		itsScale->scale();
		itsScale->rotate();
		itsObj->render();
		glPopMatrix();
	 }
  };
  
public:
  Trial () : itsObjs(), itsResponses() {
  }
  
  ~Trial () {
	 for (ObjGrp::iterator itr = itsObjs.begin(); itr != itsObjs.end(); itr++) {
		delete *itr;
		*itr = NULL;
	 }
  }
  
  void add(GrObj *obj, Scaleable *scl) { 
	 itsObjs.push_back(new ScaledObj(obj, scl));
  }
  void doTrial() {
	 for (ObjGrp::iterator itr = itsObjs.begin(); itr != itsObjs.end(); itr++) {
		(*itr)->action();
	 }
  }

  virtual int trialType() { return -1; } 
  // returns some info about relationship between objects in trial

  void recordResponse(int resp) { itsResponses.push_back(resp); }
  int lastResponse() { return itsResponses.back(); }
};

class SingleTrial : public Trial {
};

class PairTrial : public Trial {
};

class TripleTrial : public Trial {
};
