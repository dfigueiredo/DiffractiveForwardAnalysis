// system include files
#include <memory>
#include <map>
#include <vector>

// // user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/Registry.h"

// Vertices collection
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Common/interface/RefToBase.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"

#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TLorentzVector.h>

class PrimaryVertex : public reco::Vertex {
  public:
    explicit PrimaryVertex(std::vector<std::string>&, std::map<Int_t,TLorentzVector>&, std::map<Int_t,TLorentzVector>&);
    ~PrimaryVertex();
    void SetPositionMuMu(Double_t, Double_t, Double_t);
    Int_t AddTrackMuMu(const reco::TrackRef&, TString&);
    inline Int_t Electrons() { return nMatchedElectrons; }
    inline Int_t Muons() { return nMatchedMuons; }
    Double_t dZ(TVector3, Int_t);
    TVector3 Position;
    Int_t nTracks, nMatchedTracks, nUnmatchedTracks;
    std::vector<Int_t> MatchedMuons, MatchedElectrons;
  private:
    UInt_t i;
    Int_t nMatchedMuons, nMatchedElectrons;
    bool FetchMuons, FetchElectrons;
    std::map<Int_t,TLorentzVector> MuonMomenta;
    std::map<Int_t,TLorentzVector> ElectronMomenta;
};
