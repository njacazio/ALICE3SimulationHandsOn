#!/bin/bash

root -l -b <<EOF
    .L AliLog.cxx+
    .L AliCheb3DCalc.cxx+
    .L AliCheb3D.cxx+
    .L AliMagWrapCheb.cxx+
    .L AliMagFast.cxx+
    .L AliMagF.cxx+

    .L AliVMisc.cxx+
    .L AliPDG.cxx++
    .L AliVVertex.cxx+
    .L AliPID.cxx+
    .L AliVParticle.cxx+
    .L AliVTrack.cxx+
    .L AliExternalTrackParam.cxx+

    .L DetectorK/HistoManager.cxx+
    .L DetectorK/DetectorK.cxx+
    .L lutWrite.cc
    .L lutWrite.detector.cc
    .L lutWrite.tenv.cc
    printLutWriterConfiguration();

    TDatabasePDG::Instance()->AddParticle("deuteron", "deuteron", 1.8756134, kTRUE, 0.0, 3, "Nucleus", 1000010020);
    TDatabasePDG::Instance()->AddAntiParticle("anti-deuteron", -1000010020);

    TDatabasePDG::Instance()->AddParticle("triton", "triton", 2.8089218, kTRUE, 0.0, 3, "Nucleus", 1000010030);
    TDatabasePDG::Instance()->AddAntiParticle("anti-triton", -1000010030);

    TDatabasePDG::Instance()->AddParticle("helium3", "helium3", 2.80839160743, kTRUE, 0.0, 6, "Nucleus", 1000020030);
    TDatabasePDG::Instance()->AddAntiParticle("anti-helium3", -1000020030);

    TDatabasePDG::Instance()->AddParticle("helium4", "helium4", 3.727379378, kTRUE, 0.0, 6, "Nucleus", 1000020040);
    TDatabasePDG::Instance()->AddAntiParticle("anti-helium4", -1000020040);

    if (0) {
        lutWrite_detector("lutCovm.el.5kG.20cm.dat", 11, 50, 20);
    } else{
        lutWrite_tenv("lutCovm.el.20kG.20cm.dat", 11, 20, 20);
        lutWrite_tenv("lutCovm.mu.20kG.20cm.dat", 13, 20, 20);
        lutWrite_tenv("lutCovm.pi.20kG.20cm.dat", 211, 20, 20);
        lutWrite_tenv("lutCovm.ka.20kG.20cm.dat", 321, 20, 20);
        lutWrite_tenv("lutCovm.pr.20kG.20cm.dat", 2212, 20, 20);
        lutWrite_tenv("lutCovm.de.20kG.20cm.dat", 1000010020, 20, 20);
        lutWrite_tenv("lutCovm.tr.20kG.20cm.dat", 1000010030, 20, 20);
        lutWrite_tenv("lutCovm.he.20kG.20cm.dat", 1000020030, 20, 20);
        lutWrite_tenv("lutCovm.al.20kG.20cm.dat", 1000020040, 20, 20);
    }
EOF
