#! /bin/csh
set startRunID = $1
set endRunID = $2

echo ${startRunID}
echo ${endRunID}
set increment = 10
set ncores = 8
seq -f %g $startRunID ${increment} $endRunID | xargs -i --max-procs=$ncores bash -c "echo check {} ; ./runVerifier {} > /dev/null; echo end check {}"
exit 0
