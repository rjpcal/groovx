///////////////////////////////////////////////////////////////////////
//
// blocktcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun 16 19:46:54 1999
// written: Wed Oct 25 20:15:59 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCKTCL_CC_DEFINED
#define BLOCKTCL_CC_DEFINED

#include "blocklist.h"
#include "block.h"

#include "tlist.h"
#include "trialbase.h"

#include "io/iofactory.h"

#include "tcl/listitempkg.h"
#include "tcl/listpkg.h"
#include "tcl/tclcmd.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace BlockTcl {
  class AddTrialsCmd;
  class AddTrialIdsCmd;
  class InitCmd;
  class BlockPkg;

  void addTrials(Block* block, int first_trial, int last_trial, int repeat)
  {
	 // Account for the convention of using -1 to indicate 'beginning'
	 // for first_trial, and 'end' for last_trial.

	 if (first_trial == -1)
		first_trial = 0;

	 bool testing_last_trial = (last_trial != -1);

	 for (Tlist::Iterator
			  itr = Tlist::theTlist().begin(),
			  end = Tlist::theTlist().end();
			itr != end;
			++itr)
		{
		  int id = itr.getId();

		  if (id < first_trial) continue;
		  if (testing_last_trial && (id > last_trial)) continue;

		  block->addTrial(NullableItemWithId<TrialBase>(id), repeat);
		}
  }
}

//---------------------------------------------------------------------
//
// BlockTcl::AddTrialsCmd --
//
//---------------------------------------------------------------------

class BlockTcl::AddTrialsCmd : public Tcl::TclItemCmd<Block> {
public:
  AddTrialsCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Block>(pkg, cmd_name, 
							"block_id ?first_trial=-1 last_trial=-1 repeat=1?",
							2, 5, false) {}
protected:
  virtual void invoke() {
	 Block* block = getItem();

	 int first_trial = (objc() < 3) ? -1 : getIntFromArg(2);
	 int last_trial  = (objc() < 4) ? -1 : getIntFromArg(3);
	 int repeat      = (objc() < 5) ?  1 : getIntFromArg(4);

	 addTrials(block, first_trial, last_trial, repeat);
  }
};

//---------------------------------------------------------------------
//
// BlockTcl::AddTrialIdsCmd --
//
//---------------------------------------------------------------------

class BlockTcl::AddTrialIdsCmd : public Tcl::TclItemCmd<Block> {
public:
  AddTrialIdsCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Block>(pkg, cmd_name, 
							"block_id trial_id(s) ?repeat=1?",
							3, 4, false) {}
protected:
  virtual void invoke() {
	 Block* block = getItem();

	 int repeat = (objc() < 4)  ?  1 : getIntFromArg(3);

	 for (Tcl::ListIterator<int>
			  itr = beginOfArg(2, (int*)0),
			  end = endOfArg(2, (int*)0);
			itr != end;
			++itr)
		{
		  block->addTrial(NullableItemWithId<TrialBase>(*itr), repeat);
		}
  }
};

//---------------------------------------------------------------------
//
// BlockTcl::InitCmd --
//
//---------------------------------------------------------------------

class BlockTcl::InitCmd : public Tcl::TclItemCmd<Block> {
public:
  InitCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Block>(pkg, cmd_name, "block_id repeat rand_seed", 4, 4) {}
protected:
  virtual void invoke() {
	 Block* block = getItem();

	 int repeat = getIntFromArg(2);
	 int rand_seed = getIntFromArg(3);
	 
	 addTrials(block, -1, -1, repeat);
	 block->shuffle(rand_seed);
  }
};

///////////////////////////////////////////////////////////////////////
//
// BlockPkg class definition
//
///////////////////////////////////////////////////////////////////////

class BlockTcl::BlockPkg : public Tcl::ListItemPkg<Block, BlockList> {
public:
  BlockPkg(Tcl_Interp* interp) :
	 Tcl::ListItemPkg<Block, BlockList>(interp, BlockList::theBlockList(),
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

//---------------------------------------------------------------------
//
// BlockTcl::Block_Init
//
//---------------------------------------------------------------------

extern "C"
int Block_Init(Tcl_Interp* interp) {
DOTRACE("Block_Init");

  Tcl::TclPkg* pkg1 = new BlockTcl::BlockPkg(interp);
  Tcl::TclPkg* pkg2 = new Tcl::IoPtrListPkg(interp, BlockList::theBlockList(),
														  "BlockList", "$Revision$");

  IO::IoFactory::theOne().registerCreatorFunc(&Block::make);

  return pkg1->initedOk() ? pkg2->initStatus() : pkg1->initStatus();
}

static const char vcid_blocktcl_cc[] = "$Header$";
#endif // !BLOCKTCL_CC_DEFINED
