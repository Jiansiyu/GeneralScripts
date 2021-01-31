
# jget/jcache script
 jget scripts used for automaticly get the data file from the mss. It can take multi-input format. It can take run number as input, it will automaticly generate RHRS or LHRS file name. It also can take filename, filename with full path or txt file which contains runlist. It also can take multi different type of input. 

Input Type, all those type can be conbined together


* Run number
* file name 
* file name with fullpath in the mss
* file name with run split number 
* file name without run split number

Other feature:

* it will check whether the file is in the mss or not, if not it will skip that run
* when the run split number is not give, it will check all the files in the mss and try to download all the files in the mss
* if given full name with split number, it will only download this file from mss
* all the input parameter position can be flipped


## input format

---
	python3 jget.py [input(can be anything from following)] [savePath(optional)]
---


#### How to set the file path that want to save the files from MSS

It can input from command line when run the scripts. If not specified, the file(s) will be save to the current path(./).

---
	python3 jget.py [input(can be anything from following)] [savePath(optional)]
---

#### How to set the run number want to get from MSS

##### 1). run number 

example command:

---
    ./jget 21110
    ./jget 2033
---

##### 2). run name 

example command

---
    ./jget prexRHRS_20532.dat      will read all the files that match prexRHRS_20532.dat.*
    ./jget prexRHRS_20532.dat.0    will download one file with name "prexRHRS_20532.dat.0"
---

##### 3). run list
example command

---
    ./jget runlist.txt      #Read file list name must end with .txt, otherwise it will not take it
---

##### 4). run range
Get all the runs with in range 
exaple command 

```c++
  ./jcache 1000-2000  #run start and end seperate with '-', no space allowed in between
```

###### format of the run list ffile 
* it can take 1 , 2, or 3 parameters
* all the string after '#' will be take as comments 
  
  Format: 

        run name (filename or filename with path or run number) start run ID (optional) end run ID(optional)
  examples:
---
       * prexRHRS_1111.dat,     0 , 10    # will dowload all the run split from 0 to 10
        
       * prexRHRS_1111.dat            # will dowload all the run split
       * prexRHRS_1111.dat.0          #  will download one file with name "prexRHRS_1111.dat.0"
       * 1111，     0 , 1             # will dowload all the run split from 0 to 10 for run 1111
    
       * 1111                        # will dowload all the run split for run 1111
       * 1111， 1                    #will only read the one with file run ID and run ID 
       
---

4) get runs from range 
```
pytho3 jcache.py 1000-2000
```
 
## How to change to other experiment other than PRex

Just need to change the filename into filename with full path.

## contact 
jiansiyu@gmail.com