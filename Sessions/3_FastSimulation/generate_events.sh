#!/bin/bash

# This script generates the events and saves them to a file.

o2-sim-dpl-eventgen \
    --generator external \
    --nEvents 100000 \
    --aggregate-timeframe 50 \
    --configFile generator_configuration/pythia8_pp.ini -b \
    --configuration json://configuration.json |
    o2-sim-mctracks-to-aod \
        --tf-offset 391403000000 -b \
        --configuration json://configuration.json \
        --aod-writer-keep dangling
mv AnalysisResults_trees.root AO2D_pp.root
