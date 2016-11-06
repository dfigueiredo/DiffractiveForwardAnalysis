#!/usr/bin/python
# Running: python compute_pileup_Run_2016_25ns.py 

from ROOT import TH1D
from SimGeneral.MixingModule.mix_2016_25ns_SpringMC_PUScenarioV1_PoissonOOTPU_cfi import mix

prob = TH1D('pileup', '', 50, 0., 50.)

i = 0
for value in mix.input.nbPileupEvents.probValue:
    prob.Fill(i, value)
    i += 1

prob.SaveAs('MCPUdist_2016.root')
