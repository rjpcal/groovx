///////////////////////////////////////////////////////////////////////
//
// blocktcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun 16 19:46:54 1999
// written: Sat Aug 25 21:27:28 2001
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
}

//---------------------------------------------------------------------
//
// BlockTcl::Block_Init
//
//---------------------------------------------------------------------

extern "C"
int Block_Init(Tcl_Interp* interp)
{
DOTRACE("Block_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Block", "$Revision$");
  Tcl::defGenericObjCmds<Block>(pkg);

  Tcl::defTracing(pkg, Block::tracer);

  pkg->defVec("addTrialIds", "item_id(s) trial_id(s)",
              &BlockTcl::addTrialIds1);
  pkg->defVec("addTrialIds", "item_id(s) trial_id(s) repeat=1",
              &BlockTcl::addTrialIds2 );

  pkg->defGetter("currentTrial", &Block::currentTrial);
  pkg->defGetter("currentTrialType", &Block::currentTrialType);
  pkg->defGetter("isComplete", &Block::isComplete);
  pkg->defGetter("numCompleted", &Block::numCompleted);
  pkg->defGetter("numTrials", &Block::numTrials);
  pkg->defGetter("prevResponse", &Block::prevResponse);
  pkg->defAction("removeAllTrials", &Block::removeAllTrials);
  pkg->defAction("reset", &Block::resetBlock);
  pkg->defSetter("shuffle", "item_id rand_seed", &Block::shuffle);
  pkg->defGetter("trialDescription", &Block::trialDescription);
  pkg->defAction("undoPrevTrial", &Block::undoPrevTrial);
  pkg->defAttrib("verbose", &Block::getVerbose, &Block::setVerbose);

  Util::ObjFactory::theOne().registerCreatorFunc(&Block::make);

  return pkg->initStatus();
}

static const char vcid_blocktcl_cc[] = "$Header$";
#endif // !BLOCKTCL_CC_DEFINED
