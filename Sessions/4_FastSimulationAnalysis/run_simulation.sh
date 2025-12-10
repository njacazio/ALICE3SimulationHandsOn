#!/bin/bash

o2-analysis-onthefly-tracker -b --configuration json://configuration.json |
    o2-analysis-onthefly-richpid -b --configuration json://configuration.json |
    o2-analysis-alice3-dilepton -b --configuration json://configuration.json |
    o2-analysis-onthefly-tofpid -b --configuration json://configuration.json --aod-file ../3_FastSimulation/AO2D_pp.root

    # o2-analysis-alice3-pid-separation-power -b --configuration json://configuration.json |