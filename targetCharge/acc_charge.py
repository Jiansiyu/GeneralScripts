'''
'''

import os
import datetime

import rcdb
from rcdb.provider import RCDBProvider
from rcdb.model import Run
from sqlalchemy import desc
import halogcrawler
import  json
import csv

class acc_charge(object):
    TargetNameList={}  # used for match the name difference between
    summary_file="/u/scratch/siyuj/summary/summary_"
    runListFilename=''
    runJsonConfig={}
    def __init__(self):
        self.readConfig()
        pass
    def readConfig(self):
        '''
        Read JSON format config file
        :return:
        '''
        configName="./acc_config.json"
        if os.path.isfile(configName):
            with open(configName,'r') as runConfigFile:
                self.runJsonConfig=json.load(runConfigFile)
                if "summary_file" in self.runJsonConfig:
                    self.summary_file=self.runJsonConfig['summary_file']
                else:
                    print("Can NOT find {} in config file".format("summary_file"))

                if 'runList' in self.runJsonConfig:
                    self.runListFilename=self.runJsonConfig['runList']
                else:
                    print("Can NOT find runList in config file")

                if "TargetNameList" in self.runJsonConfig:
                    self.TargetNameList=self.runJsonConfig['TargetNameList']
                else:
                    print("CAN NOT FIND TargetNameList in config file")
        else:
            print("json configure file CANNOT FIND:: {}".format(configName))
            exit()

    def get_summary_output(self,run_number):
        summary=[]
        summary_filename=self.summary_file+run_number + "_000.txt"
        if not os.path.exists(summary_filename):
            return summary
        n = 0

        with open(summary_filename, 'r') as f:
            lines = filter(None, (line.strip() for line in f))
            for line in lines:
                if n > 0:
                    break
                if "Start Time:" in line:
                    value = line.split('Time:')[1]
                    summary.append(value)
                if "End Time:" in line:
                    value = line.split('Time:')[1]
                    summary.append(value)
                if "Number of events processed" in line:
                    value = line.split(':')[1]
                    summary.append(value)
                if "Percentage of good events" in line:
                    value = line.split(None)[4]
                    summary.append(value)
                # Cameron: changed behavior of charge getter to select the correct BCM on 7/30/2019 (Kent changed it before)
                if run_number < str(3583):
                    if "bcm_an_ds" in line:
                        value = line.split("Mean:")[1].split(None)[0]
                        summary.append(value)
                        n += 1
                elif run_number >= str(3583) and run_number < str(5000):
                    if "cav4cQ" in line:
                        value = line.split("Mean:")[1].split(None)[0]
                        summary.append(value)
                        n += 1
                elif run_number > str(5000):
                    if "bcm_an_us" in line:
                        value = line.split("Mean:")[1].split(None)[0]
                        summary.append(value)
                        n += 1
        return summary

    def GetChargeall(self,beginRunID=5939,EndRunID=5943,targName="D-208Pb10-D"):
        '''
        Get the Accumumated charge between runID range

        :param beginRunID:
        :param EndRunID:
        :return:
        '''

        runs=[]
        brun=beginRunID
        erun=EndRunID

        for x in range(int(brun),int(erun)+1):
            runs.append(x)

        # DB connection
        con_str = "mysql://rcdb@hallcdb.jlab.org:3306/a-rcdb"
        db = rcdb.RCDBProvider(con_str)

        # dictionary to fill
        dd = {}

        nrun = 0
        ngood = 0
        good_sum = 0
        charge_sum = 0

        # get result
        result = db.select_runs("", runs[0], runs[-1])
        for run in result:
            runno=str(run.number)
            helFlipRate = 120.0
            # PREX2
            if run.number >= 3876 and run.number < 5000:
                helFlipRate = 240.0
            dd[runno] = {}

            # from db
            run_type = run.get_condition_value("run_type")
            target_type = run.get_condition_value("target_type")
            run_flag = run.get_condition_value("run_flag")
            arm_flag = run.get_condition_value("arm_flag")

            pass_cut = True

            if run_type is None or run_type not in ['Production']:
                pass_cut = False

            if target_type is None or targName not in target_type:
              if run.get_condition_value("slug") < 3999:
                print( "Non-production target run")
                print( run.get_condition_value("target_type"))
                pass_cut = False
            good_run_flag = False
            # Tight cut
            if run_flag is not None and run_flag != 'Bad':
                good_run_flag = True

            print(run)
            print(run.start_time)

            charge1 = "0"
            charge2 = "0"

            if pass_cut:
                avg_cur = run.get_condition_value("beam_current")
                length = run.get_condition_value("run_length")
                ihwp = run.get_condition_value("ihwp")
                rhwp = run.get_condition_value("rhwp")

                # read prompt summary
                """
                0: start time
                1: end time
                2: number of events processed
                3: fraction of good events/all
                4: bcm mean
                """
                summary_output = self.get_summary_output(str(run.number))
                # if prompt analysis output exists or not
                if not summary_output:
                    # skip the run if there is no prompt summary
                    print("=== Prompt summary output does not exist for run: ", runno, ", skip this run for Good charge")
                    charge2 = "0"
                    prompt_time = "0"
                    start_time = run.start_time
                else:
                    start_time = summary_output[0]
                    if length is None:
                        # use prompt Nevent instead
                        length = float(summary_output[2]) * 1.0 / helFlipRate
                    # good charge from prompt output
                    if 'nan' in summary_output[3]:
                        print("=== No good event processed for run :", runno, " , skip this run")
                        prompt_time = "0"
                        charge2 = "0"
                    else:
                        prompt_time = float(summary_output[2]) * float(summary_output[3]) * 0.01 * 1.0 / helFlipRate
                        if run.number >= 3876 and run.number < 5000:
                            charge2 = float(prompt_time) * float(summary_output[4]) * 2
                        else:
                            charge2 = float(prompt_time) * float(summary_output[4])
                if length is None:
                    print("=== Run did not end properly...", runno, ", let's skip this")
                else:
                    # calculate charge all (from epics)
                    charge1 = float(avg_cur) * float(length)
                    # fill dict
                dd[runno]["avg_cur"] = avg_cur
                dd[runno]["length"] = length
                dd[runno]["charge_all"] = charge1
                dd[runno]["charge_good"] = charge2
                dd[runno]["eff_time"] = prompt_time
                dd[runno]["start_time"] = start_time
            else:
                # print runno, run_type, target_type, run_flag
                dd[runno]["charge_all"] = "0"
                dd[runno]["charge_good"] = "0"
                dd[runno]["start_time"] = run.start_time

            # Sum over all production runs (with 208Pb target)
            charge_sum += float(charge1)
            nrun += 1
            # Count good runs
            if dd[runno]["charge_all"] != "0":
                ngood += 1
                good_sum += float(dd[runno]["charge_good"])
        print("Total runs: %d,\t\tTotal charge sum: %.2f C" % (nrun, float(charge_sum) * 1.e-6))
        return  float(charge_sum) * 1.e-6

    def GetProductionTargType(self,runID=0):
        if os.path.isfile(self.runListFilename):
            with open(self.runListFilename) as f:
                reader=csv.DictReader(f)
                logList=list(reader)
        else:
            print('CAN NOT FIND FILE {}'.format(self.runListFilename))
        for item in logList:
            if runID == int(item["runID"]):
                return item["production_target_type"]
        return None

    def GetProductionStartTime(self,productionRunID=1):
        '''
        :param productionRunID:
        :return:
        '''
        if 'jlab' in os.uname()[1]:
            a=halogcrawler.halogCrawler()
            return  a.getStartTime(searchKey="Start_Run_{}".format(productionRunID),runID=productionRunID)
        else:
            return None

    def GetParityRunIDs(self,ProductionRunID=0, targName="D-208Pb10-D",SearchStartRunID=2573, SearchEndRunID=8000):
        '''
        :param ProductionRunID:
        :param targName:
        :param SearchStartRunID:
        :param SearchEndRunID:
        :return:
        '''
        ParityRunList=[]
        endSearchTime=datetime.datetime.strptime(self.GetProductionStartTime(productionRunID=ProductionRunID).strftime("%Y-%m-%d %H:%M:%S"), "%Y-%m-%d %H:%M:%S")
        if endSearchTime is not None:
            runs=[]
            for x in range(int(SearchStartRunID),int(SearchEndRunID)+1):
                runs.append(x)

            # DB connection
            con_str = "mysql://rcdb@hallcdb.jlab.org:3306/a-rcdb"
            db = rcdb.RCDBProvider(con_str)

            result = db.select_runs("",runs[0],runs[-1])
            for run in result:
                runno=str(run.number)
                # from db
                run_type = run.get_condition_value("run_type")
                target_type = run.get_condition_value("target_type")
                run_flag = run.get_condition_value("run_flag")
                arm_flag = run.get_condition_value("arm_flag")
                start_time = run.start_time
                end_time   = run.end_time

                if target_type is None:
                    continue

                if endSearchTime > start_time and targName in target_type:
                    print("run {}  target {}  startTime {}    :: end {}".format(runno, target_type, start_time,endSearchTime))
                    ParityRunList.append(int(runno))
        return ParityRunList

    def GetProductionTargetName(self,runID=0):
        '''
        :param runID:
        :return:
        '''
        pass

    def GetTargAccCharge(self,runID=0):
        '''
        :param runID: Production runID
        :return:
        '''
        target=self.GetProductionTargType(runID=runID)
        if not target:
            print("Can not find Counting Production target type for run <{}>".format(runID))
            return  None

        if not target in self.TargetNameList:
            print("Production Typle <{}> can not find in the Target Name List, please add it in file \"acc_config.json\"".format(target))
            return  None
        else:
            ParitytargetName = self.TargetNameList[target]

        ParityRunList=self.GetParityRunIDs(ProductionRunID=runID,targName=ParitytargetName)
        ParityRunList.sort()
        print("{} -> {}".format(ParityRunList[0],ParityRunList[-1]))

        startParityrunID=ParityRunList[0]
        endParityrunID=ParityRunList[-1]

        accumulatedCharge=self.GetChargeall(beginRunID=startParityrunID,EndRunID=endParityrunID,targName=ParitytargetName)
        print(self.TargetNameList)
        return  accumulatedCharge

    def test(self):
        self.GetTargAccCharge(runID=2047)
        # self.GetProductionTargType(runID=2047)
        # self.GetParityRunIDs(ProductionRunID=2730)
        # self.getChargeall()
if __name__ == "__main__":
    if not 'jlab' in os.uname()[1]:
        print("Script only works on JLab computers!!")
        exit(-1)
    a=acc_charge()
    a.test()