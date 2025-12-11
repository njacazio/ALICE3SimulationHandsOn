// alice3_simplified_with_staves.C
// ROOT macro: simplified ALICE3 geometry with staves

#include <Framework/Logger.h>

#include <TGeoManager.h>
#include <TGeoMaterial.h>
#include <TGeoMedium.h>
#include <TGeoTube.h>
#include <TGeoBBox.h>
#include <TGeoVolume.h>
#include <TGeoMatrix.h>
#include <TSystem.h>

#include <string>
#include <vector>
#include <cmath>

// ---------------------------------------------
// Small helper struct for cylindrical layers with staves
struct StaveLayer {
  std::string name;
  std::string medium;
  double radius;      // [cm] - radius at which staves are placed
  int nStaves;        // number of staves around the circle
  double staveWidth;  // [cm] - width of each stave
  double staveThick;  // [cm] - thickness of each stave
  double staveLength; // [cm] - half-length in z
  int color;          // ROOT color index
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
// Build a layer with discrete staves around a cylinder
void buildStaveLayer(TGeoVolume* mother, const StaveLayer& layer)
{
  if (!mother) {
    LOGF(error, "Mother volume is null");
    return;
  }

  if (layer.nStaves <= 0 || layer.radius <= 0. || layer.staveLength <= 0.) {
    LOGF(error, "Bad dimensions for stave layer %s", layer.name.c_str());
    return;
  }

  TGeoMedium* med = getOrCreateMedium(layer.medium);
  if (!med) {
    LOGF(error, "No medium for layer %s", layer.name.c_str());
    return;
  }

  // Create a box shape for a single stave
  auto* staveShape = new TGeoBBox(layer.staveWidth / 2.0, 
                                   layer.staveThick / 2.0, 
                                   layer.staveLength);
  auto* staveVol = new TGeoVolume((layer.name + "_stave").c_str(), staveShape, med);

  if (layer.color > 0) {
    staveVol->SetLineColor(layer.color);
    staveVol->SetFillColor(layer.color);
    staveVol->SetTransparency(20);
  }

  // Place staves around the cylinder
  double dphi = 360.0 / layer.nStaves; // angular spacing in degrees

  for (int i = 0; i < layer.nStaves; ++i) {
    double phi = i * dphi; // angle in degrees
    double phiRad = phi * TMath::Pi() / 180.0;

    // Position: place stave tangent to the cylinder
    double x = layer.radius * cos(phiRad);
    double y = layer.radius * sin(phiRad);
    double z = 0.0;

    // Rotation: align stave with long face pointing toward primary vertex
    // Add 90 degrees to make the width direction point radially
    auto* rot = new TGeoRotation();
    rot->RotateZ(phi + 90.0);

    auto* trans = new TGeoCombiTrans(x, y, z, rot);
    mother->AddNode(staveVol, i + 1, trans);
  }
}

// ---------------------------------------------
// Build all stave layers
void buildStaveAlice3(TGeoVolume* mother, const std::vector<StaveLayer>& layers)
{
  if (!mother) {
    LOGF(error, "Mother volume is null");
    return;
  }

  for (const auto& l : layers) {
    buildStaveLayer(mother, l);
  }
}

// ---------------------------------------------
// Main macro function
void alice3_simplified_with_staves()
{
  gSystem->Load("libGeom");

  // Create geometry manager
  new TGeoManager("ALICE3_Staves", "Simplified ALICE3 geometry with staves");

  // World volume (cave)
  auto* matVac = new TGeoMaterial("VACUUM", 0, 0, 0);
  auto* medVac = new TGeoMedium("VACUUM", 1, matVac);
  auto* top = gGeoManager->MakeBox("CAVE", medVac, 1000., 1000., 1000.);
  gGeoManager->SetTopVolume(top);

  // Define stave layers for inner tracker
  // Layer 0: inner layer with more staves, smaller radius
  // Layer 1: middle layer 
  // Layer 2: outer layer
  std::vector<StaveLayer> staveLayers;
  
  staveLayers.push_back(StaveLayer{
    "AL3_ITS3_L0",      // name
    "ITS3_SILICON",     // medium
    1.9,                // radius [cm]
    12,                 // number of staves
    0.3,                // stave width [cm]
    0.05,               // stave thickness [cm]
    30.0,               // stave half-length [cm]
    2                   // color (red)
  });

  staveLayers.push_back(StaveLayer{
    "AL3_ITS3_L1",      // name
    "ITS3_SILICON",     // medium
    2.7,                // radius [cm]
    16,                 // number of staves
    0.4,                // stave width [cm]
    0.05,               // stave thickness [cm]
    30.0,               // stave half-length [cm]
    3                   // color (green)
  });

  staveLayers.push_back(StaveLayer{
    "AL3_ITS3_L2",      // name
    "ITS3_SILICON",     // medium
    3.5,                // radius [cm]
    20,                 // number of staves
    0.5,                // stave width [cm]
    0.05,               // stave thickness [cm]
    30.0,               // stave half-length [cm]
    4                   // color (blue)
  });

  // Build the stave structure
  buildStaveAlice3(top, staveLayers);

  // Optionally add some continuous cylindrical volumes for TPC, EMCAL
  TGeoMedium* medTPC = getOrCreateMedium("TPC_GAS");
  if (medTPC) {
    auto* tpcShape = new TGeoTube(80.0, 250.0, 250.0);
    auto* tpcVol = new TGeoVolume("AL3_TPC", tpcShape, medTPC);
    tpcVol->SetLineColor(6);
    tpcVol->SetTransparency(80);
    top->AddNode(tpcVol, 1, nullptr);
  }

  TGeoMedium* medEMCAL = getOrCreateMedium("EMCAL_MIX");
  if (medEMCAL) {
    auto* emcalShape = new TGeoTube(260.0, 280.0, 260.0);
    auto* emcalVol = new TGeoVolume("AL3_EMCAL", emcalShape, medEMCAL);
    emcalVol->SetLineColor(7);
    emcalVol->SetTransparency(80);
    top->AddNode(emcalVol, 1, nullptr);
  }

  gGeoManager->CloseGeometry();

  // Draw with OpenGL viewer
  top->Draw("ogl");
}
