#!/bin/bash

InputPath="../full_chain_simple/acts_simulation_output/sim_pythia_pp/"

rm AO2D_converted.root
ln -s $PWD/.AO2D_1.root ${InputPath}/AO2D_1.root # Create a symlink to the utility AO2D file

o2-analysis-alice3-tracking-translator --aod-file ${InputPath}/AO2D_1.root -b --aod-writer-keep dangling

# unlink the utility AO2D file
unlink ${InputPath}/AO2D_1.root

mv AnalysisResults_trees.root AO2D_converted.root
