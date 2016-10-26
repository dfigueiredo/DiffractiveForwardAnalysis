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

#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TLorentzVector.h>

// HLT information
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

class HLTmatches {
  public:
    explicit HLTmatches(std::vector<std::string>);
    ~HLTmatches();
    Int_t TriggerNum(std::string);
  private:
    UInt_t i;
    std::vector<TString> HLTnames;
};
