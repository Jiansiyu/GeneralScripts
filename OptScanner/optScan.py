"""
Used for Call the Optimization bash script
The default unumber of cores are 10
"""
import os
import json
import time
import sys
from collections import OrderedDict
#from progress.bar import Bar
from datetime import date
from random import seed
from random import random, randint
from multiprocessing import Pool     # multithread process the single files
from shutil import which

class optScanner(object):
    def __init__(self,runConfigFname="runConfig.json"):
        self.runConfigJsonFname=runConfigFname
        self.OptConfigFname=""
        self.TargetPath=""
        self.CurrentWorkFolder=""
        #self.OptTemplateSubFoldersEmp=[]
        self.OptTemplateSubFolders=[]
        self.OptSourceFolder=""
        self.optScannerBashScript=""
        self.LoadConfig()
        self.GetSubFolders()
        self.farmJobCounter=0

    def LoadConfig(self, runConfigFname=""):
        if not runConfigFname:
            runConfigFname=self.runConfigJsonFname
        with open(runConfigFname) as runCondigFile:
            self.runConfig_data=json.load(runCondigFile,object_pairs_hook=OrderedDict) # keep the initial order of the database
            self.OptConfigFname=self.runConfig_data["optConfigFname"]
            self.TargetPath=self.runConfig_data["TargetPath"]
            self.OptSourceFolder=self.runConfig_data["OptSourceFolder"]
            self.optScannerBashScript=self.runConfig_data["optScannerBashScript"]
        print("{}.{}".format(self.OptConfigFname,self.TargetPath))
    
    def GetSubFolders(self,topFolder=""):
        '''
        Modified version,
        Only return the folders that have not been replayed
        '''
        if not topFolder:
            topFolder=self.TargetPath
        OptTemplateSubFoldersAll=[f.path for f in os.scandir(topFolder) if f.is_dir()]
        self.OptTemplateSubFolders=[item for item in OptTemplateSubFoldersAll if not os.path.isfile("{}/CheckDp_test_result.txt".format(item))]
    
    def OptimizeSubFolder(self,folderName=""):
        if not os.path.isfile("{}/CheckDp_test_result.txt".format(folderName)):
            if os.path.exists(folderName):
                optBashCommand="{} {} {}".format(self.optScannerBashScript,self.OptSourceFolder,folderName)
                print(optBashCommand)
                os.system(optBashCommand)
                
    def MultiThreadOptimization(self, maxThread=10):
        threadPool=Pool(maxThread)
        threadPool.map(self.OptimizeSubFolder,self.OptTemplateSubFolders)
    
    def GenerateIfarmJob(self,folderName=""):
        # create file name
        JobSaveFolder=os.path.join('./',"Jobs")
        os.makedirs(JobSaveFolder)

        if not os.path.isfile("{}/CheckDp_test_result.txt".format(folderName)):
            if os.path.exists(folderName):
                # create the job and write in the command 
                with open(os.path.join(JobSaveFolder,"Job_{}.txt".format(self.farmJobCounter))) as txt:
                    '''
                    Get the system enviroment
                    '''
                    analyzerPath=which('which  analyzer')
                    OpttemplateDB=os.path.join(folderName,"templateDB.db")
                    OptimizedDBFname=os.path.join(folderName,"templateDB.db.optimied")
                    OptimizedDBFname_postrun=os.path.join(folderName,"templateDB.db.optimied_junk")
                    OptimizationPath='/u/home/siyuj/PRex/CRexOpt/PRexOpt/Opt'

                    txt.write("PROJECT: PRex\n")
                    txt.write("TRACK: analysis\n")
                    txt.write("COMMAND:cd {} &&{}\n".format(OptimizationPath,analyzerPath)) #rootlogon.C
                    txt.write("OPTIONS: -b -q .L {} \'{} (\'true\',\"delta\",\"\'{}\'\",\"\'{}\'\")\'\n".format(os.path.join(OptimizationPath,"rootlogon.C"),os.path.join(OptimizationPath,"ROpticsOptScript.C"),OpttemplateDB,OptimizedDBFname))
                    #txt.write("OTHER_FILES: /lustre/expphy/work/hallb/prad/xbai/PRadSim/build/database/*\n")
                    txt.write("JOBNAME: Brem_Simulation\n")
                    txt.write("MEMORY: 2 GB\n")
                    txt.write("DISK_SPACE: 10 GB\n")
                    txt.write("OS: centos7\n")
                    txt.close()

                # optBashCommand="{} {} {}".format(self.optScannerBashScript,self.OptSourceFolder,folderName)
                # print(optBashCommand)
                # os.system(optBashCommand)
        
if __name__ == "__main__":
    test=optScanner(runConfigFname="runConfig_theta.json")
    test.MultiThreadOptimization(5)
