"""
"""

from os import path
from ROOT import TCanvas, TFile, TH1F,TH2F
from ROOT import gROOT, gBenchmark
import sys
import json    # used for creat template
#from pptx import Presentation

class pyROOTHistoReader(object):
    def __init__(self,filename=None):
        '''
        
        '''
        self.LoadFile(filename=filename)
    def LoadFile(self,filename=None):
        '''

        '''
        if filename is not None:
            self.file=TFile(filename)

    def ReadHisto(self,HistNamePattern="",number=[]):
        '''
        
        '''
        HistoArray=[]
        if self.file is not None:
            for i in number:
                HistoArray.append(self.file.Get(format(HistNamePattern % i)))
        return HistoArray

    def SaveHisto(self,HistoArray=[],CanvasDivied=[3,1],CanvasName="",CanvasSizeX=1800,CanvasSizeY=600,DrawOption="colz",CanvasSaveName="save.jpg"):
        '''
        Used for save the histogram in one canvas
        '''
        canvas = TCanvas(CanvasName,CanvasName,CanvasSizeX,CanvasSizeY);
        canvas.Divide(CanvasDivied[0],CanvasDivied[1])
        counter = 1
        for i in HistoArray:
            canvas.cd(counter)
            counter=counter+1
            i.Draw(DrawOption)
        canvas.Save(CanvasSaveName)


class SlidesInterface(object):
    def __init__(self):
        pass
    def CreateNew(self,fName=""):
        pass
    def CreateSlidesWithTemplate(self,template=[]):
        pass
    def Save(self):
        pass


if __name__=='__main__':
    print(sys.argv)
   
    file = TFile("/home/newdriver/Research/Eclipse_Workspace/photon/mpd4_vme_test_j1-1/results/test.root")
    
    Hist2D=[]
    canvas = TCanvas("C","C",1800,600)
    canvas.Divide(3,1)
    for i in range (0,3):
        #Hist2D.append(format("hhClusterDist_%d" % i))
        canvas.cd(i+1)
        Hist2D.append( file.Get(format("hhClusterDist_%d" % i)))
        Hist2D[i].Draw("colz")
    canvas.Print("test.jpg")
    print(Hist2D)
    