///////////////////////////////////////////////////////////////////////
// realexpt.cc
// Rob Peters
// created: Thu Mar  4 13:07:45 1999
// written: Fri Mar 12 12:55:05 1999
static const char vcid_realexpt_cc[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef REALEXPT_CC_DEFINED
#define REALEXPT_CC_DEFINED

#include "realexpt.h"

#include <fstream.h>
#include <strstream.h>

#include "face.h"
#include "objlist.h"
#include "glist.h"
#include "randutils.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// File scope declarations
///////////////////////////////////////////////////////////////////////

namespace {
  const int BUF_SIZE = 200;
  const double OBJ_SPACING = 2.0;

  ////////////////////////////
  // Local helper functions //
  ////////////////////////////

  void flushLine(istream &is) {
    DOTRACE("flushLine");
    char throw_away[BUF_SIZE];
    is.getline(throw_away, BUF_SIZE);
  }

  IOResult deserializeVector(istream &is, vector<int>& vec) {
    DOTRACE("deserializeVector");
    int size;
    is >> size;
    vec.resize(size, 0);
    for (int i = 0; i < size; i++) {
      is >> vec[i];
    }
    flushLine(is);
    return checkStream(is);
  }

  IOResult serializeVector(ostream &os, const vector<int>& vec) {
    DOTRACE("serializeVector");
    char sep = ' ';
    os << vec.size() << endl;
    for (int i = 0; i < vec.size(); i++) {
      os << vec[i] << sep;
    }
    os << endl;
    return checkStream(os);
  }

  int countFileLines(const char* filename) {
    char line[BUF_SIZE];
    ifstream ifs(filename);
    int num_lines = 0;
    while (ifs.getline(line, BUF_SIZE)) { num_lines++; }
    return num_lines;
  }

  void setClassFirstCategory(vector<int> &vec, const Glist& glist, 
									  const ObjList& olist,
									  int num_types, int repeat, int num_objs) {
    for (int i = 0; i < num_types; i++) {
      list<int> group = glist.getGroup(i);
		int id = group.front();
		int val = -1;
		Face *p = dynamic_cast<Face *>(olist[id]);
		if (p) { val = p->getCategory(); }
		for (int r = 0; r < repeat; r++) {
		  vec[i*repeat+r] = val;
		}
    }
  }

  void setClassPairSame(vector<int> &vec, const Glist& glist, 
								int num_types, int repeat, int num_objs) {
    for (int i = 0; i < num_types; i++) {
      list<int> group = glist.getGroup(i);
      list<int>::iterator itr = group.begin();
      int first = *itr;
      int second = *(++itr) - num_objs;
		int val = (first == second); // 1 if same, 0 otherwise
		for (int r = 0; r < repeat; r++) {
		  vec[i*repeat+r] = val;
		}
    }
  }

  void SetClassTriadFindside(vector<int> &vec, const Glist& glist, 
									  int num_types, int repeat, int num_objs) {
    for (int i = 0; i < num_types; i++) {
      list<int> group = glist.getGroup(i);
      list<int>::iterator itr = group.begin();
      int first = *itr;
      int second = *(++itr) - num_objs;
      int third = *(++itr) - 2*num_objs;
		int val;
      if (first == second)      val = 0;
      else if (second == third) val = 1;
      else                      val = 2;
		for (int r = 0; r < repeat; r++) {
		  vec[i*repeat+r] = val;
		}
    }
  }
}

///////////////////////////////////////////////////////////////////////
// RealExpt member functions
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

RealExpt::RealExpt(Glist **glist_h, ObjList **olist_h,
                   int repeat, int grp_size, 
                   const char* obj_filename, 
                   const char* grp_filename, int shift,
                   double t_jitter, double r_jitter) :
  itsGlist_h(glist_h),
  itsObjList_h(olist_h),
  itsNumRepeats(repeat),
  itsGrpSize(grp_size),
  itsCurTrial(0),
  itsPrevTrial(0),
  itsTjitter(t_jitter),
  itsRjitter(r_jitter)
{
DOTRACE("RealExpt::RealExpt");
  char line[BUF_SIZE];

  // peek into obj_file to count number of objs 
  itsNumObjs = countFileLines(obj_filename);

  // construct the ObjList
  delete *itsObjList_h;
  ifstream obj_file(obj_filename);
  *itsObjList_h = new ObjList(obj_file, "Face", itsNumObjs, 
                              itsGrpSize, OBJ_SPACING,
                              itsTjitter, itsRjitter);
  obj_file.close();

  // peek into grp_file to count number of lines == number of groups
  int num_groups = countFileLines(grp_filename);

  // now construct the Glist
  delete *itsGlist_h;
  ifstream grp_file(grp_filename);
  *itsGlist_h = new Glist(num_groups, shift, itsNumObjs, 
                          grp_file, **itsObjList_h);
  grp_file.close();

  itsNumTypes = (*itsGlist_h)->numGroups();
  itsNumTrials = itsNumTypes * itsNumRepeats;
  itsResponses.resize(itsNumTrials, -1);

  itsRemaining.resize(itsNumTrials, 0);
  for (int i = 0; i < itsNumTrials; i++) {
    itsRemaining[i] = i;
  }

  itsStimClasses.resize(itsNumTrials, 0);
  if (itsGrpSize == 1) {
	 setClassFirstCategory(itsStimClasses, **itsGlist_h, **itsObjList_h,
								  itsNumTypes, itsNumRepeats, itsNumObjs);
  } else if (itsGrpSize == 2) {
	 setClassPairSame(itsStimClasses, **itsGlist_h, 
								  itsNumTypes, itsNumRepeats, itsNumObjs);
  } else if (itsGrpSize == 3) {
	 SetClassTriadFindside(itsStimClasses, **itsGlist_h, 
								  itsNumTypes, itsNumRepeats, itsNumObjs);
  }
}


// construct a RealExpt from scratch from an obj file; construct
// groups based on grp_size (i.e. singles, pairs, triples)
RealExpt::RealExpt(Glist **glist_h, ObjList **olist_h,
                   int repeat, int grp_size, 
                   const char* obj_filename, 
                   double t_jitter, double r_jitter) :
  itsGlist_h(glist_h), 
  itsObjList_h(olist_h),
  itsNumRepeats(repeat),
  itsGrpSize(grp_size),
  itsCurTrial(0),
  itsPrevTrial(0),
  itsTjitter(t_jitter),
  itsRjitter(r_jitter)
{
DOTRACE("RealExpt::RealExpt");
  char line[BUF_SIZE];

  // peek into obj_file to count number of objs 
  itsNumObjs = countFileLines(obj_filename);

  // construct the ObjList
  delete *itsObjList_h;
  ifstream obj_file(obj_filename);
  *itsObjList_h = new ObjList(obj_file, "Face", itsNumObjs, 
                              itsGrpSize, OBJ_SPACING,
                              itsTjitter, itsRjitter);
  obj_file.close();

  // now construct the Glist
  switch (itsGrpSize) {
  case 1:
    delete *itsGlist_h;
    *itsGlist_h = new Glist(Glist::GLIST_SINGLES, itsNumObjs, **itsObjList_h);
    break;
  case 2:
    delete *itsGlist_h;
    *itsGlist_h = new Glist(Glist::GLIST_PAIRS, itsNumObjs, **itsObjList_h);
    break;
  case 3:
    delete *itsGlist_h;
    *itsGlist_h = new Glist(Glist::GLIST_TRIADS, itsNumObjs, **itsObjList_h);
    break;
  default:
    // do nothing (leaving itsGlist_h as it was)
    break;
  }

  if (*itsGlist_h) {
    itsNumTypes = (*itsGlist_h)->numGroups();
  }
  else {
    itsNumTypes = 0;
  }
  itsNumTrials = itsNumTypes * itsNumRepeats;
  itsResponses.resize(itsNumTrials, -1);

  itsRemaining.resize(itsNumTrials, 0);
  for (int i = 0; i < itsNumTrials; i++) {
    itsRemaining[i] = i;
  }

  itsStimClasses.resize(itsNumTrials, 0);
  itsStimClasses.resize(itsNumTrials, 0);
  if (itsGrpSize == 1) {
	 setClassFirstCategory(itsStimClasses, **itsGlist_h, **itsObjList_h,
								  itsNumTypes, itsNumRepeats, itsNumObjs);
  } else if (itsGrpSize == 2) {
	 setClassPairSame(itsStimClasses, **itsGlist_h, 
								  itsNumTypes, itsNumRepeats, itsNumObjs);
  } else if (itsGrpSize == 3) {
	 SetClassTriadFindside(itsStimClasses, **itsGlist_h, 
								  itsNumTypes, itsNumRepeats, itsNumObjs);
  }
}

RealExpt::~RealExpt() {
DOTRACE("RealExpt::~RealExpt");
}

RealExpt::RealExpt(istream &is, Glist **glist_h, ObjList **olist_h) : 
  itsGlist_h(glist_h),
  itsObjList_h(olist_h)
{
DOTRACE("RealExpt::RealExpt");
  is >> itsNumRepeats;          // 1
  is >> itsNumTypes;            // 2
  is >> itsNumTrials;           // 3
  is >> itsGrpSize;             // 4
  is >> itsNumObjs;             // 5
  is >> itsCurTrial;            // 6
  is >> itsPrevTrial;           // 7
  is >> itsTjitter;             // 8
  is >> itsRjitter;             // 9
  deserializeVector(is, itsResponses); // 10
  deserializeVector(is, itsStimClasses); // 11
  deserializeVector(is, itsRemaining); // 12
  delete *itsGlist_h;
  *itsGlist_h = new Glist(itsNumTypes, is, **itsObjList_h); // 13
  delete *itsObjList_h;
  *itsObjList_h = new ObjList(is, "Face", itsNumObjs, 
                              itsGrpSize, OBJ_SPACING,
                              itsTjitter, itsRjitter); // 14
}

IOResult RealExpt::serialize(ostream &os, IOFlag flag) const {
DOTRACE("RealExpt::serialize");
  char sep = ' ';
  os << itsNumRepeats << sep;   // 1
  os << itsNumTypes << sep;     // 2
  os << itsNumTrials << sep;    // 3
  os << itsGrpSize << sep;      // 4
  os << itsNumObjs << sep;      // 5
  os << itsCurTrial << sep;     // 6
  os << itsPrevTrial << sep;    // 7
  os << itsTjitter << sep;      // 8
  os << itsRjitter << sep;      // 9
  serializeVector(os, itsResponses); // 10
  serializeVector(os, itsStimClasses); // 11
  serializeVector(os, itsRemaining); // 12
  (*itsGlist_h)->serialize(os); // 13
  (*itsObjList_h)->writeRangeObjs(os, 0, itsNumObjs); // 14
  return checkStream(os);
}

///////////////
// accessors //
///////////////

int RealExpt::numTrials() const {
DOTRACE("RealExpt::numTrials");
  return itsNumTrials;
}

int RealExpt::numCompleted() const {
DOTRACE("RealExpt::numCompleted");
  return (itsNumTrials-itsRemaining.size());
}

int RealExpt::currentTrial() const {
DOTRACE("RealExpt::currentTrial");
  return itsCurTrial;
}

int RealExpt::currentStimClass() const {
DOTRACE("RealExpt::currentStimClass");
  return itsStimClasses[itsCurTrial];
}

int RealExpt::prevResponse() const {
DOTRACE("RealExpt::lastResponse");
  return itsResponses[itsPrevTrial];
}

bool RealExpt::isComplete() const {
DOTRACE("RealExpt::isComplete");
  return (itsRemaining.size() == 0);
}

const char* RealExpt::trialDescription() const {
DOTRACE("RealExpt::trialDescription");
  static char description[BUF_SIZE];

  ostrstream ost(description, BUF_SIZE);

  ost << "curstim " << itsCurTrial
      << ", stimclass " << itsStimClasses[itsCurTrial]
      << ", group";
  
  list<int> cur_group = (*itsGlist_h)->curGroup();
  int i = 0;
  for (list<int>::iterator ii = cur_group.begin(); 
       ii != cur_group.end();
       ii++) {
    ost << " " << (*ii - itsNumObjs*i);
    i++;
  }
  
  ost << ", categories";
  Face *p;
  for (list<int>::iterator jj = cur_group.begin();
       jj != cur_group.end();
       jj++) {
    if ( (p = dynamic_cast<Face *>((*itsObjList_h)->olObj(*jj)))
         != NULL ) {
      ost << " " << p->getCategory();
    }
  }
  
  ost << ", completed " << numCompleted() 
      << " of " << numTrials();

  ost << '\0';

  return description;
}

/////////////
// actions //
/////////////

void RealExpt::beginTrial() {
DOTRACE("RealExpt::beginTrial");
  itsPrevTrial = itsCurTrial;

  // if no trials remain, return
  if (itsRemaining.empty()) return;

  // pick new value for itsCurTrial
  itsCurTrial = itsRemaining[randIntRange(0, itsRemaining.size())];

  // show appropriate group in Glist
  (*itsGlist_h)->show(itsCurTrial/itsNumRepeats);
}

void RealExpt::recordResponse(int resp) {
DOTRACE("RealExpt::recordResponse");
  itsResponses[itsCurTrial] = resp;
  
  // update list of remaining trials
  itsRemaining.erase(find(itsRemaining.begin(), itsRemaining.end(), 
                          itsCurTrial));
}

#endif // !REALEXPT_CC_DEFINED
