'''
Used for 
'''
import os
import json
from collections import OrderedDict
from progress.bar import Bar
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
        self.LoadConfig()
        self.GetSubFolders()

    def LoadConfig(self, runConfigFname=""):
        if not runConfigFname:
            runConfigFname=self.runConfigJsonFname

        with open(runConfigFname) as runCondigFile:
            self.runConfig_data=json.load(runCondigFile,object_pairs_hook=OrderedDict) # keep the initial order of the database
            self.OptConfigFname=self.runConfig_data["optConfigFname"]
            self.TargetPath=self.runConfig_data["TargetPath"]
            self.OptSourceFolder=self.runConfig_data["OptSourceFolder"]
            self.optScannerBashScript=self.runConfig_data["optScannerBashScript"]

    def SetTargetFolder(self, topFolder=""):
        self.TargetPath=topFolder

    def GetSubFolders(self, topFolder=""):
        #self.TargetPath="/home/newdriver/Research/Eclipse_Workspace/photonSep2019/PRexOpt/OptScan/result1/"
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
        txtFilename="{}/CheckDp_test_result.txt".format(txtResultPath)
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
    
    def addimage(self, pdf=FPDF(),file1=""):
        width, height =self._getImageSize(file1)
        width, height = float(width * 0.264583), float(height * 0.264583)
        pdf_size = {'P': {'w': 210, 'h': 297}, 'L': {'w': 297, 'h': 210}}
        orientation = 'P' if width < height else 'L'
        width = width if width < pdf_size[orientation]['w'] else pdf_size[orientation]['w']
        height = height if height < pdf_size[orientation]['h'] else pdf_size[orientation]['h']
        pdf.add_page(orientation=orientation)
        pdf.image(file1,0, 0, width, height)
        # pdf.set_font('Arial', 'B', 10)
        #pdf.cell(0, 0, "{}".format(item),link="file:///{}".format(item))
    
    def addTxtFile(self, pdf=FPDF(),txtFile=""):
        if os.path.isfile(txtFile):
            pdf.add_page(orientation='P')
            pdf.set_font("Arial", size = 10) 
            f = open(txtFile, "r")
            for x in f:
                if x.startswith('D'):
                    pdf.cell(200,10,txt=x,ln=1,align='L')
    
    def addLink(self,pdf=FPDF(), text='', link=''):
        pass

    def getReport(self,pdffilename='',txtResultPath=[]):
        if len(txtResultPath) == 0:
            txtResultPath=self.OptTemplatedOptmizedFolders
            
        bar=Bar("Processing",max=len(txtResultPath))
        print("Total Processed Template {} / {}\n\n\n".format(len(self.OptTemplatedOptmizedFolders),0))#,float(self.OptTemplatedOptmizedFolders)/self.OptTemplateSubFolders))
        pdf=FPDF(orientation = 'L', unit = 'mm', format='A4')
        id=0
        for item in txtResultPath:
            self.CompileLatex(texpath=item)
            #print("Working on file:{}".format(txtResultPath))
            pdf.add_page()
            pdf.set_font('Arial', 'B', 28)
            pdf.cell(300, 50, "Run {}".format(id),ln=1,align='C')
            id = id+1
            pdf.cell(300, 60,"Click me to Open Folder",link="file:///{}".format(item),ln=1,align='C')
            
            # file1=os.path.join(item,"CheckDp_test2_DpKinDiffCanv.jpg")
            # self.addTxtFile(pdf=pdf,txtFile=os.path.join(item,"templateDB.db.optimied"))
            # if os.path.isfile(file1):
            #     width, height =self._getImageSize(file1)
            #     width, height = float(width * 0.264583), float(height * 0.264583)
            #     pdf_size = {'P': {'w': 210, 'h': 297}, 'L': {'w': 297, 'h': 210}}
            #     orientation = 'P' if width < height else 'L'
            #     width = width if width < pdf_size[orientation]['w'] else pdf_size[orientation]['w']
            #     height = height if height < pdf_size[orientation]['h'] else pdf_size[orientation]['h']
            #     pdf.add_page(orientation=orientation)
            #     pdf.image(file1,0, 0, width, height)
            # pdf.set_font('Arial', 'B', 10)
            # pdf.cell(0, 0, "{}".format(item),link="file:///{}".format(item))
            # self.addimage(pdf=pdf,file1=os.path.join(item,"CheckDp_test_RealMomemtumDifferenceCanv.png"))
            # self.addimage(pdf=pdf,file1=os.path.join(item,"CheckDp_test_DpAllCanv.jpg"))
            #self.addimage(pdf=pdf,file1=os.path.join(item,"CheckDp_test2_DpAllCanv.jpg"))
            #self.addimage(pdf=pdf,file1=os.path.join(item,"CheckDp_test2_MomemtumOptCanv.jpg"))
            #self.addimage(pdf=pdf,file1=os.path.join(item,"CheckDp_test_DpAllCanv.jpg"))
            self.addimage(pdf=pdf,file1=os.path.join(item,"CheckSieve_CThetaCorrectionError.jpg"))
            bar.next()
        bar.finish()
        print("Creating the PDF file")
        pdf.output("./resultScanReport.pdf","F")
        shutil.copyfile("./resultScanReport.pdf",os.path.join(self.TargetPath,"report.pdf"))
        
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
    test=OptScannerResult(runConfigFname="runConfig_theta.json")
    #test.ReadCheckDpMultiThread()
    test.ReadCheckDpResultText()
    test.getReport()
