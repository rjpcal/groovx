///////////////////////////////////////////////////////////////////////
//
// blocktcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun 16 19:46:54 1999
// written: Wed Jul 18 12:28:06 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCKTCL_CC_DEFINED
#define BLOCKTCL_CC_DEFINED

#include "block.h"
#include "trialbase.h"

#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"
#include "tcl/tracertcl.h"

#include "util/objfactory.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace BlockTcl
{
  void addTrialIds2(Util::Ref<Block> block, Tcl::List trial_ids, int repeat)
  {
    for (Tcl::List::Iterator<int>
           itr = trial_ids.begin<int>(),
           end = trial_ids.end<int>();
         itr != end;
         ++itr)
      {
        block->addTrial(Ref<TrialBase>(*itr), repeat);
      }
  }

  void addTrialIds1(Util::Ref<Block> block, Tcl::List trial_ids)
  {
    addTrialIds2(block, trial_ids, 1);
  }

  class BlockPkg;
}

///////////////////////////////////////////////////////////////////////
//
// BlockPkg class definition
//
///////////////////////////////////////////////////////////////////////

class BlockTcl::BlockPkg : public Tcl::Pkg {
public:
  BlockPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "Block", "$Revision$")
  {
    Tcl::defGenericObjCmds<Block>(this);

    Tcl::defTracing(this, Block::tracer);

    defVec("addTrialIds", "item_id(s) trial_id(s)", &BlockTcl::addTrialIds1);
    defVec("addTrialIds", "item_id(s) trial_id(s) repeat=1",
           &BlockTcl::addTrialIds2 );

    defGetter("currentTrial", &Block::currentTrial);
    defGetter("currentTrialType", &Block::currentTrialType);
    defGetter("isComplete", &Block::isComplete);
    defGetter("numCompleted", &Block::numCompleted);
    defGetter("numTrials", &Block::numTrials);
    defGetter("prevResponse", &Block::prevResponse);
    defAction("removeAllTrials", &Block::removeAllTrials);
    defAction("reset", &Block::resetBlock);
    defSetter("shuffle", "item_id rand_seed", &Block::shuffle);
    defGetter("trialDescription", &Block::trialDescription);
    defAction("undoPrevTrial", &Block::undoPrevTrial);
    defAttrib("verbose", &Block::getVerbose, &Block::setVerbose);
  }
};

//---------------------------------------------------------------------
//
// BlockTcl::Block_Init
//
//---------------------------------------------------------------------

extern "C"
int Block_Init(Tcl_Interp* interp)
{
DOTRACE("Block_Init");

  Tcl::Pkg* pkg = new BlockTcl::BlockPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc(&Block::make);

  return pkg->initStatus();
}

static const char vcid_blocktcl_cc[] = "$Header$";
#endif // !BLOCKTCL_CC_DEFINED
