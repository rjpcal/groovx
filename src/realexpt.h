///////////////////////////////////////////////////////////////////////
// realexpt.h
// Rob Peters
// created: Jan-99
// written: Sat Mar 13 19:54:30 1999
///////////////////////////////////////////////////////////////////////

#ifndef REALEXPT_H_DEFINED
#define REALEXPT_H_DEFINED

#include <vector>

#ifndef EXPT_H_INCLUDED
#include "expt.h"
#endif

class Glist;
class ObjList;

class RealExpt : public Expt {
public:
  //////////////
  // creators //
  //////////////

  RealExpt(Glist **glist_h, ObjList **olist_h,
			  int repeat, int grp_size, 
			  const char* obj_filename, 
			  const char* grp_filename, int shift,
			  double t_jitter, double r_jitter);
  // construct a RealExpt from scratch from an obj file and a group
  // file

  RealExpt(Glist **glist_h, ObjList **olist_h,
			  int repeat, int grp_size, 
			  const char* obj_filename, 
			  double t_jitter, double r_jitter);
  // construct a RealExpt from scratch from an obj file; construct
  // groups based on grp_size (i.e. singles, pairs, triples)

  RealExpt(istream &is, Glist **glist_h, ObjList **olist_h);
  // construct from a previously serialized RealExpt

  virtual ~RealExpt();

  virtual IOResult serialize(ostream &os, IOFlag flag = NO_FLAGS) const;

  ///////////////
  // accessors //
  ///////////////

  virtual int numTrials() const;
  virtual int numCompleted() const;
  virtual int currentTrial() const;
  virtual int currentStimClass() const;
  virtual int prevResponse() const;
  virtual bool isComplete() const;
  virtual const char* trialDescription() const;

  /////////////
  // actions //
  /////////////

  virtual void beginTrial();
  virtual void recordResponse(int resp);

private:
  Glist **itsGlist_h;
  ObjList **itsObjList_h;

  int itsNumRepeats;
  int itsNumTypes;
  int itsNumTrials;
  int itsGrpSize;
  int itsNumObjs;
  int itsCurTrial;
  int itsPrevTrial;
  double itsTjitter;
  double itsRjitter;
  vector<int> itsResponses;
  vector<int> itsStimClasses;
  vector<int> itsRemaining;
};

static const char vcid_realexpt_h[] = "$Id$";
#endif // !REALEXPT_H_DEFINED
