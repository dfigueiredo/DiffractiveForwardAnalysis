#include "PrimaryVertex.h"

// Class PrimaryVertex
// constructors and destructor

PrimaryVertex::PrimaryVertex(std::vector<std::string>& _leptonsType, std::map<Int_t,TLorentzVector>& _muonsMomenta, std::map<Int_t,TLorentzVector>& _electronsMomenta) :
  nTracks(0),
  nMatchedTracks(0),
  nUnmatchedTracks(0),
  nMatchedMuons(0),
  nMatchedElectrons(0),
  FetchMuons(false),
  FetchElectrons(false)
{
  //LeptonsType = _leptonsType;
  for (i=0; i<_leptonsType.size(); i++) {
    if (_leptonsType[i]=="Muon") {
      FetchMuons = true;
    }
    else if (_leptonsType[i]=="Electron") {
      FetchElectrons = true;
    }
  }
  MuonMomenta = _muonsMomenta;
  ElectronMomenta = _electronsMomenta;
}

PrimaryVertex::~PrimaryVertex() {
}

  void
PrimaryVertex::SetPositionMuMu(Double_t _x, Double_t _y, Double_t _z)
{
  Position.SetXYZ(_x, _y, _z);
#ifdef DEBUG
  std::cout << "[PrimaryVertex] SetPosition :: Vertex located at (" << Position.x() << ", " << Position.y() << ", " << Position.z() << ")" << std::endl;
#endif
}

  Int_t
PrimaryVertex::AddTrackMuMu(const reco::TrackRef& _track, TString& _leptonType)
{
  nTracks++; // total number of tracks matched with the vertex
  std::map<Int_t,TLorentzVector>::iterator lep;
  for (lep=MuonMomenta.begin(); lep!=MuonMomenta.end(); lep++) {
    if (fabs(_track->p()-lep->second.P())>.01) continue;
    if (fabs(_track->pt()-lep->second.Pt())>.01) continue;
    if (fabs(_track->eta()-lep->second.Eta())>.01) continue;
    if (fabs(_track->phi()-lep->second.Phi())>.01) continue;
    _leptonType = "muon";
    MatchedMuons.push_back(lep->first);
    nMatchedMuons++;
    nMatchedTracks++;
    return lep->first;
  }

  nUnmatchedTracks++;
  return -1;
}

  Double_t
PrimaryVertex::dZ(TVector3 _vmu, Int_t _muind)
{
  TLorentzVector m(MuonMomenta[_muind]);
  return (_vmu.Z()-Position.Z())-((_vmu.X()-Position.X())*m.Px()+(_vmu.Y()-Position.Y())*m.Py())/m.Pt()*m.Pz()/m.Pt();
}
