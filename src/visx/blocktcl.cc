///////////////////////////////////////////////////////////////////////
//
// blocktcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun 16 19:46:54 1999
// written: Thu Jun 24 19:21:13 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCKTCL_CC_DEFINED
#define BLOCKTCL_CC_DEFINED

#include <tcl.h>

#include "blocklist.h"
#include "expt.h"
#include "tclcmd.h"
#include "tclitempkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace BlockTcl {
  class BlockCmd;
  class InitCmd;
  class BlockListPkg;
  class BlockPkg;
}

//---------------------------------------------------------------------
//
// BlockCmd --
//
//---------------------------------------------------------------------

class BlockTcl::BlockCmd : public TclCmd {
public:
  BlockCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
	 Expt* p = new Expt();
	 int blockid = BlockList::theBlockList().insert(p);
	 returnInt(blockid);
  }
};

//---------------------------------------------------------------------
//
// BlockTcl::InitCmd --
//
//---------------------------------------------------------------------

class BlockTcl::InitCmd : public TclItemCmd<Expt> {
public:
  InitCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<Expt>(pkg, cmd_name, "block_id repeat rand_seed", 4, 4) {}
protected:
  virtual void invoke() {
	 Expt* block = getItem();

	 int repeat = getIntFromArg(2);
	 int rand_seed = getIntFromArg(3);
	 
	 block->init(repeat, rand_seed);
  }
};

///////////////////////////////////////////////////////////////////////
//
// BlockPkg class definition
//
///////////////////////////////////////////////////////////////////////

class BlockTcl::BlockPkg : public CTclIoItemPkg<Expt> {
public:
  BlockPkg(Tcl_Interp* interp) :
	 CTclIoItemPkg<Expt>(interp, "Block", "1.1")
  {
	 addCommand( new BlockCmd(interp, "Block::block") );

	 addCommand( new InitCmd(this, "Block::init") );

	 declareGetter("currentTrialType",
						new CGetter<Expt, int>(&Expt::currentTrialType));
	 declareGetter("isComplete",
						new CGetter<Expt, bool>(&Expt::isComplete));
	 declareGetter("numCompleted",
						new CGetter<Expt, int>(&Expt::numCompleted));
	 declareGetter("numTrials",
						new CGetter<Expt, int>(&Expt::numTrials));
	 declareGetter("prevResponse",
						new CGetter<Expt, int>(&Expt::prevResponse));
	 declareGetter("trialDescription",
						new CGetter<Expt, const char*>(&Expt::trialDescription));
	 declareGetter("currentTrial",
						new CGetter<Expt, int>(&Expt::currentTrial));

	 declareAction("undoPrevTrial", new CAction<Expt>(&Expt::undoPrevTrial));

	 declareAttrib("verbose",
						new CAttrib<Expt, bool>(&Expt::getVerbose,
														&Expt::setVerbose));
	 
	 int result = Tcl_Eval(interp,
		  "namespace eval Expt {\n"
		  "  proc currentTrial {} { return [Block::currentTrial 0] }\n"
		  "  proc currentTrialType {} { return [Block::currentTrialType 0] }\n"
		  "  proc isComplete {} { return [Block::isComplete 0] }\n"
		  "  proc numCompleted {} { return [Block::numCompleted 0] }\n"
		  "  proc numTrials {} { return [Block::numTrials 0] }\n"
		  "  proc prevResponse {} { return [Block::prevResponse 0] }\n"
		  "  proc trialDescription {} { return [Block::trialDescription 0] }\n"
		  "  proc verbose args { \n"
		  "      if { [llength $args] == 0 } { return [Block::verbose 0] \n"
        "      } else { Block::verbose 0 [lindex $args 0] }}"
		  "}\n");

	 DebugEvalNL(result);
	 DebugEvalNL(Tcl_GetStringResult(interp));
  }

  virtual Expt* getCItemFromId(int id) {
	 if ( !BlockList::theBlockList().isValidId(id) ) {
		throw TclError("invalid block id");
	 }
	 return BlockList::theBlockList().getPtr(id);
  }

  virtual IO& getIoFromId(int id) {
	 return dynamic_cast<IO&>( *(getCItemFromId(id)) );
  }
};

///////////////////////////////////////////////////////////////////////
//
// BlockListPkg class definition
//
///////////////////////////////////////////////////////////////////////

class BlockTcl::BlockListPkg : public CTclIoItemPkg<BlockList> {
public:
  BlockListPkg(Tcl_Interp* interp) :
	 CTclIoItemPkg<BlockList>(interp, "BlockList", "1.1", 0)
  {
	 BlockList::theBlockList().insertAt(0, new Expt());

	 declareGetter("count", new CGetter<BlockList, int>(&BlockList::count));
	 declareAction("reset", new CAction<BlockList>(&BlockList::clear));
  }
  
  virtual IO& getIoFromId(int) { return BlockList::theBlockList(); }

  virtual BlockList* getCItemFromId(int) {
	 return &BlockList::theBlockList();
  }
};

//---------------------------------------------------------------------
//
// BlockTcl::Block_Init
//
//---------------------------------------------------------------------

extern "C" Tcl_PackageInitProc Block_Init;

int Block_Init(Tcl_Interp* interp) {
DOTRACE("Block_Init");

  new BlockTcl::BlockListPkg(interp);
  new BlockTcl::BlockPkg(interp);

  return TCL_OK;
}

static const char vcid_blocktcl_cc[] = "$Header$";
#endif // !BLOCKTCL_CC_DEFINED
