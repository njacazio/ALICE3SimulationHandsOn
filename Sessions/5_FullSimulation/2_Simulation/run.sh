#!/bin/bash

if [ -z "$O2_ROOT" ]; then
    echo "Error: O2_ROOT environment variable not defined"
    exit 1
fi
echo "O2_ROOT is set to $O2_ROOT"

alien-token-info | grep -q EXPIRE || {
    echo "Error: Invalid or missing ALICE token"
    exit 1
}
echo "ALICE token is valid"

if [ ! -d SimulationResults ]; then
    echo "Creating SimulationResults directory"
    mkdir SimulationResults
    echo "*" >SimulationResults/.gitignore
fi

cd SimulationResults || {
    echo "Error: Failed to change directory to SimulationResults"
    exit 1
}

TimeOfStart=$(date +%s)
# Simulation configuration
NumberOfEvents=10
Generator=pythia8hi
Modules="TRK"

echo "Running simulation with $NumberOfEvents events"

Cmd="o2-sim-serial-run5 -n $NumberOfEvents -g $Generator -m $Modules --configKeyValues \"Diamond.position[0]=0;Diamond.position[1]=0;Diamond.position[2]=0;Diamond.width[0]=0;Diamond.width[1]=0;Diamond.width[2]=0;TRKBase.layoutML=kTurboStaves;TRKBase.layoutOL=kStaggered;\""
echo "Executing simulation command: $Cmd"
eval $Cmd || {
    echo "Error: Simulation command failed"
    exit 1
}

echo "Simulation complete."

cd - || {
    echo "Error: Failed to return to previous directory"
    exit 1
}

date
echo "Took $(($(date +%s) - TimeOfStart)) seconds"
