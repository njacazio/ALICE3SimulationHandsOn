// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.
//
//  *+-+*+-+*+-+*+-+*+-+*+-+*+-+*+-+*
//   Decay finder task for ALICE 3
//  *+-+*+-+*+-+*+-+*+-+*+-+*+-+*+-+*
//
//    Uses specific ALICE 3 PID and performance for studying
//    HF decays. Work in progress: use at your own risk!
//

#include "ALICE3/DataModel/OTFTracks.h"
#include "ALICE3/DataModel/A3DecayFinderTables.h"
#include "ALICE3/DataModel/OTFPIDTrk.h"
#include "ALICE3/DataModel/OTFRICH.h"
#include "ALICE3/DataModel/OTFTOF.h"
#include "ALICE3/DataModel/RICH.h"
#include "Common/Core/RecoDecay.h"
#include "Common/Core/TrackSelection.h"
#include "Common/Core/trackUtilities.h"
#include "Common/DataModel/TrackSelectionTables.h"

#include "CCDB/BasicCCDBManager.h"
#include "DCAFitter/DCAFitterN.h"
#include "DataFormatsCalibration/MeanVertexObject.h"
#include "DataFormatsParameters/GRPMagField.h"
#include "DataFormatsParameters/GRPObject.h"
#include "DetectorsBase/GeometryManager.h"
#include "DetectorsBase/Propagator.h"
#include "Framework/ASoAHelpers.h"
#include "Framework/AnalysisDataModel.h"
#include "Framework/AnalysisTask.h"
#include "Framework/RunningWorkflowInfo.h"
#include "Framework/runDataProcessing.h"
#include "ReconstructionDataFormats/Track.h"

#include <array>
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

using alice3tracks = soa::Join<aod::Tracks, aod::TracksCov, aod::McTrackLabels, aod::Alice3DecayMaps, aod::TracksDCA, aod::OTFLUTConfigId>;

struct alice3task
{
  static constexpr float toMicrometers = 1e+4; // from cm to µm
  static constexpr float magneticField = 20.f; // kG
  HistogramRegistry histos{"histos", {}, OutputObjHandlingPolicy::AnalysisObject};

  ConfigurableAxis axisEta{"axisEta", {80, -4.0f, +4.0f}, "#eta"};
  ConfigurableAxis axisDCA{"axisDCA", {400, 0, 400}, "DCA (#mum)"};
  ConfigurableAxis axisNSigma{"axisNSigma", {21, -10, 10}, "nsigma"};
  ConfigurableAxis axisDcaDaughters{"axisDcaDaughters", {200, 0, 100}, "DCA (mum)"};
  ConfigurableAxis axisDMass{"axisDMass", {200, 1.765f, 1.1965f}, "D Inv Mass (GeV/c^{2})"};
  ConfigurableAxis axisPt{"axisPt", {VARIABLE_WIDTH, 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f, 1.7f, 1.8f, 1.9f, 2.0f, 2.2f, 2.4f, 2.6f, 2.8f, 3.0f, 3.2f, 3.4f, 3.6f, 3.8f, 4.0f, 4.4f, 4.8f, 5.2f, 5.6f, 6.0f, 6.5f, 7.0f, 7.5f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 17.0f, 19.0f, 21.0f, 23.0f, 25.0f, 30.0f, 35.0f, 40.0f, 50.0f}, "pt axis for QA histograms"};
  

  // Filter on different alice 3 geometeries
  Configurable<int> otfConfig{"otfConfig", 0, "OTF configuration flag"};
  Configurable<bool> mcSameMotherCheck{"mcSameMotherCheck", false, "Check if pos and neg track share same mother"};
  Configurable<float> maxDauDCA{"maxDauDCA", 9999, "Maximum DCA between the daughters (cm)"};
  Configurable<float> minDCAxy{"minDCAxy", -1, "Minimum constant DCAxy for the daughters (cm)"};
  Configurable<float> minDCAz{"minDCAz", -1, "Minimum constant DCAz for the daughters (cm)"};


  SliceCache cache; // For grouping partitions with collisions
  Partition<alice3tracks> positiveTracks = (
    (aod::a3DecayMap::decayMap & trackSelectionPiPlusFromD) == trackSelectionPiPlusFromD &&
    nabs(aod::track::dcaXY) > minDCAxy &&
    nabs(aod::track::dcaZ) > minDCAz &&
    aod::otftracks::lutConfigId == otfConfig &&
    aod::track::signed1Pt > 0.0f);
    
  Partition<alice3tracks> negativeTracks = (
    (aod::a3DecayMap::decayMap & trackSelectionKaMinusFromD) == trackSelectionKaMinusFromD &&
    nabs(aod::track::dcaXY) > minDCAxy &&
    nabs(aod::track::dcaZ) > minDCAz &&
    aod::otftracks::lutConfigId == otfConfig &&
    aod::track::signed1Pt < 0.0f);

  // filter expressions for D mesons
  static constexpr uint32_t trackSelectionPiPlusFromD = 1 << kInnerTOFPion | 1 << kOuterTOFPion | 1 << kRICHPion | 1 << kTruePiPlusFromD;
  static constexpr uint32_t trackSelectionKaMinusFromD = 1 << kInnerTOFKaon | 1 << kOuterTOFKaon | 1 << kRICHKaon | 1 << kTrueKaMinusFromD;
  Partition<aod::McParticles> trueD = (aod::mcparticle::pdgCode == static_cast<int>(o2::constants::physics::kD0));


  // For candidate building
  o2::vertexing::DCAFitterN<2> fitter;

  // Helper struct to pass candidate information
  struct {
    float dca;
    float mass;
    float pt;
    float eta;
    std::array<float, 3> xyz;
    std::array<float, 3> prong0mom;
    std::array<float, 3> prong1mom;
  } dmeson;

  template <typename TTrackType>
  bool buildDecayCandidateTwoBody(TTrackType const& track0, TTrackType const& track1, float mass0, float mass1)
  {
    o2::track::TrackParCov t0 = getTrackParCov(track0);
    o2::track::TrackParCov t1 = getTrackParCov(track1);

    //}-{}-{}-{}-{}-{}-{}-{}-{}-{}
    // Move close to minima
    int nCand = 0;
    try {
      nCand = fitter.process(t0, t1);
    } catch (...) {
      return false;
    }
    if (nCand == 0) {
      return false;
    }
    //}-{}-{}-{}-{}-{}-{}-{}-{}-{}

    t0 = fitter.getTrack(0);
    t1 = fitter.getTrack(1);
    t0.getPxPyPzGlo(dmeson.prong0mom);
    t1.getPxPyPzGlo(dmeson.prong1mom);

    // get decay vertex coordinates
    const auto& vtx = fitter.getPCACandidate();
    for (int i = 0; i < 3; i++) {
      dmeson.xyz[i] = vtx[i];
    }

    // set relevant values
    dmeson.dca = std::sqrt(fitter.getChi2AtPCACandidate());
    dmeson.mass = RecoDecay::m(
      std::array{
        std::array{dmeson.prong0mom[0], dmeson.prong0mom[1], dmeson.prong0mom[2]},
        std::array{dmeson.prong1mom[0], dmeson.prong1mom[1], dmeson.prong1mom[2]}},
      std::array{mass0, mass1});
    dmeson.pt = std::hypot(dmeson.prong0mom[0] + dmeson.prong1mom[0], dmeson.prong0mom[1] + dmeson.prong1mom[1]);
    dmeson.eta = RecoDecay::eta(std::array{
      dmeson.prong0mom[0] + dmeson.prong1mom[0],
      dmeson.prong0mom[1] + dmeson.prong1mom[1],
      dmeson.prong0mom[2] + dmeson.prong1mom[2]});
    return true;
  }

  /// function to check if tracks have the same mother in MC
  template <typename TTrackType>
  bool checkSameMother(TTrackType const& track1, TTrackType const& track2)
  {
    if (track1.has_mcParticle() && track2.has_mcParticle()) {
      auto mcParticle1 = track1.template mcParticle_as<aod::McParticles>();
      auto mcParticle2 = track2.template mcParticle_as<aod::McParticles>();
      if (mcParticle1.has_mothers() && mcParticle2.has_mothers()) {
        for (auto& mcParticleMother1 : mcParticle1.template mothers_as<aod::McParticles>()) {
          for (auto& mcParticleMother2 : mcParticle2.template mothers_as<aod::McParticles>()) {
            if (mcParticleMother1.globalIndex() == mcParticleMother2.globalIndex()) {
              return true;
            }
          }
        }
      }
    } // end association check
    return false;
  }


  void init(InitContext&)
  {
    fitter.setPropagateToPCA(true);
    fitter.setMaxR(200.);
    fitter.setMinParamChange(1e-3);
    fitter.setMinRelChi2Change(0.9);
    fitter.setMaxDZIni(1e9);
    fitter.setMaxChi2(1e9);
    fitter.setUseAbsDCA(true);
    fitter.setWeightedFinalPCA(false);
    fitter.setBz(magneticField);
    fitter.setMatCorrType(o2::base::Propagator::MatCorrType::USEMatCorrNONE);


    // Add histograms here to the registry
    histos.add("hGeneratedD", "hGeneratedD", kTH1D, {axisPt});

    histos.add("hMassD", "hMassD", kTH1D, {axisDMass});
    histos.add("hDauDCA", "hDauDCA", kTH1D, {axisDcaDaughters});
    histos.add("hDCAxy", "hDCAxy", kTH1D, {axisDCA});
    histos.add("hDCAz", "hDCAz", kTH1D, {axisDCA});
  }

  void processGenerated(aod::McParticles const&)
  {
    for (const auto& mcParticle : trueD) {
      histos.fill(HIST("hGeneratedD"), mcParticle.pt());
    }
  }

  void process(aod::Collision const& collision, alice3tracks const&, aod::McParticles const&)
  {
    auto positiveTracksGrouped = positiveTracks->sliceByCached(aod::track::collisionId, collision.globalIndex(), cache);
    auto negativeTracksGrouped = negativeTracks->sliceByCached(aod::track::collisionId, collision.globalIndex(), cache);

    for (const auto& posTrack : positiveTracksGrouped) {
      for (const auto& negTrack : negativeTracksGrouped) {
        if (mcSameMotherCheck && !checkSameMother(posTrack, negTrack)) {
          continue; // Asked for MC association but pos and neg track does not share mother
        }
        if (!buildDecayCandidateTwoBody(posTrack, negTrack, o2::constants::physics::MassPionCharged, o2::constants::physics::MassKaonCharged)) {
          continue; // failed to build candidate
        }

        if (dmeson.dca > maxDauDCA) {
          continue;
        }

        histos.fill(HIST("hMassD"), dmeson.mass);
        histos.fill(HIST("hDauDCA"), dmeson.dca * toMicrometers);
      }
    }
  }

  PROCESS_SWITCH(alice3task, process, "find D mesons", true);
  PROCESS_SWITCH(alice3task, processGenerated, "Process generated", true);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<alice3task>(cfgc)};
}
