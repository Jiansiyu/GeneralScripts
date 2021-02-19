# !/bin/csh
source /u/home/siyuj/env_set/setPRex.csh
set startRunID = 0
set endRunID  = 6
set ncores = 4
set Index = $startRunID
 

seq -f %06g $startRunID $endRunID | xargs -i --max-procs=$ncores bash -c "/work/halla/parity/disk1/siyu/OptScan/OptScanner/script/OptScanner_ifarm.csh /work/halla/parity/disk1/siyu/OptScan/Opt /work/halla/parity/disk1/siyu/OptScan/OptScanner/Result/Run4/DBScan_20210218_{}"
