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

#include "ALICE3/DataModel/A3DecayFinderTables.h"
#include "ALICE3/DataModel/OTFPIDTrk.h"
#include "ALICE3/DataModel/OTFRICH.h"
#include "ALICE3/DataModel/OTFTOF.h"
#include "ALICE3/DataModel/OTFTracks.h"
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

struct alice3task {

  // filter expressions for D mesons
  static constexpr uint32_t trackSelectionPiPlusFromD = 1 << kInnerTOFPion | 1 << kOuterTOFPion | 1 << kRICHPion | 1 << kTruePiPlusFromD;
  static constexpr uint32_t trackSelectionKaMinusFromD = 1 << kInnerTOFKaon | 1 << kOuterTOFKaon | 1 << kRICHKaon | 1 << kTrueKaMinusFromD;
  Partition<aod::McParticles> trueD = (aod::mcparticle::pdgCode == static_cast<int>(o2::constants::physics::kD0));

  SliceCache cache; // For grouping partitions with collisions
  Partition<alice3tracks> positiveTracks = (aod::track::signed1Pt > 0.0f);
  Partition<alice3tracks> negativeTracks = (aod::track::signed1Pt < 0.0f);

  void init(InitContext&)
  {
  }

  void process(aod::Collision const& collision, alice3tracks const&, aod::McParticles const&)
  {
    auto positiveTracksGrouped = positiveTracks->sliceByCached(aod::track::collisionId, collision.globalIndex(), cache);
    auto negativeTracksGrouped = negativeTracks->sliceByCached(aod::track::collisionId, collision.globalIndex(), cache);

    for (const auto& posTrack : positiveTracksGrouped) {
      for (const auto& negTrack : negativeTracksGrouped) {
        LOG(info) << "Found a positive track with pT = " << posTrack.pt() << " and a negative track with pT = " << negTrack.pt();
      }
    }
  }

  PROCESS_SWITCH(alice3task, process, "find D mesons", true);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<alice3task>(cfgc)};
}
