#include "lutread.C"

void style()
{

  gStyle->SetPadColor(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasBorderSize(0);
  gStyle->SetFrameBorderMode(0);
  // gStyle->SetFrameLineColor(0);
  gStyle->SetFrameFillColor(0);

  // gStyle->SetOptStat(00000);
  // gStyle->SetTitleColor(0);
  gStyle->SetTitleBorderSize(0);
  //  gStyle->SetTitleTextColor(0);
  //  gStyle->SetTitleFillColor(0);

  /*
    gStyle->SetTitleColor(0);
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleTextColor(0);
    gStyle->SetTitleFillColor(0);
  */
  //  gStyle->SetPalette(1);
  gStyle->SetCanvasColor(0);
  gStyle->SetHistFillColor(0);
  gStyle->SetHistFillStyle(0);
  gStyle->SetOptStat(0);
  //  gStyle->SetPadTickX(1);
  //  gStyle->SetPadTickY(1);
  gStyle->SetAxisColor(1, "X");
  gStyle->SetAxisColor(1, "Y");
  gStyle->SetAxisColor(1, "Z");
  /*
    gStyle->SetLabelColor(0, "X");
    gStyle->SetLabelColor(0, "Y");
    gStyle->SetLabelColor(0, "Z");
    gStyle->SetTickLength(0.0, "X");
    gStyle->SetTickLength(0.0, "Y");
    gStyle->SetTickLength(0.0, "Z");
  */
  gStyle->SetTitleXSize(0.05);
  gStyle->SetTitleYSize(0.05);
  gStyle->SetNdivisions(506, "X");
  gStyle->SetNdivisions(506, "Y");
  gStyle->SetNdivisions(506, "Z");

  // gStyle->SetPadGridX(1);
  // gStyle->SetPadGridY(1);

  // gStyle->SetLabelOffset(0.02, "X");
  // gStyle->SetLabelOffset(0.01, "Y");
  // gStyle->SetLabelOffset(0.02, "Z");
  gStyle->SetLabelSize(0.04, "xyz");
  gStyle->SetTitleOffset(1.2, "xyz");
  gStyle->SetTitleFont(42, "xyz");

  gStyle->SetTextFont(42);
  gStyle->SetTextSize(0.06);

  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadTopMargin(0.05);

  gROOT->ForceStyle();
}

void SetGraphStyle(TGraph* g, Int_t m, Int_t c)
{
  g->SetMarkerStyle(m);
  g->SetMarkerColor(c);
  g->SetLineColor(c);
  g->SetLineWidth(1);
  g->SetFillStyle(0);
  g->SetFillColor(0);
  g->SetMarkerSize(2.0);
  if (m == 28 || m == 34 || m == 23 || m == 32 || m == 22)
    g->SetMarkerSize(2.5);
  if (m == 27 || m == 33 || m == 30 || m == 29)
    g->SetMarkerSize(3.0);
}

void SetHistoStyle(TH1* h, Int_t m, Int_t c, Int_t w = 1, Int_t s = 1)
{
  h->SetMarkerStyle(m);
  h->SetMarkerColor(c);
  h->SetLineColor(c);
  h->SetLineWidth(w);
  h->SetLineStyle(s);
  h->SetFillStyle(0);
  h->SetFillColor(0);
  h->SetMarkerSize(2.0);
  if (m == 28 || m == 34 || m == 23 || m == 32 || m == 22)
    h->SetMarkerSize(2.5);
  if (m == 27 || m == 33 || m == 30 || m == 29)
    h->SetMarkerSize(3.0);
  if (m == 0)
    h->SetMarkerSize(0);
}

void lutdraw()
{

  style();

  std::vector<std::string> name = {"el", "pi", "ka", "pr"};
  //  std::vector<std::string> title = {"electron", "pion", "kaon", "proton"};
  std::vector<std::string> title = {"e", "#pi", "K", "p"};

  TLatex latex;
  latex.SetTextAlign(33);

  auto cc = new TCanvas("cc", "cc", 800, 800);
  cc->Divide(2, 2);

  for (int i = 0; i < 4; ++i) {

    auto c = new TCanvas((std::string("c") + name[i]).c_str(),
                         (std::string("c") + name[i]).c_str(),
                         800, 800);
    c->SetLogx();
    c->SetLogy();
    c->DrawFrame(1.e-2, 1., 100., 100., ";#it{p}_{T} (GeV/#it{c});momentum resolution (%)");
    latex.DrawLatexNDC(0.9, 0.9, title[i].c_str());

    auto g1 = lutread((std::string("lutCovm.") + name[i] + std::string(".5kG.20cm.scenario1.dat")).c_str());
    g1->SetLineColor(kRed + 1);
    g1->SetLineStyle(kSolid);
    g1->SetLineWidth(3);
    g1->Draw("samel");

    auto g2 = lutread((std::string("lutCovm.") + name[i] + std::string(".5kG.20cm.scenario2.dat")).c_str());
    g2->SetLineColor(kRed + 1);
    g2->SetLineStyle(kDashed);
    g2->SetLineWidth(3);
    g2->Draw("samel");

    auto g3 = lutread((std::string("lutCovm.") + name[i] + std::string(".5kG.20cm.scenario3.dat")).c_str());
    g3->SetLineColor(kAzure - 3);
    g3->SetLineStyle(kSolid);
    g3->SetLineWidth(3);
    g3->Draw("samel");

    auto g4 = lutread((std::string("lutCovm.") + name[i] + std::string(".5kG.20cm.scenario4.dat")).c_str());
    g4->SetLineColor(kAzure - 3);
    g4->SetLineStyle(kDashed);
    g4->SetLineWidth(3);
    g4->Draw("samel");

    c->SaveAs((std::string("draw_pt_scenarios.") + name[i] + std::string(".png")).c_str());

    cc->cd(i + 1);
    c->DrawClonePad();
  }

  cc->SaveAs("draw_pt_scenarios.png");
}
