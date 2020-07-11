# Hall A log book Crawler

## Usage

### Get the list of counting run List

```
curl -sL https://raw.githubusercontent.com/Jiansiyu/GeneralScripts/master/halog/halogcrawler.py | python3
```
The data file will be saved as runList.csv

### Generate the EPIC beamE bashscripts 

```
python3 halogcrawler.py [runID]....
```