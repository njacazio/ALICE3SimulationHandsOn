## Session 75 min
https://docs.google.com/presentation/d/1r0XfPtl9-VsCOagXk0ScpxJtakn8wNqEFnH6mwDXjbs/edit?usp=sharing

## Scope of the session
- Learn what the on the fly simulation does for primary tracks and decays, integration with hyperloop 15 min
- Few details about the event generators in general (e.g. pythia) configuration and O2DPG 5 min
- Use case 1: efficiency and momentum resolution of pion kaons and protons **with multiple detectors** 20 min
- Put the LUTs on the CCDB and retrieval and run with your own detector configuration, start producing D0 events 15 min
- Use case 2: weak decay reconstruction performance with the TEnv configuration 20 min

## At the end:
- the analyser should be able to run the (staged) monte carlo simulation with different detector configurations and analyse the output to extract detector performance metrics (efficiency, resolution et cetera)

## Assignment
- Configure a custom Pythia8 event generator to use as particle producer
- Generate pp collision events and store them in the AO2D format
- Understand MC particle output from the content of the Tree
- Generate Pb-Pb events or add a new exotic particle to the simulation
- Apply detector smearing using LUTs with the on-the-fly tracking simulation, understand how to feed LUTs to the fast-tracker and add a detector smearing
- Study primary track performance for pions kaons and protons with multiple detector
- verify secondary vertex reconstrucion with the on-the-fly simulation
