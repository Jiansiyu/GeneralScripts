from subprocess import call
import sys
import os
nn = [i for i in range(0, 10)]

runList=[]

def generateRunJobs(runID=0):
    with open("job/"+str(runID)+".txt", "w") as txt:
        txt.write("PROJECT: PRex\n")
        if len(sys.argv) < 9:
            txt.write("TRACK: debug\n")
        else:
            txt.write("TRACK: analysis\n")
        
        txt.write("COMMAND: /w/halla-scifs17exp/parity/disk1/siyu/prex_replay/replay/runDp.csh {}\n".format(runID))
        # txt.write("OPTIONS: /lustre/expphy/work/hallb/prad/siyu/Simulation/PRadSim_build/run.mac\n")
        #txt.write("OTHER_FILES: /lustre/expphy/work/hallb/prad/xbai/PRadSim/build/database/*\n")
        txt.write("JOBNAME: counting_replay\n")
        txt.write("MEMORY: 4 GB\n")
        txt.write("DISK_SPACE: 20 GB\n")
        txt.write("OS: centos7\n")

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
if __name__ == "__main__":
    if len(sys.argv) > 1:
        for runID in sys.argv[1:]:
            if runID.isdigit():
                generateRunJobs(runID=runID)
                runList.append(runID)
            else:
                if '.txt' in runID:
                    for infor in decodeRunFile(RunListFileName=runID):
                        generateRunJobs(runID=infor)
                        runList.append(infor)
        for runID in runList:
            ss = "job/"+str(runID)+".txt"
            call(["jsub", ss])


# if __name__ == "__main__":
#     if len(sys.argv) > 1:
#         for runID in sys.argv[1:]:
#         # generate jsub job txt files
#             with open("job/"+str(runID)+".txt", "w") as txt:
#                 txt.write("PROJECT: PRex\n")
#                 if len(sys.argv) < 9:
#                     txt.write("TRACK: debug\n")
#                 else:
#                     txt.write("TRACK: analysis\n")
                
#                 txt.write("COMMAND: /w/halla-scifs17exp/parity/disk1/siyu/prex_replay/replay/runDp.csh {}\n".format(runID))
#                 # txt.write("OPTIONS: /lustre/expphy/work/hallb/prad/siyu/Simulation/PRadSim_build/run.mac\n")
#                 #txt.write("OTHER_FILES: /lustre/expphy/work/hallb/prad/xbai/PRadSim/build/database/*\n")
#                 txt.write("JOBNAME: counting_replay\n")
#                 txt.write("MEMORY: 4 GB\n")
#                 txt.write("DISK_SPACE: 20 GB\n")
#                 txt.write("OS: centos7\n")
#         # submit jobs
#         for runID in sys.argv[1:]:
#             ss = "job/"+str(runID)+".txt"
#             call(["jsub", ss])
