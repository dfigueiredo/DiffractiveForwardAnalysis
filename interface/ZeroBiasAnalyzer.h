#ifndef DiffractiveForwardAnalysis_ZeroBiasAnalyzer
#define DiffractiveForwardAnalysis_ZeroBiasAnalyzer

// user include files
#include <FWCore/Framework/interface/Frameworkfwd.h>
#include <FWCore/Framework/interface/EDAnalyzer.h>
#include <FWCore/Framework/interface/EDFilter.h>
#include <FWCore/ParameterSet/interface/ParameterSet.h>
#include <FWCore/ParameterSet/interface/ConfigurationDescriptions.h>  
#include <FWCore/ParameterSet/interface/ParameterSetDescription.h> 
#include <FWCore/ParameterSet/interface/ParameterDescriptionNode.h> 
#include <FWCore/Framework/interface/Event.h>
#include "FWCore/Utilities/interface/InputTag.h" 

#include "DataFormats/Common/interface/TriggerResults.h" 
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"  
#include "FWCore/Common/interface/TriggerNames.h" 

#include <TFile.h>
#include <TH1D.h>
#include <TTree.h>
#include <TLorentzVector.h>

class ZeroBiasAnalyzer : public edm::EDAnalyzer {
 public:
  explicit ZeroBiasAnalyzer(const edm::ParameterSet&);
  ~ZeroBiasAnalyzer();
  
  
 private:
  virtual void beginJob();
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
  
  // ----------member data ---------------------------
  
  edm::InputTag recTrackLabel;
  edm::InputTag theCaloTowLabel;
  edm::InputTag recCastorTowerLabel;
  edm::InputTag recZDCRecHitsLabel;
  edm::InputTag recCastorRecHitsLabel;

  std::string rootfilename;

  TFile *thefile;
  TTree *thetree;

  int nTrackCand;
  int nQualityTrackCand;
  int TRACKMAX;// used to set maximum of arrays
  int CALOMAX;
  double TrackCand_purity[500];
  int TrackCand_nhits[500];
  double TrackCand_z[500];
  double TrackCand_x[500]; 
  double TrackCand_y[500]; 
  double TrackCand_px[500];
  double TrackCand_py[500];
  double TrackCand_pz[500];
  double TrackCand_p[500];
  double TrackCand_e[500];
  double TrackCand_pt[500];
  double TrackCand_phi[500];
  double TrackCand_eta[500];
  double TrackCand_chi2[500];
  double TrackCand_ndof[500];
  int TrackCand_charge[500];

  int nCaloCand;
  int nExtraCaloTowersE0pt6eb, nExtraCaloTowersE2pt45ee, nExtraCaloTowersE1pt25hb, nExtraCaloTowersE1pt9he, nExtraCaloTowersE4pt5hfp, nExtraCaloTowersE4pt0hfm; 
  int nExtraCaloTowersE1, nExtraCaloTowersE2, nExtraCaloTowersE3, nExtraCaloTowersE4, nExtraCaloTowersE5, nExtraCaloTowersE6, nExtraCaloTowersE7, nExtraCaloTowersE8, nExtraCaloTowersE9;  
  int nExtraCaloTowersEt0pt1, nExtraCaloTowersEt0pt2, nExtraCaloTowersEt0pt5, nExtraCaloTowersEt1, nExtraCaloTowersEt2, nExtraCaloTowersEt3, nExtraCaloTowersEt4;  
  double CaloTower_e[1000];
  double CaloTower_et[1000];
  double CaloTower_eta[1000];
  double CaloTower_phi[1000];
  double CaloTower_emE[1000];
  double CaloTower_hadE[1000];
  double CaloTower_outE[1000];
  int CaloTower_ID[1000];
  double CaloTower_x[1000];
  double CaloTower_y[1000];
  double CaloTower_z[1000];
  double CaloTower_t[1000];
  int CaloTower_badhcalcells[1000];
  int CaloTower_problemhcalcells[1000];
  int CaloTower_badecalcells[1000];
  int CaloTower_problemecalcells[1000];
  double HighestCaloTower_e;
  double HighestCaloTower_eta;
  double HighestCaloTower_phi;
  double HighestEtCaloTower_et;
  double HighestEtCaloTower_eta;
  double HighestEtCaloTower_phi;
  double SumCalo_e;
  double SumHFPlus_e;
  double SumHFMinus_e;

  int nCastorTowerCand;
  double CastorTower_e[50];
  double CastorTower_eta[50];
  double CastorTower_phi[50];
  double CastorTower_emratio[50];
  double HighestCastorTowerFwd_e;
  double HighestCastorTowerBwd_e;
  double SumCastorFwd_e;
  double SumCastorBwd_e;

  int nZDChitCand;
  int ZDChit_section[50];
  double ZDChit_energy[50];
  double ZDChit_time[50];
  int ZDChit_side[50];
  double ZDCsumEMplus;
  double ZDCsumHADplus;
  double ZDCsumEMminus;
  double ZDCsumHADminus;
  double CASTORsumRecHitsE;

  int nVertexCand;
  double VertexCand_x[10];
  double VertexCand_y[10];
  double VertexCand_z[10];
  int VertexCand_tracks[10];
  double VertexCand_chi2[10];
  double VertexCand_ndof[10];

  int BX;
  int Run;
  int LumiSection;
  int EventNum; 
  double AvgInstDelLumi; 
  double BunchInstLumi[3];

  int L1TechnicalTriggers[128];

  edm::TriggerNames trigNames ;
};
#endif
