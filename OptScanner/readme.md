# Optimization Scanner Package


## post Run Check
Check the replayed folder whether replayed successfully. (it check the existant of the 2322 root file)

* same runConfig.json file as the run script
* if one of the files missing, it fill regenerate the runscript on 'postJobs' folder
### Start scan the check the folders 
```python
python postRunner.py
```
### re-run the optimization 
```python
cd postJobs
run-parts .
```


## Optimized Result Check Script [batchProfiler]

```python

./runVerifier startrunID endRunID 

```
or 
```python

./runVerifier startrunID # will check startRunID to startRunID + 1000
```

generate the ifarm ifarm slurm jobs
```python

./run.py 
```