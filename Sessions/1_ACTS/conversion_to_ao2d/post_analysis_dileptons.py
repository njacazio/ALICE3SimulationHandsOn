#!/usr/bin/env python3


import ROOT


def main():
    f = ROOT.TFile("AnalysisResults.root", "READ")
    f.ls()
    d = f.Get("alice3-dilepton/Generated/Pair")
    d.ls()

    def get(dd):
        d.ls()
        dd = d.Get(dd)
        h = dd.Get("Mass")
        h.SetTitle(dd.GetName())
        h.GetXaxis().SetRangeUser(0.511, 5)
        return h

    ULS = get("ULS")
    LSpp = get("LSpp")
    LSnn = get("LSnn")

    c = []
    for i in enumerate([ULS, LSpp, LSnn]):
        c.append(ROOT.TCanvas(f"c{i[0]}", f"c{i[0]}", 800, 600))
        c[i[0]].cd()
        i[1].Draw()
        c[i[0]].Update()

    c.append(ROOT.TCanvas("c3", "c3", 800, 600))
    c[3].cd()
    sub = ULS.DrawCopy()
    sub.SetTitle("ULS - LSpp")
    norm_range = [2, 4]

    sub.Add(LSpp, -1 * LSpp.Integral(LSpp.FindBin(norm_range[0]), LSpp.FindBin(norm_range[1])) /
            LSpp.Integral(LSpp.FindBin(norm_range[0]), LSpp.FindBin(norm_range[1])))

    c[3].Update()
    input("Press Enter to continue")


main()
