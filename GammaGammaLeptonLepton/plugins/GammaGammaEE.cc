// -*- C++ -*-
//
// Package:    GammaGammaEE
// Class:      GammaGammaEE
// 
/**\class GammaGammaEE GammaGammaEE.cc DiffractiveForwardAnalysis/GammaGammaLeptonLepton/src/GammaGammaEE.cc

Description: [one line class summary]

Implementation:
[Notes on implementation]
*/
//
// Original Author:  Laurent Forthomme,40 4-B20,+41227671567,
//         Created:  Thu Sep 13 15:17:14 CET 2012
// $Id: GammaGammaEE.cc,v 1.3 2013/04/28 08:40:45 lforthom Exp $
//
//

// WP80 cuts
const float MAX_MissingHits      = 0.0;
const float MIN_Dist             = 0.02;
const float MIN_Dcot             = 0.02;
const float cut_EB_trackRel03    = 0.09;
const float cut_EB_ecalRel03     = 0.07;
const float cut_EB_hcalRel03     = 0.10;
const float cut_EB_sigmaIetaIeta = 0.01;
const float cut_EB_deltaPhi      = 0.06;
const float cut_EB_deltaEta      = 0.004;
const float cut_EB_HoverE        = 0.04;
const float cut_EE_trackRel03    = 0.04;
const float cut_EE_ecalRel03     = 0.05;
const float cut_EE_hcalRel03     = 0.025;
const float cut_EE_sigmaIetaIeta = 0.03;
const float cut_EE_deltaPhi      = 0.03;
const float cut_EE_deltaEta      = 0.007; 
const float cut_EE_HoverE        = 0.025;

#include "GammaGammaEE.h"

//
// constructors and destructor
//
GammaGammaEE::GammaGammaEE(const edm::ParameterSet& iConfig):
  // Electron Map
  eleToken_           (consumes< edm::View<pat::Electron> >       (iConfig.getUntrackedParameter<edm::InputTag>("GlobalEleCollectionLabel", std::string("gsfElectrons")))),
  eleLooseIdMap_ (consumes< edm::ValueMap<bool> >            (iConfig.getParameter<edm::InputTag>("eleLooseIdMap"))),
  eleMediumIdMap_(consumes< edm::ValueMap<bool> >            (iConfig.getParameter<edm::InputTag>("eleMediumIdMap"))),
  eleTightIdMap_ (consumes< edm::ValueMap<bool> >            (iConfig.getParameter<edm::InputTag>("eleTightIdMap")))
{

  //now do what ever initialization is needed
  _fetchElectrons = true;

  hltMenuLabel_ = iConfig.getParameter<std::string>("HLTMenuLabel");
  triggersList_ = iConfig.getParameter<std::vector<std::string> >("TriggersList");
  _hlts = new HLTmatches(triggersList_);
  nHLT = triggersList_.size();

  recoVertexLabel_ = iConfig.getParameter<edm::InputTag>("RecoVertexLabel");

  // Generator Level
  sqrts_ = iConfig.getParameter<Double_t>("SqrtS");
  runOnMC_ = iConfig.getUntrackedParameter<bool>("RunOnMC", true);
  runOnProtons_ = iConfig.getUntrackedParameter<bool>("RunOnProtons", true);
  genLabel_ = iConfig.getParameter<edm::InputTag>("GenParticlesCollectionLabel");
  minPtMC_ = iConfig.getUntrackedParameter<Double_t>("MCAcceptPtCut", 20.);
  minEtaMC_ = iConfig.getUntrackedParameter<Double_t>("MCAcceptEtaCut", 2.5);

  // Pileup input tags
  pileupLabel_ = iConfig.getUntrackedParameter<edm::InputTag>("pileupInfo", std::string("addPileupInfo"));
  mcPileupFile_ = iConfig.getUntrackedParameter<std::string>("mcpufile", "MCPUdist_2016.root");
  mcPileupPath_ = iConfig.getUntrackedParameter<std::string>("mcpupath", "pileup");
  dataPileupFile_ = iConfig.getUntrackedParameter<std::string>("datapufile", "MyDataPileupHistogram.root");
  dataPileupPath_ = iConfig.getUntrackedParameter<std::string>("datapupath", "pileup");

  // Leptons input tags
  leptonsType_ = iConfig.getParameter< std::vector<std::string> >("LeptonsType");
  for (i=0; i<leptonsType_.size(); i++) {
    if (leptonsType_[i]=="Electron") _fetchElectrons = true;
  }

  printCandidates_ = iConfig.getUntrackedParameter<bool>("PrintCandidates", false);
  trackLabel_ = iConfig.getUntrackedParameter<edm::InputTag>("TrackCollectionLabel", std::string("generalTracks"));

  //edm::Service<TFileService> file;
  //file = new TFile(outputFile_.c_str(), "recreate");
  //file->cd();

  // Tree definition
  //tree = new TTree("ntp1", "ntp1");

  // PU reweighting
  if (runOnMC_) {
    LumiWeights = new edm::LumiReWeighting(mcPileupFile_, dataPileupFile_, mcPileupPath_, dataPileupPath_);
  }

  consumes<edm::TriggerResults>(edm::InputTag("TriggerResults","","HLT"));  
  consumes<reco::VertexCollection>(edm::InputTag("offlinePrimaryVertices"));  
  consumes<edm::View<pat::Electron>>(edm::InputTag("gsfElectrons"));
  consumes<reco::TrackCollection>(edm::InputTag("generalTracks"));    
  consumes<std::vector<PileupSummaryInfo>>(edm::InputTag("addPileupInfo"));
  consumes<std::vector<reco::GenParticle>>(edm::InputTag("genParticles"));
  tokenRPLocalTrack = consumes< edm::DetSetVector<TotemRPLocalTrack> >(edm::InputTag("totemRPLocalTrackFitter"));

}


GammaGammaEE::~GammaGammaEE()
{

  //file->Write();
  //file->Close();

  delete _hlts;
  //delete tree;

}


//
// member functions
//

  void
GammaGammaEE::LookAtTriggersEE(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  Int_t trigNum;

  // Get the trigger information from the event
  iEvent.getByLabel(edm::InputTag("TriggerResults","",hltMenuLabel_),hltResults_) ; 
  const edm::TriggerNames & trigNames = iEvent.triggerNames(*hltResults_);

  for (unsigned int i=0; i<trigNames.size(); i++) {
    trigNum = _hlts->TriggerNum(trigNames.triggerNames().at(i));
    if (trigNum==-1) continue; // Trigger didn't match the interesting ones
    HLT_Accept[trigNum] = hltResults_->accept(i) ? 1 : 0;

  }
}

// ------------ method called for each event  ------------
  void
GammaGammaEE::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  // First initialization of the variables
  nCandidatesInEvent = 0;
  nPrimVertexCand = nFilteredPrimVertexCand = 0;
  nElectronCand = nLeptonCand = 0;
  nExtraTracks = nQualityExtraTrack = 0;
  nGenElectronCand = nGenElectronCandOutOfAccept = 0;
  nGenProtCand = 0;
  nLocalProtCand = 0;

  GenPair_p = GenPair_pt = GenPair_mass = GenPair_phi = GenPair_eta = -999.;
  GenPair_dphi = GenPair_dpt = GenPair_3Dangle = 0.;

  closesttrkdxyz = closesthighpuritytrkdxyz = 999.;
  closestkalmantrkdxyz = 999.;

  for (i=0; i<MAX_LL; i++) {
    ElectronCand_p[i] = ElectronCand_px[i] = ElectronCand_py[i] = ElectronCand_pz[i] = -999.;
    ElectronCand_pt[i] = ElectronCand_eta[i] = ElectronCand_phi[i] = -999.;
    ElectronCand_innerTrackPt[i] = ElectronCand_innerTrackEta[i] = ElectronCand_innerTrackPhi[i] = -999.;
    ElectronCand_charge[i] = -999;
    ElectronCand_vtxx[i] = ElectronCand_vtxy[i] = ElectronCand_vtxz[i] = -999.;
  }
  for (i=0; i<MAX_ET; i++) {
    ExtraTrack_p[i] = ExtraTrack_px[i] = ExtraTrack_py[i] = ExtraTrack_pz[i] = -999.;
    ExtraTrack_pt[i] = ExtraTrack_eta[i] = ExtraTrack_phi[i] = -999.;
    ExtraTrack_charge[i] = ExtraTrack_ndof[i] = -999;
    ExtraTrack_chi2[i] = ExtraTrack_vtxdxyz[i] = -999.;
    ExtraTrack_vtxT[i] = ExtraTrack_vtxZ[i] = -999.;
    ExtraTrack_x[i] = ExtraTrack_y[i] = ExtraTrack_z[i] = -999.;
  }
  for (i=0; i<MAX_PAIRS; i++) {
    Pair_candidates[i][0] = Pair_candidates[i][1] = -1;
    Pair_mindist[i] = Pair_p[i] = Pair_pt[i] = Pair_mass[i] = Pair_phi[i] = Pair_eta[i] = Pair_Y[i] = -999.;
    Pair_dphi[i] = Pair_dpt[i] = Pair_3Dangle[i] = -999.;
    Pair_extratracks1mm[i] = Pair_extratracks2mm[i] = Pair_extratracks3mm[i] = 0;
    Pair_extratracks4mm[i] = Pair_extratracks5mm[i] = Pair_extratracks1cm[i] = 0;
    Pair_extratracks2cm[i] = Pair_extratracks3cm[i] = Pair_extratracks4cm[i] = 0;
    Pair_extratracks5cm[i] = Pair_extratracks10cm[i] = 0;    
  }
  for (i=0; i<MAX_VTX; i++) {
    PrimVertexCand_id[i] = -1;
    PrimVertexCand_tracks[i] = PrimVertexCand_matchedtracks[i] = PrimVertexCand_unmatchedtracks[i] = 0;
    PrimVertexCand_x[i] = PrimVertexCand_y[i] = PrimVertexCand_z[i] = -999.;
    PrimVertexCand_chi2[i] = PrimVertexCand_ndof[i] = -999.;
  }
  for (i=0; i<MAX_LOCALPCAND; i++) {
    LocalProtCand_x[i] = -999.;
    LocalProtCand_y[i] = -999.;
    LocalProtCand_z[i] = -999.; 
    LocalProtCand_xSigma[nLocalProtCand] = -999.;
    LocalProtCand_ySigma[nLocalProtCand] = -999.;
    LocalProtCand_Tx[nLocalProtCand] = -999.;
    LocalProtCand_Ty[nLocalProtCand] = -999.;
    LocalProtCand_TxSigma[nLocalProtCand] = -999.;
    LocalProtCand_TySigma[nLocalProtCand] = -999.;
  }

  KalmanVertexCand_x = KalmanVertexCand_y = KalmanVertexCand_z = -999.;
  ClosestExtraTrackKalman_vtxdxyz = 999.;
  Weight = 1.;

  foundPairInEvent = false;
  foundGenCandPairInEvent = false;
  electronsMomenta.clear();
  _leptonptmp = new TLorentzVector();

  // Run and BX information
  BX = iEvent.bunchCrossing();
  Run = iEvent.id().run();
  LumiSection = iEvent.luminosityBlock();
  EventNum = iEvent.id().event();

  // High level trigger information retrieval  
  LookAtTriggersEE(iEvent, iSetup);

  // Get the vertex collection from the event
  iEvent.getByLabel(recoVertexLabel_, recoVertexColl);
  const reco::VertexCollection* vertices = recoVertexColl.product();

  // Generator level information
  if (runOnMC_) {
    iEvent.getByLabel(genLabel_, genPartColl);

    for (genPart=genPartColl->begin(); genPart!=genPartColl->end(); genPart++) {
      if (genPart->pt()<minPtMC_ || (minEtaMC_!=-1. && fabs(genPart->eta())>minEtaMC_)) {
	if (fabs(genPart->pdgId())==13) nGenElectronCandOutOfAccept++;
	if (fabs(genPart->pdgId())==22) nGenPhotCandOutOfAccept++;
	continue;
      }
      if (genPart->pdgId()!=2212 && genPart->status()!=1) continue;
      if (fabs(genPart->pdgId())==13 && nGenElectronCand<MAX_GENMU) {
	GenElectronCand_p[nGenElectronCand] = genPart->p();
	GenElectronCand_px[nGenElectronCand] = genPart->px();
	GenElectronCand_py[nGenElectronCand] = genPart->py();
	GenElectronCand_pz[nGenElectronCand] = genPart->pz();
	GenElectronCand_pt[nGenElectronCand] = genPart->pt();
	GenElectronCand_eta[nGenElectronCand] = genPart->eta();
	GenElectronCand_phi[nGenElectronCand] = genPart->phi();

	nGenElectronCand++;
      }

      if (genPart->pdgId()==2212 && nGenProtCand<MAX_GENPRO) { 
	GenProtCand_p[nGenProtCand] = genPart->p(); 
	GenProtCand_e[nGenProtCand] = genPart->energy();  
	GenProtCand_px[nGenProtCand] = genPart->px(); 
	GenProtCand_py[nGenProtCand] = genPart->py(); 
	GenProtCand_pz[nGenProtCand] = genPart->pz(); 
	GenProtCand_pt[nGenProtCand] = genPart->pt(); 
	GenProtCand_eta[nGenProtCand] = genPart->eta(); 
	GenProtCand_phi[nGenProtCand] = genPart->phi(); 
	GenProtCand_status[nGenProtCand] = genPart->status(); 

	double xi = 1 - ((genPart->energy())/(sqrts_/2.0)); 
	double t = -(std::pow(genPart->pt(), 2)); 

	GenProtCand_xi[nGenProtCand] = xi; 
	GenProtCand_t[nGenProtCand] = t; 

	nGenProtCand++; 
      } 


      foundGenCandPairInEvent = false;
      if (_fetchElectrons) { // Looks at dielectrons
	if(nGenElectronCand!=2) continue; // FIXME maybe a bit tight according to the newer PU conditions?
	l1.SetXYZM(GenElectronCand_px[0], GenElectronCand_py[0], GenElectronCand_pz[0], MASS_E);      	
	l2.SetXYZM(GenElectronCand_px[1], GenElectronCand_py[1], GenElectronCand_pz[1], MASS_E);      	
	foundGenCandPairInEvent = true;      	
      }
      if (foundGenCandPairInEvent) {
	pair = l1+l2;
	GenPair_p = pair.P();
	GenPair_pt = pair.Pt();
	GenPair_mass = pair.M();
	GenPair_phi = pair.Phi();
	GenPair_eta = pair.Eta();
	dphi = fabs(l1.Phi()-l2.Phi());
	GenPair_dphi = (dphi<pi) ? dphi : 2.*pi-dphi; // dphi lies in [-pi, pi]
	GenPair_dpt = fabs(l1.Pt()-l2.Pt());
	GenPair_3Dangle = (l1.Angle(l2.Vect()))/pi;
      }

    }
  }

  // Pileup information
  if (runOnMC_) {
    iEvent.getByLabel(pileupLabel_, pileupInfo);

    // This part is optional if the distributions are already generated
    sum_nvtx = 0;
    npv = npvtrue = npvm1true = npvp1true = npv0true = npv0 = 0;

    for(PVI=pileupInfo->begin(); PVI!=pileupInfo->end(); PVI++) {
      beamXing = PVI->getBunchCrossing();
      npv = PVI->getPU_NumInteractions();
      npvtrue = PVI->getTrueNumInteractions();
      sum_nvtx += npvtrue;

      if(beamXing == -1) npvm1true+=npvtrue;
      if(beamXing == 0) {
	npv0 += npv;
	npv0true += npvtrue;
      }
      if(beamXing == 1) npvp1true+=npvtrue;
    }
    nTruePUforPUWeightBX0 = npv0true;
    Weight = LumiWeights->weight( nTruePUforPUWeightBX0 );
  }

  std::vector<reco::TransientTrack> translepttrks;  
  reco::TrackCollection * lepttrks = new reco::TrackCollection;  
  edm::ESHandle<TransientTrackBuilder> theKalVtx;  
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theKalVtx);  
  iEvent.getByLabel(trackLabel_, trackColl);  

  // RP
  if(runOnProtons_)
  {
    // Placeholder for more awesome stuff

    edm::Handle< edm::DetSetVector<TotemRPLocalTrack> > rplocaltracks; 
    iEvent.getByToken(tokenRPLocalTrack, rplocaltracks); 

    for (auto &ds1 : *rplocaltracks)
    {
      for (auto &tr1 : ds1)
      {
	if (! tr1.isValid())
	  continue;

	LocalProtCand_x[nLocalProtCand] = (tr1.getX0())/1000.0;
	LocalProtCand_y[nLocalProtCand] = (tr1.getY0())/1000.0; 
	LocalProtCand_z[nLocalProtCand] = (tr1.getZ0())/1000.0; 
	LocalProtCand_xSigma[nLocalProtCand] = (tr1.getX0Sigma())/1000.0;
	LocalProtCand_ySigma[nLocalProtCand] = (tr1.getY0Sigma())/1000.0; 
	LocalProtCand_Tx[nLocalProtCand] = tr1.getTx();  
	LocalProtCand_Ty[nLocalProtCand] = tr1.getTy();   
	LocalProtCand_TxSigma[nLocalProtCand] = tr1.getTxSigma(); 
	LocalProtCand_TySigma[nLocalProtCand] = tr1.getTySigma();  

	nLocalProtCand++; 
      }
    }
  }

  // Get the electrons collection from the event
  if (_fetchElectrons) {

    //iEvent.getByLabel(electronLabel_, electronColl);

    edm::Handle<edm::View<pat::Electron> > electronColl;
    iEvent.getByToken(eleToken_, electronColl);

    edm::Handle<edm::ValueMap<bool> > loose_id_decisions;
    iEvent.getByToken(eleLooseIdMap_, loose_id_decisions);

    edm::Handle<edm::ValueMap<bool> > medium_id_decisions;
    iEvent.getByToken(eleMediumIdMap_, medium_id_decisions);

    edm::Handle<edm::ValueMap<bool> > tight_id_decisions;
    iEvent.getByToken(eleTightIdMap_, tight_id_decisions);

    for (unsigned int j=0; j<electronColl->size(); j++) {
      const auto electron = electronColl->ptrAt(j);
      ElectronCand_p[nElectronCand] = electron->p();
      ElectronCand_px[nElectronCand] = electron->px();
      ElectronCand_py[nElectronCand] = electron->py();
      ElectronCand_pz[nElectronCand] = electron->pz();
      ElectronCand_pt[nElectronCand] = electron->pt();
      ElectronCand_eta[nElectronCand] = electron->eta();
      ElectronCand_phi[nElectronCand] = electron->phi();
      ElectronCand_charge[nElectronCand] = electron->charge();

      if(electron->closestCtfTrackRef().isNonnull()) 
      {
	ElectronCand_innerTrackPt[nElectronCand] = electron->closestCtfTrackRef()->pt(); 
	ElectronCand_innerTrackEta[nElectronCand] = electron->closestCtfTrackRef()->eta();  
	ElectronCand_innerTrackPhi[nElectronCand] = electron->closestCtfTrackRef()->phi();  
	ElectronCand_innerTrackVtxz[nElectronCand] = electron->closestCtfTrackRef()->vertex().z();  
	_leptonptmp->SetXYZM(electron->closestCtfTrackRef()->px(), electron->closestCtfTrackRef()->py(), electron->closestCtfTrackRef()->pz(), electron->mass());

	// Use 2 highest pT electrons for vertexing
	if(nElectronCand < 2)
	{
	  lepttrks->push_back (*(electron->gsfTrack()));
	  reco::TransientTrack tmptrk = (*theKalVtx).build( *(electron->gsfTrack() ) ); 
	  translepttrks.push_back( tmptrk ); 
	}
      }
      else
      {
	_leptonptmp->SetXYZM(electron->px(), electron->py(), electron->pz(), electron->mass());
	ElectronCand_innerTrackPt[nElectronCand] = -999.;
	ElectronCand_innerTrackEta[nElectronCand] = -999.;
	ElectronCand_innerTrackPhi[nElectronCand] = -999.;
	ElectronCand_innerTrackVtxz[nElectronCand] = -999.;
      }

      electronsMomenta.insert(std::pair<Int_t,TLorentzVector>(nElectronCand, *_leptonptmp));

      ElectronCand_vtxx[nElectronCand] = electron->vertex().x();
      ElectronCand_vtxy[nElectronCand] = electron->vertex().y();
      ElectronCand_vtxz[nElectronCand] = electron->vertex().z();

      ElectronCand_deltaPhi[nElectronCand] = electron->deltaPhiSuperClusterTrackAtVtx();
      ElectronCand_deltaEta[nElectronCand] = electron->deltaEtaSuperClusterTrackAtVtx();
      ElectronCand_HoverE[nElectronCand] = electron->hcalOverEcal();
      ElectronCand_trackiso[nElectronCand] = electron->dr03TkSumPt() / electron->et();
      ElectronCand_ecaliso[nElectronCand] = electron->dr03EcalRecHitSumEt() / electron->et();
      ElectronCand_hcaliso[nElectronCand] = electron->dr03HcalTowerSumEt() / electron->et();
      ElectronCand_sigmaIetaIeta[nElectronCand] = electron->sigmaIetaIeta(); 
      ElectronCand_convDist[nElectronCand] = fabs(electron->convDist()); 
      ElectronCand_convDcot[nElectronCand] = fabs(electron->convDcot());
      ElectronCand_ecalDriven[nElectronCand] = electron->ecalDrivenSeed();

      /*
	 ElectronCand_looseID[nElectronCand] = (*loose_id_decisions)[electron];
	 ElectronCand_mediumID[nElectronCand] = (*medium_id_decisions)[electron];
	 ElectronCand_tightID[nElectronCand] = (*tight_id_decisions)[electron];
	 */

      nElectronCand++;

    }
  }

  // If 2 electrons, make a vertex and compute pair quantities from the leading 2
  // For now we do this "outside-in", rather than starting from the list of offlinePrimaryVertices and looking for electrons
  if(translepttrks.size() >= 2)
  {
    l1.SetXYZM(ElectronCand_px[0], 
	ElectronCand_py[0],
	ElectronCand_pz[0],
	MASS_E); 
    l2.SetXYZM(ElectronCand_px[1],
	ElectronCand_py[1],
	ElectronCand_pz[1],
	MASS_E); 

    pair = l1+l2; 

    Pair_p[0] = pair.P(); 
    Pair_pt[0] = pair.Pt(); 
    Pair_mass[0] = pair.M(); 
    Pair_phi[0] = pair.Phi(); 
    Pair_eta[0] = pair.Eta(); 
    Pair_Y[0] = pair.Rapidity();
    dphi = fabs(l1.Phi()-l2.Phi()); 
    Pair_dphi[0] = (dphi<pi) ? dphi : 2.*pi-dphi; // dphi lies in [-pi, pi] 
    Pair_dpt[0] = fabs(l1.Pt()-l2.Pt()); 
    Pair_3Dangle[0] = (l1.Angle(l2.Vect()))/pi; 
    nPrimVertexCand++;

    KalmanVertexFitter fitter(true); 
    TransientVertex dileptonVertex = fitter.vertex(translepttrks); 
    if(dileptonVertex.isValid()) { 
      foundPairInEvent = true;
      KalmanVertexCand_x = dileptonVertex.position().x(); 
      KalmanVertexCand_y = dileptonVertex.position().y(); 
      KalmanVertexCand_z = dileptonVertex.position().z(); 

      etind = 0; 

      //  Count nearby tracks
      for ( track = trackColl->begin(); track != trackColl->end() && etind<MAX_ET; ++track ) 
      {
	if((track->pt() == ElectronCand_innerTrackPt[0]) || (track->pt() == ElectronCand_innerTrackPt[1]))
	  continue;

	vtxdst = sqrt(std::pow((track->vertex().x()-KalmanVertexCand_x),2)+ 
	    std::pow((track->vertex().y()-KalmanVertexCand_y),2)+ 
	    std::pow((track->vertex().z()-KalmanVertexCand_z),2)); 

	nExtraTracks++;

	if (vtxdst<0.1) Pair_extratracks1mm[0]++; 
	if (vtxdst<0.2) Pair_extratracks2mm[0]++; 
	if (vtxdst<0.3) Pair_extratracks3mm[0]++; 
	if (vtxdst<0.4) Pair_extratracks4mm[0]++; 
	if (vtxdst<0.5) Pair_extratracks5mm[0]++; 
	if (vtxdst<1.0) Pair_extratracks1cm[0]++; 
	if (vtxdst<2.0) Pair_extratracks2cm[0]++; 
	if (vtxdst<3.0) Pair_extratracks3cm[0]++; 
	if (vtxdst<4.0) Pair_extratracks4cm[0]++; 
	if (vtxdst<5.0) Pair_extratracks5cm[0]++; 
	if (vtxdst<10.) Pair_extratracks10cm[0]++; 
	if (vtxdst<closesttrkdxyz) { 
	  closesttrkdxyz = vtxdst; 
	} 
	if (track->quality(reco::TrackBase::highPurity)==1) { 
	  if (vtxdst<closesthighpuritytrkdxyz) { 
	    closesthighpuritytrkdxyz = vtxdst; 
	  } 
	} 

	// Save track properties if within 5mm
	if(vtxdst<0.5) 
	{
	  ExtraTrack_purity[etind] = track->quality(reco::TrackBase::highPurity);  
	  ExtraTrack_nhits[etind] = track->numberOfValidHits();  

	  ExtraTrack_p[etind] = track->p();  
	  ExtraTrack_px[etind] = track->px();  
	  ExtraTrack_py[etind] = track->py();  
	  ExtraTrack_pz[etind] = track->pz();  
	  ExtraTrack_pt[etind] = track->pt();  
	  ExtraTrack_eta[etind] = track->eta();  
	  ExtraTrack_phi[etind] = track->phi();  
	  ExtraTrack_charge[etind] = track->charge();  
	  ExtraTrack_chi2[etind] = track->chi2();  
	  ExtraTrack_ndof[etind] = track->ndof();  
	  ExtraTrack_vtxdxyz[etind] = vtxdst;  
	  ExtraTrack_vtxT[etind] = sqrt(std::pow(track->vertex().x()-KalmanVertexCand_x,2)+std::pow(track->vertex().y()-KalmanVertexCand_y,2));  
	  ExtraTrack_vtxZ[etind] = fabs(track->vertex().z()-KalmanVertexCand_z);
	  ExtraTrack_x[etind] = track->vertex().x();  
	  ExtraTrack_y[etind] = track->vertex().y();  
	  ExtraTrack_z[etind] = track->vertex().z();  
	  etind++; 
	}

      }
      ClosestExtraTrack_vtxdxyz = closesttrkdxyz; 
      ClosestHighPurityExtraTrack_vtxdxyz = closesthighpuritytrkdxyz;
    } 
  }

  for (vertex=vertices->begin(); vertex!=vertices->end() && nPrimVertexCand<MAX_VTX; ++vertex) 
  { 
    PrimVertexCand_id[nPrimVertexCand] = nPrimVertexCand; 
    PrimVertexCand_x[nPrimVertexCand] = vertex->x(); 
    PrimVertexCand_y[nPrimVertexCand] = vertex->y(); 
    PrimVertexCand_z[nPrimVertexCand] = vertex->z(); 
    PrimVertexCand_tracks[nPrimVertexCand] = vertex->nTracks(); 
    PrimVertexCand_chi2[nPrimVertexCand] = vertex->chi2(); 
    PrimVertexCand_ndof[nPrimVertexCand] = vertex->ndof(); 
    nPrimVertexCand++;
  }

  delete _leptonptmp;

  if(foundPairInEvent){
    tree->Fill();
    nCandidates++;
  }
}


// ------------ method called once each job just before starting event loop  ------------
  void 
GammaGammaEE::beginJob()
{

  edm::Service<TFileService> file;
  tree = file->make<TTree>("ntp1", "ntp1");

  tree->Branch("Run", &Run, "Run/I");
  tree->Branch("LumiSection", &LumiSection, "LumiSection/I");
  tree->Branch("BX", &BX, "BX/I");
  tree->Branch("EventNum", &EventNum, "EventNum/I");
  tree->Branch("nHLT", &nHLT, "nHLT/I");
  tree->Branch("HLT_Accept", HLT_Accept, "HLT_Prescl[nHLT]/I");
  tree->Branch("HLT_Prescl", HLT_Prescl, "HLT_Prescl[nHLT]/I");

  if (_fetchElectrons) {
    tree->Branch("nElectronCand", &nElectronCand, "nElectronCand/I");
    tree->Branch("ElectronCand_px", ElectronCand_px, "ElectronCand_px[nElectronCand]/D");
    tree->Branch("ElectronCand_py", ElectronCand_py, "ElectronCand_py[nElectronCand]/D");
    tree->Branch("ElectronCand_pz", ElectronCand_pz, "ElectronCand_pz[nElectronCand]/D");
    tree->Branch("ElectronCand_p", ElectronCand_p, "ElectronCand_p[nElectronCand]/D");
    tree->Branch("ElectronCand_pt", ElectronCand_pt, "ElectronCand_pt[nElectronCand]/D");
    tree->Branch("ElectronCand_eta", ElectronCand_eta, "ElectronCand_eta[nElectronCand]/D");
    tree->Branch("ElectronCand_phi", ElectronCand_phi, "ElectronCand_phi[nElectronCand]/D");
    tree->Branch("ElectronCand_charge", ElectronCand_charge, "ElectronCand_charge[nElectronCand]/I");
    tree->Branch("ElectronCand_vtxx", ElectronCand_vtxx, "ElectronCand_vtxx[nElectronCand]/D");
    tree->Branch("ElectronCand_vtxy", ElectronCand_vtxy, "ElectronCand_vtxy[nElectronCand]/D");
    tree->Branch("ElectronCand_vtxz", ElectronCand_vtxz, "ElectronCand_vtxz[nElectronCand]/D");
    tree->Branch("ElectronCand_innerTrackPt", ElectronCand_innerTrackPt, "ElectronCand_innerTrackPt[nElectronCand]/D");
    tree->Branch("ElectronCand_innerTrackEta", ElectronCand_innerTrackEta, "ElectronCand_innerTrackEta[nElectronCand]/D"); 
    tree->Branch("ElectronCand_innerTrackPhi", ElectronCand_innerTrackPhi, "ElectronCand_innerTrackPhi[nElectronCand]/D"); 
    tree->Branch("ElectronCand_innerTrackVtxz", ElectronCand_innerTrackVtxz, "ElectronCand_innerTrackVtxz[nElectronCand]/D");
    tree->Branch("ElectronCand_deltaPhi", ElectronCand_deltaPhi, "ElectronCand_deltaPhi[nElectronCand]/D"); 
    tree->Branch("ElectronCand_deltaEta", ElectronCand_deltaEta, "ElectronCand_deltaEta[nElectronCand]/D"); 
    tree->Branch("ElectronCand_HoverE", ElectronCand_HoverE, "ElectronCand_HoverE[nElectronCand]/D"); 
    tree->Branch("ElectronCand_trackiso", ElectronCand_trackiso, "ElectronCand_trackiso[nElectronCand]/D"); 
    tree->Branch("ElectronCand_ecaliso", ElectronCand_ecaliso, "ElectronCand_ecaliso[nElectronCand]/D"); 
    tree->Branch("ElectronCand_hcaliso", ElectronCand_hcaliso, "ElectronCand_hcaliso[nElectronCand]/D"); 
    tree->Branch("ElectronCand_sigmaIetaIeta", ElectronCand_sigmaIetaIeta, "ElectronCand_sigmaIetaIeta[nElectronCand]/D"); 
    tree->Branch("ElectronCand_convDist", ElectronCand_convDist, "ElectronCand_convDist[nElectronCand]/D"); 
    tree->Branch("ElectronCand_convDcot", ElectronCand_convDcot, "ElectronCand_convDcot[nElectronCand]/D"); 
    tree->Branch("ElectronCand_ecalDriven", ElectronCand_ecalDriven, "ElectronCand_ecalDriven[nElectronCand]/D"); 
    /*
       tree->Branch("ElectronCand_tightID", ElectronCand_tightID, "ElectronCand_tightID[nElectronCand]/D"); 
       tree->Branch("ElectronCand_mediumID", ElectronCand_mediumID, "ElectronCand_mediumID[nElectronCand]/D"); 
       tree->Branch("ElectronCand_looseID", ElectronCand_looseID, "ElectronCand_looseID[nElectronCand]/D");  
       */
    if (runOnMC_) {
      tree->Branch("nGenElectronCand", &nGenElectronCand, "nGenElectronCand/I");
      tree->Branch("nGenElectronCandOutOfAccept", &nGenElectronCandOutOfAccept, "nGenElectronCandOutOfAccept/I");    
      tree->Branch("GenElectronCand_p", GenElectronCand_p, "GenElectronCand_p[nGenElectronCand]/D");
      tree->Branch("GenElectronCand_px", GenElectronCand_px, "GenElectronCand_px[nGenElectronCand]/D");
      tree->Branch("GenElectronCand_py", GenElectronCand_py, "GenElectronCand_py[nGenElectronCand]/D");
      tree->Branch("GenElectronCand_pz", GenElectronCand_pz, "GenElectronCand_pz[nGenElectronCand]/D");
      tree->Branch("GenElectronCand_pt", GenElectronCand_pt, "GenElectronCand_pt[nGenElectronCand]/D");
      tree->Branch("GenElectronCand_eta", GenElectronCand_eta, "GenElectronCand_eta[nGenElectronCand]/D");
      tree->Branch("GenElectronCand_phi", GenElectronCand_phi, "GenElectronCand_phi[nGenElectronCand]/D");
      tree->Branch("nGenProtCand", &nGenProtCand, "nGenProtCand/I");     
      tree->Branch("GenProtCand_p", GenProtCand_p, "GenProtCand_p[nGenProtCand]/D"); 
      tree->Branch("GenProtCand_px", GenProtCand_px, "GenProtCand_px[nGenProtCand]/D"); 
      tree->Branch("GenProtCand_py", GenProtCand_py, "GenProtCand_py[nGenProtCand]/D"); 
      tree->Branch("GenProtCand_pz", GenProtCand_pz, "GenProtCand_pz[nGenProtCand]/D"); 
      tree->Branch("GenProtCand_e", GenProtCand_e, "GenProtCand_e[nGenProtCand]/D");  
      tree->Branch("GenProtCand_pt", GenProtCand_pt, "GenProtCand_pt[nGenProtCand]/D"); 
      tree->Branch("GenProtCand_eta", GenProtCand_eta, "GenProtCand_eta[nGenProtCand]/D"); 
      tree->Branch("GenProtCand_phi", GenProtCand_phi, "GenProtCand_phi[nGenProtCand]/D"); 
      tree->Branch("GenProtCand_status", GenProtCand_status, "GenProtCand_status[nGenProtCand]/I"); 
      tree->Branch("GenProtCand_xi", GenProtCand_xi, "GenProtCand_xi[nGenProtCand]/D");   
      tree->Branch("GenProtCand_t", GenProtCand_t, "GenProtCand_t[nGenProtCand]/D");   
    }
  }
  if(runOnProtons_){
    tree->Branch("nLocalProtCand", &nLocalProtCand, "nLocalProtCand/I");
    tree->Branch("LocalProtCand_x", LocalProtCand_x, "LocalProtCand_x[nLocalProtCand]/D");
    tree->Branch("LocalProtCand_y", LocalProtCand_y, "LocalProtCand_y[nLocalProtCand]/D"); 
    tree->Branch("LocalProtCand_z", LocalProtCand_z, "LocalProtCand_z[nLocalProtCand]/D"); 
    tree->Branch("LocalProtCand_xSigma", LocalProtCand_xSigma, "LocalProtCand_xSigma[nLocalProtCand]/D");
    tree->Branch("LocalProtCand_ySigma", LocalProtCand_ySigma, "LocalProtCand_ySigma[nLocalProtCand]/D"); 
    tree->Branch("LocalProtCand_Tx", LocalProtCand_Tx, "LocalProtCand_Tx[nLocalProtCand]/D");
    tree->Branch("LocalProtCand_Ty", LocalProtCand_Ty, "LocalProtCand_Ty[nLocalProtCand]/D"); 
    tree->Branch("LocalProtCand_TxSigma", LocalProtCand_TxSigma, "LocalProtCand_TxSigma[nLocalProtCand]/D"); 
    tree->Branch("LocalProtCand_TySigma", LocalProtCand_TySigma, "LocalProtCand_TySigma[nLocalProtCand]/D");  
  }

  // Primary vertices information
  tree->Branch("nPrimVertexCand", &nPrimVertexCand, "nPrimVertexCand/I");
  tree->Branch("PrimVertexCand_id", PrimVertexCand_id, "PrimVertexCand_id[nPrimVertexCand]/I");
  tree->Branch("PrimVertexCand_x", PrimVertexCand_x, "PrimVertexCand_x[nPrimVertexCand]/D");
  tree->Branch("PrimVertexCand_y", PrimVertexCand_y, "PrimVertexCand_y[nPrimVertexCand]/D");
  tree->Branch("PrimVertexCand_z", PrimVertexCand_z, "PrimVertexCand_z[nPrimVertexCand]/D");
  tree->Branch("PrimVertexCand_tracks", PrimVertexCand_tracks, "PrimVertexCand_tracks[nPrimVertexCand]/I");
  tree->Branch("PrimVertexCand_matchedtracks", PrimVertexCand_matchedtracks, "PrimVertexCand_matchedtracks[nPrimVertexCand]/I");
  tree->Branch("PrimVertexCand_unmatchedtracks", PrimVertexCand_unmatchedtracks, "PrimVertexCand_unmatchedtracks[nPrimVertexCand]/I");
  tree->Branch("PrimVertexCand_chi2", PrimVertexCand_chi2, "PrimVertexCand_chi2[nPrimVertexCand]/D");
  tree->Branch("PrimVertexCand_ndof", PrimVertexCand_ndof, "PrimVertexCand_ndof[nPrimVertexCand]/I");
  tree->Branch("nFilteredPrimVertexCand", &nFilteredPrimVertexCand, "nFilteredPrimVertexCand/I");

  // Kalman dilepton vertex information
  tree->Branch("KalmanVertexCand_x", &KalmanVertexCand_x, "KalmanVertexCand_x/D");
  tree->Branch("KalmanVertexCand_y", &KalmanVertexCand_y, "KalmanVertexCand_y/D");
  tree->Branch("KalmanVertexCand_z", &KalmanVertexCand_z, "KalmanVertexCand_z/D");
  tree->Branch("ClosestExtraTrackKalman_vtxdxyz", &ClosestExtraTrackKalman_vtxdxyz, "ClosestExtraTrackKalman_vtxdxyz/D");

  // Lepton pairs' information
  tree->Branch("Pair_candidates", Pair_candidates, "Pair_candidates[nPrimVertexCand][2]/I");
  tree->Branch("Pair_mindist", Pair_mindist, "Pair_mindist[nPrimVertexCand]/D");
  tree->Branch("Pair_p", Pair_p, "Pair_p[nPrimVertexCand]/D");
  tree->Branch("Pair_pt", Pair_pt, "Pair_pt[nPrimVertexCand]/D");
  tree->Branch("Pair_dpt", Pair_dpt, "Pair_dpt[nPrimVertexCand]/D");
  tree->Branch("Pair_mass", Pair_mass, "Pair_mass[nPrimVertexCand]/D");
  tree->Branch("Pair_eta", Pair_eta, "Pair_eta[nPrimVertexCand]/D");
  tree->Branch("Pair_phi", Pair_phi, "Pair_phi[nPrimVertexCand]/D");
  tree->Branch("Pair_dphi", Pair_dphi, "Pair_dphi[nPrimVertexCand]/D");
  tree->Branch("Pair_3Dangle", Pair_3Dangle, "Pair_3Dangle[nPrimVertexCand]/D");
  tree->Branch("Pair_Y", Pair_Y, "Pair_Y[nPrimVertexCand]/D"); 
  tree->Branch("Pair_extratracks1mm", Pair_extratracks1mm, "Pair_extratracks1mm[nPrimVertexCand]/I");
  tree->Branch("Pair_extratracks2mm", Pair_extratracks2mm, "Pair_extratracks2mm[nPrimVertexCand]/I");
  tree->Branch("Pair_extratracks3mm", Pair_extratracks3mm, "Pair_extratracks3mm[nPrimVertexCand]/I");
  tree->Branch("Pair_extratracks4mm", Pair_extratracks4mm, "Pair_extratracks4mm[nPrimVertexCand]/I");
  tree->Branch("Pair_extratracks5mm", Pair_extratracks5mm, "Pair_extratracks5mm[nPrimVertexCand]/I");
  tree->Branch("Pair_extratracks1cm", Pair_extratracks1cm, "Pair_extratracks1cm[nPrimVertexCand]/I");
  tree->Branch("Pair_extratracks2cm", Pair_extratracks2cm, "Pair_extratracks2cm[nPrimVertexCand]/I");
  tree->Branch("Pair_extratracks3cm", Pair_extratracks3cm, "Pair_extratracks3cm[nPrimVertexCand]/I");
  tree->Branch("Pair_extratracks4cm", Pair_extratracks4cm, "Pair_extratracks4cm[nPrimVertexCand]/I");
  tree->Branch("Pair_extratracks5cm", Pair_extratracks5cm, "Pair_extratracks5cm[nPrimVertexCand]/I");
  tree->Branch("Pair_extratracks10cm", Pair_extratracks10cm, "Pair_extratracks10cm[nPrimVertexCand]/I");
  if (runOnMC_) {
    tree->Branch("GenPair_p", &GenPair_p, "GenPair_p/D");
    tree->Branch("GenPair_pt", &GenPair_pt, "GenPair_pt/D");
    tree->Branch("GenPair_dpt", &GenPair_dpt, "GenPair_dpt/D");
    tree->Branch("GenPair_mass", &GenPair_mass, "GenPair_mass/D");
    tree->Branch("GenPair_eta", &GenPair_eta, "GenPair_eta/D");
    tree->Branch("GenPair_phi", &GenPair_phi, "GenPair_phi/D");
    tree->Branch("GenPair_dphi", &GenPair_dphi, "GenPair_dphi/D");
    tree->Branch("GenPair_3Dangle", &GenPair_3Dangle, "GenPair_3Dangle[nPrimVertexCand]/D");
  }
  // Extra tracks on vertex's information
  tree->Branch("nExtraTracks", &nExtraTracks, "nExtraTracks/I");
  tree->Branch("ExtraTrack_purity", ExtraTrack_purity, "ExtraTrack_purity[nExtraTracks]/I");
  tree->Branch("ExtraTrack_nhits", ExtraTrack_nhits, "ExtraTrack_nhits[nExtraTracks]/I");
  tree->Branch("ExtraTrack_charge", ExtraTrack_charge, "ExtraTrack_charge[nExtraTracks]/I");
  tree->Branch("ExtraTrack_ndof", ExtraTrack_ndof, "ExtraTrack_ndof[nExtraTracks]/I");
  tree->Branch("ExtraTrack_p", ExtraTrack_p, "ExtraTrack_p[nExtraTracks]/D");
  tree->Branch("ExtraTrack_pt", ExtraTrack_pt, "ExtraTrack_pt[nExtraTracks]/D");
  tree->Branch("ExtraTrack_px", ExtraTrack_px, "ExtraTrack_px[nExtraTracks]/D");
  tree->Branch("ExtraTrack_py", ExtraTrack_py, "ExtraTrack_py[nExtraTracks]/D");
  tree->Branch("ExtraTrack_pz", ExtraTrack_pz, "ExtraTrack_pz[nExtraTracks]/D");
  tree->Branch("ExtraTrack_eta", ExtraTrack_eta, "ExtraTrack_eta[nExtraTracks]/D");
  tree->Branch("ExtraTrack_phi", ExtraTrack_phi, "ExtraTrack_phi[nExtraTracks]/D");
  tree->Branch("ExtraTrack_chi2", ExtraTrack_chi2, "ExtraTrack_chi2[nExtraTracks]/D");
  tree->Branch("ExtraTrack_vtxdxyz", ExtraTrack_vtxdxyz, "ExtraTrack_vtxdxyz[nExtraTracks]/D");
  tree->Branch("ExtraTrack_vtxT", ExtraTrack_vtxT, "ExtraTrack_vtxT[nExtraTracks]/D");
  tree->Branch("ExtraTrack_vtxZ", ExtraTrack_vtxZ, "ExtraTrack_vtxZ[nExtraTracks]/D");
  tree->Branch("ExtraTrack_x", ExtraTrack_x, "ExtraTrack_x[nExtraTracks]/D");
  tree->Branch("ExtraTrack_y", ExtraTrack_y, "ExtraTrack_y[nExtraTracks]/D");
  tree->Branch("ExtraTrack_z", ExtraTrack_z, "ExtraTrack_z[nExtraTracks]/D");
  tree->Branch("nQualityExtraTrack", &nQualityExtraTrack, "nQualityExtraTrack/I");
  tree->Branch("ClosestExtraTrack_vtxdxyz",&ClosestExtraTrack_vtxdxyz,"ClosestExtraTrack_vtxdxyz/D");
  tree->Branch("ClosestHighPurityExtraTrack_vtxdxyz",&ClosestHighPurityExtraTrack_vtxdxyz,"ClosestHighPurityExtraTrack_vtxdxyz/D");
  tree->Branch("ClosestTrack_vtxdxyz",&ClosestTrack_vtxdxyz,"ClosestTrack_vtxdxyz/D");

  // Pileup reweighting
  tree->Branch("nTruePUforPUWeight",&nTruePUforPUWeight,"nTruePUforPUWeight/I");
  tree->Branch("nTruePUafterPUWeight",&nTruePUafterPUWeight,"nTruePUafterPUWeight/D");
  tree->Branch("nTruePUforPUWeightBXM1", &nTruePUforPUWeightBXM1, "nTruePUforPUWeightBXM1/I");
  tree->Branch("nTruePUafterPUWeightBXM1", &nTruePUafterPUWeightBXM1, "nTruePUafterPUWeightBXM1/D");
  tree->Branch("nTruePUforPUWeightBXP1", &nTruePUforPUWeightBXP1, "nTruePUforPUWeightBXP1/I"); 
  tree->Branch("nTruePUafterPUWeightBXP1", &nTruePUafterPUWeightBXP1, "nTruePUafterPUWeightBXP1/D"); 
  tree->Branch("nTruePUforPUWeightBX0", &nTruePUforPUWeightBX0, "nTruePUforPUWeightBX0/I");
  tree->Branch("nTruePUafterPUWeightBX0", &nTruePUafterPUWeightBX0, "nTruePUafterPUWeightBX0/D");
  tree->Branch("Weight", &Weight, "Weight/D");
  tree->Branch("PUWeightTrue", &PUWeightTrue, "PUWeightTrue/D");
  nCandidates = 0;

}

// ------------ method called once each job just after ending the event loop  ------------
  void 
GammaGammaEE::endJob() 
{
  std::cout << "==> Number of candidates in the dataset : " << nCandidates << std::endl;
}

// ------------ method called when starting to processes a run  ------------
  void 
GammaGammaEE::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{

  bool changed;
  std::string triggerName_;

  changed = true;
  if (hltConfig_.init(iRun, iSetup, hltMenuLabel_, changed)) {
    if (changed) {
      // check if trigger name in (new) config
      triggerName_ = "HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v*";
      if (triggerName_!="@") { // "@" means: analyze all triggers in config
	const UInt_t n = hltConfig_.size();
	const UInt_t triggerIndex = hltConfig_.triggerIndex(triggerName_);
	if (triggerIndex>=n) {
	  std::cout << "GammaGammaEE::analyze:"
	    << " TriggerName " << triggerName_ 
	    << " not available in (new) config!"
	    << std::endl;
	}
      }
    }
  }
  else {
    std::cout << "GammaGammaEE::beginRun:"
      << " config extraction failure with process name "
      << hltMenuLabel_
      << std::endl;
  }
}

// ------------ method called when ending the processing of a run  ------------
  void 
GammaGammaEE::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
  void 
GammaGammaEE::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
  void 
GammaGammaEE::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
GammaGammaEE::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


//define this as a plug-in
DEFINE_FWK_MODULE(GammaGammaEE);
