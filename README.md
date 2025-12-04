# ALICE3SimulationHandsOn
Hands on software for the ALICE3 simulation

https://indico.cern.ch/event/1605731/

## Instructions per session:

1. **[ACTS](Sessions/1_ACTS/README.md)** - 75 min
   - Running ACTS with ALICE configuration
   - Converting ACTS simulation output to AO2D format

2. **[Fast Analysis Tool (FAT)](Sessions/2_FastAnalysisTool/README.md)** - 45 min
   - Learn what FAT does and how to extract detector performance
   - Learn how to produce Look-Up Tables (LUTs)

3. **[Fast Simulation](Sessions/3_FastSimulation/README.md)** - 75 min
   - On-the-fly simulation for primary tracks and decays
   - Event generators and O2DPG configuration
   - Efficiency and momentum resolution studies
   - Weak decay reconstruction performance

4. **[Fast Simulation Analysis](Sessions/4_FastSimulationAnalysis/README.md)** - 75 min
   - Utility tasks for on-the-fly simulation (TOF, RICH, ClusterSize)
   - TOF PID in analysis with multiple detector configurations
   - D meson invariant mass reconstruction

5. **[Full Simulation](Sessions/5_FullSimulation/README.md)** - 75 min
   - What is full simulation and geometry
   - Current status of full simulation in O2
   - Running your own full simulation within O2

6. **[Full Simulation Analysis](Sessions/6_FullSimulationAnalysis/README.md)** - 75 min
   - Reading hit information
   - Drawing detector geometry
   - Particle level information between hits and particles
   - Advanced analysis studies


## Instructions to get started

In order to get ready for the hands on you should have the most recent **O2Physics** and **ACTS** installed via *aliBuild*.
It doesn't hurt to setup your lxplus access: https://linux-training.web.cern.ch/lxplus/introduction/ just as a backup.


### O2Physics Installation Decision Tree

```
START: Do you have O2Physics installed?
│
├─ NO ──> Do you have aliBuild installed? To know if you have it type `aliBuild version`
│         │
│         ├─ NO ──> Install prerequisites: https://alice-doc.github.io/alice-analysis-tutorial/building/custom.html then restart the tree
│         │         │
│         │         ├─ Are you using macOS?
│         │         │   └─ YES ──> follow https://alice-doc.github.io/alice-analysis-tutorial/building/prereq-macos.html then restart the tree
│         │         │
│         │         ├─ Are you using Ubuntu/Debian?
│         │         │   └─ YES ──> follow https://alice-doc.github.io/alice-analysis-tutorial/building/prereq-ubuntu.html then restart the tree
│         │         │
│         │         └─ Are you using something else?
│         │             └─ YES ──> follow the guide according to your system https://alice-doc.github.io/alice-analysis-tutorial/building/custom.html#prerequisites then restart the tree
│         │
│         └─ YES ──> Do you have `ALIBUILD_WORK_DIR` defined in your environment ? to check run e.g. `echo $ALIBUILD_WORK_DIR`  
│                   │
│                   ├─ NO ──> Do you have other software installed with aliBuild?
│                   │         |
|                   |         ├─ NO ──> go in a favourite directory and run `aliBuild init O2Physics`
│                   │         |
│                   │         |
|                   |         └─ YES ──> go where you previously installed software with aliBuild, update alidist with `cd alidist && git pull --rebase && cd -`and then run `aliBuild init O2Physics`
│                   │
│                   │
│                   └─ YES ──> Move to your software directory:
│                              `cd $ALIBUILD_WORK_DIR/../`
│                              `aliBuild init O2Physics@master`
│                              │
│                              └──> Build O2Physics:
│                                   `aliBuild build O2Physics --defaults o2`
│
└─ YES ──> Is O2Physics up to date?
           │
           ├─ NO ──> Update O2Physics:
           │         cd ~/alice/O2Physics (or your O2Physics path)
           │         git pull --rebase
           │         cd ~/alice
           │         aliBuild build O2Physics --defaults o2 --force-unknown-architecture
           │         │
           │         └──> Load environment:
           │              alienv enter O2Physics/latest
           │
           └─ YES ──> Load environment and verify:
                      alienv enter O2Physics/latest
                      o2-sim --version
                      │
                      └──> Is ACTS installed?
                           (try to run `alienv enter ACTS/latest`)
                           │
                           ├─ NO ──> Install ACTS:
                           │         aliBuild build ACTS --defaults o2
                           │
                           └─ YES ──> READY FOR THE NEXT DECISION TREE! ✓

Troubleshooting:
├─ Build fails? ──> Check: disk space, internet connection, system dependencies, RAM
└─ Still problems? ──> Get in touch
```


### ACTS Installation Decision Tree

```
START: Did you follow the O2Physics installation decision tree above?
│
├─ NO ──> ⚠️  STOP! Go back and complete O2Physics installation first
│         ACTS requires the same aliBuild environment as O2Physics
│         Once O2Physics is installed, come back here and restart the tree
│
└─ YES ──> Do you have ACTS installed?
           │
           ├─ NO ──> Do you have `ALIBUILD_WORK_DIR` defined in your environment? to check run e.g. `echo $ALIBUILD_WORK_DIR`
           │         │
           │         ├─ NO ──> Go to the O2Physics installation directory
           │         │                    │
           │         │                    └──> Initialize ACTS: (before make sure that alidist is up to date!!)
           │         │                         `aliBuild init ACTS`
           │         │                         │
           │         │                         └──> Build ACTS:
           │         │                              `aliBuild build ACTS`
           │         │
           │         └─ YES ──> Go to your O2Physics installation directory with `cd $ALIBUILD_WORK_DIR/../`
           │                    `cd $ALIBUILD_WORK_DIR/../`
           │                              │
           │                              └──> Initialize ACTS: (before make sure that alidist is up to date!!)
           │                                   `aliBuild init ACTS`
           │                                   │
           │                                   └──> Build ACTS:
           │                                        `aliBuild build ACTS`
           │
           └─ YES ──> Is ACTS on the branch v43.0.1 with `git branch`?
                      │
                      ├─ NO ──> Update ACTS:
                      │         cd $ALIBUILD_WORK_DIR/../
                      │         Update alidist: `cd alidist && git pull --rebase && cd -`
                      │         Update ACTS branch: `cd ACTS && git switch v43.0.1 --rebase && cd -`
                      │         Update ACTS code: `cd ACTS && git pull --rebase && cd -`
                      │         Rebuild: `aliBuild build ACTS --defaults o2`
                      │         Restart this decision tree
                      │
                      └─ YES ──> Load environment and verify:
                                 `alienv enter ACTS/latest`
                                 │
                                 └──> Does ACTS work?
                                      ├─ NO ──> Environment mismatch detected
                                      │
                                      └─ YES ──> READY FOR ACTS SESSION! ✓

Troubleshooting:
└─ ACTS build fails? ──> Get in touch
```
