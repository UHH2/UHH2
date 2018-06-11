import json, sys
from ROOT import TFile, TH2D, TDirectory
from array import array


def getBinBorders(inString):
    binBorders = []

    bSplit = inString.split(':')
    bSplit2 = bSplit[1].split(',')
    binlow = bSplit2[0].replace('[','')
    binhigh = bSplit2[1].replace(']','')
    
    binBorders.append(binlow)
    binBorders.append(binhigh)

    return binBorders



if len(sys.argv) < 2:
    print "usage: <input file name (.json)> <output file name (.root)>"
    sys.exit(1)

inFile = json.load(open(sys.argv[1]))
outFile = TFile(sys.argv[2], "RECREATE")

for dirName in inFile:

    effDir = outFile.mkdir(dirName)
    effDir.cd()

    for histName in inFile[dirName]:

        xAxisName = ''
        yAxisName = ''
        BinsYlow = []
        BinsYhigh = []
        BinsXlow = []
        BinsXhigh = []
        BinsYSet = False

        #read Binning 
        for BinX in sorted(inFile[dirName][histName]):
            xAxisName = (BinX.split(':'))[0]
            binBordersX = getBinBorders(BinX)
            BinsXlow.append(binBordersX[0])
            BinsXhigh.append(binBordersX[1])
          
            if not BinsYSet:
                for BinY in sorted(inFile[dirName][histName][BinX]):
                    yAxisName = (BinY.split(':'))[0]
                    binBordersY = getBinBorders(BinY)
                    BinsYlow.append(binBordersY[0])
                    BinsYhigh.append(binBordersY[1])
                BinsYSet = True

        #create histogram
        BinsX = []
        for bin in BinsXlow:
            BinsX.append(float(bin))
        BinsX.append(float(BinsXhigh[len(BinsXhigh)-1]))
        histBinsX = array('d', BinsX)

        BinsY = []
        for bin in BinsYlow:
            BinsY.append(float(bin))
        BinsY.append(float(BinsYhigh[len(BinsYhigh)-1]))
        histBinsY = array('d', BinsY)

        hist = TH2D(histName, ';'+xAxisName+';'+yAxisName, len(BinsX)-1, histBinsX, len(BinsY)-1, histBinsY)

        #set histogram entries
        for i in range(0,len(BinsXlow)):
            for j in range(0,len(BinsYlow)):
                binKeyX = xAxisName+':['+BinsXlow[i]+','+BinsXhigh[i]+']'
                binKeyY = yAxisName+':['+BinsYlow[j]+','+BinsYhigh[j]+']'
                content =  inFile[dirName][histName][binKeyX][binKeyY]['value']
                error =  inFile[dirName][histName][binKeyX][binKeyY]['error']

                hist.SetBinContent(i+1,j+1,content)
                hist.SetBinError(i+1,j+1,error)
        
        hist.Write(histName)
                     

outFile.Close()
       

