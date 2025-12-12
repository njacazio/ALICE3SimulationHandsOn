// connectHitsToParticles.C
// root -l -b -q 'connectHitsToParticles.C("../5_FullSimulation/2_Simulation/SimulationResults/")'

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
#include <map>
#include <set>
#include <string>
#include <vector>

void connectHitsToParticles(const char* inputPath = "../5_FullSimulation/2_Simulation/SimulationResults/")
{
  // Create output directory for plots_connect_hits_to_particles
  std::filesystem::create_directory("plots_connect_hits_to_particles");

  // Set global style
  gStyle->SetOptStat(1111);
  gStyle->SetPalette(55);
  gStyle->SetNumberContours(100);

  // Open the kinematics file
  std::string kineFile = std::string(inputPath) + "/o2sim_Kine.root";
  TFile* fileKine = TFile::Open(kineFile.c_str(), "READ");
  if (!fileKine || fileKine->IsZombie()) {
    std::cerr << "Error: Cannot open file " << kineFile << std::endl;
    return;
  }

  // Open the hits file
  std::string hitsFile = std::string(inputPath) + "/o2sim_HitsTRK.root";
  TFile* fileHits = TFile::Open(hitsFile.c_str(), "READ");
  if (!fileHits || fileHits->IsZombie()) {
    std::cerr << "Error: Cannot open file " << hitsFile << std::endl;
    fileKine->Close();
    return;
  }

  // Get the trees
  TTree* treeKine = (TTree*)fileKine->Get("o2sim");
  TTree* treeHits = (TTree*)fileHits->Get("o2sim");

  if (!treeKine || !treeHits) {
    std::cerr << "Error: Cannot find trees in files" << std::endl;
    fileKine->Close();
    fileHits->Close();
    return;
  }

  std::cout << "Processing " << treeKine->GetEntries() << " events..." << std::endl;

  // Create histograms for all electrons
  TH1D* hAllElectrons_Pt = new TH1D("hAllElectrons_Pt", "All Electrons p_{T};p_{T} [GeV/c];Counts", 100, 0, 5);
  TH1D* hAllElectrons_Eta = new TH1D("hAllElectrons_Eta", "All Electrons #eta;#eta;Counts", 100, -5, 5);
  TH1D* hAllElectrons_Phi = new TH1D("hAllElectrons_Phi", "All Electrons #phi;#phi [rad];Counts", 100, -TMath::Pi(), TMath::Pi());
  TH2F* hAllElectrons_XY = new TH2F("hAllElectrons_XY", "All Electrons Production Vertex X-Y;X [cm];Y [cm]", 200, -50, 50, 200, -50, 50);
  TH2F* hAllElectrons_RZ = new TH2F("hAllElectrons_RZ", "All Electrons Production Vertex R-Z;Z [cm];R [cm]", 400, -100, 100, 200, 0, 100);

  // Create histograms for electrons from conversions
  TH1D* hConvElectrons_Pt = new TH1D("hConvElectrons_Pt", "Conversion Electrons p_{T};p_{T} [GeV/c];Counts", 100, 0, 5);
  TH1D* hConvElectrons_Eta = new TH1D("hConvElectrons_Eta", "Conversion Electrons #eta;#eta;Counts", 100, -5, 5);
  TH1D* hConvElectrons_Phi = new TH1D("hConvElectrons_Phi", "Conversion Electrons #phi;#phi [rad];Counts", 100, -TMath::Pi(), TMath::Pi());
  TH2F* hConvElectrons_XY = new TH2F("hConvElectrons_XY", "Conversion Electrons Vertex X-Y;X [cm];Y [cm]", 200, -50, 50, 200, -50, 50);
  TH2F* hConvElectrons_RZ = new TH2F("hConvElectrons_RZ", "Conversion Electrons Vertex R-Z;Z [cm];R [cm]", 400, -100, 100, 200, 0, 100);
  TH1D* hConvRadius = new TH1D("hConvRadius", "Conversion Radius;R [cm];Conversions", 100, 0, 100);
  TH1D* hConvZ = new TH1D("hConvZ", "Conversion Z Position;Z [cm];Conversions", 100, -100, 100);

  // Histograms for hits from conversion electrons
  TH1D* hConvHits_Energy = new TH1D("hConvHits_Energy", "Hits from Conversion Electrons - Energy;Energy [GeV];Hits", 100, 0, 2);
  TH1D* hConvHits_Momentum = new TH1D("hConvHits_Momentum", "Hits from Conversion Electrons - Momentum;|p| [GeV/c];Hits", 100, 0, 5);
  TH2F* hConvHits_XY = new TH2F("hConvHits_XY", "Hits from Conversion Electrons X-Y;X [cm];Y [cm]", 200, -100, 100, 200, -100, 100);
  TH2F* hConvHits_RZ = new TH2F("hConvHits_RZ", "Hits from Conversion Electrons R-Z;Z [cm];R [cm]", 400, -100, 100, 200, 0, 100);
  TH1D* hConvHits_DetID = new TH1D("hConvHits_DetID", "Hits from Conversion Electrons - Detector ID;Detector ID;Hits", 100, 0, 50000);
  TH1D* hConvHits_PerTrack = new TH1D("hConvHits_PerTrack", "Hits per Conversion Electron;Number of Hits;Electrons", 50, 0, 500);

  // Photon histograms
  TH1D* hPhoton_Pt = new TH1D("hPhoton_Pt", "Photons that Convert p_{T};p_{T} [GeV/c];Photons", 100, 0, 5);
  TH1D* hPhoton_Eta = new TH1D("hPhoton_Eta", "Photons that Convert #eta;#eta;Photons", 100, -5, 5);

  // Setup tree readers
  TTreeReader readerKine(treeKine);
  TTreeReaderArray<Int_t> pdgCode(readerKine, "MCTrack.mPdgCode");
  TTreeReaderArray<Int_t> motherID(readerKine, "MCTrack.mMotherTrackId");
  TTreeReaderArray<Float_t> startX(readerKine, "MCTrack.mStartVertexCoordinatesX");
  TTreeReaderArray<Float_t> startY(readerKine, "MCTrack.mStartVertexCoordinatesY");
  TTreeReaderArray<Float_t> startZ(readerKine, "MCTrack.mStartVertexCoordinatesZ");
  TTreeReaderArray<Float_t> px(readerKine, "MCTrack.mStartVertexMomentumX");
  TTreeReaderArray<Float_t> py(readerKine, "MCTrack.mStartVertexMomentumY");
  TTreeReaderArray<Float_t> pz(readerKine, "MCTrack.mStartVertexMomentumZ");

  TTreeReader readerHits(treeHits);
  TTreeReaderArray<Int_t> hitTrackID(readerHits, "TRKHit.mTrackID");
  TTreeReaderArray<Float_t> hitX(readerHits, "TRKHit.mPos.fCoordinates.fX");
  TTreeReaderArray<Float_t> hitY(readerHits, "TRKHit.mPos.fCoordinates.fY");
  TTreeReaderArray<Float_t> hitZ(readerHits, "TRKHit.mPos.fCoordinates.fZ");
  TTreeReaderArray<Float_t> hitE(readerHits, "TRKHit.mE");
  TTreeReaderArray<Float_t> hitPx(readerHits, "TRKHit.mMomentum.fCoordinates.fX");
  TTreeReaderArray<Float_t> hitPy(readerHits, "TRKHit.mMomentum.fCoordinates.fY");
  TTreeReaderArray<Float_t> hitPz(readerHits, "TRKHit.mMomentum.fCoordinates.fZ");
  TTreeReaderArray<UShort_t> hitDetID(readerHits, "TRKHit.mDetectorID");

  int nConversionElectrons = 0;
  int nAllElectrons = 0;
  int nPhotonsConverted = 0;
  std::set<int> photonsThatConverted;

  // First pass: identify conversion electrons
  int eventID = 0;
  std::map<int, std::set<int>> conversionElectronsPerEvent; // event -> set of track IDs

  while (readerKine.Next()) {
    std::set<int> convElectronTracks;

    int nTracks = pdgCode.GetSize();

    for (int i = 0; i < nTracks; i++) {
      int pdg = pdgCode[i];

      // Check if it's an electron or positron
      if (TMath::Abs(pdg) == 11) {
        nAllElectrons++;

        float pT = TMath::Sqrt(px[i] * px[i] + py[i] * py[i]);
        float p = TMath::Sqrt(px[i] * px[i] + py[i] * py[i] + pz[i] * pz[i]);
        float eta = 0.5 * TMath::Log((p + pz[i]) / (p - pz[i]));
        float phi = TMath::ATan2(py[i], px[i]);
        float r = TMath::Sqrt(startX[i] * startX[i] + startY[i] * startY[i]);

        hAllElectrons_Pt->Fill(pT);
        hAllElectrons_Eta->Fill(eta);
        hAllElectrons_Phi->Fill(phi);
        hAllElectrons_XY->Fill(startX[i], startY[i]);
        hAllElectrons_RZ->Fill(startZ[i], r);

        // Check if mother is a photon
        int mother = motherID[i];
        if (mother >= 0 && mother < nTracks) {
          int motherPDG = pdgCode[mother];

          if (motherPDG == 22) { // Mother is a photon
            nConversionElectrons++;
            convElectronTracks.insert(i);

            hConvElectrons_Pt->Fill(pT);
            hConvElectrons_Eta->Fill(eta);
            hConvElectrons_Phi->Fill(phi);
            hConvElectrons_XY->Fill(startX[i], startY[i]);
            hConvElectrons_RZ->Fill(startZ[i], r);
            hConvRadius->Fill(r);
            hConvZ->Fill(startZ[i]);

            // Check if we've already counted this photon
            int globalMotherID = eventID * 1000000 + mother; // Create unique photon ID
            if (photonsThatConverted.find(globalMotherID) == photonsThatConverted.end()) {
              photonsThatConverted.insert(globalMotherID);
              nPhotonsConverted++;

              float photonPT = TMath::Sqrt(px[mother] * px[mother] + py[mother] * py[mother]);
              float photonP = TMath::Sqrt(px[mother] * px[mother] + py[mother] * py[mother] + pz[mother] * pz[mother]);
              float photonEta = 0.5 * TMath::Log((photonP + pz[mother]) / (photonP - pz[mother]));

              hPhoton_Pt->Fill(photonPT);
              hPhoton_Eta->Fill(photonEta);
            }
          }
        }
      }
    }

    conversionElectronsPerEvent[eventID] = convElectronTracks;
    eventID++;
  }

  std::cout << "\nParticle Statistics:" << std::endl;
  std::cout << "  Total electrons/positrons: " << nAllElectrons << std::endl;
  std::cout << "  Electrons from conversions: " << nConversionElectrons << std::endl;
  std::cout << "  Photons that converted: " << nPhotonsConverted << std::endl;

  // Second pass: process hits
  eventID = 0;
  std::map<int, int> hitsPerConvElectron;
  int totalConversionHits = 0;

  while (readerHits.Next()) {
    const auto& convElectrons = conversionElectronsPerEvent[eventID];

    int nHits = hitTrackID.GetSize();

    for (int i = 0; i < nHits; i++) {
      int trackID = hitTrackID[i];

      // Check if this hit comes from a conversion electron
      if (convElectrons.find(trackID) != convElectrons.end()) {
        totalConversionHits++;
        hitsPerConvElectron[trackID]++;

        float r = TMath::Sqrt(hitX[i] * hitX[i] + hitY[i] * hitY[i]);
        float p = TMath::Sqrt(hitPx[i] * hitPx[i] + hitPy[i] * hitPy[i] + hitPz[i] * hitPz[i]);

        hConvHits_Energy->Fill(hitE[i]);
        hConvHits_Momentum->Fill(p);
        hConvHits_XY->Fill(hitX[i], hitY[i]);
        hConvHits_RZ->Fill(hitZ[i], r);
        hConvHits_DetID->Fill(hitDetID[i]);
      }
    }

    eventID++;
  }

  // Fill hits per track histogram
  for (const auto& entry : hitsPerConvElectron) {
    hConvHits_PerTrack->Fill(entry.second);
  }

  std::cout << "\nHit Statistics:" << std::endl;
  std::cout << "  Total hits from conversion electrons: " << totalConversionHits << std::endl;
  std::cout << "  Average hits per conversion electron: "
            << (nConversionElectrons > 0 ? (float)totalConversionHits / nConversionElectrons : 0) << std::endl;

  // Create and save canvases

  // 1. Electron kinematics comparison
  TCanvas* c1 = new TCanvas("c1", "Electron Kinematics", 1800, 600);
  c1->Divide(3, 1);

  c1->cd(1);
  gPad->SetLogy();
  hAllElectrons_Pt->SetLineColor(kBlue);
  hAllElectrons_Pt->SetLineWidth(2);
  hConvElectrons_Pt->SetLineColor(kRed);
  hConvElectrons_Pt->SetLineWidth(2);
  hAllElectrons_Pt->Draw();
  hConvElectrons_Pt->Draw("same");
  TLegend* leg1 = new TLegend(0.6, 0.7, 0.88, 0.88);
  leg1->AddEntry(hAllElectrons_Pt, "All electrons", "l");
  leg1->AddEntry(hConvElectrons_Pt, "From conversions", "l");
  leg1->Draw();

  c1->cd(2);
  hAllElectrons_Eta->SetLineColor(kBlue);
  hAllElectrons_Eta->SetLineWidth(2);
  hConvElectrons_Eta->SetLineColor(kRed);
  hConvElectrons_Eta->SetLineWidth(2);
  hAllElectrons_Eta->Draw();
  hConvElectrons_Eta->Draw("same");

  c1->cd(3);
  hAllElectrons_Phi->SetLineColor(kBlue);
  hAllElectrons_Phi->SetLineWidth(2);
  hConvElectrons_Phi->SetLineColor(kRed);
  hConvElectrons_Phi->SetLineWidth(2);
  hAllElectrons_Phi->Draw();
  hConvElectrons_Phi->Draw("same");

  c1->SaveAs("plots_connect_hits_to_particles/electron_kinematics_comparison.png");
  c1->SaveAs("plots_connect_hits_to_particles/electron_kinematics_comparison.pdf");

  // 2. Conversion vertex positions
  TCanvas* c2 = new TCanvas("c2", "Conversion Vertices", 1600, 800);
  c2->Divide(2, 2);

  c2->cd(1);
  gPad->SetLogz();
  hConvElectrons_XY->Draw("colz");

  c2->cd(2);
  gPad->SetLogz();
  hConvElectrons_RZ->Draw("colz");

  c2->cd(3);
  hConvRadius->SetLineColor(kRed);
  hConvRadius->SetLineWidth(2);
  hConvRadius->Draw();

  c2->cd(4);
  hConvZ->SetLineColor(kRed);
  hConvZ->SetLineWidth(2);
  hConvZ->Draw();

  c2->SaveAs("plots_connect_hits_to_particles/conversion_vertices.png");
  c2->SaveAs("plots_connect_hits_to_particles/conversion_vertices.pdf");

  // 3. Conversion electron hits spatial distribution
  TCanvas* c3 = new TCanvas("c3", "Conversion Electron Hits", 1600, 800);
  c3->Divide(2, 1);

  c3->cd(1);
  gPad->SetLogz();
  hConvHits_XY->Draw("colz");

  c3->cd(2);
  gPad->SetLogz();
  hConvHits_RZ->Draw("colz");

  c3->SaveAs("plots_connect_hits_to_particles/conversion_hits_spatial.png");
  c3->SaveAs("plots_connect_hits_to_particles/conversion_hits_spatial.pdf");

  // 4. Conversion electron hit properties
  TCanvas* c4 = new TCanvas("c4", "Conversion Hit Properties", 1800, 600);
  c4->Divide(3, 1);

  c4->cd(1);
  gPad->SetLogy();
  hConvHits_Energy->SetLineColor(kGreen + 2);
  hConvHits_Energy->SetLineWidth(2);
  hConvHits_Energy->Draw();

  c4->cd(2);
  gPad->SetLogy();
  hConvHits_Momentum->SetLineColor(kMagenta);
  hConvHits_Momentum->SetLineWidth(2);
  hConvHits_Momentum->Draw();

  c4->cd(3);
  gPad->SetLogy();
  hConvHits_DetID->SetLineColor(kOrange);
  hConvHits_DetID->SetLineWidth(2);
  hConvHits_DetID->Draw();

  c4->SaveAs("plots_connect_hits_to_particles/conversion_hit_properties.png");
  c4->SaveAs("plots_connect_hits_to_particles/conversion_hit_properties.pdf");

  // 5. Photon properties
  TCanvas* c5 = new TCanvas("c5", "Converting Photons", 1200, 600);
  c5->Divide(2, 1);

  c5->cd(1);
  gPad->SetLogy();
  hPhoton_Pt->SetLineColor(kViolet);
  hPhoton_Pt->SetLineWidth(2);
  hPhoton_Pt->Draw();

  c5->cd(2);
  hPhoton_Eta->SetLineColor(kViolet);
  hPhoton_Eta->SetLineWidth(2);
  hPhoton_Eta->Draw();

  c5->SaveAs("plots_connect_hits_to_particles/converting_photons.png");
  c5->SaveAs("plots_connect_hits_to_particles/converting_photons.pdf");

  // 6. Hits per conversion electron
  TCanvas* c6 = new TCanvas("c6", "Hits per Conversion Electron", 800, 600);
  hConvHits_PerTrack->SetLineColor(kCyan + 2);
  hConvHits_PerTrack->SetLineWidth(2);
  hConvHits_PerTrack->Draw();
  c6->SaveAs("plots_connect_hits_to_particles/hits_per_conversion_electron.png");
  c6->SaveAs("plots_connect_hits_to_particles/hits_per_conversion_electron.pdf");

  // 7. Summary canvas
  TCanvas* c7 = new TCanvas("c7", "Conversion Summary", 1800, 1200);
  c7->Divide(3, 2);

  c7->cd(1);
  gPad->SetLogy();
  hConvElectrons_Pt->Draw();

  c7->cd(2);
  gPad->SetLogz();
  hConvElectrons_RZ->Draw("colz");

  c7->cd(3);
  hConvRadius->Draw();

  c7->cd(4);
  gPad->SetLogz();
  hConvHits_RZ->Draw("colz");

  c7->cd(5);
  gPad->SetLogy();
  hConvHits_Momentum->Draw();

  c7->cd(6);
  hConvHits_PerTrack->Draw();

  c7->SaveAs("plots_connect_hits_to_particles/conversion_summary.png");
  c7->SaveAs("plots_connect_hits_to_particles/conversion_summary.pdf");

  // Save all histograms to ROOT file
  TFile* outFile = new TFile("plots_connect_hits_to_particles/conversion_analysis.root", "RECREATE");

  hAllElectrons_Pt->Write();
  hAllElectrons_Eta->Write();
  hAllElectrons_Phi->Write();
  hAllElectrons_XY->Write();
  hAllElectrons_RZ->Write();

  hConvElectrons_Pt->Write();
  hConvElectrons_Eta->Write();
  hConvElectrons_Phi->Write();
  hConvElectrons_XY->Write();
  hConvElectrons_RZ->Write();
  hConvRadius->Write();
  hConvZ->Write();

  hConvHits_Energy->Write();
  hConvHits_Momentum->Write();
  hConvHits_XY->Write();
  hConvHits_RZ->Write();
  hConvHits_DetID->Write();
  hConvHits_PerTrack->Write();

  hPhoton_Pt->Write();
  hPhoton_Eta->Write();

  outFile->Close();

  std::cout << "\nPlots saved in plots_connect_hits_to_particles/ directory:" << std::endl;
  std::cout << "  - electron_kinematics_comparison.png/pdf" << std::endl;
  std::cout << "  - conversion_vertices.png/pdf" << std::endl;
  std::cout << "  - conversion_hits_spatial.png/pdf" << std::endl;
  std::cout << "  - conversion_hit_properties.png/pdf" << std::endl;
  std::cout << "  - converting_photons.png/pdf" << std::endl;
  std::cout << "  - hits_per_conversion_electron.png/pdf" << std::endl;
  std::cout << "  - conversion_summary.png/pdf" << std::endl;
  std::cout << "  - conversion_analysis.root (all histograms)" << std::endl;

  //   fileKine->Close();
  //   fileHits->Close();
}
