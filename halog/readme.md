# Hall A log book Crawler

## Usage

### Get the list of counting run List AND parity run list

```
curl -sL https://raw.githubusercontent.com/Jiansiyu/GeneralScripts/master/halog/halogcrawler.py | python3
```
The data file will be saved as runList.csv, ParityrunList.csv

### Get the list of counting run List

```
python3 halogcrawler.py counting
```

### Get the list of counting run List

```
python3 halogcrawler.py parity
```


### Generate the EPIC beamE bashscripts 

```
python3 halogcrawler.py [runID]....
```