'''
Used for 
'''
import unicodedata
import os
import json
from collections import OrderedDict
from progress.bar import Bar
import datetime
from datetime import date
from random import seed
from random import random, randint
from multiprocessing import Pool     # multithread process the single files
import statistics
import shutil
from progress.bar import Bar
import glob
from fpdf import FPDF
import PIL
import sys

class OptScannerResult(object):
    def __init__(self,runConfigFname="runConfig.json"):
        self.runConfigJsonFname=runConfigFname
        self.OptConfigFname=""
        self.TargetPath=""
        self.CurrentWorkFolder=""
        self.OptTemplatedOptmizedFolders=[]
        self.OptTemplateSubFolders=[]
        self.OptSourceFolder=""
        self.optScannerBashScript=""
        self.reportSizeLimit =200
        self.pdfsavePath = "./Report"
        self.folderIndexTemplate = "{:06}"
        self.LoadConfig()
        # get the subfolders in the given path
        # self.GetSubFoldersFast()
        # self.GetSubFolders()
        self.GetSubFoldersBinTree()

    def LoadConfig(self, runConfigFname=""):
        if not runConfigFname:
            runConfigFname=self.runConfigJsonFname
        print(self.runConfigJsonFname)
        with open(runConfigFname) as runCondigFile:
            self.runConfig_data=json.load(runCondigFile,object_pairs_hook=OrderedDict) # keep the initial order of the database
            self.OptConfigFname=self.runConfig_data["optConfigFname"]
            self.TargetPath=self.runConfig_data["TargetPath"]
            self.OptSourceFolder=self.runConfig_data["OptSourceFolder"]
            self.optScannerBashScript=self.runConfig_data["optScannerBashScript"]
            self.reportSizeLimit = self.runConfig_data["reportSizeLimit"]
            self.pdfsavePath = self.runConfig_data["pdfsavePath"]

        if not os.path.isdir(self.pdfsavePath):
            os.mkdir(self.pdfsavePath)

    def SetTargetFolder(self, topFolder=""):
        self.TargetPath=topFolder

    def _addFolderFast(self,folder=""):
        filename = os.path.join(folder,"templateDB.db.optimied")
        self.OptTemplateSubFolders.append(folder)
        if os.path.isfile(filename):
            self.OptTemplatedOptmizedFolders.append(folder)

    def _getSubFolderBT(self,mainFolderTemplate = "",startIndex = 0, endIndex = 1000000):
        randomNumb_surFix = self.folderIndexTemplate.format(endIndex)
        foldername = mainFolderTemplate.format(randomNumb_surFix)
        while os.path.isdir(foldername):
            endIndex = 2*endIndex
            randomNumb_surFix = self.folderIndexTemplate.format(endIndex)
            foldername = mainFolderTemplate.format(randomNumb_surFix)

        start = startIndex
        end = endIndex

        while start<end:
            mid = (start + end) // 2
            randomNumb_surFix = self.folderIndexTemplate.format(mid)
            foldername = mainFolderTemplate.format(randomNumb_surFix)
            if os.path.isdir(foldername):
                start = mid+1
            else:
                end = mid
        return start-1

    def GetSubFoldersBinTree(self,topFolder="",maxDateLookBack=10,maxFileIndex = 100000):
        if not topFolder:
            topFolder = self.TargetPath
        today = date.today()
        datePreFix = today.strftime("%Y%m%d")

        for i in range(0, maxDateLookBack):
            curr = today - datetime.timedelta(days=i)
            folder0 = os.path.join(topFolder, "DBScan_{}_{}".format(curr.strftime("%Y%m%d"),self.folderIndexTemplate.format(0)));
            if os.path.isdir(os.path.join(topFolder, "DBScan_{}_{}".format(curr.strftime("%Y%m%d"),self.folderIndexTemplate.format(0)))):
                filenameTemplate =os.path.join(os.path.join(topFolder, "DBScan_{}".format(curr.strftime("%Y%m%d"))))
                filenameTemplate = filenameTemplate + "_{}"
                endIndex = self._getSubFolderBT(filenameTemplate)
                bar = Bar("Optimization Result Looking Back {}".format(curr.strftime("%Y%m%d")), max=endIndex)
                for runIndex in range(0,endIndex+1):
                    randomNumb_surFix = self.folderIndexTemplate.format(runIndex)
                    pathName = os.path.join(topFolder,"DBScan_{}_{}".format(curr.strftime("%Y%m%d"),randomNumb_surFix))
                    filename = os.path.join(topFolder,"DBScan_{}_{}".format(curr.strftime("%Y%m%d"),randomNumb_surFix),"templateDB.db.optimied")
                    self.OptTemplateSubFolders.append(pathName)
                    self.OptTemplatedOptmizedFolders.append(pathName)
                    # if os.path.isfile(filename):
                    #     self.OptTemplatedOptmizedFolders.append(pathName)
                    bar.next()
                bar.finish()
            else:
                print("skip folder:{}".format(folder0))

        print("Total Processed Template {} / {}\n\n\n".format(len(self.OptTemplatedOptmizedFolders),
                                                              len(self.OptTemplateSubFolders)))

    def GetSubFoldersFast(self,topFolder="",maxDateLookBack=10,maxFileIndex = 100000):
        if not topFolder:
            topFolder=self.TargetPath
        today=date.today()
        datePreFix=today.strftime("%Y%m%d")

        for i in range(0,maxDateLookBack):
            curr = today - datetime.timedelta(days=i)
            folder0 = os.path.join(topFolder,"DBScan_{}_{}".format(curr.strftime("%Y%m%d"),self.folderIndexTemplate.format(0)));
            if os.path.isdir(os.path.join(topFolder,"DBScan_{}_{}".format(curr.strftime("%Y%m%d"),self.folderIndexTemplate.format(0)))):
                bar = Bar("Optimization Result Looking Back {}".format(curr.strftime("%Y%m%d")), max=maxFileIndex)

                # bin tree search instead of loop for the run one by on

                for fileIndex in range(0,maxFileIndex):
                    randomNumb_surFix = self.folderIndexTemplate.format(fileIndex)
                    if os.path.isdir(os.path.join(topFolder,"DBScan_{}_{}".format(curr.strftime("%Y%m%d"),randomNumb_surFix))):
                        self.OptTemplateSubFolders.append(os.path.join(topFolder,"DBScan_{}_{}".format(curr.strftime("%Y%m%d"),randomNumb_surFix)))
                        if os.path.isfile(os.path.join(topFolder,"DBScan_{}_{}".format(curr.strftime("%Y%m%d"),randomNumb_surFix),"templateDB.db.optimied")):
                            self.OptTemplatedOptmizedFolders.append(os.path.join(topFolder,"DBScan_{}_{}".format(curr.strftime("%Y%m%d"),randomNumb_surFix)))
                            bar.next()
                    else:
                        break
                bar.finish()
            else:
                print("skip folder:{}".format(folder0))

        print("Total Processed Template {} / {}\n\n\n".format(len(self.OptTemplatedOptmizedFolders),len(self.OptTemplateSubFolders)))

    def GetSubFolders(self, topFolder=""):
        if not topFolder:
            topFolder=self.TargetPath
        self.OptTemplateSubFolders= [f.path for f in os.scandir(topFolder) if f.is_dir()]
        self.OptTemplatedOptmizedFolders=[item for item in self.OptTemplateSubFolders if os.path.isfile("{}/templateJson.json".format(item))]
        print("Total Processed Template {} / {}\n\n\n".format(len(self.OptTemplatedOptmizedFolders),len(self.OptTemplateSubFolders)))

    
    def ReadCheckDpResultText(self):
        bar=Bar("Looking for result:: ", max=len(self.OptTemplatedOptmizedFolders))
        #for path in self.OptTemplateSubFolders:
        for path in self.OptTemplatedOptmizedFolders:
            bar.next()
            txtFilename="{}/CheckDp_test_result.txt".format(path)
            if os.path.isfile(txtFilename):
                with open(txtFilename) as txtFileIO:
                    try:
                        result=json.load(txtFileIO)
                        standDevLib=[]
                        for item in result["DpSeperation"]:
                            if item != '999':
                                #print("{}:{}".format(item,result["DpSeperation"][item]))
                                standDevLib.append(float(result["DpSeperation"][item]))
                        #if float(result["DpSeperation"]['0']) > 4.4 and float(result["DpSeperation"]['3']) < 4.5 and float(result["DpSeperation"]['3']) > 4.4:
                        print("\n{}==>Mean:{}, stdv:{}\n{}\n".format(standDevLib,statistics.mean(standDevLib[1:]),statistics.stdev(standDevLib[1:]),path))
                    except :
                        pass
        bar.finish()
    
    def ReadSingleCheckDpResultText(self,txtResultPath=""):
        print("Total Processed Template {} / {}\n\n\n".format(len(self.OptTemplatedOptmizedFolders),0))#,float(self.OptTemplatedOptmizedFolders)/self.OptTemplateSubFolders))
        txtFilename="{}/templateDB.db".format(txtResultPath)
        if os.path.isfile(txtFilename):
            with open(txtFilename) as txtFileIO:
                result=json.load(txtFileIO)
                standDevLib=[]
                for item in result["DpSeperation"]:
                    if item != '999':
                        standDevLib.append(float(result["DpSeperation"][item]))
                if float(result["DpSeperation"]['0']) > 4.4 and float(result["DpSeperation"]['3']) < 4.5:
                    print("{}==>Mean:{}, stdv:{}".format(standDevLib,statistics.mean(standDevLib),statistics.stdev(standDevLib)))

    def _getImageSize(self,imageFile):
        cover=PIL.Image.open(imageFile)
        width, height=cover.size
        return width,height
    
    def addimage(self, pdf=FPDF(),file1="",item=""):
        width, height =self._getImageSize(file1)
        width, height = float(width * 0.264583), float(height * 0.264583)
        pdf_size = {'P': {'w': 210, 'h': 297}, 'L': {'w': 297, 'h': 210}}
        orientation = 'P' if width < height else 'L'
        width = width if width < pdf_size[orientation]['w'] else pdf_size[orientation]['w']
        height = height if height < pdf_size[orientation]['h'] else pdf_size[orientation]['h']
        pdf.add_page(orientation=orientation)
        pdf.image(file1,0, 0, width, height)
        # pdf.set_font('Arial', 'B', 10)
        if item:
            pdf.set_font('Arial', 'B', 15)
            pdf.cell(50, -10,"[Click me to Open Folder]",link="file:///{}".format(item),ln=1,align='C')
        if file1 and not item:
            pdf.set_font('Arial', 'B', 10)
            pdf.cell(10, -10, "{}".format(file1.split('/')[-2].split('_')[1]), link="file:///{}".format(item), ln=1, align='C')

    def addimages(self,pdf=FPDF(),fileList=[],item=""):
        for image in fileList:
            if not os.path.isfile(image):
                return None
        if len(fileList)==0:
            return None
        width, height =self._getImageSize(fileList[0])
        width, height = float(width * 0.264583), float(height * 0.264583)
        pdf_size = {'P': {'w': 210, 'h': 297}, 'L': {'w': 297, 'h': 210}}
        orientation = 'P' if width < height else 'L'
        width = width if width < pdf_size[orientation]['w'] else pdf_size[orientation]['w']
        height = height if height < pdf_size[orientation]['h'] else pdf_size[orientation]['h']
        pdf.add_page(orientation=orientation)

        if len(fileList) <= 4:
            for file1 in fileList:
                id=fileList.index(file1)
                row=id // 2
                col=id % 2
                pdf.image(file1,col*width/2,row*height/2,width/2,height/2)
        elif len(fileList) <= 6:
            for file1 in fileList:
                id=fileList.index(file1)
                row=id // 2
                col=id % 2
                pdf.image(file1,col*width/2,row*height/3,width/2,height/3)
                # add the file name
                # pdf.set_font('Arial', 'B', 10)
                # pdf.cell(width/2,10,file1.split("/")[-2],ln=0,align='C')
        else:
            for file1 in fileList:
                spliter = len(fileList)//2
                id = fileList.index(file1)
                row = id // 2
                col = id % 2
                pdf.image(file1, col * width / 2, row * height / spliter, width / 2, height / spliter)

        if item:
            pdf.set_font('Arial', 'B', 10)
            pdf.cell(50, -13,"[Click me to Open Folder]",link="file:///{}".format(item),ln=1,align='C')

    def addTxtFile(self, pdf=FPDF(),txtFile=""):
        if os.path.isfile(txtFile):
            pdf.add_page(orientation='P')
            pdf.set_font("Arial", size = 10) 
            f = open(txtFile, "r")
            for x in f:
                if x.startswith('D'):
                    pdf.cell(200,10,txt=x,ln=1,align='L')


    def addimages_3(self,pdf=FPDF(),fileList=[],item=""):
        if len(fileList) != 5:
            return None
        for item in fileList:
            if not os.path.isfile(item):
                return None
        width, height = self._getImageSize(fileList[0])
        width, height = float(width * 0.264583), float(height * 0.264583)
        pdf_size = {'P': {'w': 210, 'h': 297}, 'L': {'w': 297, 'h': 210}}
        orientation = 'P' if width < height else 'L'
        width = width if width < pdf_size[orientation]['w'] else pdf_size[orientation]['w']
        height = height if height < pdf_size[orientation]['h'] else pdf_size[orientation]['h']
        pdf.add_page(orientation=orientation)

        # add the first image
        pdf.image(fileList[0], 0, 0, width, height)
        pdf.image(fileList[1],-width*1/3,height/3,width,height)
        pdf.set_font('Arial', 'B', 10)
        pdf.cell(10, 130, "{}".format(fileList[1].split('/')[-2].split('_')[1]), ln=1,
                 align='C')

        pdf.image(fileList[2], width*1/3, height/3,width,height)
        pdf.set_font('Arial', 'B', 10)
        pdf.cell(400, -130, "{}".format(fileList[2].split('/')[-2].split('_')[1]), ln=1,
                 align='C')

        pdf.image(fileList[3], -width * 1 / 3, height*2/3, width, height)
        pdf.image(fileList[4], width * 1 / 3, height*2/ 3, width, height)

        if item:
            pdf.set_font('Arial', 'B', 10)
            pdf.cell(50, -13,"[Click me to Open Folder]",link="file:///{}".format(item),ln=1,align='C')

    def getLargeRunReport(self,txtResultPath=[]):
        '''

        '''

        if len(txtResultPath) == 0:
            txtResultPath=self.OptTemplatedOptmizedFolders
        txtResultPath.sort()

        ResultChop = []
        if len(txtResultPath) > self.reportSizeLimit:
            indexCounter = 0
            while indexCounter*self.reportSizeLimit < len(txtResultPath):
                print("Generating Report {}/{}....".format(indexCounter+1, len(txtResultPath)//self.reportSizeLimit+1))
                headIndex = indexCounter*self.reportSizeLimit
                tailIndex = min((indexCounter+1)*self.reportSizeLimit, len(txtResultPath))
                currArray = txtResultPath[headIndex:tailIndex]
                currArray.insert(0,"{}/resultScanReport_{}.pdf".format(self.pdfsavePath,indexCounter)) # the first element will be used as file name
                ResultChop.append(currArray)
                indexCounter = indexCounter + 1
        else:
            self.getReport()
            return

        # concurrnt approach
        threadPool = Pool(80)
        threadPool.map(self.getReportFast, ResultChop)

    def getReportFast(self,txtResultPath=[]):

        if len(txtResultPath) == 0:
            txtResultPath = self.OptTemplatedOptmizedFolders

        pdffilename = './resultScanReport.pdf'

        if ".pdf" in txtResultPath[0]:
            pdffilename = txtResultPath[0]
            txtResultPath = txtResultPath[1:]
        txtResultPath.sort()

        bar = Bar("Processing", max=len(txtResultPath))
        pdf = FPDF(orientation='L', unit='mm', format='A4')
        id = 0
        for item in txtResultPath:
            self.CompileLatex(texpath=item)
            fileList = [
                os.path.join(item, "Sieve._2241_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
                os.path.join(item, "Sieve._2240_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
                os.path.join(item, "Sieve._2257_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
                os.path.join(item, "Sieve._2256_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
                os.path.join(item, "Sieve._2244_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
                os.path.join(item, "Sieve._2245_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg")
            ]
            # self.addimages(pdf=pdf, fileList=fileList, item=item)

            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve.Full_LargeDataSet.f51/CheckSieve_SieveCheck2.jpg"),item=item)
            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve._2239_p4.f51_reform/CheckSieve_SieveCheck2.jpg"))
            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve._2241_p4.f51_reform/CheckSieve_SieveCheck2.jpg"))
            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve._2244_p4.f51_reform/CheckSieve_SieveCheck2.jpg"))
            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve._2245_p4.f51_reform/CheckSieve_SieveCheck2.jpg"))

            fileList1 = [
                os.path.join(item, "Sieve.Full_LargeDataSet.f51/CheckSieve_SieveCheck2.jpg"),
                os.path.join(item, "Sieve._2241_p4.f51_reform/CheckSieve_SieveCheck2.jpg"),
                os.path.join(item, "Sieve._2239_p4.f51_reform/CheckSieve_SieveCheck2.jpg"),
                os.path.join(item, "Sieve._2245_p4.f51_reform/CheckSieve_SieveCheck2.jpg"),
                os.path.join(item, "Sieve._2244_p4.f51_reform/CheckSieve_SieveCheck2.jpg")
            ]
            self.addimages_3(pdf=pdf,fileList=fileList1,item=item)

            # fileList = [
            #     os.path.join(item, "Sieve._2241_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
            #     # os.path.join(item, "Sieve._2239_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
            #     os.path.join(item, "Sieve._2245_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
            #     os.path.join(item, "Sieve._2244_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg")
            # ]
            # self.addimages(pdf=pdf,fileList=fileList,item=item)
            bar.next()
        bar.finish()
        print("Creating the PDF file")
        pdf.output(pdffilename, "F")
        shutil.copyfile(pdffilename, os.path.join(self.TargetPath, "report.pdf"))
        pass

    def getReport(self,pdffilename='./resultScanReport.pdf',txtResultPath=[]):
        if len(txtResultPath) == 0:
            txtResultPath=self.OptTemplatedOptmizedFolders

        txtResultPath.sort()

        bar=Bar("Processing",max=len(txtResultPath))
        # print("Total Processed Template {} / {}\n\n\n".format(len(self.OptTemplatedOptmizedFolders),0))#,float(self.OptTemplatedOptmizedFolders)/self.OptTemplateSubFolders))
        pdf=FPDF(orientation = 'L', unit = 'mm', format='A4')
        id=0
        for item in txtResultPath:
            self.CompileLatex(texpath=item)
            # print("Working on file:{}".format(txtResultPath))
            # pdf.add_page()
            # pdf.set_font('Arial', 'B', 28)
            # pdf.cell(300, 50, "Run {}".format(id),ln=1,align='C')
            # id = id+1
            # pdf.cell(300, 60,"Click me to Open Folder",link="file:///{}".format(item),ln=1,align='C')
            # pdf.cell(0, 0, "{}".format(item),link="file:///{}".format(item))
            # self.addimage(pdf=pdf,file1=os.path.join(item,"CheckDp_test_RealMomemtumDifferenceCanv.png"))
            # self.addimage(pdf=pdf,file1=os.path.join(item,"CheckDp_test_DpAllCanv.jpg"))
            #self.addimage(pdf=pdf,file1=os.path.join(item,"CheckDp_test2_DpAllCanv.jpg"))
            #self.addimage(pdf=pdf,file1=os.path.join(item,"CheckDp_test2_MomemtumOptCanv.jpg"))
            #self.addimage(pdf=pdf,file1=os.path.join(item,"CheckDp_test_DpAllCanv.jpg"))
            # self.addimage(pdf=pdf,file1=os.path.join(item,"CheckSieve_CThetaCorrectionError.jpg"),item=item)
            # fileList=[
            #     os.path.join(item,"Sieve._2241_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
            #     os.path.join(item,"Sieve._2245_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
            #     os.path.join(item,"Sieve._2240_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
            #     os.path.join(item,"Sieve._2239_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg")
            # ]
            #
            fileList = [
                os.path.join(item, "Sieve._2241_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
                os.path.join(item, "Sieve._2240_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
                os.path.join(item, "Sieve._2257_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
                os.path.join(item, "Sieve._2256_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
                os.path.join(item, "Sieve._2244_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
                os.path.join(item, "Sieve._2245_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg")
            ]
            self.addimages(pdf=pdf,fileList=fileList,item=item)

            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve.Full_LargeDataSet.f51/CheckSieve_SieveCheck2.jpg"),item=item)
            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve._2239_p4.f51_reform/CheckSieve_SieveCheck2.jpg"))
            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve._2241_p4.f51_reform/CheckSieve_SieveCheck2.jpg"))
            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve._2244_p4.f51_reform/CheckSieve_SieveCheck2.jpg"))
            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve._2245_p4.f51_reform/CheckSieve_SieveCheck2.jpg"))

            fileList1 = [
                os.path.join(item, "Sieve.Full_LargeDataSet.f51/CheckSieve_SieveCheck2.jpg"),
                os.path.join(item, "Sieve._2241_p4.f51_reform/CheckSieve_SieveCheck2.jpg"),
                os.path.join(item, "Sieve._2239_p4.f51_reform/CheckSieve_SieveCheck2.jpg"),
                os.path.join(item, "Sieve._2245_p4.f51_reform/CheckSieve_SieveCheck2.jpg"),
                os.path.join(item, "Sieve._2244_p4.f51_reform/CheckSieve_SieveCheck2.jpg")
            ]
            # self.addimages_3(pdf=pdf,fileList=fileList1,item=item)


            # fileList = [
            #     os.path.join(item, "Sieve._2241_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
            #     # os.path.join(item, "Sieve._2239_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
            #     os.path.join(item, "Sieve._2245_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg"),
            #     os.path.join(item, "Sieve._2244_p4.f51_reform/CheckSieve_CThetaCorrectionError.jpg")
            # ]
            # self.addimages(pdf=pdf,fileList=fileList,item=item)

            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve._2239_p4.f51_reform/CheckSieve_SieveCheck2.jpg"))
            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve._2240_p4.f51_reform/CheckSieve_SieveCheck2.jpg"))
            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve._2241_p4.f51_reform/CheckSieve_SieveCheck2.jpg"))
            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve._2244_p4.f51_reform/CheckSieve_SieveCheck2.jpg"))
            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve._2245_p4.f51_reform/CheckSieve_SieveCheck2.jpg"))
            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve._2256_p4.f51_reform/CheckSieve_SieveCheck2.jpg"))
            # self.addimage(pdf=pdf, file1=os.path.join(item, "Sieve._2257_p4.f51_reform/CheckSieve_SieveCheck2.jpg"))
            # self.addimages(pdf=pdf, fileList=fileList, item=item)
            bar.next()
        bar.finish()
        print("Creating the PDF file")
        pdf.output(pdffilename,"F")
        shutil.copyfile(pdffilename,os.path.join(self.TargetPath,"report.pdf"))
        
    def ReadCheckDpMultiThread(self,maxThread=5):
        threadPool=Pool(maxThread)
        threadPool.map(self.ReadSingleCheckDpResultText, self.OptTemplateSubFolders)
        # self.getReport(txtResultPath=self.OptTemplatedOptmizedFolders)
    
    def CompileLatex(self, texpath):
        for item in glob.glob("{}/*tex".format(texpath)):
            os.system("cd {}; pdflatex {}".format(texpath, item))
            # os.system("mv {}/*tex {}/texpdf".format(texpath, texpath))
            # os.system("mv {}/*pdf {}/texpdf".format(texpath, texpath))
            # os.system("mv {}/*log {}/texpdf".format(texpath, texpath))
            # os.system("mv {}/*aux {}/texpdf".format(texpath, texpath))
        for item in glob.glob("{}/texpdf/*tex".format(texpath)):
            os.system("cd {}/texpdf; pdflatex {}".format(texpath, item))
            
        
if __name__ == "__main__":
    runConfigFname="runConfig_run.json"
    if len(sys.argv)>1 and ".json" in sys.argv[1]:
        runConfigFname=sys.argv[1]

    test=OptScannerResult(runConfigFname=runConfigFname)
    # test.ReadCheckDpResultText()
    test.getLargeRunReport()  # construct the pdf file concurrently
    # test.getReport()
