// alice3_simplified_pixels.C
// ROOT macro: simplified ALICE3 geometry with 5mm x 5mm pixels on silicon cylinders

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
  double rMin;    // [cm]
  double rMax;    // [cm]
  double halfZ;   // [cm]
  int color;      // ROOT color index
  bool pixelated; // if true, segment into 5mm x 5mm pixels
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
// Build cylindrical layers, with optional 5mm x 5mm pixel segmentation
void buildSimplifiedAlice3WithPixels(TGeoVolume* mother, const std::vector<CylLayer>& layers)
{
  if (!mother) {
    LOGF(error, "Mother volume is null");
    return;
  }

  constexpr double pixelPitch = 0.6;  // 6 mm = 0.6 cm (5mm pixel + 1mm border)
  constexpr double pixelSize = 0.5;   // 5 mm = 0.5 cm (active pixel size)
  constexpr double borderGap = 0.1;   // 1 mm = 0.1 cm (border/gap between pixels)

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

    // Create a mother tube to hold either the full layer or the pixels
    double rCenter = 0.5 * (l.rMin + l.rMax);
    double thickness = (l.rMax - l.rMin);

    auto* tubeShape = new TGeoTube(l.rMin, l.rMax, l.halfZ);
    auto* layerVol = new TGeoVolume(l.name.c_str(), tubeShape, med);

    if (l.color > 0) {
      layerVol->SetLineColor(l.color);
      layerVol->SetFillColor(l.color);
      layerVol->SetFillStyle(0); // wireframe
    }

    mother->AddNode(layerVol, 1, nullptr);

    if (!l.pixelated) {
      // Just a simple tube, no segmentation
      continue;
    }

    // ------------------------------------------------------------
    // Pixel segmentation: 5mm (0.5 cm) along phi-arc and z
    // ------------------------------------------------------------

    // Effective circumference at the layer center
    double circumference = 2.0 * M_PI * rCenter;

    // Number of pixels in phi and z (based on pitch, which includes border)
    int nPhi = static_cast<int>(std::floor(circumference / pixelPitch));
    int nZ = static_cast<int>(std::floor((2.0 * l.halfZ) / pixelPitch));

    if (nPhi <= 0 || nZ <= 0) {
      LOGF(warning, "Too few pixels for layer %s (nPhi=%d, nZ=%d) - skipping segmentation",
           l.name.c_str(), nPhi, nZ);
      continue;
    }

    double dPhi = 2.0 * M_PI / static_cast<double>(nPhi);

    // Pixel shape: thickness in radial direction, 5mm in phi and z
    auto* pixelShape = new TGeoBBox("PIX_SHAPE",
                                    thickness * 0.5,  // local X: radial (thickness)
                                    pixelSize * 0.5,  // local Y: along phi (5mm)
                                    pixelSize * 0.5); // local Z: along beam (5mm)

    // You can use the same medium as the layer (or define a specific "SENS" one)
    auto* pixelVol = new TGeoVolume((l.name + "_PIX").c_str(), pixelShape, med);
    pixelVol->SetLineColor(l.color > 0 ? l.color : kRed);
    pixelVol->SetFillColor(l.color > 0 ? l.color : kRed);
    pixelVol->SetFillStyle(1001); // solid

    int copyNo = 1;
    for (int iphi = 0; iphi < nPhi; ++iphi) {
      double phi = (iphi + 0.5) * dPhi; // center angle of this pixel [rad]
      double x = rCenter * std::cos(phi);
      double y = rCenter * std::sin(phi);

      // Rotation: align local X (thickness) with radial direction
      auto* rot = new TGeoRotation();
      rot->RotateZ(phi * 180.0 / M_PI);

      for (int iz = 0; iz < nZ; ++iz) {
        double z = -l.halfZ + (iz + 0.5) * pixelPitch; // center of pixel in z (with border spacing)

        auto* comb = new TGeoCombiTrans(x, y, z, rot);
        layerVol->AddNode(pixelVol, copyNo++, comb);
      }
    }

    LOGF(info, "Layer %s: r ~ %.2f cm, |z| < %.2f cm, nPhi = %d, nZ = %d, total pixels = %d",
         l.name.c_str(), rCenter, l.halfZ, nPhi, nZ, nPhi * nZ);
  }
}

// ---------------------------------------------
void alice3_simplified_pixels()
{
  gSystem->Load("libGeom");

  // Create geometry manager
  new TGeoManager("ALICE3_PIX", "Simplified ALICE3 with pixelated silicon");

  // World volume (cave)
  auto* matVac = new TGeoMaterial("VACUUM", 0, 0, 0);
  auto* medVac = new TGeoMedium("VACUUM", 1, matVac);
  auto* top = gGeoManager->MakeBox("CAVE", medVac, 1000., 1000., 1000.);
  gGeoManager->SetTopVolume(top);

  // Define some example layers
  // Silicon layers get pixelated = true; others (TPC, EMCAL) stay as tubes
  std::vector<CylLayer> layers = {
    {"AL3_ITS3_L0", "ITS3_SILICON", 1.8, 2.0, 30.0, kRed, true}, // pixelated
    {"AL3_ITS3_L1", "ITS3_SILICON", 2.6, 2.8, 30.0, kGreen, true},
    {"AL3_ITS3_L2", "ITS3_SILICON", 3.4, 3.6, 30.0, kBlue, true},
    {"AL3_TPC", "TPC_GAS", 80.0, 250.0, 250.0, kMagenta, false},
    {"AL3_EMCAL", "EMCAL_MIX", 260.0, 280.0, 260.0, kCyan, false}};

  // Build geometry with pixels
  buildSimplifiedAlice3WithPixels(top, layers);

  gGeoManager->CloseGeometry();

  // Draw with OpenGL viewer
  top->Draw("ogl");
}
