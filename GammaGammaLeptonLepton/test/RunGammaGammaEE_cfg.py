import FWCore.ParameterSet.Config as cms

process = cms.Process("ggee")

runOnMC = False
runOnProtons = True

#########################
#    General options    #
#########################
process.load("FWCore.MessageService.MessageLogger_cfi")
process.options   = cms.untracked.PSet(
    #wantSummary = cms.untracked.bool(True),
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )
#process.MessageLogger.cerr.FwkReport.reportEvery = 1000

#########################
#      Input files      #
#########################
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
'/store/data/Run2016C/SingleElectron/AOD/PromptReco-v2/000/275/657/00000/086DD2F4-903B-E611-AB5B-02163E014279.root'
#'/store/data/Run2016C/DoubleMuon/AOD/PromptReco-v2/000/275/657/00000/16858380-6F3B-E611-AE0A-02163E0118AD.root',
#'/store/mc/RunIISpring16DR80/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/AODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/000CB19C-6CF2-E511-8F52-001517F7F510.root'
#'/store/user/dmf/SamplesDebugCMSSW8012/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8-AODSIM-PUSpring16_80X_mcRun2.root'
    ),
#    firstEvent = cms.untracked.uint32(1)
)

#########################
#        Triggers       #
#########################
process.load("DiffractiveForwardAnalysis.GammaGammaLeptonLepton.HLTFilter_cfi")
process.hltFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT")
process.hltFilter.HLTPaths = ['HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v*']

#########################
#      Preskimming      #
#########################
process.load("Configuration.StandardSequences.GeometryDB_cff") ## FIXME need to ensure that this is the good one
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.GlobalTag.globaltag = cms.string('START52_V9::All')
process.GlobalTag.globaltag = cms.string('80X_dataRun2_Prompt_v8')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")

#########################
#     PAT-ification     #
#########################
## Look at https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuidePATTools#Core_Tools for more information

# PAT Layer 0+1
process.load("PhysicsTools.PatAlgos.patSequences_cff")
from Configuration.EventContent.EventContent_cff import *
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoCleaning, patExtraAodEventContent
from PhysicsTools.PatAlgos.tools.coreTools import *

from PhysicsTools.PatAlgos.producersLayer1.electronProducer_cff import *
from PhysicsTools.PatAlgos.producersLayer1.muonProducer_cff import *
from PhysicsTools.PatAlgos.producersLayer1.photonProducer_cff import *
from PhysicsTools.PatAlgos.producersLayer1.tauProducer_cff import *
from PhysicsTools.PatAlgos.producersLayer1.jetProducer_cff import *

from PhysicsTools.PatAlgos.producersLayer1.patCandidates_cff import *

if not runOnMC:
	getattr(process, 'makePatElectrons').remove(electronMatch)
	getattr(process, 'makePatMuons').remove(muonMatch)
	getattr(process, 'makePatPhotons').remove(photonMatch)
	getattr(process, 'makePatTaus').remove(tauMatch)
	getattr(process, 'makePatTaus').remove(tauGenJets)
	getattr(process, 'makePatTaus').remove(tauGenJetsSelectorAllHadrons)
	getattr(process, 'makePatTaus').remove(tauGenJetMatch)
	getattr(process, 'makePatJets').remove(patJetPartonMatch)
	getattr(process, 'makePatJets').remove(patJetGenJetMatch)
	getattr(process, 'makePatJets').remove(patJetFlavourIdLegacy)
	getattr(process, 'makePatJets').remove(patJetFlavourId)
	getattr(process, 'patCandidates').remove(makePatMETs)

process.out = cms.OutputModule("PoolOutputModule",
    outputCommands = cms.untracked.vstring(
        'drop *',
        'keep *_offlinePrimaryVertices*_*_*',
        'keep *_*Muons*_*_*',
        'keep *_*Electrons*_*_*',
        'keep *_selectedPatElectrons*_*_*',
        'keep recoPFCandidates_particleFlow_*_*',
    ),
)


#########################
#       Analysis        #
#########################
process.load("DiffractiveForwardAnalysis.GammaGammaLeptonLepton.GammaGammaEE_cfi")
process.ggee.TriggersList = process.hltFilter.HLTPaths
process.ggee.LeptonsType = cms.vstring('Electron')
process.ggee.RecoVertexLabel = cms.InputTag("offlinePrimaryVertices")
process.ggee.RunOnMC = cms.untracked.bool(runOnMC)
process.ggee.RunOnProtons = cms.untracked.bool(runOnProtons)

#########################
#       Protons         #
#########################
# RP geometry
#process.load("Geometry.VeryForwardGeometry.geometryRP_cfi")
#process.XMLIdealGeometryESSource.geomXMLFiles.append("Geometry/VeryForwardData/data/RP_Garage/RP_Dist_Beam_Cent.xml")

# local RP reconstruction chain with standard settings
process.load("RecoCTPPS.Configuration.recoCTPPS_cff")

process.TFileService = cms.Service("TFileService", fileName = cms.string("output_rp.root"))

if not runOnMC:
	process.p = cms.Path(
	     process.patDefaultSequence+
	#    getattr(process,"patPF2PATSequence"+postfix)+
	    process.hltFilter +
	#    process.patDefaultSequence+
	#    process.recoCTPPS + 
	    process.ggee
	)
else:
        process.p = cms.Path(
             process.patDefaultSequence+
            process.ggee
        )


#print process.dumpPython()
