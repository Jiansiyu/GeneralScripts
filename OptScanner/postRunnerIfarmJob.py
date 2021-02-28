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
        pass

    def getRunBash(self,bashPath='./'):
        if not bashPath or not os.path.isdir(bashPath):
            print("Can not find folder {}".format(bashPath))
            return  None

        OptBashFiles = [os.path.join(bashPath,f.name) for f in os.scandir(bashPath) if f.is_file()]
        return  OptBashFiles

    def getIfarmJobs(self,BashFiles=[]):
        ifarmJobCounter = 0
        for runFiles in BashFiles:
            with open("postIfarm/PRexOpt_{}.txt".format(ifarmJobCounter),'x') as txt:
                txt.write("PROJECT: PRex\n")
                txt.write("TRACK: analysis\n")
                txt.write("COMMAND: {}\n".format(runFiles))
                txt.write("JOBNAME: Opt_{}\n".format(ifarmJobCounter))
                txt.write("MEMORY: {} GB\n".format(12))
                txt.write("CPU : {}\n".format(4))
                txt.write("DISK_SPACE: {} GB\n".format(10))
                txt.close()
                ifarmJobCounter += 1


if __name__ == '__main__':
    if len(sys.argv) > 1 and os.path.isdir(sys.argv[1]):
        test = optPostRunner()
        jobs = test.getRunBash(sys.argv[1])
        test.getIfarmJobs(jobs)
    else:
        print("can not find arguments")