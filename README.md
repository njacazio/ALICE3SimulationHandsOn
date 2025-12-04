# ALICE3SimulationHandsOn
Hands on software for the ALICE3 simulation

https://indico.cern.ch/event/1605731/


## Instructions to get started

In order to get ready for the hands on you should have the most recent **O2Physics** and **ACTS** installed via *aliBuild*.

### O2Physics Installation Decision Tree

```
START: Do you have O2Physics installed?
│
├─ NO ──> Do you have aliBuild installed? To know if you have it type `aliBuild -h`
│         │
│         ├─ NO ──> Install prerequisites: https://alice-doc.github.io/alice-analysis-tutorial/building/custom.htmlthen restart the tree
│         │         │
│         │         ├─ Are you using macOS?
│         │         │   └─ YES ──> follow https://alice-doc.github.io/alice-analysis-tutorial/building/prereq-macos.htmlthen restart the tree
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
                           │
                           ├─ NO ──> Install ACTS:
                           │         aliBuild build ACTS --defaults o2
                           │
                           └─ YES ──> READY FOR HANDS-ON! ✓

Troubleshooting:
├─ Build fails? ──> Check: disk space, internet connection, system dependencies
├─ Environment issues? ──> Run: alienv q (query installed packages)
└─ Still problems? ──> Get in touch
```