///////////////////////////////////////////////////////////////////////
//
// blocktcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun 16 19:46:54 1999
// written: Tue Jun 29 18:35:40 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCKTCL_CC_DEFINED
#define BLOCKTCL_CC_DEFINED

#include <tcl.h>

#include "blocklist.h"
#include "block.h"
#include "tclcmd.h"
#include "tclitempkg.h"
#include "listpkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace BlockTcl {
  class BlockCmd;
  class AddTrialsCmd;
  class InitCmd;
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
	 Block* p = new Block();
	 int blockid = BlockList::theBlockList().insert(p);
	 returnInt(blockid);
  }
};

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

class BlockTcl::BlockPkg : public CTclIoItemPkg<Block> {
public:
  BlockPkg(Tcl_Interp* interp) :
	 CTclIoItemPkg<Block>(interp, "Block", "1.1")
  {
	 addCommand( new BlockCmd(interp, "Block::block") );

	 addCommand( new InitCmd(this, "Block::init") );

	 addCommand( new AddTrialsCmd(this, "Block::addTrials") );

	 declareSetter("shuffle", new CSetter<Block, int>(&Block::shuffle),
						"item_id rand_seed");

	 declareAction("reset", new CAction<Block>(&Block::reset));

	 declareGetter("currentTrialType",
						new CGetter<Block, int>(&Block::currentTrialType));
	 declareGetter("isComplete",
						new CGetter<Block, bool>(&Block::isComplete));
	 declareGetter("numCompleted",
						new CGetter<Block, int>(&Block::numCompleted));
	 declareGetter("numTrials",
						new CGetter<Block, int>(&Block::numTrials));
	 declareGetter("prevResponse",
						new CGetter<Block, int>(&Block::prevResponse));
	 declareGetter("trialDescription",
						new CGetter<Block, const char*>(&Block::trialDescription));
	 declareGetter("currentTrial",
						new CGetter<Block, int>(&Block::currentTrial));

	 declareAction("undoPrevTrial", new CAction<Block>(&Block::undoPrevTrial));

	 declareAttrib("verbose",
						new CAttrib<Block, bool>(&Block::getVerbose,
														&Block::setVerbose));
  }

  virtual Block* getCItemFromId(int id) {
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

namespace BlockListTcl {
  class BlockListPkg;
}

class BlockListTcl::BlockListPkg : public ListPkg<BlockList> {
public:
  BlockListPkg(Tcl_Interp* interp) :
	 ListPkg<BlockList>(interp, "BlockList", "3.0")
  {
	 BlockList::theBlockList().insertAt(0, new Block());
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

  new BlockTcl::BlockPkg(interp);
  new BlockListTcl::BlockListPkg(interp);

  return TCL_OK;
}

static const char vcid_blocktcl_cc[] = "$Header$";
#endif // !BLOCKTCL_CC_DEFINED
