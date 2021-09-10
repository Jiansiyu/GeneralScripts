from subprocess import call

nn = [i for i in range(0, 248831,1000)]
# generate jsub job txt files
for i in nn:
    with open("job/"+str(i)+".txt", "w") as txt:
        txt.write("PROJECT: PRad\n")
        txt.write("TRACK: analysis\n")
        txt.write("COMMAND: /w/halla-scifs17exp/parity/disk1/siyu/OptScan/OptScanner/batchProfiler/runVerifier {} {}\n".format(i,i+1000))
        txt.write("JOBNAME: prex_opt_res_check\n")
        txt.write("MEMORY: 4 GB\n")
        txt.write("DISK_SPACE: 4 GB\n")

# submit jobs
# for i in nn:
#     ss = "job/"+str(i)+".txt"
#     call(["jsub", ss])
