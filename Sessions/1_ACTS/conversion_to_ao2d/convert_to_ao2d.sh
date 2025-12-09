#!/bin/bash


InputPath="/home/njacazio/cernbox/Share/MKUltra/ACTS/odd_output/"
InputPath="/home/njacazio/cernbox/Dropshipping/DATA/"


o2-analysis-alice3-tracking-translator --aod-file ${InputPath}/AO2D_1.root -b --aod-writer-keep dangling