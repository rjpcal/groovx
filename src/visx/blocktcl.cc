///////////////////////////////////////////////////////////////////////
//
// blocktcl.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Jun 16 19:46:54 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCKTCL_CC_DEFINED
#define BLOCKTCL_CC_DEFINED

#include "visx/block.h"
#include "visx/element.h"

#include "tcl/itertcl.h"
#include "tcl/objpkg.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"
#include "tcl/tracertcl.h"

#include "util/iter.h"
#include "util/objfactory.h"
#include "util/strings.h"

#include "util/trace.h"


namespace
{
  void addElementIds(Util::Ref<Block> block, Tcl::List trial_ids, int repeat)
  {
    for (Tcl::List::Iterator<Util::UID>
           itr = trial_ids.begin<Util::UID>(),
           end = trial_ids.end<Util::UID>();
         itr != end;
         ++itr)
      {
        block->addElement(Ref<Element>(*itr), repeat);
      }
  }
}


extern "C"
int Block_Init(Tcl_Interp* interp)
{
DOTRACE("Block_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Block", "$Revision$");
  pkg->inheritPkg("IO");
  Tcl::defGenericObjCmds<Block>(pkg);

  Tcl::defTracing(pkg, Block::tracer);

  pkg->defVec("addTrialIds", "item_id(s) trial_id(s)",
              Util::bindLast(&addElementIds, 1));
  pkg->defVec("addTrialIds", "item_id(s) trial_id(s) repeat=1", &addElementIds );

  pkg->defGetter("currentTrial", &Block::currentElement);
  pkg->defGetter("currentTrialType", &Block::trialType);
  pkg->defGetter("isComplete", &Block::isComplete);
  pkg->defGetter("numCompleted", &Block::numCompleted);
  pkg->defGetter("numTrials", &Block::numElements);
  pkg->defGetter("prevResponse", &Block::lastResponse);
  pkg->defAction("removeAllTrials", &Block::clearElements);
  pkg->defAction("reset", &Block::vxReset);
  pkg->defSetter("shuffle", "item_id rand_seed", &Block::shuffle);
  pkg->defGetter("trials", &Block::getElements);
  pkg->defGetter("trialDescription", &Block::status);
  pkg->defAction("undoPrevTrial", &Block::vxUndo);

  Util::ObjFactory::theOne().registerCreatorFunc(&Block::make);

  return pkg->initStatus();
}

static const char vcid_blocktcl_cc[] = "$Header$";
#endif // !BLOCKTCL_CC_DEFINED
