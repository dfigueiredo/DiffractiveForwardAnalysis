from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.General.requestName = 'GammaGammaEE_LPAIR_Single_Analysis'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'RunGammaGammaEE_MC_cfg.py'

config.Data.inputDataset = '/GammaGammaEE/dmf-LPAIR-Single-AODSIM-13f20a8a55814d9e58ebb00fa2397214/USER'
config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 2
config.Data.outLFNDirBase = '/store/user/%s/' % (getUsernameFromSiteDB())
config.Data.publication = False
config.Data.outputDatasetTag = 'LPAIR-Single-Analysis'

config.Site.storageSite = 'T2_CH_CERN'
