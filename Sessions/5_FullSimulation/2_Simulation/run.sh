#!/bin/bash

alien-token-info


o2-sim-serial-run5 -n 10 -g pythia8hi -m TRK --configKeyValues "Diamond.position[0]=0;Diamond.position[1]=0;Diamond.position[2]=0;Diamond.width[0]=0;Diamond.width[1]=0;Diamond.width[2]=0;TRKBase.layoutML=kTurboStaves;TRKBase.layoutOL=kStaggered;" 

