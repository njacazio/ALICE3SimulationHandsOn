#!/bin/bash

. $ACTS_ROOT/python/setup.sh


export ACTS_SEQUENCER_DISABLE_FPEMON=true

python3 full_chain_Nov_2025.py \
    --out_dir_prefix TEST_GEANT4 \
    -n5 --nThreads 1 \
    --field 2.0 --gunMult 1 \
    --gunPID 211 \
    --gunPtRange 0.5 0.5001 \
    --gunEtaRange 0.2 0.2001 \
    --detSim Fatras \
    --seedingLayers VD \
    --minSeedPt 0.07 \
    --useFieldMap \
    --seedingAlgo GridTriplet \
    --nMeasurementsMin 7
