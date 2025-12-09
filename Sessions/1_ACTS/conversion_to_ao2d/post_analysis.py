#!/usr/bin/env python3


import ROOT


def main(pdg=211):
    f = ROOT.TFile("AnalysisResults.root", "READ")
    f.ls()
    d = f.Get("alice3-tracking-performance")
    d.ls()

    num = d.Get(f"ptDistribution_{pdg}")
    den = d.Get(f"particlePtDistribution_{pdg}")

    c1 = ROOT.TCanvas("c1", "c1", 800, 600)
    c1.Divide(3, 1)
    c1.cd(1)
    num.Draw()
    c1.cd(2)
    den.Draw()
    c1.cd(3)
    ROOT.gPad.DrawFrame(0, 0, 10, 2, "p_{T} (GeV/c);Efficiency;")
    ratio = num.DrawCopy("same")
    ratio.Divide(num, den, 1, 1, "B")
    c1.Update()
    input("Press Enter to exit...")


main()
