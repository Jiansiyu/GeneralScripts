import os
import matplotlib.pyplot as plt
import statistics
import pandas as pd

class beamEInfor(object):

    currentPath=os.getcwd()
    plotFolder='result'
    runListFile='runList/runList.txt'
    # BeamEFilePath='/home/newdriver/Storage/Server/JLabStorage/beamE'
    BeamEFilePath = './beamE'
    BeamEfileNamePattern='RHRS_{}_BeamE.txt'
    runList= {}    # {runID: runFullname}
    BeamEData={}   # {runID, [data]}   all the raw data
    BeanEAverageArray={}   # {runID, avaerage E for this run}


    def __init__(self):
        pass

    def LoadBeamEFile(self,runID=123, fileName=''):
        print("Loading Data {} :{} ".format(runID,fileName))
        if fileName:
            with open(fileName) as fileio:
                lines=fileio.readlines()
                if len(lines) > 10:
                    content=[x.strip() for x in lines]
                    beamEArray = [float(line.split()[-1]) for line in content]
                    beamEArray = beamEArray#[0:900]
                    self.BeamEData[runID]=beamEArray

    def BeamE_SingRun(self):
        '''
        Plot the single Run Beam E
        :return:
        '''
        beamEList={}
        for item in self.BeamEData:
            mean = statistics.mean(self.BeamEData[item])
            stdv = statistics.stdev(self.BeamEData[item])
            #  need to filt the data remove the beam shotdown
            df    =pd.DataFrame(self.BeamEData[item], index=[x for x in range(0,len(self.BeamEData[item]))],columns=[str(item)])
            meandf=pd.DataFrame([mean for x in range(0, len(self.BeamEData[item]))],index=[x for x in range(0,len(self.BeamEData[item]))], columns=['mean'])
            df=df.join(meandf)
            # print(df)
            print(("runID:{} -> {}".format(item, mean)))

            plt.plot(df)
            plt.text(10, mean, "Mean:{}, stdv{}".format(mean, stdv), size=18)
            plt.title('EPICS BeamE {}'.format(item))
            plt.savefig("./result/BeamE{}.jpg".format(item))
            plt.show()

            beamEList[item]=mean
        #write the beam e into file
        with open("beamE.txt","w") as fileio:
            for key, value in beamEList.items():
                fileio.write("{} {}\n".format(key,value))
        fileio.close()

    def GetRunList(self,runListfname=None):
        '''
        :param runListfname:
        :return:
        '''
        if runListfname is None:
            runListfname=self.runListFile
        with open(runListfname) as fileIO:
            lines=fileIO.readlines()
            for line in lines:

                if line.replace(' ',"").replace('\n','').isdigit():
                    runID=int(line)

                    runfullname=os.path.join(self.BeamEFilePath,self.BeamEfileNamePattern.format(runID))
                    print(runfullname)
                    if os.path.isfile(runfullname):
                        self.runList[runID]=runfullname
        print("{} files detected!!".format(len(self.runList)))

    def PlotE(self):
        pass

    def GetBeamEPlot(self):

        pass

    def test(self):
        self.GetRunList()
        for key, value in self.runList.items():
            print("{}: {}".format(key,value))
            self.LoadBeamEFile(runID=key,fileName=value)
        self.BeamE_SingRun()
        pass


if __name__ == '__main__':
    test=beamEInfor()
    test.test()