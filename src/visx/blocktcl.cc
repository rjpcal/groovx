///////////////////////////////////////////////////////////////////////
//
// blocktcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun 16 19:46:54 1999
// written: Tue Nov 30 19:05:39 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCKTCL_CC_DEFINED
#define BLOCKTCL_CC_DEFINED

#include <tcl.h>
#include <vector>

#include "iomgr.h"
#include "blocklist.h"
#include "block.h"
#include "tclcmd.h"
#include "listitempkg.h"
#include "listpkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace BlockTcl {
  class AddTrialsCmd;
  class AddTrialIdsCmd;
  class InitCmd;
  class BlockPkg;
}

//---------------------------------------------------------------------
//
// BlockTcl::AddTrialsCmd --
//
//---------------------------------------------------------------------

class BlockTcl::AddTrialsCmd : public TclItemCmd<Block> {
public:
  AddTrialsCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<Block>(pkg, cmd_name, 
							"block_id ?first_trial=-1 last_trial=-1 repeat=1?",
							2, 5, false) {}
protected:
  virtual void invoke() {
	 Block* block = getItem();

	 int first_trial = (objc() < 3) ? -1 : getIntFromArg(2);
	 int last_trial  = (objc() < 4) ? -1 : getIntFromArg(3);
	 int repeat      = (objc() < 5) ?  1 : getIntFromArg(4);

	 block->addTrials(first_trial, last_trial, repeat);
  }
};

//---------------------------------------------------------------------
//
// BlockTcl::AddTrialIdsCmd --
//
//---------------------------------------------------------------------

class BlockTcl::AddTrialIdsCmd : public TclItemCmd<Block> {
public:
  AddTrialIdsCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<Block>(pkg, cmd_name, 
							"block_id trial_id(s) ?repeat=1?",
							3, 4, false) {}
protected:
  virtual void invoke() {
	 Block* block = getItem();

	 vector<int> ids;
	 getSequenceFromArg(2, back_inserter(ids), (int*) 0);

	 int repeat = (objc() < 4)  ?  1 : getIntFromArg(3);

 	 for (size_t i = 0; i < ids.size(); ++i) {
 		block->addTrial(ids[i], repeat);
 	 }
	 returnVoid();
  }
};

//---------------------------------------------------------------------
//
// BlockTcl::InitCmd --
//
//---------------------------------------------------------------------

class BlockTcl::InitCmd : public TclItemCmd<Block> {
public:
  InitCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<Block>(pkg, cmd_name, "block_id repeat rand_seed", 4, 4) {}
protected:
  virtual void invoke() {
	 Block* block = getItem();

	 int repeat = getIntFromArg(2);
	 int rand_seed = getIntFromArg(3);
	 
	 block->addTrials(-1, -1, repeat);
	 block->shuffle(rand_seed);
  }
};

///////////////////////////////////////////////////////////////////////
//
// BlockPkg class definition
//
///////////////////////////////////////////////////////////////////////

class BlockTcl::BlockPkg : public ListItemPkg<Block, BlockList> {
public:
  BlockPkg(Tcl_Interp* interp) :
	 ListItemPkg<Block, BlockList>(interp, BlockList::theBlockList(),
											 "Block", "1.1")
  {
	 addCommand( new InitCmd(this, "Block::init") );
	 addCommand( new AddTrialsCmd(this, "Block::addTrials") );
	 addCommand( new AddTrialIdsCmd(this, "Block::addTrialIds") );

	 declareCGetter("currentTrial", &Block::currentTrial);
	 declareCGetter("currentTrialType", &Block::currentTrialType);
	 declareCGetter("isComplete", &Block::isComplete);
	 declareCGetter("numCompleted", &Block::numCompleted);
	 declareCGetter("numTrials", &Block::numTrials);
	 declareCGetter("prevResponse", &Block::prevResponse);
	 declareCAction("removeAllTrials", &Block::removeAllTrials);
	 declareCAction("reset", &Block::resetBlock);
	 declareCSetter("shuffle", &Block::shuffle, "item_id rand_seed");
	 declareCGetter("trialDescription", &Block::trialDescription);
	 declareCAction("undoPrevTrial", &Block::undoPrevTrial);
	 declareCAttrib("verbose", &Block::getVerbose, &Block::setVerbose);
  }
};

///////////////////////////////////////////////////////////////////////
//
// BlockListPkg class definition
//
///////////////////////////////////////////////////////////////////////

namespace BlockListTcl {
  class BlockListPkg;
}

class BlockListTcl::BlockListPkg : public ListPkg<BlockList> {
public:
  BlockListPkg(Tcl_Interp* interp) :
	 ListPkg<BlockList>(interp, BlockList::theBlockList(), "BlockList", "3.0")
  {
	 BlockList::theBlockList().insertAt(0, new Block());
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

  new BlockTcl::BlockPkg(interp);
  new BlockListTcl::BlockListPkg(interp);

  FactoryRegistrar<IO, Block>::registerWith(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_blocktcl_cc[] = "$Header$";
#endif // !BLOCKTCL_CC_DEFINED
