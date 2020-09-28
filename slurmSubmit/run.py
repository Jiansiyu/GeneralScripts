from subprocess import call
import sys
import os
from pathlib import Path

# nn = [i for i in range(0, 10)]

runList=[]

def generateRunJobs(runID=0):
    if os.path.isfile("job/"+str(runID)+".txt"):
        # Job have been submitted
        return True
    with open("job/"+str(runID)+".txt", "w") as txt:
        txt.write("PROJECT: PRex\n")
        if len(sys.argv) < 10:
            txt.write("TRACK: debug\n")
        else:
            txt.write("TRACK: analysis\n")
        
        txt.write("COMMAND: /w/halla-scifs17exp/parity/disk1/siyu/prex_replay/replay/runDp.csh {}\n".format(runID))
        # txt.write("OPTIONS: /lustre/expphy/work/hallb/prad/siyu/Simulation/PRadSim_build/run.mac\n")
        #txt.write("OTHER_FILES: /lustre/expphy/work/hallb/prad/xbai/PRadSim/build/database/*\n")
        txt.write("JOBNAME: counting_replay\n")
        txt.write("MEMORY: 4 GB\n")
        txt.write("DISK_SPACE: 20 GB\n")
        #txt.write("OS: centos7\n")
    return True

def decodeRunFile(RunListFileName):
    '''
    read the file list from the file 
    add support for multi format support
    formate can be 
    run nmae (or run number) start run ID (optional) end run ID(optional)
    prexRHRS_1111.dat,     0 , 1
    or
    prexRHRS_1111.dat
    or 
    1111     0 , 1
    '''
    runListArray=[]
    FileListArray=[]
    runIDListArray=[]
    if ".txt" in RunListFileName:
        with open(RunListFileName) as fileio:
            for line in fileio:
                if '#' not in line:
                    runListArray.append([item for item in line.strip().split(',')])
                else:
                    runListArray.append([item for item in line.strip().split('#')[0].strip().split(',')])
    for line in runListArray:
        if len(line) == 2 and len(line[0]) > 2:
            print('Currently unsupport')
        elif len(line) == 3 and len(line[0]) > 2:
            print('Currently unsupport')
        elif len(line) ==1 and len(line[0]) > 2:
            runIDListArray.append(line[0])
    return runIDListArray

def CheckFileExist(runID=1,recacheList="recacheList.txt"):
    filenameStr='/cache/halla/happexsp/raw/prexLHRS_{}.dat.0'.format(runID)
    if int(runID) > 20000:
        filenameStr='/cache/halla/happexsp/raw/prexRHRS_{}.dat.0'.format(runID)
    if os.path.isfile(filenameStr):
        if Path(filenameStr).stat().st_size > 198432768:
            print('Find :: {}'.format(filenameStr))
            return True 
        else:
            #os.system('python3 /u/home/siyuj/Tools/jcache.py {}'.format(runID))
            with open(recacheList,"a") as fileio:
                fileio.write("{}\n".format(runID))
            print('Size too small  [re cache it] :: {}'.format(filenameStr))
            return False
    else:
        # print('Not Exist :: {}'.format(filenameStr))
        return False

def IsReplayed(runID=1,bashPath="/u/scratch/siyuj/Result"):

    filenameStr='/u/scratch/siyuj/Result/prexLHRS_{}_-1.root'.format(runID)
    if int(runID) > 20000:
        filenameStr='/u/scratch/siyuj/Result/prexRHRS_{}_-1.root'.format(runID)
    if os.path.isfile(filenameStr):
        print('Find :: {}'.format(filenameStr))
        return False
    else:
        # print('Not Exist :: {}'.format(filenameStr))
        return True

if __name__ == "__main__":
    if len(sys.argv) > 1:
        for runID in sys.argv[1:]:
            if runID.isdigit():
                if CheckFileExist(runID=runID) and IsReplayed(runID=runID):
                    if generateRunJobs(runID=runID):
                        runList.append(runID)
            else:
                if '.txt' in runID:
                    for infor in decodeRunFile(RunListFileName=runID):
                        if CheckFileExist(runID=infor) and IsReplayed(runID=infor):
                            if generateRunJobs(runID=infor):
                                runList.append(infor)
                if '-' in runID:
                    runTempRange=runID.split('-')
                    runStart=runTempRange[0]
                    runEnd=runTempRange[-1]
                    if runStart.isdigit() and runEnd.isdigit():
                        runStart=int(runStart)
                        runEnd=int(runEnd)
                        while runStart <= runEnd:
                            if CheckFileExist(runID=runStart) and IsReplayed(runID=runStart):
                                if generateRunJobs(runID=runStart):
                                    runList.append(runStart)
                                    runStart = runStart + 1
        for runID in runList:
            ss = "job/"+str(runID)+".txt"
            if os.path.isfile(ss):
                call(["jsub", ss])