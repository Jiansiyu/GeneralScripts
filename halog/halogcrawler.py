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
class halogCrawler(object):
    def __init__(self):
        self.baseurl='http://logbooks.jlab.org/api/elog/entries'
        pass
    def getTargetInfor(self, searchKey=''):
        '''
        :param searchKey:
        :return:
        '''
        pass

    def getCountingRunTime(self,runID=0):
        StartRunSearchKey='Start_Run_{}'.format(runID)
        EndRunSearchKey='End_of_Run_{}'.format(runID)

        pass


    def getStartTime(self, searchKey=''):
        '''
        :param searchKey:
        :return:
        '''
        pass
    def getEndTime(self, searchKey=''):
        '''
        :param searchKey:
        :return:
        '''
        pass

    def _writeDic2csv(self, header=['StartDate','lognumber','runID','run_type','production_target_type','optics_target_type','comment_text','url','StartTimestamp','fulltitle'],data={},filename=''):
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

        self._writeDic2csv(data=decodedData)

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
                            entryData['runID']=int(titleSplitItem.split('_')[-1])

                        if 'Run_type' in titleSplitItem:
                            entryData['run_type'] = titleSplitItem.split('=')[-1]

                        if 'production_target_type' in titleSplitItem:
                            entryData['production_target_type'] =  titleSplitItem.split('=')[-1]

                        if 'optics_target_type' in titleSplitItem:
                            entryData['optics_target_type'] =  titleSplitItem.split('=')[-1]

                        if 'comment_text' in titleSplitItem:
                            entryData['comment_text'] = titleSplitItem.split('=')[-1]


                    # decode the body content
                    if 'body' in item:
                        if 'content' in item['body']:
                            bodytext = item['body']['content']
                            for bodyItem in bodytext.split('\n'):
                                if 'Run Timestamp:' in bodyItem:
                                    entryData['StartTimestamp'] = bodyItem.replace('Run Timestamp:', '')



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

                    print(entryData)
                    decodedData.append(entryData)

        else:
            print('[Error]:: wrong return json')
        if len(decodedData) != totalItem:
            print("[Worning] :: the size does not match ")
        return  decodedData

    def test(self):
        self.getRunList(searchKey='Start_Run_',StartTime='2019-06-01',EndTime='2019-10-21')
    def testSingleRun(self):
        self.getCountingRunTime(runID=20891)

if __name__ == '__main__':
    test=halogCrawler()
    test.test()
    # test.testSingleRun()