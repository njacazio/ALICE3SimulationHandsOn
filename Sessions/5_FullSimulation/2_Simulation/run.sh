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

export ALICE3_SIM_FIELD=ON
export ALICE3_MAGFIELD_MACRO=/tmp/ALICE3Field.C

if [ ! -f $ALICE3_MAGFIELD_MACRO ]; then
    echo "ALICE3_MAGFIELD_MACRO is not found, downloading the macro..."
    wget https://raw.githubusercontent.com/AliceO2Group/AliceO2/refs/heads/dev/Detectors/Upgrades/ALICE3/macros/ALICE3Field.C
    mv ALICE3Field.C $ALICE3_MAGFIELD_MACRO
fi


TimeOfStart=$(date +%s)
# Simulation configuration
NumberOfEvents=500
# Generator="-g pythia8hi"
Generator="-g pythia8pp"
Modules="-m TRK"
Modules="-m TF3"
Modules="-m TRK TF3"
# Modules="--detectorList ALICE3 --skipModules HALL --skipModules MAG"

echo "Running simulation with $NumberOfEvents events"

Cmd="o2-sim-serial-run5 -n $NumberOfEvents $Generator $Modules --configKeyValues \"Diamond.position[0]=0;Diamond.position[1]=0;Diamond.position[2]=0;Diamond.width[0]=0;Diamond.width[1]=0;Diamond.width[2]=0;TRKBase.layoutML=kTurboStaves;TRKBase.layoutOL=kStaggered;\""
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
