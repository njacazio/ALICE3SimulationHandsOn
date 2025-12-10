#!/bin/bash

o2-analysis-onthefly-tracker -b --configuration json://configuration.json |
    o2-analysis-track-propagation -b --configuration json://configuration.json --track-propagation-output-filename propagated_tracks.root
