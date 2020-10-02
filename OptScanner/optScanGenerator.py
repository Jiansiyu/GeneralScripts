'''
Used for automaticly generate the database template
'''

import os
import json
import time
import sys
from collections import OrderedDict
try:
    from progress.bar import Bar
except ImportError as e:
    pass

from datetime import date
from random import seed
from random import random, randint

class optDatabaseTemplateGenerator():

    def __init__(self,runConfigFname="runConfig.json"):
        self.runConfigJsonFname=runConfigFname
        self.OptConfigFname=""
        self.TargetPath=""
        self.CurrentWorkFolder=""
        self.DBElementPrefix="T "
        self.OptTemplateFname=""
        self.LoadConfig()
        self.OptCombinationCount=0
        self.OptDBFileCount=0
        

    def LoadConfig(self, runConfigFname=""):
        if not runConfigFname:
            runConfigFname=self.runConfigJsonFname
            
        with open(runConfigFname) as runCondigFile:
            self.runConfig_data=json.load(runCondigFile,object_pairs_hook=OrderedDict) # keep the initial order of the database
            self.OptConfigFname=self.runConfig_data["optConfigFname"]
            self.TargetPath=self.runConfig_data["TargetPath"]
            self.OptTemplateFname=self.runConfig_data["optTemplateFname"]
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

    def GenerateRandomNamedFolder(self):
        today=date.today()
        datePreFix=today.strftime("%Y%m%d")
        # randomNumb_surFix=randint(11111111,99999999)
        randomNumb_surFix="{:05}".format(self.OptDBFileCount)
        self.OptDBFileCount=self.OptDBFileCount+1
        pathCandidate=os.path.join(self.TargetPath,'DBScan_{}_{}'.format(datePreFix,randomNumb_surFix))
        while os.path.exists(pathCandidate):
            randomNumb_surFix=randint(111111111,99999999999)
            pathCandidate=os.path.join("",self.TargetPath,'DBScan_{}_{}'.format(datePreFix,randomNumb_surFix))
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
    def WriteTemplate(self, workDir="./"):
        '''
        write the template database file to seperate sub-folders
        
        '''
        
        self.ReadDatabaseTemplate(TemplateFname=self.OptTemplateFname)
        templateArray=self.GenerateDBConbinations()

        bar=Bar("Genarating template",max=len(templateArray))
        for item in templateArray:
            
            '''
            Create the folders that contines the combinations
            '''
            self.RequestNewFolder()
            #Create folder that contines the combinations jsons
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


    def _IsExist_(self):
        '''
        used for check whether 
        '''
        pass

if __name__ == "__main__":
    # runConfigFile="runConfig_test.json"
    runConfigFile="runConfig_theta.json"
    if len(sys.argv)>1:
        runConfigFile=sys.argv[1]
    
    test=optDatabaseTemplateGenerator(runConfigFname=runConfigFile)
    test.WriteTemplate()
    
