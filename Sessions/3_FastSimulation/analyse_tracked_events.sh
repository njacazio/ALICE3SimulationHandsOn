#!/bin/bash

o2-analysis-onthefly-tracker -b --configuration json://configuration.json |
    o2-analysis-alice3-tracking-performance -b --configuration json://configuration.json --aod-file AO2D_pp.root
