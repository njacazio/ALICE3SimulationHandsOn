// Author: Andrea Sofia Triolo 2025

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TStyle.h"

#include <ROOT/RDF/RInterface.hxx>
#include <ROOT/RDataFrame.hxx>

using namespace ROOT;
using namespace ROOT::VecOps;

void saveCanvas(TH1* h, std::string title = "", std::string option = "", bool logz = true, bool logy = false, int width = 700, int height = 600)
{
  TCanvas* c = new TCanvas(title.c_str(), title.c_str(), width, height);
  h->Draw(option.c_str());
  if (logz)
    c->SetLogz();
  if (logy)
    c->SetLogy();
  c->SaveAs(Form("plots_digits/%s.pdf", title.c_str()));
  c->SaveAs(Form("plots_digits/%s.png", title.c_str()));
  h->SaveAs(Form("plots_digits/%s.root", title.c_str()));
}

static void slice_rows_P0L0(ROOT::RDF::RNode dfVDP0L0, int slice)
{

  auto hRowColVDP0L0 = dfVDP0L0.Histo2D({"hRowColVDL0", "VD P0 L0;TRKDigit.mCol;TRKDigit.mRow",
                                         5000, slice * 5000., (slice + 1) * 5000., 785, 0, 785},
                                        "selCol", "selRow");
  gStyle->SetPalette(55);
  saveCanvas(hRowColVDP0L0.GetPtr(),
             Form("digits_row_vs_col_VDP0L0_slice%d", slice), "colz", true,
             false, 1200, 800);
}

void drawDigitsTutorial()
{
  /*This macro reads the trkdigits.root digit file and produces simple maps for
   * a quarter of VD layer and an ML module*/

  std::filesystem::create_directory("plots_digits");

  ROOT::RDataFrame df("o2sim", "../5_FullSimulation/2_Simulation/SimulationResults/trkdigits.root"); /// reading the full file

  ////// defyning tables for detector parts of interest
  auto dfVDP0L0 = df.Define("mask", "(TRKDigit.mChipIndex == 0)")
                    .Define("selRow", "TRKDigit.mRow[mask]")
                    .Define("selCol", "TRKDigit.mCol[mask]")
                    .Define("selChip", "TRKDigit.mChipIndex[mask]")
                    .Define("selCharge", "TRKDigit.mCharge[mask]");
  //////////

  //_____________________________________ histograms

  // distibution of charge in VD P0 L0
  auto hCharge = dfVDP0L0.Histo1D({"hCharge", ";TRKDigit.mCharge;# Pixels", 100, 0, 2000}, "selCharge");
  gStyle->SetOptStat(1);
  saveCanvas(hCharge.GetPtr(), "charge_VDP0L0", "", false, false);

  // map row vs col in VD P0 L0
  auto hRowColVDP0L0 = dfVDP0L0.Histo2D({"hRowColVDP0L0", "VD P0 L0;TRKDigit.mCol;TRKDigit.mRow",
                                         50000, 0, 50000, 785, 0, 785},
                                        "selCol", "selRow");
  gStyle->SetPalette(55);
  gStyle->SetOptStat(0);
  saveCanvas(hRowColVDP0L0.GetPtr(), "digits_row_vs_col_VDP0L0", "colz", true,
             false, 1200, 800);

  // zoom in different column intervals
  /************************** */
  for (int i = 0; i < 10; i++) {
    slice_rows_P0L0(dfVDP0L0, i);
  }
  /************************** */

  /********** printing a ML module: stave 0 module 4 = chips from 36+(28 to 35) = 64 to 71 */
  gStyle->SetPalette(55);
  gStyle->SetOptStat(0);
  TCanvas* cMod4 = new TCanvas("cMod4", "cMod4", 2000, 800);
  cMod4->Divide(4, 2);

  std::vector<ROOT::RDF::RNode> dfModList;

  for (int chip = 64; chip <= 71; chip++) { //// creating a DF for each chip to separate them
    dfModList.push_back(df.Define("mask", "TRKDigit.mChipIndex == " + std::to_string(chip))
                          .Define("selRow", "TRKDigit.mRow[mask]")
                          .Define("selCol", "TRKDigit.mCol[mask]")
                          .Define("selChip", "TRKDigit.mChipIndex[mask]")
                          .Define("selCharge", "TRKDigit.mCharge[mask]"));
  }

  ///// manually setting the canvas int the right pads - not the best way but it was not working in a loop :)
  int chip = 65;
  cMod4->cd(1);
  auto hChip0 = dfModList[chip - 64].Histo2D({Form("chip%d", chip - 64),
                                              Form("Chip %d;Col;Row", chip),
                                              640, 0, 640, 470, 0, 470},
                                             "selCol", "selRow");
  hChip0->SetName(Form("chip%d", chip - 64));
  hChip0->Draw("colz");

  chip = 67;
  cMod4->cd(2);
  auto hChip1 = dfModList[chip - 64].Histo2D({Form("chip%d", chip - 64),
                                              Form("Chip %d;Col;Row", chip),
                                              640, 0, 640, 470, 0, 470},
                                             "selCol", "selRow");
  hChip1->SetName(Form("chip%d", chip - 64));
  hChip1->Draw("colz");

  chip = 69;
  cMod4->cd(3);
  auto hChip2 = dfModList[chip - 64].Histo2D({Form("chip%d", chip - 64),
                                              Form("Chip %d;Col;Row", chip),
                                              640, 0, 640, 470, 0, 470},
                                             "selCol", "selRow");
  hChip2->SetName(Form("chip%d", chip - 64));
  hChip2->Draw("colz");

  chip = 71;
  cMod4->cd(4);
  auto hChip3 = dfModList[chip - 64].Histo2D({Form("chip%d", chip - 64),
                                              Form("Chip %d;Col;Row", chip),
                                              640, 0, 640, 470, 0, 470},
                                             "selCol", "selRow");
  hChip3->SetName(Form("chip%d", chip - 64));
  hChip3->Draw("colz");

  chip = 64;
  cMod4->cd(5);
  auto hChip4 = dfModList[chip - 64].Histo2D({Form("chip%d", chip - 64),
                                              Form("Chip %d;Col;Row", chip),
                                              640, 0, 640, 470, 0, 470},
                                             "selCol", "selRow");
  hChip4->SetName(Form("chip%d", chip - 64));
  hChip4->SetMaximum(2);
  hChip4->Draw("colz");

  chip = 66;
  cMod4->cd(6);
  auto hChip5 = dfModList[chip - 64].Histo2D({Form("chip%d", chip - 64),
                                              Form("Chip %d;Col;Row", chip),
                                              640, 0, 640, 470, 0, 470},
                                             "selCol", "selRow");
  hChip5->SetName(Form("chip%d", chip - 64));
  hChip5->SetMaximum(2);
  hChip5->Draw("colz");

  chip = 68;
  cMod4->cd(7);
  auto hChip6 = dfModList[chip - 64].Histo2D({Form("chip%d", chip - 64),
                                              Form("Chip %d;Col;Row", chip),
                                              640, 0, 640, 470, 0, 470},
                                             "selCol", "selRow");
  hChip6->SetName(Form("chip%d", chip - 64));
  hChip6->Draw("colz");

  chip = 70;
  cMod4->cd(8);
  auto hChip7 = dfModList[chip - 64].Histo2D({Form("chip%d", chip - 64),
                                              Form("Chip %d;Col;Row", chip),
                                              640, 0, 640, 470, 0, 470},
                                             "selCol", "selRow");
  hChip7->SetName(Form("chip%d", chip - 64));
  hChip7->Draw("colz");

  cMod4->SaveAs("plots_digits/ML_modules.png");
  cMod4->SaveAs("plots_digits/ML_modules.pdf");
  cMod4->SaveAs("plots_digits/ML_modules.root");
}
