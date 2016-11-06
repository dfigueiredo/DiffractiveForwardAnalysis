import FWCore.ParameterSet.Config as cms

ggee = cms.EDAnalyzer(
    'GammaGammaEE',
    GlobalEleCollectionLabel = cms.untracked.InputTag("selectedPatElectrons"),
    eleLooseIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Spring15-25ns-V1-standalone-loose"),
    eleMediumIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Spring15-25ns-V1-standalone-medium"),
    eleTightIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Spring15-25ns-V1-standalone-tight"),
    SqrtS = cms.double(13000.),
    HLTMenuLabel = cms.string("HLT"),
    LeptonsType = cms.InputTag('electrons'),
    RunOnMC = cms.untracked.bool(False),
    RunOnProtons = cms.untracked.bool(True),
    MCAcceptPtCut = cms.untracked.double(0.),
    MCAcceptEtaCut = cms.untracked.double(-1.),
    GenParticlesCollectionLabel = cms.InputTag('genParticles'),
    PrintCandidates = cms.untracked.bool(False),
)
