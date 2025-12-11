// alice3_simplified.C
// ROOT macro: simplified cylindrical ALICE3 geometry

#include <Framework/Logger.h>

#include <TGeoBBox.h>
#include <TGeoManager.h>
#include <TGeoMaterial.h>
#include <TGeoMatrix.h>
#include <TGeoMedium.h>
#include <TGeoTube.h>
#include <TGeoVolume.h>
#include <TSystem.h>

#include <cmath>
#include <string>
#include <vector>

// ---------------------------------------------
// Small helper struct for cylindrical layers
struct CylLayer {
  std::string name;
  std::string medium;
  double rMin;  // [cm]
  double rMax;  // [cm]
  double halfZ; // [cm]
  int color;    // ROOT color index
};

// ---------------------------------------------
// Simple medium helper: create if missing
TGeoMedium* getOrCreateMedium(const std::string& name)
{
  auto* geo = gGeoManager;
  if (!geo) {
    LOGF(error, "gGeoManager not initialised");
    return nullptr;
  }

  if (auto* med = geo->GetMedium(name.c_str())) {
    return med;
  }

  // Fallback: simple silicon-like medium
  auto* mat = new TGeoMaterial(("MAT_" + name).c_str(), 28.0855, 14., 2.33);
  auto* med = new TGeoMedium(name.c_str(), geo->GetListOfMedia()->GetSize() + 1, mat);
  return med;
}

// ---------------------------------------------
// Build a set of cylindrical layers into 'mother'
void buildSimplifiedAlice3(TGeoVolume* mother, const std::vector<CylLayer>& layers)
{
  if (!mother) {
    LOGF(error, "Mother volume is null");
    return;
  }

  for (const auto& l : layers) {
    if (l.rMin >= l.rMax || l.halfZ <= 0.) {
      LOGF(error, "Bad dimensions for layer %s", l.name.c_str());
      continue;
    }

    TGeoMedium* med = getOrCreateMedium(l.medium);
    if (!med) {
      LOGF(error, "No medium for layer %s", l.name.c_str());
      continue;
    }

    auto* shape = new TGeoTube(l.rMin, l.rMax, l.halfZ);
    auto* vol = new TGeoVolume(l.name.c_str(), shape, med);

    if (l.color > 0) {
      vol->SetLineColor(l.color);
      vol->SetFillColor(l.color);
      vol->SetFillStyle(0); // wireframe-style
    }

    mother->AddNode(vol, 1, nullptr);
  }
}

// ---------------------------------------------
// Main macro function
void alice3_simplified()
{
  gSystem->Load("libGeom");

  // Create geometry manager
  new TGeoManager("ALICE3", "Simplified ALICE3 cylindrical geometry");

  // World volume (cave)
  auto* matVac = new TGeoMaterial("VACUUM", 0, 0, 0);
  auto* medVac = new TGeoMedium("VACUUM", 1, matVac);
  auto* top = gGeoManager->MakeBox("CAVE", medVac, 1000., 1000., 1000.);
  gGeoManager->SetTopVolume(top);

  // Define some example layers (numbers are placeholders!)
  std::vector<CylLayer> layers;
  layers.push_back(CylLayer{"AL3_ITS3_L0", "ITS3_SILICON", 1.8, 2.0, 30.0, 2}); // inner tracker layer 0
  layers.push_back(CylLayer{"AL3_ITS3_L1", "ITS3_SILICON", 2.6, 2.8, 30.0, 3}); // inner tracker layer 1
  layers.push_back(CylLayer{"AL3_ITS3_L2", "ITS3_SILICON", 3.4, 3.6, 30.0, 4}); // inner tracker layer 2
  layers.push_back(CylLayer{"AL3_TPC", "TPC_GAS", 80.0, 250.0, 250.0, 6});      // big gas cylinder
  layers.push_back(CylLayer{"AL3_EMCAL", "EMCAL_MIX", 260.0, 280.0, 260.0, 7}); // calorimeter shell

  // Build them into the world
  buildSimplifiedAlice3(top, layers);

  gGeoManager->CloseGeometry();

  // Draw with OpenGL viewer
  top->Draw("ogl");
}
