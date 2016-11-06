import FWCore.ParameterSet.Config as cms
import copy

# Trigger
from HLTrigger.HLTfilters.hltHighLevel_cfi import *
hltFilter = copy.deepcopy(hltHighLevel)
hltFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT")
hltFilter.HLTPaths = ['HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v*']
hltFilter.throw = cms.bool(False)
