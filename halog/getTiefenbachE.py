from sys import platform
import os
import json
import urllib
import urllib.request
import csv
import datetime
from time import sleep
import sys
import requests
import  re

from dateutil import tz
from dateutil.parser import parse

class halogCrawler(object):
    '''
    Get the counting run List

    Entrience Funtion
        *getRunList

    Typical Usage

    #  Get the Run List
    python3 halogcrawler.py
    
    # Gnerate the  BeamE bash sripts

    python3 halogcrawler.py [runID]...
    '''

    def __init__(self):
        self.baseurl='http://logbooks.jlab.org/api/elog/entries'
        pass
    def getTargetInfor(self, searchKey=''):
        '''
        :param searchKey:
        :return:
        '''
        pass

    def getCountingBeamEbash(self,runIDs=[]):
        currentTime=datetime.datetime.now()
        filename='BeamE{}.sh'.format(currentTime.strftime("_%Y_%m_%d_%H_%M_%S"))
        with open(filename,'w') as fileio:
            try:
                for id in runIDs:
                    print('Working on {}'.format(id))
                    RunstartTime, RunendTime=self.getCountingRunTime(runID=id)
                    if RunendTime and RunstartTime:
                        fileio.write('myget -b \"{startTime}\" -e \"{endTime}\" -c HALLA:p > RHRS_{runID}_BeamE.txt\n'.format(startTime=RunstartTime.strftime("%Y-%m-%d %H:%M:%S"),endTime=RunendTime.strftime("%Y-%m-%d %H:%M:%S"),runID=id))
            except IOError:
                print('[WORNING]: IO ERROR WHEN WRITING TO FILE')
        return  filename

    def getCountingRunTime(self,runID=0):
        try:
            StartRunSearchKey='Start_Run_{}'.format(runID)
            EndRunSearchKey='End_of_Run_{}'.format(runID)
            startTime=self.getStartTime(searchKey=StartRunSearchKey,runID=runID)
            endTime=self.getEndTime(searchKey=EndRunSearchKey,runID=runID)
            print(startTime.strftime("%Y-%m-%d %H:%M:%S"))
            print(endTime.strftime("%Y-%m-%d %H:%M:%S"))
            return startTime,endTime
        except:
            return None, None

    def getStartTime(self, searchKey='',runID=0):
        '''

        :param searchKey:
        :param runID:
        :return:
        '''
        startDatetime = datetime.datetime.strptime('2019-06-01', '%Y-%m-%d')
        endDatetime = datetime.datetime.strptime('2020-08-07', '%Y-%m-%d')
        url = '{}?book={}&title={}&startdate={}&enddate={}&field=lognumber&field=title&field=body&limit=1000'.format(
            self.baseurl, 'HALOG', searchKey, startDatetime.strftime("%Y-%m-%d"), endDatetime.strftime("%Y-%m-%d"))
        print('{}'.format(url))
        # urllib.request(url)
        with urllib.request.urlopen(url) as urldata:
            data = json.loads(urldata.read().decode())
            decodedData = []
            if data['stat'] == 'ok':
                totalItem = int(data['data']['totalItems'])
                pageLimit = int(data['data']['pageLimit'])

                if totalItem <= pageLimit and totalItem >= 1:

                    for item in data['data']['entries']:
                        entryData = {}
                        entryData['lognumber'] = item['lognumber']
                        entryData['url'] = 'https://logbooks.jlab.org/entry/{}'.format(item['lognumber'])

                        titleSplit = item['title'].split(',')

                        if not titleSplit[0].startswith(searchKey):
                            continue

                        entryData['fulltitle'] = item['title']

                        for titleSplitItem in titleSplit:
                            if searchKey in titleSplitItem:
                                if (titleSplitItem.split('_')[-1]).isdigit():
                                    entryData['runID']=int(titleSplitItem.split('_')[-1])
                                else:
                                    entryData['runID'] =0

                            if 'Run_type' in titleSplitItem:
                                entryData['run_type'] = titleSplitItem.split('=')[-1]

                            if 'production_target_type' in titleSplitItem:
                                entryData['production_target_type'] = titleSplitItem.split('=')[-1]

                            if 'optics_target_type' in titleSplitItem:
                                entryData['optics_target_type'] = titleSplitItem.split('=')[-1]

                            if 'comment_text' in titleSplitItem:
                                entryData['comment_text'] = titleSplitItem.split('=')[-1]

                        # decode the body content
                        if 'body' in item:
                            if 'content' in item['body']:
                                bodytext = item['body']['content']
                                if bodytext:
                                    for bodyItem in bodytext.split('\n'):
                                        if 'Run Timestamp:' in bodyItem:
                                            entryData['StartTimestamp'] = bodyItem.replace('Run Timestamp:', '')

                        if not 'runID' in entryData.keys():
                            entryData['runID'] = 0

                        if not 'run_type' in entryData.keys():
                            entryData['run_type'] = 'None'

                        if not 'production_target_type' in entryData.keys():
                            entryData['production_target_type'] = 'None'

                        if not 'optics_target_type' in entryData.keys():
                            entryData['optics_target_type'] = 'None'

                        if not 'StartTimestamp' in entryData.keys():
                            entryData['StartTimestamp'] = 'None'

                        print(entryData)
                        decodedData.append(entryData)
            else:
                print('[Error]:: wrong return json')
            if len(decodedData) != totalItem:
                print("[Worning] :: the size does not match ")

            ET = tz.gettz('US/Eastern')
            CT = tz.gettz('US/Central')
            MT = tz.gettz('US/Mountain')
            PT = tz.gettz('US/Pacific')

            us_tzinfos = {'CST': CT, 'CDT': CT,
                          'EST': ET, 'EDT': ET,
                          'MST': MT, 'MDT': MT,
                          'PST': PT, 'PDT': PT}

            dt_pst = parse(decodedData[0]['StartTimestamp'])
            print(dt_pst)
            return dt_pst

    def getEndTime(self, searchKey='',runID=0):
        '''
        :param searchKey:
        :param runID:
        :return:
        '''

        startDatetime = datetime.datetime.strptime('2019-06-01', '%Y-%m-%d')
        endDatetime = datetime.datetime.strptime('2020-08-07', '%Y-%m-%d')

        url = '{}?book={}&title={}&startdate={}&enddate={}&field=lognumber&field=title&field=body&limit=1000'.format(
            self.baseurl, 'HALOG', searchKey, startDatetime.strftime("%Y-%m-%d"), endDatetime.strftime("%Y-%m-%d"))
        print('{}'.format(url))
        # urllib.request(url)
        with urllib.request.urlopen(url) as urldata:
            data = json.loads(urldata.read().decode())
            decodedData = []
            if data['stat'] == 'ok':
                totalItem = int(data['data']['totalItems'])
                pageLimit = int(data['data']['pageLimit'])

                if totalItem <= pageLimit and totalItem >= 1:

                    for item in data['data']['entries']:
                        entryData = {}
                        entryData['lognumber'] = item['lognumber']
                        entryData['url'] = 'https://logbooks.jlab.org/entry/{}'.format(item['lognumber'])

                        titleSplit = item['title'].split(',')

                        if not titleSplit[0].startswith(searchKey):
                            continue

                        entryData['fulltitle'] = item['title']

                        for titleSplitItem in titleSplit:
                            if searchKey in titleSplitItem:
                                if (titleSplitItem.split('_')[-1]).isdigit():
                                    entryData['runID']=int(titleSplitItem.split('_')[-1])
                                else:
                                    entryData['runID'] =0

                            if 'Run_type' in titleSplitItem:
                                entryData['run_type'] = titleSplitItem.split('=')[-1]

                            if 'production_target_type' in titleSplitItem:
                                entryData['production_target_type'] = titleSplitItem.split('=')[-1]

                            if 'optics_target_type' in titleSplitItem:
                                entryData['optics_target_type'] = titleSplitItem.split('=')[-1]

                            if 'comment_text' in titleSplitItem:
                                entryData['comment_text'] = titleSplitItem.split('=')[-1]

                        # decode the body content
                        if 'body' in item:
                            if 'content' in item['body']:
                                bodytext = item['body']['content']
                                if bodytext:
                                    for bodyItem in bodytext.split('\n'):
                                        if 'End-of-run EPICS data' in bodyItem:
                                            entryData['EndTimestamp'] = bodyItem.replace('End-of-run EPICS data for Run Number {} at'.format(runID), '')

                        if not 'runID' in entryData.keys():
                            entryData['runID'] = 0

                        if not 'run_type' in entryData.keys():
                            entryData['run_type'] = 'None'

                        if not 'production_target_type' in entryData.keys():
                            entryData['production_target_type'] = 'None'

                        if not 'optics_target_type' in entryData.keys():
                            entryData['optics_target_type'] = 'None'

                        if not 'EndTimestamp' in entryData.keys():
                            entryData['EndTimestamp'] = 'None'

                        print(entryData)
                        decodedData.append(entryData)
            else:
                print('[Error]:: wrong return json')
            if len(decodedData) != totalItem:
                print("[Worning] :: the size does not match ")

            ET = tz.gettz('US/Eastern')
            CT = tz.gettz('US/Central')
            MT = tz.gettz('US/Mountain')
            PT = tz.gettz('US/Pacific')

            us_tzinfos = {'CST': CT, 'CDT': CT,
                          'EST': ET, 'EDT': ET,
                          'MST': MT, 'MDT': MT,
                          'PST': PT, 'PDT': PT}
            # decode the time
            # date_time_obj = datetime.strptime(decodedData[0]['EndTimestamp'], '')
            dt_pst = parse(decodedData[0]['EndTimestamp'])
            print(dt_pst)
            return  dt_pst


    def _writeDic2csv(self, header=['StartDate','lognumber','runID','run_type','production_target_type','optics_target_type','TiefenbachE','beam_current','url','StartTimestamp','comment_text','fulltitle'],data={},filename=''):
        if not filename:
            filename='text.csv'
        try:
            with open(filename,'w') as csvfile:
                writer=csv.DictWriter(csvfile,fieldnames=header)
                writer.writeheader()
                for dataItem in data:
                    writer.writerow(dataItem)
            pass
        except IOError:
            print("IO/ error when write data to file")
        pass
    def getRunList(self, searchKey='',StartTime='',EndTime='',timeBin=datetime.timedelta(days=1)):
        '''
        Search the Halog, and write the information to csv file
        :param searchKey:
        :param StartTime:
        :param EndTime:
        :return:
        '''
        # convert the time to the string
        startDatetime=datetime.datetime.strptime(StartTime,'%Y-%m-%d')
        endDatetime = datetime.datetime.strptime(EndTime, '%Y-%m-%d')

        decodedData=[]
        # binary the file
        while startDatetime < endDatetime:
            sleep(5)
            # form the query string
            nextTime=startDatetime+timeBin
            url='{}?book={}&title={}&startdate={}&enddate={}&field=lognumber&field=title&field=body&limit=1000'.format(self.baseurl,'HALOG',searchKey,startDatetime.strftime("%Y-%m-%d"),nextTime.strftime("%Y-%m-%d"))
            startDatetime=nextTime
            print('{}  {}'.format(startDatetime,url))
            # urllib.request(url)
            with urllib.request.urlopen(url) as urldata:
                data = json.loads(urldata.read().decode())
                for decodedItem in self._runListJsonDecoder(data=data):
                    decodedItem['StartDate']=startDatetime.strftime("%Y-%m-%d")
                    decodedData.append(decodedItem)

        self._writeDic2csv(data=decodedData,filename='runList.csv')

    def _runListJsonDecoder(self,data={}):
        '''
        :param data:
        :return:
        '''

        decodedData=[]

        if data['stat']=='ok':
            totalItem=int(data['data']['totalItems'])
            pageLimit=int(data['data']['pageLimit'])

            if totalItem <= pageLimit and totalItem >= 1:

                for item in data['data']['entries']:
                    entryData = {}
                    entryData['lognumber']=item['lognumber']
                    entryData['url'] = 'https://logbooks.jlab.org/entry/{}'.format(item['lognumber'])
                    titleSplit=item['title'].split(',')

                    entryData['fulltitle']=item['title']


                    for titleSplitItem in titleSplit:
                        if 'Start_' in titleSplitItem:
                            if (titleSplitItem.split('_')[-1]).isdigit():
                                entryData['runID']=int(titleSplitItem.split('_')[-1])
                            else:
                                entryData['runID'] =0

                        if 'Run_type' in titleSplitItem:
                            entryData['run_type'] = titleSplitItem.split('=')[-1]

                        if 'production_target_type' in titleSplitItem:
                            entryData['production_target_type'] =  titleSplitItem.split('=')[-1]

                        if 'optics_target_type' in titleSplitItem:
                            entryData['optics_target_type'] =  titleSplitItem.split('=')[-1]

                        if 'comment_text' in titleSplitItem:
                            if titleSplitItem.split('=')[-1]:
                                entryData['comment_text'] = titleSplitItem.split('=')[-1]

                    # decode the body content
                    if 'body' in item:
                        if 'content' in item['body']:
                            bodytext = item['body']['content']
                            if bodytext:
                                for bodyItem in bodytext.split('\n'):
                                    if 'Run Timestamp:' in bodyItem:
                                        entryData['StartTimestamp'] = bodyItem.replace('Run Timestamp:', '')
                            if bodytext:
                                for bodyItem in bodytext.split('\n'):
                                    if 'Tiefenbach' in bodyItem and 'MeV' in bodyItem:
                                        print(bodyItem.replace(' ','').split(':')[-1])
                                        entryData['TiefenbachE']=bodyItem.replace(' ','').split(':')[-1]

                            if bodytext:
                                for bodyItem in bodytext.split('\n'):
                                    if 'Beam Current :' in bodyItem:
                                        entryData['beam_current'] = bodyItem.replace(' ', '').split(':')[-1]


                    if not 'runID' in entryData.keys():
                        entryData['runID'] =0

                    if not 'run_type' in entryData.keys():
                        entryData['run_type'] = 'None'

                    if not 'production_target_type' in entryData.keys():
                        entryData['production_target_type'] = 'None'

                    if not 'optics_target_type' in entryData.keys():
                        entryData['optics_target_type'] = 'None'

                    if not 'StartTimestamp' in entryData.keys():
                        entryData['StartTimestamp'] = 'None'

                    if not 'comment_text' in entryData.keys():
                        entryData['comment_text'] = 'None'

                    if not 'Tiefenbach' in entryData.keys():
                        entryData['Tiefenbach'] = 'None'

                    if not 'beam_current' in entryData.keys():
                        entryData['beam_current'] = 'None'

                    print(entryData)
                    decodedData.append(entryData)

        else:
            print('[Error]:: wrong return json')
        if len(decodedData) != totalItem:
            print("[Worning] :: the size does not match ")
        return  decodedData



    def getParityRunList(self, searchKey='',StartTime='',EndTime='',timeBin=datetime.timedelta(days=1)):
        '''
        Search the Halog, and write the information to csv file
        :param searchKey:
        :param StartTime:
        :param EndTime:
        :return:
        '''
        # convert the time to the string
        startDatetime=datetime.datetime.strptime(StartTime,'%Y-%m-%d')
        endDatetime = datetime.datetime.strptime(EndTime, '%Y-%m-%d')

        decodedData=[]
        # binary the file
        while startDatetime < endDatetime:
            sleep(5)
            # form the query string
            nextTime=startDatetime+timeBin
            url='{}?book={}&title={}&startdate={}&enddate={}&field=lognumber&field=title&field=body&limit=1000'.format(self.baseurl,'HALOG',searchKey,startDatetime.strftime("%Y-%m-%d"),nextTime.strftime("%Y-%m-%d"))
            startDatetime=nextTime
            print('{}  {}'.format(startDatetime,url))
            # urllib.request(url)
            with urllib.request.urlopen(url) as urldata:
                data = json.loads(urldata.read().decode())
                for decodedItem in self._ParityrunListJsonDecoder(data=data):
                    decodedItem['StartDate']=startDatetime.strftime("%Y-%m-%d")
                    decodedData.append(decodedItem)

        self._writeDic2csv(data=decodedData,filename='ParityrunList.csv',header=['StartDate','lognumber','runID','Run_type','target_type','CODA_Config','beam_current','beam_energy','beam_raster','parity_feedback','parity_ihwp','fulltitle','url'])

    def _ParityrunListJsonDecoder(self,data={}):
        '''
        :param data:
        :return:
        '''
        decodedData=[]

        if data['stat']=='ok':
            totalItem=int(data['data']['totalItems'])
            pageLimit=int(data['data']['pageLimit'])

            if totalItem <= pageLimit and totalItem >= 1:
                for item in data['data']['entries']:
                    entryData = {}
                    entryData['lognumber']=item['lognumber']
                    entryData['url'] = 'https://logbooks.jlab.org/entry/{}'.format(item['lognumber'])
                    titleSplit=item['title'].split(',')

                    entryData['fulltitle']=item['title']


                    for titleSplitItem in titleSplit:
                        print(titleSplitItem)
                        try:
                            if 'Start_' in titleSplitItem:
                                if (titleSplitItem.split('_')[-1]).isdigit():
                                    entryData['runID']=int(titleSplitItem.split('_')[-1])
                                else:
                                    entryData['runID'] =0
                            if 'CODA_Config' in titleSplitItem:
                                entryData['CODA_Config'] = titleSplitItem.split('=')[-1]

                            if 'Run_type' in titleSplitItem:
                                entryData['Run_type'] = titleSplitItem.split('=')[-1]

                            if 'beam_current' in titleSplitItem:
                                entryData['beam_current'] = titleSplitItem.split('=')[-1]

                            if 'beam_energy' in titleSplitItem:
                                entryData['beam_energy'] = titleSplitItem.split('=')[-1]

                            if 'beam_raster' in titleSplitItem:
                                entryData['beam_raster'] = titleSplitItem.split('=')[-1]

                            if 'parity_feedback' in titleSplitItem:
                                entryData['parity_feedback'] = titleSplitItem.split('=')[-1]

                            if 'parity_ihwp' in titleSplitItem:
                                entryData['parity_ihwp'] = titleSplitItem.split('=')[-1]

                            if 'target_type' in titleSplitItem:
                                entryData['target_type'] = titleSplitItem.split('=')[-1]
                        except :
                            print("decode error")

                    # decode the body content
                    if 'body' in item:
                        if 'content' in item['body']:
                            bodytext = item['body']['content']
                            if bodytext:
                                for bodyItem in bodytext.split('\n'):
                                    if 'Run Timestamp:' in bodyItem:
                                        entryData['StartTimestamp'] = bodyItem.replace('Run Timestamp:', '')
                            if bodytext:
                                for bodyItem in bodytext.split('\n'):
                                    if 'Tiefenbach 6GeV Beam energy (MeV) :' in bodyItem:
                                        entryData['TiefenbachE'] = bodyItem.replace('Tiefenbach 6GeV Beam energy (MeV) :', '')
                                    else:
                                        entryData['TiefenbachE'] ='None'

                    if not 'runID' in entryData.keys():
                        entryData['runID'] =0

                    if not 'CODA_Config' in entryData.keys():
                        entryData['CODA_Config'] = 'None'

                    if not 'Run_type' in entryData.keys():
                        entryData['Run_type'] = 'None'

                    if not 'beam_current' in entryData.keys():
                        entryData['beam_current'] = 'None'

                    if not 'beam_energy' in entryData.keys():
                        entryData['beam_energy'] = 'None'

                    if not 'beam_raster' in entryData.keys():
                        entryData['beam_raster'] = 'None'

                    if not 'parity_feedback' in entryData.keys():
                        entryData['parity_feedback'] = 'None'

                    if not 'parity_ihwp' in entryData.keys():
                        entryData['parity_ihwp'] = 'None'

                    if not 'target_type' in entryData.keys():
                        entryData['target_type'] = 'None'

                    print(entryData)
                    decodedData.append(entryData)

        else:
            print('[Error]:: wrong return json')
        if len(decodedData) != totalItem:
            print("[Worning] :: the size does not match ")
        return  decodedData


    def GetChargeLog(self):
        '''
        Data Structure
            LogTime:
            runID:
            runType:
            profuction Target
            OPtics Target
            runType:
            url:
            coment_text:
        :return:
        '''
        pass

    def _getChargeParityRunList(self, searchKey='',StartTime='',EndTime='',timeBin=datetime.timedelta(days=1)):
        '''
        Search the Halog, and write the information to csv file
        :param searchKey:
        :param StartTime:
        :param EndTime:
        :return:
        '''
        # convert the time to the string
        startDatetime=datetime.datetime.strptime(StartTime,'%Y-%m-%d')
        endDatetime = datetime.datetime.strptime(EndTime, '%Y-%m-%d')

        decodedData=[]
        # binary the file
        while startDatetime < endDatetime:
            sleep(5)
            # form the query string
            nextTime=startDatetime+timeBin
            url='{}?book={}&title={}&startdate={}&enddate={}&field=lognumber&field=title&field=body&limit=1000'.format(self.baseurl,'HALOG',searchKey,startDatetime.strftime("%Y-%m-%d"),nextTime.strftime("%Y-%m-%d"))
            startDatetime=nextTime
            print('{}  {}'.format(startDatetime,url))
            # urllib.request(url)
            with urllib.request.urlopen(url) as urldata:
                data = json.loads(urldata.read().decode())
                for decodedItem in self._ParityrunListJsonDecoder(data=data):
                    decodedItem['StartDate']=startDatetime.strftime("%Y-%m-%d")
                    decodedData.append(decodedItem)
        return  decodedData

    def _getChargeRunList(self, searchKey='',StartTime='',EndTime='',timeBin=datetime.timedelta(days=1)):
        '''
        Search the Halog, and write the information to csv file
        :param searchKey:
        :param StartTime:
        :param EndTime:
        :return:
        '''
        # convert the time to the string
        startDatetime=datetime.datetime.strptime(StartTime,'%Y-%m-%d')
        endDatetime = datetime.datetime.strptime(EndTime, '%Y-%m-%d')

        decodedData=[]
        # binary the file
        while startDatetime < endDatetime:
            sleep(5)
            # form the query string
            nextTime=startDatetime+timeBin
            url='{}?book={}&title={}&startdate={}&enddate={}&field=lognumber&field=title&field=body&limit=1000'.format(self.baseurl,'HALOG',searchKey,startDatetime.strftime("%Y-%m-%d"),nextTime.strftime("%Y-%m-%d"))
            startDatetime=nextTime
            print('{}  {}'.format(startDatetime,url))
            # urllib.request(url)
            with urllib.request.urlopen(url) as urldata:
                data = json.loads(urldata.read().decode())
                for decodedItem in self._runListJsonDecoder(data=data):
                    decodedItem['StartDate']=startDatetime.strftime("%Y-%m-%d")
                    decodedData.append(decodedItem)

        return  decodedData

    


    def test(self):
        self.getRunList(searchKey='Start_Run_',StartTime='2019-06-01',EndTime='2020-09-20')
    def testSingleRun(self):
        self.getCountingRunTime(runID=20891)

    def testPariyRunList(self):
        self.getParityRunList(searchKey='Start_Parity_Run_',StartTime='2019-06-01',EndTime='2020-09-20')



if __name__ == '__main__':
    test = halogCrawler()
    if len(sys.argv) ==1:
        test.testPariyRunList()
        test.test()
    else:
        runIDList=[]
        for item in sys.argv:
            if item.isdigit():
                runIDList.append(item)
            else:
                if 'parity' in item.lower():
                    test.testPariyRunList()
                else:
                    if 'counting' in item.lower():
                        test.test()
        if 'jlab' in os.uname()[1]:
            test.getCountingBeamEbash(runIDs=sys.argv[1:])
        else:
            print('[ERROR]:: beamE generator only can works on the APAR on aonl machine!!!!')
