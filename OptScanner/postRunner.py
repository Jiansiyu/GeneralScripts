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


class optPostRunner():

    def __init__(self, runConfigFname="runConfig.json"):
        self.runConfigJsonFname = runConfigFname
        self.OptConfigFname = ""
        self.TargetPath = ""
        self.CurrentWorkFolder = ""
        self.DBElementPrefix = "P "
        self.OptTemplateFname = ""
        self.OptCombinationCount = 0
        self.OptDBFileCount = 0
        self.jobsPerNode = 100  # the concurrent jobs on each node
        self.jobsfolder = "./"  # jlab ifarm job submit script save folder
        self.templateFolderList = []  # the path of all the
        self.optScannerBashScript = ""
        self.jobsEnv = ""
        self.coresPerNode = 4
        self.folderIndexTemplate = "{:06}"
        self.LoadConfig()

    def postRunCheck(self,runfolder=""):
        '''
        check replayed sucessfully or not
        '''
        if not os.path.isfile(os.path.join(runfolder,"Sieve._2322_p4.f51_reform/CheckSieve_Report.root")):
            print("Missing {}".format(os.path.join(runfolder,"Sieve._2322_p4.f51_reform/CheckSieve_Report.root")))
        return os.path.isfile(os.path.join(runfolder,"Sieve._2322_p4.f51_reform/CheckSieve_Report.root"))

    def LoadConfig(self, runConfigFname=""):
        if not runConfigFname:
            runConfigFname = self.runConfigJsonFname

        with open(runConfigFname) as runCondigFile:
            self.runConfig_data = json.load(runCondigFile,
                                            object_pairs_hook=OrderedDict)  # keep the initial order of the database
            self.OptConfigFname = self.runConfig_data["optConfigFname"]
            self.TargetPath = self.runConfig_data["TargetPath"]
            self.OptSourceFolder = self.runConfig_data["OptSourceFolder"]
            self.optScannerBashScript = self.runConfig_data["optScannerBashScript"]
            self.OptTemplateFname = self.runConfig_data["optTemplateFname"]
            self.jobsPerNode = int(self.runConfig_data["jobsPerNode"])
            self.jobsfolder = self.runConfig_data["jobsfolder"]
            self.jobsEnv = self.runConfig_data["jobsEnv"]
            self.coresPerNode = self.runConfig_data["coresPerNode"]

        print("{}.{}".format(self.OptConfigFname, self.TargetPath))

    def _getSubFolderBT(self, mainFolderTemplate="", startIndex=0, endIndex=1000000):
        randomNumb_surFix = self.folderIndexTemplate.format(endIndex)
        foldername = mainFolderTemplate.format(randomNumb_surFix)
        while os.path.isdir(foldername):
            endIndex = 2 * endIndex
            randomNumb_surFix = self.folderIndexTemplate.format(endIndex)
            foldername = mainFolderTemplate.format(randomNumb_surFix)
        start = startIndex
        end = endIndex
        while start < end:
            mid = (start + end) // 2
            randomNumb_surFix = self.folderIndexTemplate.format(mid)
            foldername = mainFolderTemplate.format(randomNumb_surFix)
            if os.path.isdir(foldername):
                start = mid + 1
            else:
                end = mid
        return start - 1

    def GetSubFoldersBinTree(self, topFolder="", maxDateLookBack=10):
        if not topFolder:
            topFolder = self.TargetPath
        today = date.today()
        datePreFix = today.strftime("%Y%m%d")
        for i in range(0, maxDateLookBack):
            curr = today - datetime.timedelta(days=i)
            folder0 = os.path.join(topFolder, "DBScan_{}_000000".format(curr.strftime("%Y%m%d")));
            if os.path.isdir(os.path.join(topFolder, "DBScan_{}_000000".format(curr.strftime("%Y%m%d")))):
                filenameTemplate = os.path.join(os.path.join(topFolder, "DBScan_{}".format(curr.strftime("%Y%m%d"))))
                filenameTemplate = filenameTemplate + "_{}"
                endIndex = self._getSubFolderBT(filenameTemplate)
                bar = Bar("Optimization Result Looking Back {}".format(curr.strftime("%Y%m%d")), max=endIndex)
                for runIndex in range(0, endIndex + 1):
                    randomNumb_surFix = self.folderIndexTemplate.format(runIndex)
                    pathName = os.path.join(topFolder,
                                            "DBScan_{}_{}".format(curr.strftime("%Y%m%d"), randomNumb_surFix))
                    self.templateFolderList.append(pathName)
                    bar.next()
                bar.finish()
            else:
                print("skip folder:{}".format(folder0))

    def createSlurmFiles(self, slurmCMD="", slurmJobfilename="", slurmJobName="PRex_Optics", slurmRunMode="analysis"):
        with open(slurmJobfilename, 'x') as txt:
            txt.write("PROJECT: PRex\n")
            txt.write("TRACK: {}\n".format(slurmRunMode))
            txt.write("COMMAND: {}\n".format(slurmCMD))
            txt.write("JOBNAME: {}\n".format(slurmJobName))
            txt.write("MEMORY: {} GB\n".format(self.coresPerNode * 3))
            txt.write("CPU : {}\n".format(self.coresPerNode))
            txt.write("DISK_SPACE: {} GB\n".format(self.coresPerNode * 4))
            txt.close()
        return True

    def PostRunChecker(self, topFolder="", maxDateLookBack=10):
        if not topFolder:
            topFolder = self.TargetPath
        today = date.today()
        datePreFix = today.strftime("%Y%m%d")
        for i in range(0, maxDateLookBack):
            curr = today - datetime.timedelta(days=i)
            folder0 = os.path.join(topFolder, "DBScan_{}_000000".format(curr.strftime("%Y%m%d")));
            if os.path.isdir(os.path.join(topFolder, "DBScan_{}_000000".format(curr.strftime("%Y%m%d")))):
                filenameTemplate = os.path.join(os.path.join(topFolder, "DBScan_{}".format(curr.strftime("%Y%m%d"))))
                filenameTemplate = filenameTemplate + "_{}"
                endIndex = self._getSubFolderBT(filenameTemplate)
                # bar = Bar("Optimization Result Looking Back {}".format(curr.strftime("%Y%m%d")), max=endIndex)
                runSubfolderIndex = 0
                while runSubfolderIndex < endIndex:
                    print("working on {}".format(runSubfolderIndex))
                    for i in range(200):
                        subfolderfname =os.path.join(topFolder, "DBScan_{}_{:06}".format(curr.strftime("%Y%m%d"),runSubfolderIndex + i))
                        if not self.postRunCheck(runfolder=subfolderfname):

                            self.generateScripts(startRunID=runSubfolderIndex,endRunID=runSubfolderIndex+200,runFolderTemplate=subfolderfname)
                            break
                    runSubfolderIndex += 200
            else:
                print("skip folder:{}".format(folder0))

    def generateScripts(self,startRunID, endRunID,runFolderTemplate = "",workdir = "./postJobs"):
        self.jobsfolder = workdir
        if not os.path.isdir(self.jobsfolder):
            os.mkdir(self.jobsfolder)
        jobScriptsfname = "slurmJob_{}_{}.csh".format(startRunID,endRunID)
        jobScriptsfname = os.path.join(self.jobsfolder, jobScriptsfname)
        if os.path.isfile(jobScriptsfname):
            return
        with open(jobScriptsfname, "w") as runCMDio:
            if not os.access(jobScriptsfname, os.X_OK):
                st = os.stat(jobScriptsfname)
                os.chmod(jobScriptsfname, st.st_mode | stat.S_IEXEC)
            runCMDio.write("#!/bin/csh \n")
            runCMDio.write("source {}\n".format(self.jobsEnv))
            runCMDio.write("set startRunID = {}\n".format(startRunID))
            runCMDio.write("set endRunID = {}\n".format(endRunID))
            runCMDio.write("set ncores = {}\n".format(self.coresPerNode))
            runCMDio.write(
                "seq -f %06g $startRunID $endRunID | xargs -i --max-procs=$ncores bash -c \"echo start run{{}};{} {} {}_{{}} > /dev/null\"\n".format(
                    self.optScannerBashScript, self.OptSourceFolder, runFolderTemplate[:-7]))
            runCMDio.write("exit 0\n")
            runCMDio.close()

    def generateSlurmJobs(self, workdir="./jobs"):
        '''
        generate the slurm run Job txt files
        '''
        if not self.jobsfolder:
            self.jobsfolder = workdir
        if not os.path.isdir(self.jobsfolder):
            os.mkdir(self.jobsfolder)
        self.runCMDList = set()  # buffer all the run Job script that ready to submit to the ifarm

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
                runCMDio.write(
                    "seq -f %06g $startRunID $endRunID | xargs -i --max-procs=$ncores bash -c \"echo start run{{}};{} {} {}_{{}} > /dev/null\"\n".format(
                        self.optScannerBashScript, self.OptSourceFolder, self.templateFolderList[fileCounter][:-7]))
                runCMDio.close()
            fileCounter = endRunID
        # finish creating bundle command that ready to send to ifarm
        # create the job script
        self.jobRunScriptList = []
        for key, filename in enumerate(self.runCMDList):
            slurmJobfilename = os.path.join(self.jobsfolder, "PRexOpt_{}.txt".format(key))
            slurmJobMode = "analysis"
            self.createSlurmFiles(slurmCMD=filename, slurmJobfilename=slurmJobfilename,
                                  slurmJobName="PRexOpt_Job{}".format(key), slurmRunMode=slurmJobMode)
            self.jobRunScriptList.append(slurmJobfilename)

        # call the slurm Submit to send the jobs
        for job in self.jobRunScriptList:
            self.slurmSubmit(slurmRunfilename=job)

    def slurmSubmit(self, slurmRunfilename=""):
        if os.path.isfile(slurmRunfilename):
            print(slurmRunfilename)

if __name__ == "__main__":
    runConfigFile = "runConfig_run.json"
    if len(sys.argv) > 1 and ".json" in sys.argv[1]:
        runConfigFile = sys.argv[1]

    test = optPostRunner(runConfigFname=runConfigFile)
    test.PostRunChecker();
