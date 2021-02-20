'''
Opt Scan Template Generator

* On the local PC

* work on ifarm
 -- generate the files in seperate folder
 -- create the ifarm Job files
 -- put the jlob files in seperate folder, ready to submit the job seperately on the ifarm

TODO  in the future need to put files in different folders
'''

import os
import json
import sys
from collections import OrderedDict
from subprocess import call
import multiprocessing
from multiprocessing import Pool

try:
    from progress.bar import Bar
except ImportError as e:
    pass
import datetime
from datetime import date
from random import seed
from random import random, randint
import stat

class optDatabaseTemplateGenerator():

    def __init__(self,runConfigFname="runConfig.json"):
        self.runConfigJsonFname=runConfigFname
        self.OptConfigFname=""
        self.TargetPath=""
        self.CurrentWorkFolder=""
        self.DBElementPrefix="P "
        self.OptTemplateFname=""
        self.OptCombinationCount=0
        self.OptDBFileCount=0
        self.jobsPerNode = 100   # the concurrent jobs on each node
        self.jobsfolder = "./"   # jlab ifarm job submit script save folder
        self.templateFolderList =[] # the path of all the
        self.optScannerBashScript = ""
        self.jobsEnv = ""
        self.coresPerNode = 4
        self.folderIndexTemplate = "{:06}"
        self.LoadConfig()

    def preRunCheck(self):
        runStatusFlag = True
        if not os.path.isfile(self.jobsEnv):
            print("[Error]:: can not find {}".format(self.jobsEnv))
            runStatusFlag = runStatusFlag and False
        pass

    def LoadConfig(self, runConfigFname=""):
        if not runConfigFname:
            runConfigFname=self.runConfigJsonFname
            
        with open(runConfigFname) as runCondigFile:
            self.runConfig_data=json.load(runCondigFile,object_pairs_hook=OrderedDict) # keep the initial order of the database
            self.OptConfigFname   = self.runConfig_data["optConfigFname"]
            self.TargetPath       = self.runConfig_data["TargetPath"]
            self.OptSourceFolder=self.runConfig_data["OptSourceFolder"]
            self.optScannerBashScript=self.runConfig_data["optScannerBashScript"]
            self.OptTemplateFname = self.runConfig_data["optTemplateFname"]
            self.jobsPerNode      = int(self.runConfig_data["jobsPerNode"])
            self.jobsfolder       = self.runConfig_data["jobsfolder"]
            self.jobsEnv          = self.runConfig_data["jobsEnv"]
            self.coresPerNode     = self.runConfig_data["coresPerNode"]


        print("{}.{}".format(self.OptConfigFname,self.TargetPath))

    def GenerateDBConbinations(self,jsonFname=""):
        # need to calculate how many combinations it could be
        if not jsonFname:
            jsonFname=self.OptConfigFname

        print("Generate the combinations:")
        totalCombinations=1
        totalParameters=0
        with open(jsonFname) as json_file:
            self.optConfig_data=json.load(json_file,object_pairs_hook=OrderedDict) # keep the initial order of the database
            loopTemplate=[]
            for item in self.optConfig_data["DpOpt"]:
                key=item
                startOrder=int(self.optConfig_data["DpOpt"][item]["startOrder"])
                endOrder=int(self.optConfig_data["DpOpt"][item]["endOrder"])
                totalCombinations=totalCombinations*(endOrder+1-startOrder)
                totalParameters=totalParameters+1
        # bar=Bar("Processing",max=totalCombinations)
        loopTemplate=[]
        with open(jsonFname) as json_file:
            self.optConfig_data=json.load(json_file,object_pairs_hook=OrderedDict) # keep the initial order of the database
            
            for item in self.optConfig_data["DpOpt"]:
                print("{}.{}.{}".format(item,int(self.optConfig_data["DpOpt"][item]["startOrder"]),int(self.optConfig_data["DpOpt"][item]["endOrder"])))
                key=item
                startOrder=int(self.optConfig_data["DpOpt"][item]["startOrder"])
                endOrder=int(self.optConfig_data["DpOpt"][item]["endOrder"])
                # if the loop template  is empty, then push the first on into the list
                if not loopTemplate:
                    for number in range(startOrder,endOrder+1):
                        current={}
                        current[key]=number
                        loopTemplate.append(current)
                        # bar.next()
                    # print(loopTemplate)
                else:
                    curentArray=[]
                    for item in loopTemplate:
                        for number in range(startOrder,endOrder+1):
                            currentItem=item.copy()
                            currentItem[key]=number
                            curentArray.append(currentItem)
                            # bar.next()
                    loopTemplate=curentArray.copy()
        
        # bar.finish()
        # print(len(loopTemplate))
        return loopTemplate

    def ReadDatabaseTemplate(self,TemplateFname="CRex_LHRS_template.db"):
        '''
        
        % read the initial template, which should contains all the configurations 
        % all the sub-template will used this mother template as template
        '''
        self.TemplateDatabase=dict()
        with open(TemplateFname) as templateFile:
            line=templateFile.readline()
            while line:
                if line.startswith("R "):
                    self.TemplateDatabase[line[0:9]]=line
                elif line.startswith("XF") or line.startswith("TF") or line.startswith("PF") or line.startswith("YF"):
                    self.TemplateDatabase[line[0:12]]=line
                else:
                    self.TemplateDatabase[line[0:7]]=line
                line=templateFile.readline()
        #for item in self.TemplateDatabase:
        #    print(item)
                

    def RequestNewFolder(self):
        self.CurrentWorkFolder=self.GenerateRandomNamedFolder()
        return  self.CurrentWorkFolder

    def GenerateRandomNamedFolder(self):
        '''
        TODO when the file exist, just recreate it instead if use a random number
        '''
        today=date.today()
        datePreFix=today.strftime("%Y%m%d")
        datePreFix = '20210218'
        # randomNumb_surFix=randint(11111111,99999999)
        randomNumb_surFix="{:06}".format(self.OptDBFileCount)
        self.OptDBFileCount=self.OptDBFileCount+1
        pathCandidate=os.path.join(self.TargetPath,'DBScan_{}_{}'.format(datePreFix,randomNumb_surFix))
        # while os.path.exists(pathCandidate):
        #     randomNumb_surFix=randint(111111111,99999999999)
        #     pathCandidate=os.path.join("",self.TargetPath,'DBScan_{}_{}'.format(datePreFix,randomNumb_surFix))
        if not os.path.isdir(pathCandidate):
            os.makedirs(pathCandidate)
        return pathCandidate

    def CheckCombinationExist(self,combinations={}):
        """
        Read in the json file and check whether the combination have been generated, if generated, 
        then copy the generated file into the current folder
        and mark it as replayed 
        """
        return False
    def LogTemplateCombinations(self,combinations={}):
        pass

    def WriteTemplateSingle(self,workDir="./",dbCombination={}):
        '''
        Single thread process generate template
        '''
        
        pass

    def _getSubFolderBT(self,mainFolderTemplate = "",startIndex = 0, endIndex = 1000000):
        randomNumb_surFix = self.folderIndexTemplate.format(endIndex)
        foldername = mainFolderTemplate.format(randomNumb_surFix)
        while os.path.isdir(foldername):
            endIndex = 2*endIndex
            randomNumb_surFix = self.folderIndexTemplate.format(endIndex)
            foldername = mainFolderTemplate.format(randomNumb_surFix)

        start = startIndex
        end = endIndex

        while start<end:
            mid = (start + end) // 2
            randomNumb_surFix = self.folderIndexTemplate.format(mid)
            foldername = mainFolderTemplate.format(randomNumb_surFix)
            if os.path.isdir(foldername):
                start = mid+1
            else:
                end = mid
        return start-1

    def GetSubFoldersBinTree(self,topFolder="",maxDateLookBack=10):
        if not topFolder:
            topFolder = self.TargetPath
        today = date.today()
        datePreFix = today.strftime("%Y%m%d")

        for i in range(0, maxDateLookBack):
            curr = today - datetime.timedelta(days=i)
            folder0 = os.path.join(topFolder, "DBScan_{}_000000".format(curr.strftime("%Y%m%d")));
            if os.path.isdir(os.path.join(topFolder, "DBScan_{}_000000".format(curr.strftime("%Y%m%d")))):
                filenameTemplate =os.path.join(os.path.join(topFolder, "DBScan_{}".format(curr.strftime("%Y%m%d"))))
                filenameTemplate = filenameTemplate + "_{}"
                endIndex = self._getSubFolderBT(filenameTemplate)
                bar = Bar("Optimization Result Looking Back {}".format(curr.strftime("%Y%m%d")), max=endIndex)
                for runIndex in range(0,endIndex+1):
                    randomNumb_surFix = self.folderIndexTemplate.format(runIndex)
                    pathName = os.path.join(topFolder,"DBScan_{}_{}".format(curr.strftime("%Y%m%d"),randomNumb_surFix))
                    #filename = os.path.join(topFolder,"DBScan_{}_{}".format(curr.strftime("%Y%m%d"),randomNumb_surFix),"templateDB.db.optimied")
                    self.templateFolderList.append(pathName)
                    bar.next()
                bar.finish()
            else:
                print("skip folder:{}".format(folder0))


    def WriteTemplate(self, workDir="./"):
        '''
        write the template database file to seperate sub-folders
        '''
        self.ReadDatabaseTemplate(TemplateFname=self.OptTemplateFname)
        templateArray=self.GenerateDBConbinations()

        if any([True for _ in os.scandir(self.TargetPath)]):
            self.GetSubFoldersBinTree(topFolder=self.TargetPath)
            return

        bar=Bar("Genarating template",max=len(templateArray))
        for item in templateArray:
            '''
            Create the folders that contines the combinations
            '''
            self.RequestNewFolder()
            self.templateFolderList.append(self.CurrentWorkFolder)  # put the current work folder
            #Create folder that contines the combinations jsons
            # check the existance of the files
            templatefilename = "{}/templateJson.json".format(self.CurrentWorkFolder)
            if os.path.isfile(templatefilename):
                continue

            with open("{}/templateJson.json".format(self.CurrentWorkFolder),"w") as jsonFileIO:
                json.dump(item,jsonFileIO)
            jsonFileIO.close()

            NewtemplateFilename="{}/templateDB.db".format(self.CurrentWorkFolder)
            templateIO=open(NewtemplateFilename,"w")
            if not self.CheckCombinationExist(combinations=item):
                self.LogTemplateCombinations(item)
                for templateLine in self.TemplateDatabase:
                    # print(templateLine)
                    tempstr="{}".format(self.TemplateDatabase[templateLine])
                    # print(self.DBElementPrefix)
                    if tempstr.startswith(self.DBElementPrefix):
                        if templateLine in item.keys():
                            templateString="{}".format(self.TemplateDatabase[templateLine])
                            templateStringRemoveLast=templateString.rsplit(' ',1)[0]
                            templateStringFinal="{} {}\n".format(templateStringRemoveLast,item[templateLine])
                            templateIO.write("{}".format(templateStringFinal))
                            # print(self.TemplateDatabase[templateLine])
                    else:
                        templateIO.write("{}".format(self.TemplateDatabase[templateLine]))
                        # print(self.TemplateDatabase[templateLine])
            else:
                pass
            templateIO.close()
            bar.next()
        bar.finish()

    def createSlurmFiles(self,slurmCMD="",slurmJobfilename="",slurmJobName="PRex_Optics",slurmRunMode = "analysis"):
        with open(slurmJobfilename,'x') as txt:
            txt.write("PROJECT: PRex\n")
            txt.write("TRACK: {}\n".format(slurmRunMode))
            txt.write("COMMAND: {}\n".format(slurmCMD))
            txt.write("JOBNAME: {}\n".format(slurmJobName))
            txt.write("MEMORY: {} GB\n".format(self.coresPerNode*3))
            txt.write("CPU : {}\n".format(self.coresPerNode))
            txt.write("DISK_SPACE: {} GB\n".format(self.coresPerNode*4))
            txt.close()
        return True

    def generateSlurmJobs(self,workdir="./jobs"):
        '''
        generate the slurm run Job txt files
        '''
        if not self.jobsfolder:
            self.jobsfolder = workdir
        if not os.path.isdir(self.jobsfolder):
            os.mkdir(self.jobsfolder)
        self.runCMDList= set()  # buffer all the run Job script that ready to submit to the ifarm

        fileCounter = 0
        while fileCounter < len(self.templateFolderList):
            jobScriptsfname = "slurmJob_{}_{}.csh".format(fileCounter // self.jobsPerNode * self.jobsPerNode,
                                                          fileCounter // self.jobsPerNode * self.jobsPerNode + self.jobsPerNode)
            jobScriptsfname = os.path.join(self.jobsfolder, jobScriptsfname)
            endRunID = fileCounter // self.jobsPerNode * self.jobsPerNode + self.jobsPerNode
            with open(jobScriptsfname, "w") as runCMDio:
                self.runCMDList.add(jobScriptsfname)
                if not os.access(jobScriptsfname, os.X_OK):
                    st = os.stat(jobScriptsfname)
                    os.chmod(jobScriptsfname, st.st_mode | stat.S_IEXEC)
                runCMDio.write("#!/bin/csh \n")
                runCMDio.write("source {}\n".format(self.jobsEnv))
                runCMDio.write("set startRunID = {}\n".format(fileCounter))
                runCMDio.write("set endRunID = {}\n".format(endRunID))
                runCMDio.write("set ncores = {}\n".format(self.coresPerNode))
                runCMDio.write("seq -f %06g $startRunID $endRunID | xargs -i --max-procs=$ncores bash -c \"{} {} {}_{{}} > /dev/null\"\n".format(self.optScannerBashScript,self.OptSourceFolder,self.templateFolderList[fileCounter][:-7]))
                runCMDio.close()
            fileCounter= endRunID
        # finish creating bundle command that ready to send to ifarm
        # create the job script
        self.jobRunScriptList = []
        for key, filename in enumerate(self.runCMDList):
            slurmJobfilename = os.path.join(self.jobsfolder,"PRexOpt_{}.txt".format(key))
            slurmJobMode = "analysis"
            self.createSlurmFiles(slurmCMD=filename,slurmJobfilename=slurmJobfilename,slurmJobName="PRexOpt_Job{}".format(key),slurmRunMode=slurmJobMode)
            self.jobRunScriptList.append(slurmJobfilename)

        #call the slurm Submit to send the jobs
        for job in self.jobRunScriptList:
            self.slurmSubmit(slurmRunfilename=job)
        # ncores = min(10,multiprocessing.cpu_count())
        # threadPool = Pool(ncores)
        # threadPool.map(self.slurmSubmit, self.jobRunScriptList)

    def slurmSubmit(self,slurmRunfilename = ""):
        if os.path.isfile(slurmRunfilename):
            print(slurmRunfilename)
            #if "jlab" in os.uname()[1]:
            #    call(["jsub", slurmRunfilename])
        #TODO log and exception process


if __name__ == "__main__":
    runConfigFile="runConfig_run.json"
    if len(sys.argv)>1 and ".json" in sys.argv[1]:
        runConfigFile=sys.argv[1]

    test=optDatabaseTemplateGenerator(runConfigFname=runConfigFile)
    test.WriteTemplate()
    test.generateSlurmJobs()
