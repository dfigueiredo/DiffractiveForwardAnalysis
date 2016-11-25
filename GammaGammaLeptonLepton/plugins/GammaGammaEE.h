#ifndef DiffractiveForwardAnalysis_GammaGammaEE8TeV_h
#define DiffractiveForwardAnalysis_GammaGammaEE8TeV_h

// system include files
#include <memory>
#include <map>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/Registry.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

// HLT information
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

// Generator level collection
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"

// Pileup
#include "DataFormats/Luminosity/interface/LumiSummary.h"
#include "DataFormats/Luminosity/interface/LumiDetails.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

// Electrons collection
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"
#include "DataFormats/EgammaCandidates/interface/Electron.h"  
#include "DataFormats/EgammaCandidates/interface/ElectronFwd.h"   
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"

// Particle flow collection
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

// Vertices collection
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Common/interface/RefToBase.h" 
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "RecoVertex/VertexPrimitives/interface/ConvertError.h"
#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"

// Jets/MET collection
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"

// HPS acceptance
#include "DiffractiveForwardAnalysis/GammaGammaLeptonLepton/interface/AcceptanceTableHelper.h"

// RP 
#include "DataFormats/Common/interface/DetSetVector.h" 
#include "DataFormats/TotemRPDetId/interface/TotemRPDetId.h" 
#include "DataFormats/TotemDigi/interface/TotemRPDigi.h" 
#include "DataFormats/TotemDigi/interface/TotemVFATStatus.h" 
#include "DataFormats/CTPPSReco/interface/TotemRPCluster.h" 
#include "DataFormats/CTPPSReco/interface/TotemRPRecHit.h" 
#include "DataFormats/CTPPSReco/interface/TotemRPUVPattern.h" 
#include "DataFormats/CTPPSReco/interface/TotemRPLocalTrack.h" 

// Analysis classes
#include "HLTmatches.h"
#include "PrimaryVertex.h"

// ROOT
#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <map>

#define MAX_HLT    2 // Maximum number of HLT to check
#define MAX_LL     3 // Maximum number of leptons per event
#define MAX_ELECTRONS  3 // Maximum number of muons per event
#define MAX_ELE    3 // Maximum number of electrons per event
#define MAX_PHO    50 // Maximum number of photons per event
#define MAX_PAIRS  1 // Maximum number of leptons pairs per event
#define MAX_VTX    100 // Maximum number of primary vertices per event
#define MAX_ET     5000// Maximum number of extra tracks per event
#define MAX_GENMU  3 // Maximum number of generator level muons per event
#define MAX_GENELE 3 // Maximum number of generator level electrons per event
#define MAX_GENPHO 3 // Maximum number of generator level photons per event
#define MAX_JETS   3 // Maximum nulber of jets per event
#define MAX_GENPRO 10 // Maximum number of generated protons
#define MAX_LOCALPCAND 10 // Maximum number of reconstructed local tracks in RPs
#define MASS_MU 0.1057
#define MASS_E  0.000511
#define MASS_P  0.938272029
#define pi 3.14159265359

typedef std::vector< edm::Handle< edm::ValueMap<double> > > IsoDepositVals; 

//
// class declaration
//

class GammaGammaEE : public edm::EDAnalyzer {
  public:
    explicit GammaGammaEE(const edm::ParameterSet&);
    ~GammaGammaEE();
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void beginJob() ;
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
    virtual void endJob() ;

    virtual void beginRun(edm::Run const&, edm::EventSetup const&);
    virtual void endRun(edm::Run const&, edm::EventSetup const&);
    virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
    virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
    virtual void LookAtTriggersEE(const edm::Event&, const edm::EventSetup&);

    // ----------member data ---------------------------
    UInt_t i;
    Int_t lep1, lep2;
    Double_t vtxdst;
    Double_t closesttrkdxyz, closesthighpuritytrkdxyz;
    Double_t closestkalmantrkdxyz;
    Int_t parttype;
    Double_t leadingphotpx, leadingphotpy, leadingphotpz, leadingphotp;
    Double_t photdeta, photdphi, photdr;
    Double_t endphotdeta, endphotdphi, endphotdr;
    Double_t pairgmass;
    bool istight;

    // Input tags
    std::string hltMenuLabel_;
    std::vector<std::string> triggersList_, leptonsType_;
    edm::InputTag beamSpotLabel_, recoVertexLabel_;
    edm::InputTag genLabel_, muonLabel_, trackLabel_, conversions_;

    edm::EDGetTokenT< edm::View<pat::Electron> > eleToken_;
    edm::EDGetTokenT< edm::ValueMap<bool> > eleLooseIdMap_;
    edm::EDGetTokenT< edm::ValueMap<bool> > eleMediumIdMap_;
    edm::EDGetTokenT< edm::ValueMap<bool> > eleTightIdMap_;
    edm::EDGetToken electronsTokenGsf_;

    edm::InputTag conversionsLabel_, rhoIsoLabel_;
    edm::InputTag pileupLabel_;
    edm::InputTag pflowLabel_;
    edm::InputTag jetLabel_, metLabel_;
    std::vector<edm::InputTag> isoValLabel_; 
    bool runOnMC_, printCandidates_, runOnProtons_;
    Double_t minPtMC_, minEtaMC_;
    Double_t sqrts_;

    // RP
    edm::EDGetTokenT< edm::DetSetVector<TotemRPLocalTrack> > tokenRPLocalTrack;  

    // Beam spot
    edm::Handle<reco::BeamSpot> beamspot_h;

    // Generator level information
    edm::Handle<reco::GenParticleCollection> genPartColl;
    reco::GenParticleCollection::const_iterator genPart;
    std::string fullAcceptancePath;
    edm::FileInPath *myDataFile;

    // HPS acceptance tables
    TFile *f;
    AcceptanceTableHelper helper420beam1, helper220beam1, helper420a220beam1;
    AcceptanceTableHelper helper420beam2, helper220beam2, helper420a220beam2;

    // Trigger information
    HLTmatches *_hlts;
    HLTConfigProvider hltConfig_;
    edm::Handle<edm::TriggerResults> hltResults_;

    // Pileup information
    edm::LumiReWeighting *LumiWeights; 
    edm::Handle<std::vector< PileupSummaryInfo > >  pileupInfo;
    std::vector<PileupSummaryInfo>::const_iterator PVI;
    Int_t sum_nvtx, beamXing;
    Int_t npv, npvtrue, npvm1true, npvp1true, npv0true, npv0;
    std::string mcPileupFile_, mcPileupPath_, dataPileupFile_, dataPileupPath_;

    // Switches
    bool _fetchMuons, _fetchElectrons;

    // Two-leptons matching
    bool foundPairInEvent, foundPairOnVertex;
    bool foundGenCandPairInEvent;
    Double_t leptonsDist, minDist; 
    TLorentzVector* _leptonptmp;
    TLorentzVector l1, l2;
    std::map<Int_t, TLorentzVector> electronsMomenta;

    // Isolation
    Double_t rhoIso;
    Double_t iso_ch, iso_em, iso_nh; // Electron isolation quantities
    Int_t vtxind; // Primary vertex index (used in loop over vertices)
    Int_t etind; // Extra tracks on vertex index (used in loop over tracks)

    // Vertices
    edm::Handle<reco::VertexCollection> recoVertexColl;
    reco::VertexCollection::const_iterator vertex;
    TString* _leptonType;

    // RECO tracks
    edm::Handle<reco::TrackCollection> trackColl;
    reco::TrackCollection::const_iterator track;

    // RECO electrons
    edm::Handle<reco::GsfElectronCollection> eleCollRECO;
    edm::Handle<reco::ConversionCollection> conversions_h;
    edm::Handle<double> rhoIso_h; 

    TLorentzVector pair;
    Double_t dphi;

    ////// Tree contents //////

    // Run/event quantities
    Int_t BX, Run, LumiSection, EventNum;

    // HLT quantities
    Int_t nHLT;
    Int_t HLT_Accept[MAX_HLT], HLT_Prescl[MAX_HLT];

    // Generator level quantities
    Int_t nGenElectronCand, nGenElectronCandOutOfAccept;
    Double_t GenElectronCand_px[MAX_GENMU], GenElectronCand_py[MAX_GENMU], GenElectronCand_pz[MAX_GENMU];
    Double_t GenElectronCand_p[MAX_GENMU], GenElectronCand_pt[MAX_GENMU];
    Double_t GenElectronCand_eta[MAX_GENMU], GenElectronCand_phi[MAX_GENMU];
    Double_t GenPair_p, GenPair_pt, GenPair_mass;
    Double_t GenPair_phi, GenPair_eta;
    Double_t GenPair_dphi, GenPair_dpt, GenPair_3Dangle;
    Int_t nGenPhotCand, nGenPhotCandOutOfAccept;
    Double_t GenPhotCand_pt[MAX_GENPHO], GenPhotCand_eta[MAX_GENPHO], GenPhotCand_phi[MAX_GENPHO];

    Int_t nGenProtCand; 
    Double_t GenProtCand_p[MAX_GENPRO], GenProtCand_px[MAX_GENPRO], GenProtCand_py[MAX_GENPRO], GenProtCand_pz[MAX_GENPRO]; 
    Double_t GenProtCand_pt[MAX_GENPRO], GenProtCand_eta[MAX_GENPRO], GenProtCand_phi[MAX_GENPRO]; 
    Double_t GenProtCand_e[MAX_GENPRO], GenProtCand_xi[MAX_GENPRO], GenProtCand_t[MAX_GENPRO];  
    Int_t GenProtCand_status[MAX_GENPRO]; 

    Int_t nLeptonCand, nLeptonsInPrimVertex, nCandidates, nCandidatesInEvent;

    // CTPPS quantities
    Int_t nLocalProtCand;
    Double_t LocalProtCand_x[MAX_LOCALPCAND], LocalProtCand_y[MAX_LOCALPCAND], LocalProtCand_z[MAX_LOCALPCAND];
    Double_t LocalProtCand_xSigma[MAX_LOCALPCAND], LocalProtCand_ySigma[MAX_LOCALPCAND];
    Double_t LocalProtCand_Tx[MAX_LOCALPCAND], LocalProtCand_Ty[MAX_LOCALPCAND];  
    Double_t LocalProtCand_TxSigma[MAX_LOCALPCAND], LocalProtCand_TySigma[MAX_LOCALPCAND]; 

    // Pileup reweighting quantities
    Double_t nTruePUafterPUWeight;
    Double_t nTruePUafterPUWeightBXM1, nTruePUafterPUWeightBXP1, nTruePUafterPUWeightBX0;
    Double_t PUWeightTrue;
    Int_t nTruePUforPUWeight;
    Int_t nTruePUforPUWeightBXM1, nTruePUforPUWeightBXP1, nTruePUforPUWeightBX0;
    Double_t Weight;

    // Electron quantities
    Int_t nElectronCand;
    Double_t ElectronCand_px[MAX_LL], ElectronCand_py[MAX_LL], ElectronCand_pz[MAX_LL];
    Double_t ElectronCand_p[MAX_LL], ElectronCand_pt[MAX_LL];
    Double_t ElectronCand_eta[MAX_LL], ElectronCand_phi[MAX_LL];
    Double_t ElectronCand_vtxx[MAX_LL], ElectronCand_vtxy[MAX_LL], ElectronCand_vtxz[MAX_LL];
    Int_t ElectronCand_charge[MAX_LL];
    Int_t ElectronCand_nstatseg[MAX_LL], ElectronCand_npxlhits[MAX_LL], ElectronCand_ntrklayers[MAX_LL];
    Double_t ElectronCand_[MAX_LL];
    Int_t ElectronCandTrack_nechits[MAX_LL];
    Double_t ElectronCandTrack_chisq[MAX_LL];
    Int_t ElectronCand_isglobal[MAX_LL], ElectronCand_istracker[MAX_LL], ElectronCand_isstandalone[MAX_LL], ElectronCand_ispfelectron[MAX_LL];
    Int_t ElectronCand_istight[MAX_LL];
    Double_t ElectronCand_innerTrackPt[MAX_LL],ElectronCand_innerTrackEta[MAX_LL],ElectronCand_innerTrackPhi[MAX_LL];
    Double_t ElectronCand_innerTrackVtxz[MAX_LL];
    Double_t ElectronCand_deltaPhi[MAX_LL];
    Double_t ElectronCand_deltaEta[MAX_LL];
    Double_t ElectronCand_HoverE[MAX_LL];
    Double_t ElectronCand_trackiso[MAX_LL], ElectronCand_ecaliso[MAX_LL], ElectronCand_hcaliso[MAX_LL];
    Double_t ElectronCand_sigmaIetaIeta[MAX_LL];
    Double_t ElectronCand_convDist[MAX_LL], ElectronCand_convDcot[MAX_LL];
    Int_t ElectronCand_ecalDriven[MAX_LL];
    Int_t ElectronCand_tightID[MAX_LL], ElectronCand_mediumID[MAX_LL], ElectronCand_looseID[MAX_LL];

    // Pair quantities
    Int_t Pair_candidates[MAX_PAIRS][2];
    Double_t Pair_mindist[MAX_PAIRS];
    Double_t Pair_p[MAX_PAIRS], Pair_pt[MAX_PAIRS], Pair_dpt[MAX_PAIRS];
    Double_t Pair_mass[MAX_PAIRS], Pair_dphi[MAX_PAIRS];
    Double_t Pair_eta[MAX_PAIRS], Pair_phi[MAX_PAIRS], Pair_3Dangle[MAX_PAIRS], Pair_Y[MAX_PAIRS];
    Double_t PairGamma_mass[MAX_PAIRS][MAX_PHO];

    // Extra tracks
    Int_t Pair_extratracks1mm[MAX_PAIRS], Pair_extratracks2mm[MAX_PAIRS];
    Int_t Pair_extratracks3mm[MAX_PAIRS], Pair_extratracks4mm[MAX_PAIRS];
    Int_t Pair_extratracks5mm[MAX_PAIRS], Pair_extratracks1cm[MAX_PAIRS];
    Int_t Pair_extratracks2cm[MAX_PAIRS], Pair_extratracks3cm[MAX_PAIRS];
    Int_t Pair_extratracks4cm[MAX_PAIRS], Pair_extratracks5cm[MAX_PAIRS];
    Int_t Pair_extratracks10cm[MAX_PAIRS];

    // Vertex quantities
    Int_t nPrimVertexCand;
    Int_t PrimVertexCand_id[MAX_VTX];
    Double_t PrimVertexCand_x[MAX_VTX], PrimVertexCand_y[MAX_VTX], PrimVertexCand_z[MAX_VTX];
    Int_t PrimVertexCand_tracks[MAX_VTX], PrimVertexCand_matchedtracks[MAX_VTX], PrimVertexCand_unmatchedtracks[MAX_VTX];
    Double_t PrimVertexCand_chi2[MAX_VTX];
    Int_t PrimVertexCand_ndof[MAX_VTX];
    Int_t nFilteredPrimVertexCand;

    // Kalman dilepton vertex quantities
    Double_t KalmanVertexCand_x, KalmanVertexCand_y, KalmanVertexCand_z;
    Double_t ClosestExtraTrackKalman_vtxdxyz;

    // Extra tracks on vertex quantities
    Int_t nExtraTracks;
    Int_t ExtraTrack_purity[MAX_ET], ExtraTrack_nhits[MAX_ET];
    Int_t ExtraTrack_charge[MAX_ET], ExtraTrack_ndof[MAX_ET];
    Double_t ExtraTrack_p[MAX_ET], ExtraTrack_pt[MAX_ET];
    Double_t ExtraTrack_px[MAX_ET], ExtraTrack_py[MAX_ET], ExtraTrack_pz[MAX_ET];
    Double_t ExtraTrack_eta[MAX_ET], ExtraTrack_phi[MAX_ET];
    Double_t ExtraTrack_chi2[MAX_ET];
    Double_t ExtraTrack_vtxdxyz[MAX_ET];
    Double_t ExtraTrack_vtxT[MAX_ET], ExtraTrack_vtxZ[MAX_ET];
    Double_t ExtraTrack_x[MAX_ET], ExtraTrack_y[MAX_ET], ExtraTrack_z[MAX_ET];
    Double_t ClosestExtraTrack_vtxdxyz, ClosestHighPurityExtraTrack_vtxdxyz, ClosestTrack_vtxdxyz;
    Int_t nQualityExtraTrack;

    //TFile* file;
    TTree* tree;

};

#endif