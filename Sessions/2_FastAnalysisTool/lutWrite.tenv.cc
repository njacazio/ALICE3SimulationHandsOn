#include "lutWrite.cc"

const std::string filename = "/home/njacazio/alice/O2Physics/ALICE3/macros/a3geo.ini";
void fatInit_tenv(float field = 0.5, float rmin = 100.)
{
  fat.SetBField(field);
  fat.SetdNdEtaCent(400.);

  TEnv env(filename.c_str());
  THashList* table = env.GetTable();
  std::vector<std::string> layers;
  for (int i = 0; i < table->GetEntries(); ++i) {
    const std::string key = table->At(i)->GetName();
    // key should contain exactly one dot
    if (key.find('.') == std::string::npos || key.find('.') != key.rfind('.')) {
      std::cout << "Key " << key << " does not contain exactly one dot" << std::endl;
      continue;
    }
    const std::string firstPart = key.substr(0, key.find('.'));
    if (std::find(layers.begin(), layers.end(), firstPart) == layers.end()) {
      layers.push_back(firstPart);
    }
  }
  env.Print();
  fat.AddLayer((char*)"vertex", 0.0, 0, 0); // dummy vertex for matrix calculation

  // Layers
  if (1) {
    for (const auto& layer : layers) {
      std::cout << " Reading layer " << layer << std::endl;

      auto getKey = [&layer, &env](const std::string& name, const bool required = true) {
        std::string key = layer + "." + name;
        if (!env.Defined(key.c_str())) {
          if (required) {
            std::cout << "Key " << key << " not defined in configuration file" << std::endl;
          }
          std::cout << "Key " << key << " not defined in configuration file, getting the default value" << std::endl;
        }
        std::cout << " Getting key " << key << " from configuration file" << std::endl;
        return key;
      };
      const float r = env.GetValue(getKey("r").c_str(), -1.0f);
      std::cout << " Layer " << layer << " has radius " << r << std::endl;
      const float z = env.GetValue(getKey("z").c_str(), -1.0f);
      const float x0 = env.GetValue(getKey("x0").c_str(), 0.0f);
      const float xrho = env.GetValue(getKey("xrho").c_str(), 0.0f);
      const float resRPhi = env.GetValue(getKey("resRPhi").c_str(), 0.0f);
      const float resZ = env.GetValue(getKey("resZ").c_str(), 0.0f);
      const float eff = env.GetValue(getKey("eff").c_str(), 0.0f);
      const int type = env.GetValue(getKey("type").c_str(), 0);
      const char* deadPhiRegions = env.GetValue(getKey("deadPhiRegions", false).c_str(), "");

      // void AddLayer(TString name, float r, float z, float x0, float xrho, float resRPhi = 0.0f, float resZ = 0.0f, float eff = 0.0f, int type = 0);
      std::cout << " Adding layer " << layer << " r=" << r << " z=" << z << " x0=" << x0 << " xrho=" << xrho << " resRPhi=" << resRPhi << " resZ=" << resZ << " eff=" << eff << " type=" << type << " deadPhiRegions=" << deadPhiRegions << std::endl;

      fat.AddLayer(layer.c_str(), r, x0, xrho, resRPhi, resZ, eff);
    }
  }
  fat.SetAtLeastHits(4);
  fat.SetAtLeastCorr(4);
  fat.SetAtLeastFake(0);
  //
  fat.SetMinRadTrack(rmin);
  //
  fat.PrintLayout();
}

void lutWrite_tenv(const char* filename = "lutCovm.dat", int pdg = 211, float field = 0.5, float rmin = 100.)
{

  // init FAT
  fatInit_tenv(field, rmin);
  // write
  lutWrite(filename, pdg, field);
}
