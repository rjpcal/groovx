///////////////////////////////////////////////////////////////////////
//
// blocktcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun 16 19:46:54 1999
// written: Mon Jun 11 14:49:19 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCKTCL_CC_DEFINED
#define BLOCKTCL_CC_DEFINED

#include "block.h"
#include "trialbase.h"

#include "tcl/ioitempkg.h"
#include "tcl/tclcmd.h"
#include "tcl/tracertcl.h"

#include "util/objfactory.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace BlockTcl {
  class AddTrialIdsCmd;
  class BlockPkg;
}

//---------------------------------------------------------------------
//
// BlockTcl::AddTrialIdsCmd --
//
//---------------------------------------------------------------------

class BlockTcl::AddTrialIdsCmd : public Tcl::TclItemCmd<Block> {
public:
  AddTrialIdsCmd(Tcl::CTclItemPkg<Block>* pkg, const char* cmd_name) :
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
		  block->addTrial(Ref<TrialBase>(*itr), repeat);
		}
  }
};

///////////////////////////////////////////////////////////////////////
//
// BlockPkg class definition
//
///////////////////////////////////////////////////////////////////////

class BlockTcl::BlockPkg : public Tcl::IoItemPkg<Block> {
public:
  BlockPkg(Tcl_Interp* interp) :
	 Tcl::IoItemPkg<Block>(interp, "Block", "$Revision$")
  {
	 Tcl::addTracing(this, Block::tracer);

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

  Tcl::TclPkg* pkg = new BlockTcl::BlockPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc(&Block::make);

  return pkg->initStatus();
}

static const char vcid_blocktcl_cc[] = "$Header$";
#endif // !BLOCKTCL_CC_DEFINED
