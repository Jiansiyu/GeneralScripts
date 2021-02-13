source venv/bin/activate
workDir="/home/newdriver/Storage/Research/Eclipse_Workspace/photonSep2019/PRexOpt/OptScan_PRex_2021/result/LHRS/20210201_scan_Phi_test5/*"
echo "Will delet all file in folder : ${workDir}"
read -p "Are you sure re-run Opt? " -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
    rm ${workDir} -r
else
    exit -1
fi
python3 optScanGenerator.py
python3 optScan.py
python3 optScanResult.py