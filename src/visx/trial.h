// trial.h

// class GrObj : public TrialEvent {};

class Trial {
  typedef vec<ScaledObj *> ObjGrp;
  ObjGrp itsObjs;
  list<int> itsResponses;
  int itsType;

  class ScaledObj : public GrObj {
	 GrObj *itsObj;
	 Scaleable *itsScale;
  public:
	 ScaledObj(GrObj *obj, Scaleable *scl) : itsObj(obj), itsScaleable(scl) {}
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
  void recordResponse(int resp) { itsResponses.push_back(resp); }
  int lastResponse() { return itsResponses.back(); }
};

class SingleTrial : public Trial {
};

class PairTrial : public Trial {
};

class TripleTrial : public Trial {
};
