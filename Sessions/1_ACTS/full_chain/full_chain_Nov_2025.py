#!/usr/bin/env python3
from acts.examples.reconstruction import (
    addSeeding,
    # TruthSeedRanges,
    SeedFinderConfigArg,
    SeedFinderOptionsArg,
    SeedFilterConfigArg,
    SpacePointGridConfigArg,
    SeedingAlgorithmConfigArg,
    SeedingAlgorithm,
    # ParticleSmearingSigmas,
    addCKFTracks,
    addTruthTrackingGsf,
    #    CKFPerformanceConfig,
    TrackSelectorConfig,
    addKalmanTracks,
    addAmbiguityResolution,
    AmbiguityResolutionConfig,
    CkfConfig,
    addVertexFitting,
    VertexFinder,
    addSpacePointsMaking,  # May 2025
    addHoughVertexFinding,  # May 2025
)
from acts.examples.simulation import (
    addParticleGun,
    MomentumConfig,
    EtaConfig,
    PhiConfig,
    ParticleConfig,
    addPythia8,
    addGenParticleSelection,
    addSimParticleSelection,
    addFatras,
    addGeant4,
    ParticleSelectorConfig,
    addDigitization,
    addDigiParticleSelection,
)
import pathlib
import acts
import acts.examples
import alice3
import acts.examples.geant4


u = acts.UnitConstants

geo_dir = pathlib.Path.cwd()

import sys


def getArgumentParser():
    """Get arguments from command line"""
    parser = argparse.ArgumentParser(
        description="Command line arguments for full chain setup"
    )

    ##### General parameters
    parser.add_argument(
        "--nThreads", "-nthr", help="Number of threads", type=int, default=-1
    )
    parser.add_argument(
        "--nEv",
        "-n",
        dest="nEvents",
        help="nEvents",
        type=int,
        default=1,
    )
    parser.add_argument(
        "--field",
        "-MF",
        dest="MF",
        help="MF, Tesla",
        type=float,
        default=2.0,
    )
    parser.add_argument(
        "--useFieldMap",
        help="Use field map instead of Constant field",
        action="store_false",  # default is true
    )

    parser.add_argument(
        "--out_dir_prefix",
        dest="out_dir_prefix",
        help="out_dir_prefix",
        type=str,
        default="test",
    )

    # parser.add_argument(
    #     "-i",
    #     "--indir",
    #     dest="indir",
    #     help="Directory with input root files",
    #     default="./",
    # )
    # parser.add_argument(
    #     "-o",
    #     "--output",
    #     dest="outdir",
    #     help="Output directory for new ntuples",
    #     default="./",
    # )

    ##### SIM: Particle simulation - particle gun

    parser.add_argument(
        "--gunMult",
        "-gunN",
        dest="gunMult",
        help="Particle gun multiplicity",
        type=int,
        default=1,
    )
    parser.add_argument(
        "--gunPtRange",
        dest="gunPtRange",
        help="Particle gun pt min, max, GeV/c",
        nargs=2,
        metavar=("ptMin", "ptMax"),
        type=float,
        default=(0.1, 5.0),
    )
    parser.add_argument(
        "--gunEtaRange",
        dest="gunEtaRange",
        help="Particle gun eta min, max",
        nargs=2,
        metavar=("etaMin", "etaMax"),
        type=float,
        default=(-2.5, 2.5),
    )

    parser.add_argument(
        "--gunPID",
        help="Particle type for a particle gun",
        type=int,
        default=1,
    )

    parser.add_argument(
        "--gunRandCharge",
        help="Randomize charge for a particle gun",
        action="store_true",  # default is false
    )

    ##### SIM: Particle simulation - PYTHIA
    parser.add_argument(
        "--usePythia",
        help="Use Pythia8 instead of particle gun",
        action="store_true",  # default is false
    )
    parser.add_argument(
        "--system",
        type=str,
        choices=["pp", "PbPbMB", "PbPbCentral"],
        required=False,
        default="pp",
        help="Choose colliding system for Pythia8",
    )

    parser.add_argument(
        "--pileup",
        dest="pileup",
        help="pileup",
        type=int,
        default=1,
    )

    parser.add_argument(
        "--sigmaVz",
        dest="sigmaVz",
        help="sigmaVz for collisions, mm",
        type=float,
        default=50,
    )

    ##### SIM: Detector response sim params
    parser.add_argument(
        "--detSim",
        type=str,
        choices=["Fatras", "Geant4"],
        required=False,
        default="Fatras",
        help="Choose which tool we use for detector simulation response",
    )

    parser.add_argument(
        "--minFatrasPt",
        dest="minFatrasPt",
        help="min pT of particles from the Fatras sim, GeV/c",
        type=float,
        default=0.001,
    )
    parser.add_argument(
        "--particleSelectionEta",
        dest="particleSelectionEta",
        help="particles within this |eta| range are considered; should be wide enough to have realistic hit occupancies in wide collision vZ range",
        type=float,
        default=4.2,
    )

    ##### REC: Seeding params
    parser.add_argument(
        "--seedingLayers",
        type=str,
        choices=["VD", "ML3", "MLall", "VDML"],
        required=False,
        default="VD",
        help="Choose seeding layers",
    )
    parser.add_argument(
        "--seedingAlgo",
        type=str,
        choices=["Default", "GridTriplet"],
        required=False,
        default="GridTriplet",
        help="Choose seeding algo",
    )

    parser.add_argument(
        "--minSeedPt",
        dest="minSeedPt",
        help="min pt for seed candidates, GeV/c",
        type=float,
        default=0.08,
    )

    parser.add_argument(
        "--impParForSeeds",
        dest="impParForSeeds",
        help="impParForSeeds",
        type=float,
        default=1.0,  # mm
    )
    parser.add_argument(
        "--radLengthPerSeed",
        dest="radLengthPerSeed",
        help="Average Radiation Length per seed",
        type=float,
        default=0.05,  # BEST TUNED WITH PARTICLE GUN - May 16. 2025 - if combined with sigmaScattering = 5
    )
    parser.add_argument(
        "--sigmaScattering",
        dest="sigmaScattering",
        help="How many sigmas of scattering to include in seeds",
        type=float,
        default=5.0,  # BEST TUNED WITH PARTICLE GUN - May 16. 2025 - if combined with radLengthPerSeed = 0.05
    )

    parser.add_argument(
        "--maxSeedsPerSpM",
        dest="maxSeedsPerSpM",
        help="max number of seeds per seed middle spacepoint",
        type=int,
        default=2,  # ok for pp, for PbPb should be 3 or more?
    )

    ##### REC: tracking params
    parser.add_argument(
        "--nMeasurementsMin",
        dest="nMeasurementsMin",
        help="nMeasurementsMin",
        type=int,
        default=7,
    )
    parser.add_argument(
        "--ckfMeasPerSurf",
        dest="ckfMeasPerSurf",
        help="ckf maxNumAssocMeasOnSurface",
        type=int,
        default=1,
    )
    parser.add_argument(
        "--ckfChi2Measurement",
        dest="ckfChi2Measurement",
        help="ckfChi2Measurement",
        type=float,
        default=45,
    )
    parser.add_argument(
        "--ckfChi2Outlier",
        dest="ckfChi2Outlier",
        help="ckfChi2Outlier",
        type=float,
        default=100,
    )

    parser.add_argument(
        "--maxSharedHits",
        dest="maxSharedHits",
        help="max number of shared hits. Doesn't have impact if the hit smearing digi is used.",
        type=int,
        default=2,
    )

    parser.add_argument(
        "--twoWayCKF",
        help="Set twoWayCKF option for CKF",
        action="store_true",  # default is false
        default=True,
    )
    parser.add_argument(
        "--seedDeduplication",
        help="Set seedDeduplication option for CKF",
        action="store_true",  # default is false
        default=True,
    )
    parser.add_argument(
        "--stayOnSeed",
        help="Set stayOnSeed option for CKF",
        action="store_true",  # default is false
        default=False,
    )

    ##### Special flags for tracking in several iterations
    parser.add_argument(
        "--iterationId",
        dest="iterationId",
        help="iterationId",
        type=int,
        default=0,
    )

    parser.add_argument(
        "--isUsedHitsRemoved",
        help="Use hit.root, where the used hits are removed",
        action="store_true",  # default: false
    )
    #####
    return parser


import argparse

pars = getArgumentParser().parse_args()


##########################
##### SOME OTHER PARAMS
##########################

IA_collisionRegion_forSeeds = (
    250 if pars.impParForSeeds < 2.0 else 500
)  # mm; large values - for V0 daughter reconstruction

IA_whichSeedingAlg = (
    SeedingAlgorithm.GridTriplet
)  # Nov 2025: is the default in addSeeding!
if pars.seedingAlgo == "Default":
    IA_whichSeedingAlg = SeedingAlgorithm.Default

IA_maxSeedsPerSpMConf = 1
IA_maxQualitySeedsPerSpMConf = 1
IA_enableMaterial = True


IA_outputDirName = (
    "output/"
    + pars.out_dir_prefix
    + "_nEv"
    + str(pars.nEvents)
    + "_B"
    + str(pars.MF)
    + "_PU"
    + str(pars.pileup)
    + "_iterationId"
    + str(pars.iterationId)
)
# +"_twoWayCKF_"+str(pars.twoWayCKF) +"_seedDeduplication_"+str(pars.seedDeduplication)  +"_stayOnSeed_"+str(pars.stayOnSeed) \


outputDir = pathlib.Path.cwd() / IA_outputDirName


if not outputDir.exists():
    outputDir.mkdir(mode=0o777, parents=True, exist_ok=True)

detector = alice3.buildALICE3Geometry(
    geo_dir, IA_enableMaterial, False, acts.logging.INFO
)
trackingGeometry = detector.trackingGeometry()
decorators = detector.contextDecorators()

field = acts.ConstantBField(acts.Vector3(0.0, 0.0, pars.MF * u.T))
if pars.useFieldMap:
    field = acts.examples.MagneticFieldMapXyz(
        "../try_MF_solenoid_dipole/fieldMap_Alice3_with_absorber_2T_solenoid_5-9-23.txt"
    )
# field = acts.examples.MagneticFieldMapXyz( "../try_MF_solenoid_dipole/fieldMap_Alice3_with_absorber_1T_solenoid_v2_5-9-23.txt" )
# field = acts.examples.MagneticFieldMapRz( "../try_MF_solenoid_dipole/L3_r_z_Br_Bz_in_mm_extended_by_hand.txt")
# field = acts.examples.MagneticFieldMapRz( "../try_MF_solenoid_dipole/1T_7.5m_solenoid_1m_free_bore.txt" ) # NEEDS MORE TUNINGS?..

# more info https://github.com/acts-project/acts/blob/v27.1.0/Examples/Scripts/Python/bfield_writing.py :

# field = acts.examples.MagneticFieldMapRz( "../try_MF_solenoid_dipole/8m_solenoid_no_iron_TWICE_REDUCED_FIELD.txt")
# field = acts.examples.MagneticFieldMapXyz( "../try_MF_solenoid_dipole/fieldMap_solenoid_dipoles_4-10-21_xyz_in_mm.txt" )
# field = acts.examples.MagneticFieldMapXyz( "fieldMap_solenoid_dipoles_4-10-21_xyz_in_mm_TWICE_REDUCED_FIELD.txt" )
# field = acts.examples.MagneticFieldMapXyz( "fieldMap_solenoid_dipoles_4-10-21_xyz_in_mm_StretchedPointsZ_by_1.25.txt" )


rnd = acts.examples.RandomNumbers(seed=42)


# s = acts.examples.Sequencer(events=pars.nEvents, numThreads=-1)
s = acts.examples.Sequencer(events=pars.nEvents, numThreads=pars.nThreads)

# if True: #pars.usedHitsRemoval==0:
if not pars.isUsedHitsRemoved:
    if not pars.usePythia:
        addParticleGun(
            s,
            MomentumConfig(
                pars.gunPtRange[0] * u.GeV, pars.gunPtRange[1] * u.GeV, transverse=True
            ),
            EtaConfig(pars.gunEtaRange[0], pars.gunEtaRange[1], uniform=True),
            # PhiConfig(0, 0.001), #, uniform=True),
            ### list of available particles: https://github.com/acts-project/acts/blob/main/Core/include/Acts/Definitions/PdgParticle.hpp#L21
            ParticleConfig(
                pars.gunMult,
                acts.PdgParticle(pars.gunPID),
                randomizeCharge=pars.gunRandCharge,
            ),
            vtxGen=acts.examples.GaussianVertexGenerator(
                stddev=acts.Vector4(
                    0.0001 * u.mm, 0.0001 * u.mm, 0.0001 * u.mm, 0.0001 * u.ns
                ),
                mean=acts.Vector4(0, 0, 0 * u.mm, 0),
            ),
            rnd=rnd,
            logLevel=acts.logging.INFO,
            outputDirRoot=outputDir,
        )

    else:
        if pars.system == "pp":
            s = addPythia8(
                s,
                npileup=pars.pileup,
                # for pp:
                beam=acts.PdgParticle.eProton,  # eLead,
                cmsEnergy=13.6 * acts.UnitConstants.TeV,  # 5 * acts.UnitConstants.TeV,
                hardProcess=[
                    "SoftQCD:inelastic = on",
                    "Tune:pp = 14",
                    "ParticleDecays:limitTau0 = on",
                    "ParticleDecays:tau0Max = 10",
                ],  # tune 14 is Monash
                vtxGen=acts.examples.GaussianVertexGenerator(
                    #    stddev=acts.Vector4(0.0125 * u.mm, 0.0125 * u.mm, pars.sigmaVz * u.mm, 5.0 * u.ns),
                    stddev=acts.Vector4(
                        0.000125 * u.mm,
                        0.000125 * u.mm,
                        pars.sigmaVz * u.mm,
                        0.0001 * u.ns,
                    ),  # 5.0 * u.ns),
                    # stddev=acts.Vector4(0.0125 * u.mm, 0.0125 * u.mm, 0.1 * u.mm, 5.0 * u.ns),
                    mean=acts.Vector4(0, 0, 0, 0),
                    # mean=acts.Vector4(0, 0, 100 * u.mm, 0),
                ),
                rnd=rnd,
                logLevel=acts.logging.INFO,
                outputDirRoot=outputDir,
            )
        elif pars.system == "PbPbMB":
            s = addPythia8(
                s,
                npileup=pars.pileup,
                # for Pb-Pb:
                beam=acts.PdgParticle.eLead,
                cmsEnergy=5.36 * acts.UnitConstants.TeV,  # 5 * acts.UnitConstants.TeV,
                # # hardProcess=[ "SoftQCD:inelastic = on", "HeavyIon:SigFitErr =  0.02,0.02,0.1,0.05,0.05,0.0,0.1,0.0",\
                hardProcess=[
                    "SoftQCD:inelastic = on",
                    "HeavyIon:SigFitErr =  0.02,0.02,0.1,0.05,0.05,0.0,0.1,0.0",
                    "HeavyIon:SigFitDefPar = 17.24,2.15,0.33,0.0,0.0,0.0,0.0,0.0",
                    "HeavyIon:SigFitNGen = 20",
                    "ParticleDecays:limitTau0 = on",
                    "ParticleDecays:tau0Max = 10",
                ],
                vtxGen=acts.examples.GaussianVertexGenerator(
                    #    stddev=acts.Vector4(0.0125 * u.mm, 0.0125 * u.mm, pars.sigmaVz * u.mm, 5.0 * u.ns),
                    stddev=acts.Vector4(
                        0.000125 * u.mm,
                        0.000125 * u.mm,
                        pars.sigmaVz * u.mm,
                        0.0001 * u.ns,
                    ),  # 5.0 * u.ns),
                    # stddev=acts.Vector4(0.0125 * u.mm, 0.0125 * u.mm, 0.1 * u.mm, 5.0 * u.ns),
                    mean=acts.Vector4(0, 0, 0, 0),
                    # mean=acts.Vector4(0, 0, 100 * u.mm, 0),
                ),
                rnd=rnd,
                logLevel=acts.logging.INFO,
                outputDirRoot=outputDir,
            )
        elif pars.system == "PbPbCentral":
            s = addPythia8(
                s,
                npileup=pars.pileup,
                # for Pb-Pb:
                beam=acts.PdgParticle.eLead,
                cmsEnergy=5.36 * acts.UnitConstants.TeV,  # 5 * acts.UnitConstants.TeV,
                hardProcess=[
                    "SoftQCD:inelastic = on",
                    "HeavyIon:bWidth=0.1",
                    "HeavyIon:SigFitErr =  0.02,0.02,0.1,0.05,0.05,0.0,0.1,0.0",
                    "HeavyIon:SigFitDefPar = 17.24,2.15,0.33,0.0,0.0,0.0,0.0,0.0",
                    "HeavyIon:SigFitNGen = 20",
                    "ParticleDecays:limitTau0 = on",
                    "ParticleDecays:tau0Max = 10",
                ],
                vtxGen=acts.examples.GaussianVertexGenerator(
                    #    stddev=acts.Vector4(0.0125 * u.mm, 0.0125 * u.mm, pars.sigmaVz * u.mm, 5.0 * u.ns),
                    stddev=acts.Vector4(
                        0.000125 * u.mm,
                        0.000125 * u.mm,
                        pars.sigmaVz * u.mm,
                        0.0001 * u.ns,
                    ),  # 5.0 * u.ns),
                    # stddev=acts.Vector4(0.0125 * u.mm, 0.0125 * u.mm, 0.1 * u.mm, 5.0 * u.ns),
                    mean=acts.Vector4(0, 0, 0, 0),
                    # mean=acts.Vector4(0, 0, 100 * u.mm, 0),
                ),
                rnd=rnd,
                logLevel=acts.logging.INFO,
                outputDirRoot=outputDir,
            )

else:  # use already generated particles
    s.addReader(
        acts.examples.RootParticleReader(
            level=acts.logging.INFO,
            # filePath="./output/testIterations_iterId"+str(pars.iterationId)+"/hits.root",
            filePath=IA_outputDirName + "/particles.root",
            outputParticles="particles_generated",  # _generated",
            # outputSimHits="simhits",
            # particleCollection="particles",
            # inputDir="output",
            # inputFile="pythia8_particles.root",
        )
    )
    s.addWhiteboardAlias("particles", "particles_generated")

addGenParticleSelection(
    s,
    ParticleSelectorConfig(
        eta=(-pars.particleSelectionEta, pars.particleSelectionEta),
        pt=(0.001 * u.MeV, None),
        removeNeutral=False,
        # rho=(0.0, 24 * u.mm),
        # absZ=(0.0, 1.0 * u.m),
    ),
)

if not pars.isUsedHitsRemoved:
    alice3_gdml = geo_dir / "geom/o2sim_geometry.gdml"

    gdml_detector = acts.examples.geant4.GdmlDetector(path=str(alice3_gdml))

    if pars.detSim == "Fatras":
        addFatras(
            s,
            trackingGeometry,
            field,
            enableInteractions=True,
            rnd=rnd,
            pMin=pars.minFatrasPt,  # GeV, May 2025
            outputDirRoot=outputDir,
            logLevel=acts.logging.INFO,
        )
    elif pars.detSim == "Geant4":
        addGeant4(
            s,
            gdml_detector,
            trackingGeometry,
            field,
            # materialMappings=["TRK_SI", "Silicon", "Silicon_elm"],
            materialMappings=["TRK_SILICON", "TF3_SILICON"],
            outputDirRoot=outputDir,
            rnd=rnd,
            logLevel=acts.logging.DEBUG,  # acts.logging.INFO
            killVolume=trackingGeometry.highestTrackingVolume,
            killAfterTime=40 * u.ns,  # 25 * u.ns,
        )


else:
    s.addReader(
        acts.examples.RootSimHitReader(
            level=acts.logging.INFO,
            # filePath="./output/testIterations_iterId"+str(pars.iterationId)+"/hits.root",
            filePath=IA_outputDirName + "/hits.root",
            outputSimHits="simhits",
        )
    )


addSimParticleSelection(
    s,
    ParticleSelectorConfig(
        # eta=(-4.1,4.1), pt=(1 * u.MeV, None), removeNeutral=True, # May 2025
        hits=(7, None),  # Nov 2025
        # rho=(0.0, 24 * u.mm),
        # absZ=(0.0, 1.0 * u.m),
    ),
)


s = addDigitization(
    s,
    trackingGeometry,
    field,
    #    digiConfigFile=geo_dir / "alice3-smearing-config_InnerBarrelRes_5mkm.json", #"alice3-smearing-config_DEFAULT.json",
    # digiConfigFile=geo_dir / "digi-smearing-config_with_TOFs.json",
    # digiConfigFile=geo_dir / "digi-smearing-config_with_TOFs_tunedSmearingForTOF.json",
    digiConfigFile=geo_dir / "digi-smearing-config_no_TOFs.json",
    # digiConfigFile=geo_dir / "digi-segm_config_no_TOFs.json", # TRY SEGMENTED DIGI
    # digiConfigFile=geo_dir / "digi-smearing-config_with_TOFs_ALL_LAYERS.json",
    # digiConfigFile=geo_dir / "digi-smearing-config_with_TOFs_NO_TIMING.json",
    # digiConfigFile=geo_dir / "digi-smearing-config_with_TOFs_try_segmentation.json",
    # minEnergyDeposit=0.001,
    outputDirRoot=outputDir,
    rnd=rnd,
    logLevel=acts.logging.DEBUG,
    # doMerge=True,
)
# for details on segmented digi: class Channelizer {..} https://github.com/acts-project/acts/blob/v36.3.2/Fatras/include/ActsFatras/Digitization/Channelizer.hpp#L21
# auto channelsRes = m_channelizer.channelize() - https://github.com/acts-project/acts/blob/v36.2.1/Examples/Algorithms/Digitization/src/DigitizationAlgorithm.cpp#L212


# addDigiParticleSelection(
#     s,
#     ParticleSelectorConfig(
#         eta=(-4.1,4.1), pt=(1 * u.MeV, None), removeNeutral=True, # May 2025
#         hits=(9, None),  # Nov 2025
#         # rho=(0.0, 24 * u.mm),
#         # absZ=(0.0, 1.0 * u.m),
#     ),
# )

# s = addHoughVertexFinding(
#     s,
#     outputDirRoot=outputDir,
#     inputSpacePoints=addSpacePointsMaking(
#         s,
#         trackingGeometry,
#         geo_dir / "geoSelectionForSeedingInner_BARREL_LayersOnly.json",
#         outputName = "spacePointsForHoughVertexing",
#         logLevel = acts.logging.VERBOSE
#     ),
#     logLevel = acts.logging.VERBOSE
# )


strWhichSeedingLayers = ""
if pars.seedingLayers == "VD":
    strWhichSeedingLayers = (
        "geoSelectionForSeedingInner_BOTH_Barrel_Endcaps_NEW_VOLUMES_Feb2025.json"
    )
elif pars.seedingLayers == "ML3":
    strWhichSeedingLayers = "geoSelectionForSeeding_3firstML.json"
elif pars.seedingLayers == "MLall":
    strWhichSeedingLayers = "geoSelectionForSeeding_5ML.json"
elif pars.seedingLayers == "VDML":
    strWhichSeedingLayers = "geoSelectionForSeeding_IB_ML.json"
print("strWhichLayers=", strWhichSeedingLayers)


s = addSeeding(
    s,
    trackingGeometry,
    field,
    SeedFinderConfigArg(
        r=(None, 210 * u.mm),  # iTOF is at 190 mm! if we want it for seeding
        # r=(None, 150 * u.mm),
        # r=(None, 30 * u.mm),
        # deltaR=(1 * u.mm, 120 * u.mm),  # deltaR=(1. * u.mm, 60 * u.mm),
        deltaR=(1 * u.mm, 200 * u.mm),  # deltaR=(1. * u.mm, 60 * u.mm),
        collisionRegion=(
            -IA_collisionRegion_forSeeds * u.mm,
            IA_collisionRegion_forSeeds * u.mm,
        ),
        z=(-1000 * u.mm, 1000 * u.mm),
        maxSeedsPerSpM=pars.maxSeedsPerSpM,  # 2 is minimum, >2 is better for Pb-Pb
        sigmaScattering=pars.sigmaScattering,
        radLengthPerSeed=pars.radLengthPerSeed,  # more info: https://github.com/acts-project/acts/blob/main/Core/include/Acts/Seeding/SeedFinderConfig.hpp
        minPt=pars.minSeedPt * u.GeV,
        impactMax=pars.impParForSeeds
        * u.mm,  # important! IB vs ML seeds (e.g. 1 mm is ok for IB seeds, 5 mm - for ML seeds)
        cotThetaMax=27.2899,
        seedConfirmation=True,
        centralSeedConfirmationRange=acts.SeedConfirmationRangeConfig(
            zMinSeedConf=-620 * u.mm,
            zMaxSeedConf=620 * u.mm,
            rMaxSeedConf=4.9
            * u.mm,  # 36 * u.mm,  # IA: dramatically affects acceptance at eta ~4. <5 * u.mm  gives best results
            nTopForLargeR=1,  # number of top space points that confirm my seed at larger R, 1 - no confirmation
            nTopForSmallR=2,
        ),
        forwardSeedConfirmationRange=acts.SeedConfirmationRangeConfig(
            zMinSeedConf=-1220 * u.mm,
            zMaxSeedConf=1220 * u.mm,
            rMaxSeedConf=26 * u.mm,  # 15 * u.mm,  #36 * u.mm,
            nTopForLargeR=1,
            nTopForSmallR=2,
        ),
        # skipPreviousTopSP=True,
        useVariableMiddleSPRange=True,
        deltaRMiddleSPRange=(0.2 * u.mm, 1.0 * u.mm),
    ),
    SeedFinderOptionsArg(bFieldInZ=pars.MF * u.T, beamPos=(0 * u.mm, 0 * u.mm)),
    SeedFilterConfigArg(
        seedConfirmation=True if pars.impParForSeeds < 2.0 else False,  # mm
        # If seedConfirmation is true we classify seeds as "high-quality" seeds.
        # Seeds that are not confirmed as "high-quality" are only selected if no
        # other "high-quality" seed has been found for that inner-middle doublet
        # Maximum number of normal seeds (not classified as "high-quality" seeds)
        # in seed confirmation
        maxSeedsPerSpMConf=IA_maxSeedsPerSpMConf,  # 1 - USED_FOR_AUG_2025,#3, # CRUCIAL!!!!!!
        maxQualitySeedsPerSpMConf=IA_maxQualitySeedsPerSpMConf,  # 1 - USED_FOR_AUG_2025,   # Core/include/Acts/Seeding/SeedFilterConfig.hpp
        # Maximum number of "high-quality" seeds for each inner-middle SP-dublet in
        # seed confirmation. If the limit is reached we check if there is a normal
        # quality seed to be replaced
    ),
    SpacePointGridConfigArg(
        # zBinEdges=[
        # -4000.0,
        # -2500.0,
        # -2000.0,
        # -1320.0,
        # -625.0,
        # -350.0,
        # -250.0,
        # 250.0,
        # 350.0,
        # 625.0,
        # 1320.0,
        # 2000.0,
        # 2500.0,
        # 4000.0,
        # ],
        impactMax=1.0 * u.mm,
        phiBinDeflectionCoverage=3,
    ),
    SeedingAlgorithmConfigArg(
        # zBinNeighborsTop=[
        # [0, 0],
        # [-1, 0],
        # [-1, 0],
        # [-1, 0],
        # [-1, 0],
        # [-1, 0],
        # [-1, 1],
        # [0, 1],
        # [0, 1],
        # [0, 1],
        # [0, 1],
        # [0, 1],
        # [0, 0],
        # ],
        # zBinNeighborsBottom=[
        # [0, 1],
        # [0, 1],
        # [0, 1],
        # [0, 1],
        # [0, 1],
        # [0, 1],
        # [0, 0],
        # [-1, 0],
        # [-1, 0],
        # [-1, 0],
        # [-1, 0],
        # [-1, 0],
        # [-1, 0],
        # ],
        # numPhiNeighbors=1,
    ),
    # particleHypothesis=acts.ParticleHypothesis.pion, # IA
    # particleHypothesis=acts.ParticleHypothesis.electron, # IA
    geoSelectionConfigFile=geo_dir / strWhichSeedingLayers,
    # seedingAlgorithm=SeedingAlgorithm.TruthSmeared,
    seedingAlgorithm=IA_whichSeedingAlg,  # SeedingAlgorithm.Default, # Nov 2025: default is GridTriplet !!!
    outputDirRoot=outputDir,
    #    initialVarInflation = (50,50,50,50,50,50)  # IA
    #    initialVarInflation = (0.2,0.2,0.2,0.2,0.2,0.2)  #IA
)

# from https://github.com/acts-project/acts/blob/v41.0.0/Examples/Scripts/Python/hashing_seeding.py:
if False:  # if we want to save seeds to root file
    rootSeedsWriter = acts.examples.RootSeedWriter(
        level=acts.logging.VERBOSE,
        inputSeeds="seeds",
        filePath=str(outputDir / "seeds.root"),
        writingMode="big",  # IA
    )
    s.addWriter(rootSeedsWriter)


# s = addHoughVertexFinding(
#     s,
#     outputDirRoot=outputDir,
#     inputSpacePoints="spacepoints",
#     logLevel = acts.logging.VERBOSE
# )

### useful info: https://github.com/acts-project/acts/blob/main/Core/include/Acts/TrackFinding/MeasurementSelector.hpp
s = addCKFTracks(
    s,
    trackingGeometry,
    field,
    TrackSelectorConfig(
        pt=(0.06 * u.GeV, 120 * u.GeV),
        nMeasurementsMin=pars.nMeasurementsMin,
        maxSharedHits=pars.maxSharedHits,
    ),  # IA, was: 500.0 * u.MeV
    ckfConfig=CkfConfig(
        chi2CutOffOutlier=pars.ckfChi2Outlier,
        chi2CutOffMeasurement=pars.ckfChi2Measurement,
        numMeasurementsCutOff=pars.ckfMeasPerSurf,
        seedDeduplication=pars.seedDeduplication,
        stayOnSeed=pars.stayOnSeed,
    ),
    twoWay=pars.twoWayCKF,  # default: True,
    outputDirRoot=outputDir,
    writeTrackSummary=False,
    logLevel=acts.logging.INFO,
)

# s = addTruthTrackingGsf(
#     s,
#     trackingGeometry,
#     field,
# )

s = addAmbiguityResolution(
    s,
    AmbiguityResolutionConfig(
        maximumSharedHits=pars.maxSharedHits, nMeasurementsMin=pars.nMeasurementsMin
    ),
    outputDirRoot=outputDir,
    logLevel=acts.logging.INFO,
)

s.run()


# import shutil

# shutil.copyfile("full_chain_Nov_2025.py", IA_outputDirName + "/full_chain_Nov_2025.py")

# # IA_outputDirName="test"
# with open("lastRun.txt", "w") as fh:
#     fh.write(IA_outputDirName + "\n")
