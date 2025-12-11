// drawHits.C
// root -l -b -q 'drawHits.C("../5_FullSimulation/2_Simulation/SimulationResults/")'

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TMath.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TTreeReaderValue.h"

#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

void drawHitsTRK(const char* inputPath = "../5_FullSimulation/2_Simulation/SimulationResults/")
{
  // Create output directory for plots_hits_TRK
  std::filesystem::create_directory("plots_hits_TRK");

  // Set global style
  gStyle->SetOptStat(1111);
  gStyle->SetPalette(55);
  gStyle->SetNumberContours(100);

  // Open the hits file
  std::string hitsFile = std::string(inputPath) + "/o2sim_HitsTRK.root";
  TFile* file = TFile::Open(hitsFile.c_str(), "READ");
  if (!file || file->IsZombie()) {
    std::cerr << "Error: Cannot open file " << hitsFile << std::endl;
    return;
  }

  // Get the tree
  TTree* tree = (TTree*)file->Get("o2sim");
  if (!tree) {
    std::cerr << "Error: Cannot find tree o2sim in file" << std::endl;
    file->Close();
    return;
  }

  std::cout << "Processing " << tree->GetEntries() << " events..." << std::endl;

  // Create histograms
  TH1D* hHitsPerEvent = new TH1D("hHitsPerEvent", "Hits per Event;Number of Hits;Events", 200, 0, 100000);
  TH1D* hDetectorID = new TH1D("hDetectorID", "Detector ID Distribution;Detector ID;Hits", 100, 0, 50000);
  TH1D* hEnergy = new TH1D("hEnergy", "Energy Distribution;Energy [GeV];Hits", 100, 0, 5);
  TH1D* hTime = new TH1D("hTime", "Hit Time Distribution;Time [ns];Hits", 100, 0, 100);

  TH2F* hXY = new TH2F("hXY", "Hit Distribution X-Y;X [cm];Y [cm]", 200, -100, 100, 200, -100, 100);
  TH2F* hXZ = new TH2F("hXZ", "Hit Distribution X-Z;Z [cm];X [cm]", 400, -100, 100, 200, -100, 100);
  TH2F* hYZ = new TH2F("hYZ", "Hit Distribution Y-Z;Z [cm];Y [cm]", 400, -100, 100, 200, -100, 100);
  TH2F* hRZ = new TH2F("hRZ", "Hit Distribution R-Z;Z [cm];R [cm]", 400, -100, 100, 200, 0, 100);

  TH1D* hMomentum = new TH1D("hMomentum", "Momentum Distribution;|p| [GeV/c];Hits", 100, 0, 5);
  TH1D* hTrackID = new TH1D("hTrackID", "Track ID Distribution;Track ID;Hits", 100, 0, 100000);

  // Setup tree reader
  TTreeReader reader(tree);
  TTreeReaderArray<Float_t> x(reader, "TRKHit.mPos.fCoordinates.fX");
  TTreeReaderArray<Float_t> y(reader, "TRKHit.mPos.fCoordinates.fY");
  TTreeReaderArray<Float_t> z(reader, "TRKHit.mPos.fCoordinates.fZ");
  TTreeReaderArray<Float_t> time(reader, "TRKHit.mTime");
  TTreeReaderArray<Float_t> energy(reader, "TRKHit.mE");
  TTreeReaderArray<UShort_t> detID(reader, "TRKHit.mDetectorID");
  TTreeReaderArray<Int_t> trackID(reader, "TRKHit.mTrackID");
  TTreeReaderArray<Float_t> px(reader, "TRKHit.mMomentum.fCoordinates.fX");
  TTreeReaderArray<Float_t> py(reader, "TRKHit.mMomentum.fCoordinates.fY");
  TTreeReaderArray<Float_t> pz(reader, "TRKHit.mMomentum.fCoordinates.fZ");

  // Loop over events
  int eventCount = 0;
  while (reader.Next()) {
    int nHits = x.GetSize();
    hHitsPerEvent->Fill(nHits);

    // Loop over hits in this event
    for (int i = 0; i < nHits; i++) {
      float xPos = x[i];
      float yPos = y[i];
      float zPos = z[i];
      float r = TMath::Sqrt(xPos * xPos + yPos * yPos);
      float p = TMath::Sqrt(px[i] * px[i] + py[i] * py[i] + pz[i] * pz[i]);

      // Fill histograms
      hXY->Fill(xPos, yPos);
      hXZ->Fill(zPos, xPos);
      hYZ->Fill(zPos, yPos);
      hRZ->Fill(zPos, r);
      hDetectorID->Fill(detID[i]);
      hEnergy->Fill(energy[i]);
      hTime->Fill(time[i]);
      hMomentum->Fill(p);
      hTrackID->Fill(trackID[i]);
    }

    eventCount++;
  }

  std::cout << "Processed " << eventCount << " events" << std::endl;
  std::cout << "Total hits: " << hXY->GetEntries() << std::endl;

  // Create and save canvases

  // 1. Hits per event
  TCanvas* c1 = new TCanvas("c1", "Hits per Event", 800, 600);
  c1->SetLogy();
  hHitsPerEvent->SetLineColor(kBlue);
  hHitsPerEvent->SetLineWidth(2);
  hHitsPerEvent->Draw();
  c1->SaveAs("plots_hits_TRK/hits_per_event.png");
  c1->SaveAs("plots_hits_TRK/hits_per_event.pdf");

  // 2. XY distribution
  TCanvas* c2 = new TCanvas("c2", "XY Distribution", 800, 800);
  c2->SetLogz();
  hXY->Draw("colz");
  c2->SaveAs("plots_hits_TRK/hit_distribution_XY.png");
  c2->SaveAs("plots_hits_TRK/hit_distribution_XY.pdf");

  // 3. XZ distribution
  TCanvas* c3 = new TCanvas("c3", "XZ Distribution", 1200, 600);
  c3->SetLogz();
  hXZ->Draw("colz");
  c3->SaveAs("plots_hits_TRK/hit_distribution_XZ.png");
  c3->SaveAs("plots_hits_TRK/hit_distribution_XZ.pdf");

  // 4. YZ distribution
  TCanvas* c4 = new TCanvas("c4", "YZ Distribution", 1200, 600);
  c4->SetLogz();
  hYZ->Draw("colz");
  c4->SaveAs("plots_hits_TRK/hit_distribution_YZ.png");
  c4->SaveAs("plots_hits_TRK/hit_distribution_YZ.pdf");

  // 5. RZ distribution (cylindrical view)
  TCanvas* c5 = new TCanvas("c5", "RZ Distribution", 1200, 600);
  c5->SetLogz();
  hRZ->SetTitle("Hit Distribution R-Z (Cylindrical);Z [cm];R [cm]");
  hRZ->Draw("colz");
  c5->SaveAs("plots_hits_TRK/hit_distribution_RZ.png");
  c5->SaveAs("plots_hits_TRK/hit_distribution_RZ.pdf");

  // 6. Detector ID distribution
  TCanvas* c6 = new TCanvas("c6", "Detector ID", 800, 600);
  c6->SetLogy();
  hDetectorID->SetLineColor(kRed);
  hDetectorID->SetLineWidth(2);
  hDetectorID->Draw();
  c6->SaveAs("plots_hits_TRK/detector_id_distribution.png");
  c6->SaveAs("plots_hits_TRK/detector_id_distribution.pdf");

  // 7. Energy distribution
  TCanvas* c7 = new TCanvas("c7", "Energy Distribution", 800, 600);
  c7->SetLogy();
  hEnergy->SetLineColor(kGreen + 2);
  hEnergy->SetLineWidth(2);
  hEnergy->Draw();
  c7->SaveAs("plots_hits_TRK/energy_distribution.png");
  c7->SaveAs("plots_hits_TRK/energy_distribution.pdf");

  // 8. Momentum distribution
  TCanvas* c8 = new TCanvas("c8", "Momentum Distribution", 800, 600);
  c8->SetLogy();
  hMomentum->SetLineColor(kMagenta);
  hMomentum->SetLineWidth(2);
  hMomentum->Draw();
  c8->SaveAs("plots_hits_TRK/momentum_distribution.png");
  c8->SaveAs("plots_hits_TRK/momentum_distribution.pdf");

  // 9. Combined spatial distributions
  TCanvas* c9 = new TCanvas("c9", "Spatial Distributions", 1600, 1200);
  c9->Divide(2, 2);

  c9->cd(1);
  gPad->SetLogz();
  hXY->Draw("colz");

  c9->cd(2);
  gPad->SetLogz();
  hRZ->Draw("colz");

  c9->cd(3);
  gPad->SetLogz();
  hXZ->Draw("colz");

  c9->cd(4);
  gPad->SetLogz();
  hYZ->Draw("colz");

  c9->SaveAs("plots_hits_TRK/spatial_distributions_combined.png");
  c9->SaveAs("plots_hits_TRK/spatial_distributions_combined.pdf");

  // 10. Summary canvas
  TCanvas* c10 = new TCanvas("c10", "Hit Summary", 1600, 1200);
  c10->Divide(3, 2);

  c10->cd(1);
  gPad->SetLogy();
  hHitsPerEvent->Draw();

  c10->cd(2);
  gPad->SetLogy();
  hDetectorID->Draw();

  c10->cd(3);
  gPad->SetLogy();
  hEnergy->Draw();

  c10->cd(4);
  gPad->SetLogy();
  hMomentum->Draw();

  c10->cd(5);
  gPad->SetLogy();
  hTime->Draw();

  c10->cd(6);
  gPad->SetLogy();
  hTrackID->Draw();

  c10->SaveAs("plots_hits_TRK/hit_summary.png");
  c10->SaveAs("plots_hits_TRK/hit_summary.pdf");

  // Save histograms to ROOT file
  TFile* outFile = new TFile("plots_hits_TRK/hit_distributions.root", "RECREATE");
  hHitsPerEvent->Write();
  hDetectorID->Write();
  hEnergy->Write();
  hTime->Write();
  hXY->Write();
  hXZ->Write();
  hYZ->Write();
  hRZ->Write();
  hMomentum->Write();
  hTrackID->Write();
  outFile->Close();

  std::cout << "\nPlots saved in plots_hits_TRK/ directory:" << std::endl;
  std::cout << "  - hits_per_event.png/pdf" << std::endl;
  std::cout << "  - hit_distribution_XY/XZ/YZ/RZ.png/pdf" << std::endl;
  std::cout << "  - detector_id_distribution.png/pdf" << std::endl;
  std::cout << "  - energy_distribution.png/pdf" << std::endl;
  std::cout << "  - momentum_distribution.png/pdf" << std::endl;
  std::cout << "  - spatial_distributions_combined.png/pdf" << std::endl;
  std::cout << "  - hit_summary.png/pdf" << std::endl;
  std::cout << "  - hit_distributions.root (all histograms)" << std::endl;

  //   file->Close();
}
