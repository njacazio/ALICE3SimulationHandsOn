#!/bin/bash

export O2PHYSICS_COMPONENTS=""
export O2PHYSICS_COMPONENTS="${O2PHYSICS_COMPONENTS} Common/install"
export O2PHYSICS_COMPONENTS="${O2PHYSICS_COMPONENTS} ALICE3/install"

aliBuild build O2Physics -j 10 -d 
