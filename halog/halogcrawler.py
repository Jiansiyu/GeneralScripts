from sys import platform
import os
import json
import urllib
import urllib.request
import csv
import datetime
import requests

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

        # binary the file
        while startDatetime < endDatetime:
            # form the query string
            nextTime=startDatetime+timeBin
            url='{}?book={}&title={}&startdate={}&enddate={}&field=lognumber&field=title'.format(self.baseurl,'HALOG',searchKey,startDatetime.strftime("%Y-%m-%d"),nextTime.strftime("%Y-%m-%d"))
            startDatetime=nextTime
            print('{}  {}'.format(startDatetime,url))
            # urllib.request(url)
            with urllib.request.urlopen(url) as urldata:
                data = json.loads(urldata.read().decode())
                print(data)

    def _runListJsonDecoder(self):
        pass

    def test(self):
        self.getRunList(searchKey='Start_Run_',StartTime='2019-08-06',EndTime='2019-08-09')
if __name__ == '__main__':
    test=halogCrawler()
    test.test()