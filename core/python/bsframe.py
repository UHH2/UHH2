#!/usr/bin/env python

import datablock
import math
from optparse import OptionParser
import os
import re
import shutil
import sys
import time
import xmlparser

parser = OptionParser()
parser.add_option("-c", "--cfg", dest="configxml", default="", help="Input XML Config File")
parser.add_option("-j", "--jobname", dest="jobname", default="", help="Job Name")
parser.add_option("-k", "--kill", dest="kill",  default="", help="Kill Jobs: all, 1-5, or 1,3,5,9")
parser.add_option("-n", "--numjobs", dest="numjobs", type="int", default=0, help="Number of Jobs")
parser.add_option("-o", "--output", dest="output", default="", help="Output directory: The default is jobname/results")
parser.add_option("-s", "--submit", dest="submit",  default="", help="Submit Jobs: all, 1-5, or 1,3,5,9")
parser.add_option("-l", "--clean", dest="clean",  default="", help="Clean log and result files: all, 1-5, 1,3,5,9")
parser.add_option("-e", "--resubmit", dest="resubmit",  default="", help="Clean log and result files and then submit jobs: all, 1-5, 1,3,5,9")
parser.add_option("-t", "--usetarball", dest="usetarball",  default="", help="Use target tarball. If it doesn't exsist create it.")

parser.add_option("--clobber", action="store_true", dest="clobber", default=False, help="Overwrite Job Directory")
parser.add_option("--create", action="store_true", dest="create", default=False, help="Create job and configuration files.")
parser.add_option("--datablocks", action="store_true", dest="datablocks", default=False, help="Seperate jobs based on data blocks.")
parser.add_option("--status", action="store_true", dest="status", default=False, help="Get job status")
parser.add_option("--retar", action="store_true", dest="retar", default=False, help="Recreate the job tarball.")
parser.add_option("--ttbargencut", action="store_true", dest="ttbargencut", default=False, help="Apply ttbar generator cut")
parser.add_option("--notar", action="store_true", dest="notar", default=False, help="Do not create tarball. For debugging configs.")

parser.add_option("--append", dest="append", default="", help="Append string to job name")
parser.add_option("--flavor", dest="flavor", default="", help="Apply flavor selection: bflavor, cflavor, lflavor")
parser.add_option("--pileupfile", dest="pileupfile", default="", help="Specify pileup file")
parser.add_option("--bjets", dest="bjets", default="", help="Apply bjet Systematic: up-bjets, down-bjets, up-ljets, down-ljets")
parser.add_option("--toptag", dest="tjets", default="", help="Apply toptag scale Systematic: up-mistag, down-mistag, up-toptag, down-toptag")
parser.add_option("--toppt", dest="toppt", default="", help="Apply top pT reweighting Systematic: mean, up, down")
parser.add_option("--JEC", dest="jec", default="", help="Apply JEC Systematic: up or down")
parser.add_option("--JER", dest="jer", default="", help="Apply JER Systematic: up or down")
parser.add_option("--EleSF", dest="elesf", default="", help="Apply EleSF Systematic: up or down")
parser.add_option("--MuSF", dest="musf", default="", help="Apply MuonSF Systematic: up or down")
parser.add_option("--PDF", dest="pdf", default="", help="Apply PDF Systematics: CT10 or cteq66")
parser.add_option("--PDFDir", dest="pdfdir", default="", help="Location of PDF systematic files.")
parser.add_option("--filter", dest="filter", default="", help="Run only samples that pass filter.")
parser.add_option("--veto", dest="veto", default="", help="Remove samples that pass filter.")
(options, args) = parser.parse_args()

if options.jobname == "" and options.configxml == "":
    print "ERROR: Please provide either a configuration file or job directory"
    exit(1)
if options.jobname == "":
    options.jobname = options.configxml.strip(".xml")
    if options.ttbargencut: options.jobname += "_TTBar"
    if options.flavor != "": options.jobname += "_"+options.flavor
    if options.bjets != "": options.jobname += "_"+options.bjets
    if options.jec != "": options.jobname += "_JEC"+options.jec
    if options.jer != "": options.jobname += "_JER"+options.jer
    if options.elesf != "": options.jobname += "_EleSF"+options.elesf
    if options.musf != "": options.jobname += "_MuSF"+options.musf
    if options.pdf != "": options.jobname += "_"+options.pdf
    if options.append != "": options.jobname += "_"+options.append
else:
    if options.jobname.endswith('/'): options.jobname = options.jobname[:-1]
if options.resubmit != "":
    options.clean = options.resubmit
    options.submit = options.resubmit
if not os.path.isfile(options.configxml) and not os.path.isdir(options.jobname):
    if options.configxml!="": print "ERROR: "+options.configxml+" is not a valid file!"
    else: print "ERROR: "+options.jobname+" is not a valid directory!"
    exit(1)
while os.popen("voms-proxy-info").readlines() == []:
    print "Get a CMS proxy certificate"
    os.system("voms-proxy-init -voms cms")
    
def makejoblist(joblist):
    newjoblist=[]
    joblistarray=joblist.split(",")
    for job in joblistarray:
        if job.find("-")!=-1:
            for i in range(int(job.split('-')[0]),int(job.split('-')[1])+1):
                newjoblist.append(i)
        else: newjoblist.append(int(job))
    return newjoblist

def makestringlist(joblist):
    stringlist=""
    for job in joblist: stringlist += str(job)+","
    stringlist=stringlist[:-1]
    return stringlist

def applypostfix(infile, append):
    postfix = re.search('PostFix="[^ ]+"',infile)
    if postfix: infile = infile.replace(postfix.group(0),postfix.group(0)[:-1]+"_"+str(append)+'"')
    elif infile.find('PostFix=""') != -1: infile = infile.replace('PostFix=""','PostFix="_'+str(append)+'"')
    else:
        cyclepos = infile[infile.find("<Cycle "):].find(">")+infile.find("<Cycle ")
        infile = infile[:cyclepos]+'PostFix="_'+str(append)+'" '+infile[cyclepos:]
    return infile

def additem(infile, name, value):
    location = re.search('Name="'+name+'" Value="[^ ]+"',infile)
    if location: infile = infile.replace(location.group(0),'Name="'+name+'" Value="'+str(value)+'"')
    else:
        configpos = infile.find("</UserConfig>")
        infile = infile[:configpos]+'  <Item Name="'+name+'" Value="'+str(value)+'" />\n    '+infile[configpos:]
    return infile

def applyttbargencut(infile):
    infile=applypostfix(infile,"0to700")
    infile=additem(infile,"ApplyMttbarGenCut","True")
    return infile

def applybjetsystematic(infile,bjets):
    infile=applypostfix(infile,bjets)
    infile=additem(infile,"BTaggingScaleFactors",bjets)
    return infile

def applyflavorselection(infile,flavor):
    infile=applypostfix(infile,flavor)
    infile=additem(infile,"ApplyFlavorSelection",flavor)
    return infile

def applyjesystematic(infile,jectype,jecdirection):
    infile=applypostfix(infile,jectype+jecdirection)
    infile=additem(infile,"SystematicUncertainty",jectype)
    infile=additem(infile,"SystematicVariation",jecdirection)
    return infile

def applyelesfsystematic(infile,direction):
    infile=applypostfix(infile,"EleSF"+direction)
    infile=additem(infile,"SystematicUncertainty","EleSF")
    infile=additem(infile,"SystematicVariation",direction)
    return infile

def applymusfsystematic(infile,direction):
    infile=applypostfix(infile,"MuSF"+direction)
    infile=additem(infile,"SystematicUncertainty","MuonSF")
    infile=additem(infile,"SystematicVariation",direction)
    return infile

def applypdfsystematics(infile, options, pdfindex):
    infile=applypostfix(infile, options.pdf+"_"+str(pdfindex))
    infile=additem(infile,"SystematicUncertainty","PDF")
    infile=additem(infile,"SystematicVariation","up")
    infile=additem(infile,"PDFName",options.pdf)
    if options.pdfdir != "": infile=additem(infile,"PDFWeightFilesDirectory",options.pdfdir)
    infile=additem(infile,"PDFIndex",pdfindex)
    return infile

def applytjetsystematic(infile,tjets):
    infile=applypostfix(infile,tjets)
    infile=additem(infile,"TopTaggingScaleFactors",tjets)
    return infile

def applytopptsystematic(infile,toppt):
    infile=applypostfix(infile,"toppt"+toppt)
    infile=additem(infile,"toppagptweight",toppt)
    return infile

def changepileupfile(infile,pileupfile):
    infile = additem(infile,"PU_Filename_Data",pileupfile)
    return infile

def getoutputfilenames(configfile):
    rawxmlfile = open(configfile).read()
    typelist = xmlparser.parse(rawxmlfile,"Type")
    versionlist = xmlparser.parse(rawxmlfile,"Version")
    namelist = xmlparser.parse(rawxmlfile,"Name")
    postfix = xmlparser.parse(rawxmlfile,"PostFix")
    rootfilenamelist = ""
    for name in namelist:
        # if name.find("Cycle") != -1: cyclename=name
        if name.find("uhh2::AnalysisModuleRunner") != -1: cyclename=name.replace("::",".")
    for itype,version in zip(typelist,versionlist): rootfilenamelist += cyclename+"."+itype+"."+version+postfix[0]+".root, "
    return rootfilenamelist[:-2]

def createcondortxt(options, jobnumber, jobdir):
    rootfiles = getoutputfilenames(options.jobname+"/xml/"+options.jobname+"_"+str(jobnumber)+".xml").replace(".root","."+str(jobnumber)+".root").split(",")
    outputfiles = ""
    os.chdir(options.jobname+"/configs")
    condorfile = open(options.jobname+"_"+str(jobnumber)+".txt", 'w')
    if options.output == "" : options.output = jobdir+"/results"
    for rootfile in rootfiles: outputfiles += options.output+"/"+rootfile.strip()+", "
    outputfiles = outputfiles[:-2]
    if options.output.find("/store/") != -1: outputfiles=""
    userproxy = os.popen("echo $X509_USER_PROXY").readline().strip()
    print >> condorfile, """universe = vanilla
Executable = %s/configs/%s_%d.sh
Should_Transfer_Files = YES
WhenToTransferOutput = ON_EXIT
Transfer_Input_Files = %s/configs/%s.tgz
Transfer_Output_Files = %s
Output = %s/logs/%s_%d.stdout
Error = %s/logs/%s_%d.stderr
Log = %s/logs/%s_%d.log
notify_user = ${LOGNAME}@FNAL.GOV
x509userproxy = %s
Arguments = %s
Queue 1""" %(options.jobname, options.jobname, jobnumber, jobdir, options.jobname, outputfiles, jobdir, options.jobname, jobnumber, jobdir, options.jobname, jobnumber, jobdir, options.jobname, jobnumber, userproxy, os.getcwd())
    condorfile.close()
    os.chdir("../..")

def createcondorscript(options, jobnumber):
    os.chdir(options.jobname+"/configs")
    scriptname = options.jobname+"_"+str(jobnumber)+".sh"
    scriptfile = open(scriptname, 'w')
    print >> scriptfile, """#!/bin/bash
echo "Starting job on " `date`
echo "Running on: `uname -a`"
echo "System software: `cat /etc/redhat-release`"
source /cvmfs/cms.cern.ch/cmsset_default.sh
WORKINGDIR=$PWD
TARNAME=`/bin/ls *.tgz`
tar -xzf $TARNAME
SFRAMEDIR=`find . -type f -name setup.sh | xargs dirname`
cd $SFRAMEDIR
scramv1 b ProjectRename
eval `scramv1 runtime -sh`
source setup.sh
cd $WORKINGDIR
ANALYSISDIR=CMSSW`echo ${1##*CMSSW}`/../../
cd $ANALYSISDIR
FILENAME=%s_%d.xml
cp %s/xml/${FILENAME} .
if [ `/bin/ls ${WORKINGDIR} | grep .root | wc -l` -gt 0 ]; then mv ${WORKINGDIR}/*.root .; fi
sframe_main %s_%d.xml
CYCLENAME=`grep "Cycle Name" %s_%d.xml | sed 's|.*<Cycle Name="\([^ \\t\\r\\n\\v\\f]*\)".*|\\1|'` | sed 's|::|.|'
for filename in `/bin/ls ${CYCLENAME}*.root`; do
    newfilename=`echo $filename | sed 's|.root|.%d.root|'`
    mv $filename $newfilename
    echo "MD5Sum $newfilename"
    md5sum $newfilename
done
mv ${CYCLENAME}*.root $WORKINGDIR""" %(options.jobname, jobnumber, options.jobname, options.jobname, jobnumber, options.jobname, jobnumber, jobnumber)
    if options.output.find("/store/") != -1: print >> scriptfile, """
cd $WORKINGDIR
OUTPUTDIR=%s/
EOSDIR=`echo $OUTPUTDIR | sed 's|/eos/uscms/store/|root://cmseos.fnal.gov//store/|'`
for FILE in `/bin/ls ${CYCLENAME}*.root`; do
    xrdcp -f $FILE $EOSDIR
    XRDCPEXITCODE=$?
    echo "xrdcp status: $XRDCPEXITCODE"
#    COUNTER=0
#    while [ "$XRDCPEXITCODE" != 0 -a "$COUNTER" -lt 5]; do
#        xrdcp -f $FILE $EOSDIR
#        XRDCPEXITCODE=$?
#        let COUNTER+=1
#    done
    if [ $XRDCPEXITCODE -eq 0 ]; then rm $FILE; fi
done""" %(options.output)
    os.chmod(scriptname, 493) #493==755 in python chmod
    os.chdir("../..")

def begindatablock(mydatablock):
    return '<InputData Lumi="'+mydatablock.blocklumi+'" NEventsMax="'+mydatablock.neventsmax+'" Type="'+mydatablock.type+'" Version="'+mydatablock.version+'" Cacheable="'+mydatablock.cacheable+'">\n'

def enddatablock(mydatablock,indent):
    returnstring = indent+"  "+'<InputTree Name="AnalysisTree" />\n'
    if len(mydatablock.namelist)>1: returnstring += indent+"  "+'<OutputTree Name="AnalysisTree" />\n'
    returnstring += indent+"</InputData>\n"
    return returnstring

def makedatablocks(xmlfile,options):
    text=xmlfile
    datablocklist=[]
    while text.find("<InputData ") != -1:
        datablock = text[text.find("<InputData "):text.find("</InputData>")+12]
        version = xmlparser.parse(datablock,"Version")[0]
        vetoflag=0
        if options.veto.find(",") != -1: vetolist=options.veto.split(",")
        else: vetolist = [options.veto]
        if options.veto != "":
            for veto in vetolist:
                vetoflag = re.search(veto,version)
                if vetoflag: break
        filterflag=1
        if options.filter.find(",") != -1: filterlist=options.filter.split(",")
        else: filterlist = [options.filter]
        if options.filter != "":
            for ifilter in filterlist:
                filterflag = re.search(ifilter,version)
                if filterflag: break
        if filterflag and not vetoflag: datablocklist.append(datablock)
        text = text[text.find("</InputData>")+12:]
    return datablocklist

def resolveentities(text):
    begincomment = text.find("<!--")
    endcomment = text.find("-->")
    while begincomment!=-1 and endcomment!=-1:
        text = text[:begincomment]+text[endcomment+3:]
        begincomment = text.find("<!--")
        endcomment = text.find("-->")
    textlist = text.split("\n")
    entitydict = {}
    for line in textlist:
        if line.count("ENTITY")>0 and line.count("SYSTEM")>0:
            entityname = line[line.find("ENTITY ")+7:line.find(" SYSTEM")]
            entityfile = line[line.find('"')+1:line.rfind('"')]
            if not os.path.isfile(entityfile): continue
            entityfiles = open(entityfile).read()
            entitydict[entityname] = entityfiles
        if line.count("ENTITY")>0 and line.count("SYSTEM")==0:
            entityname = line.split()[1]
            entityvalue = line.split()[2].strip('">')
            entitydict[entityname] = entityvalue
    for entity in entitydict: text=text.replace("&"+entity+";", entitydict[entity])
    #xmldump=open("BSFrame_config_dump.xml",'w');xmldump.write(text);xmldump.close()
    return text

def createxmlfiles(options):
    pdfmax = 1
    if options.pdf == "CT10": pdfmax = 52
    if options.pdf == "cteq66": pdfmax = 44
    rawxmlfile = open(options.configxml).read()
    xmlfile = resolveentities(rawxmlfile)
    xmldatablocks = makedatablocks(xmlfile,options)
    totalfilelist = []
    datablocklist = []
    for xmldatablock in xmldatablocks:
        filetype = xmlparser.parse(xmldatablock,"Type")[0]
        version = xmlparser.parse(xmldatablock,"Version")[0]
        maxevents = xmlparser.parse(xmldatablock,"NEventsMax")[0]
        cacheable = xmlparser.parse(xmldatablock,"Cacheable")[0]
        filelist = xmlparser.parse(xmldatablock,"FileName")
        totalfilelist.extend(filelist)
        lumi = xmlparser.parse(xmldatablock,"Lumi")
        blocklumi = lumi.pop(0)
        namelist = xmlparser.parse(xmldatablock,"Name")
        mydatablock=datablock.datablock(blocklumi, filelist, lumi, namelist, filetype, version, maxevents, cacheable)
        datablocklist.append(mydatablock)
    if options.numjobs==0 and options.pdf=="": options.numjobs=len(totalfilelist)
    if options.numjobs==0 and options.pdf!="": options.numjobs=1
    if options.datablocks: options.numjobs=len(datablocklist)
    numjoblist=[]
    if options.datablocks:
        for data in datablocklist: numjoblist.append(len(data.filelist))
    else:
        for i in range(options.numjobs): numjoblist.append(0)
        jobindex = 0
        for i in range(len(totalfilelist)):
            if jobindex>=options.numjobs: jobindex-=options.numjobs
            numjoblist[jobindex]+=1
            jobindex+=1
    for pdfindex in range(pdfmax):
        datablocknumber = 0
        blockindex = 0
        for jobnumber in range(options.numjobs):
            blockindex,datablocknumber = createxmlfile(xmlfile, jobnumber+1, datablocklist, datablocknumber, blockindex, numjoblist[jobnumber], options, pdfindex+1)

def createxmlfile(infile, jobnumber, datablocklist, datablocknumber, blockindex, numfiles, options, pdfindex):
    jobnumber = jobnumber+options.numjobs*(pdfindex-1)
    filename = options.jobname+"_"+str(jobnumber)+".xml"
    os.chdir(options.jobname+"/xml")
    if options.ttbargencut: infile = applyttbargencut(infile)
    if options.flavor != "": infile = applyflavorselection(infile, options.flavor)
    if options.jec != "": infile = applyjesystematic(infile, "JEC", options.jec)
    if options.jer != "": infile = applyjesystematic(infile, "JER", options.jer)
    if options.elesf != "": infile = applyelesfsystematic(infile, options.elesf)
    if options.musf != "": infile = applymusfsystematic(infile, options.musf)
    if options.pileupfile != "": infile = changepileupfile(infile, options.pileupfile)
    if options.bjets != "": infile = applybjetsystematic(infile, options.bjets)
    if options.tjets != "": infile = applytjetsystematic(infile, options.tjets)
    if options.toppt != "": infile = applytopptsystematic(infile, options.toppt)
    if options.pdf != "": infile = applypdfsystematics(infile, options, pdfindex)
    frontend = infile[:infile.find("<InputData ")]
    indent = frontend[frontend.rfind("\n")+1:]
    backend = infile[infile.rfind("</InputData>")+13:]
    if len(datablocklist)>0:
        inputfilestring = begindatablock(datablocklist[datablocknumber])
        filelist=datablocklist[datablocknumber].filelist
        lumilist=datablocklist[datablocknumber].lumilist
    else:
        print "Error: No DataBlocks Found!\n"
        exit(1)

    while numfiles>0:
        if blockindex == len(filelist):
            inputfilestring += enddatablock(datablocklist[datablocknumber],indent)
            datablocknumber += 1
            if datablocknumber < len(datablocklist):
                inputfilestring += indent+begindatablock(datablocklist[datablocknumber])
                filelist=datablocklist[datablocknumber].filelist
                lumilist=datablocklist[datablocknumber].lumilist
                blockindex=0
        else:
            inputfilestring += indent+"  "+'<In FileName="'+filelist[blockindex]+'" Lumi="'+lumilist[blockindex]+'"/>\n'
            blockindex += 1
            numfiles -= 1
    inputfilestring += enddatablock(datablocklist[datablocknumber],indent)

    if blockindex == len(filelist):
        datablocknumber += 1
        blockindex = 0

    finalxmlfile = frontend + inputfilestring + backend
    outputdirectory = re.search('OutputDirectory="[^ ]*"',finalxmlfile)
    if not outputdirectory: print "Error: OutputDirectory not found in "+infile
    finalxmlfile = finalxmlfile.replace(outputdirectory.group(0),'OutputDirectory="./"')
    outfile = open(filename,'w')
    outfile.write(finalxmlfile)
    outfile.close()
    os.chdir("../..")
    return blockindex,datablocknumber

def getcondornode(jobid):
    condorstatus=os.popen("condor_q -submitter $USER").readlines()
    for linenumber in range(len(condorstatus)):
        if condorstatus[linenumber].find(jobid) != -1:
            for nodeline in reversed(range(0,linenumber)):
                if condorstatus[nodeline].find("Submitter:") != -1:
                    statuslist = condorstatus[nodeline].split(" ")
                    nodeindex = len(statuslist)-1
                    return statuslist[nodeindex].strip("\n")
    print "Error: Condor node with jobid: "+jobid+" not found!"
    return -1

def checklog(jobname, jobnumber):
    errorinfo = os.popen("egrep -i 'exit|break|exceed|error|traceback|aborted' "+jobname+"/logs/"+jobname+"_"+str(jobnumber)+".log").readline().strip("\n")
    errorline = errorinfo[errorinfo.find(")")+2:]
    return errorline

def checkstdout(jobname, jobnumber, offset):
    errors = os.popen('egrep -i "exit|break|exceed|error|traceback|aborted|E R R O R|find tree AnalysisTree|fatal" '+jobname+"/logs/"+jobname+"_"+str(jobnumber)+".stdout | sort -u").readlines()
    returnerror = ""
    if len(errors)>0:
        for error in errors: returnerror += offset+error
#             if error.find(":") != -1: returnerror += offset+error.split(":")[-1:][0]
#             else: returnerror += effset+error
    return returnerror.strip('\n')

def getjobstatus(statusdict, jobid, jobstatus):
    if jobid not in statusdict and jobstatus != "Killed" and jobstatus != "Cleaned": return "Missing"
    elif jobid not in statusdict: return jobstatus
    jobstatuscode = statusdict[jobid]
    if jobstatuscode=="I": return "Idle"
    if jobstatuscode=="R": return "Running"
    if jobstatuscode=="H": return "Held"
    if jobstatuscode=="X": return "Killed"
    if jobstatuscode=="C": return "Completed"
    if jobstatuscode=="<": return "Transfering Input"
    if jobstatuscode==">": return "Transfering Ouput"
    return "Unknown"

def getoutputdir(jobname, jobnumber):
    filelist = os.popen("grep Transfer_Output_Files "+jobname+"/configs/"+jobname+"_"+str(jobnumber)+".txt").readline().strip().split()
    outputdir = set()
    for ifile in filelist:
        if ifile.find(".root") == -1: continue
        outputdir.add(ifile[:ifile.rfind("/")+1])
    if len(outputdir) == 0: outputdir.add(os.popen("grep OUTPUTDIR "+jobname+"/configs/"+jobname+"_"+str(jobnumber)+".sh").readline().strip().split("=")[1])
    if len(outputdir) > 1: print "Warning! Output directory set length greater than 1!"
    return outputdir.pop()

def getjobinfo(jobname,jobnumber,resubmitjobs,jobstatus,jobid):
    rootfiles = getoutputfilenames(jobname+"/xml/"+jobname+"_"+str(jobnumber)+".xml").replace(".root","."+str(jobnumber)+".root").split(",")
    outputdirectory = getoutputdir(jobname, jobnumber)
    jobinfo=""
    offset="                                     "
    for filename in rootfiles:
        filename=filename.strip(", ")
        if jobinfo != "": jobinfo += "\n"+offset
        filepath = outputdirectory+"/"+filename
        jobstatus = open(options.jobname+"/status/"+options.jobname+"_"+str(jobnumber)+".status").read().strip("\n")
        if os.path.isfile(filepath):
            if os.path.getsize(filepath)==0:
                if jobstatus=="Done" or jobstatus=="Missing":
                    jobinfo += " Root File "+filename+" Empty!"
                    if resubmitjobs.count(jobnumber)<1: resubmitjobs.append(jobnumber)
                    jobstatus="Error"
            else:
                rootfile = ROOT.TFile.Open(filepath)
                try: iszombie=rootfile.IsZombie()
                except: iszombie=True
                if iszombie:
                    jobinfo += " Root File "+filename+" is Zombie!"
                    if resubmitjobs.count(jobnumber)<1: resubmitjobs.append(jobnumber)
                    jobstatus="Error"
                elif rootfile.Get("AnalysisTree"):
                    analysistree = rootfile.Get("AnalysisTree")
                    jobinfo += " Root File "+filename+" is Valid: "+str(int(analysistree.GetEntries()))+" Events."
                    os.system("echo 'Done' >& "+options.jobname+"/status/"+options.jobname+"_"+str(jobnumber)+".status")
                    jobstatus="Done"
                else:
                    hist = rootfile.Get("input/pvN")
                    if not hist:
                        jobstatus="Error"
                        jobinfo += " No output histogram found in root file!"
                    else:
                        jobinfo += " Root File "+filename+" is Valid: "+str(int(hist.GetEntries()))+" Events."
                        os.system("echo 'Done' >& "+options.jobname+"/status/"+options.jobname+"_"+str(jobnumber)+".status")
                        jobstatus="Done"
                if not iszombie: rootfile.Close()
        elif jobstatus=="Done" or jobstatus=="Missing":
            jobinfo += " Output file "+filename+" is not found!"
            if resubmitjobs.count(jobnumber)<1: resubmitjobs.append(jobnumber)
            jobstatus="Error"
    if jobstatus=="Held":
        if resubmitjobs.count(jobnumber)<1: resubmitjobs.append(jobnumber)
    if jobstatus=="Running":
        condorstatus=os.popen("condor_q -submitter $USER").read()
        if condorstatus.find(jobid) != -1:
            subnode = getcondornode(jobid)
            if subnode != -1:
                if subnode==currentnode: jobinfo = os.popen("condor_tail "+jobid+".0 | grep Processing | tail -1").readline().strip("\n")
                else: jobinfo = os.popen("ssh "+subnode+" 'condor_tail "+jobid+".0' | grep Processing | tail -1").readline().strip("\n")
                if jobinfo.find("Processing entry")!=-1:
                    numberator = float(jobinfo.split("(")[2].split(" ")[0])
                    denominator = float(jobinfo.split("(")[2].split(" ")[2])
                    jobinfo = " %.2f" %(numberator/denominator*100)+"% Complete"
                else: jobinfo = " "+jobinfo[jobinfo.find(":")+1:].strip()
    if jobstatus=="Done" or jobstatus=="Error":
        stdouterror = checkstdout(jobname, jobnumber, offset)
        logerror = ""
        #logerror = checklog(jobname, jobnumber)
        if (stdouterror != "" or logerror != "") and resubmitjobs.count(jobnumber)<1:
            resubmitjobs.append(jobnumber)
            os.system("echo 'Error' >& "+options.jobname+"/status/"+options.jobname+"_"+str(jobnumber)+".status")
            jobstatus="Error"
        if stdouterror != "": jobinfo += "\n"+stdouterror
        if logerror != "": jobinfo += "\n"+offset+logerror
    return jobinfo,jobstatus

if not options.create and options.submit=="" and options.kill=="" and options.clean=="" and not options.status and not options.retar:
    print "ERROR: Must either create, retar, submit jobs, kill, clean, or check the status of jobs"

workingdir=os.getcwd()
cmsswbase=os.getenv("CMSSW_BASE")
username=os.getenv("USER")
currentnode=os.getenv("HOST")
if options.usetarball != "": options.usetarball = os.path.abspath(options.usetarball)

if options.create:
    if not os.path.isdir(options.jobname) and not options.clobber: os.mkdir(options.jobname)
    elif os.path.isdir(options.jobname) and not options.clobber:
        print "ERROR: Job directory "+options.jobname+" exists!\nPlease remove job directory or enable --clobber option.."
        exit(1)
    elif options.clobber:
        if os.path.isdir(options.jobname): shutil.rmtree(options.jobname)
        os.mkdir(options.jobname)
    os.mkdir(options.jobname+"/configs")
    os.mkdir(options.jobname+"/logs")
    os.mkdir(options.jobname+"/results")
    os.mkdir(options.jobname+"/status")
    os.mkdir(options.jobname+"/xml")

    if not os.path.isdir(options.output) and options.output!="": os.makedirs(options.output)

    print "Creating configuration files for task: "+options.jobname
    createxmlfiles(options)
    for xmlfile in os.popen("/bin/ls "+options.jobname+"/xml/*.xml").readlines():
        xmlfile = xmlfile.strip("\n")
        jobnumber = int(xmlfile[xmlfile.rfind("_")+1:xmlfile.rfind(".")])
        createcondortxt(options,jobnumber,workingdir+"/"+options.jobname)
        createcondorscript(options,jobnumber)
        os.system("echo 'Created' >& "+options.jobname+"/status/"+options.jobname+"_"+str(jobnumber)+".status")

    if not options.notar:
        os.chdir(cmsswbase+"/..")
        tarball = options.jobname+".tgz"
        target = os.popen("echo ${CMSSW_BASE##*/}").readline().strip("\n")+"/"
        if (options.usetarball != "" and not os.path.isfile(options.usetarball)) or options.usetarball == "":
            print "Creating tarball of "+target+" area."
            os.system("tar -czf "+tarball+" "+target+" --exclude-caches --exclude=.git --exclude='*.tgz'")
            if options.usetarball != "": os.system("mv "+tarball+" "+options.usetarball)
        if options.usetarball == "": os.system("mv "+tarball+" "+workingdir+"/"+options.jobname+"/configs")
        else: os.system("cp "+options.usetarball+" "+workingdir+"/"+options.jobname+"/configs/"+options.jobname+".tgz")
        os.chdir(workingdir+"/"+options.jobname)
        os.system('echo "Signature: 8a477f597d28d172789f06886806bc55" >& CACHEDIR.TAG')
    os.chdir(workingdir)

if not os.path.isdir(options.jobname):
    print "ERROR: Job directory "+options.jobname+" does not exist!\nPlease create job with bsframe.py -c myconfig.xml --create."
    exit(1)

if options.retar:
    if options.create: print "There is no point in creating a task and then recreating the tarball."
    if options.notar: print "You are stupid!"
    os.chdir(workingdir+"/"+options.jobname)
    if os.path.isfile("CACHEDIR.TAG"): os.remove("CACHEDIR.TAG")
    os.chdir(cmsswbase+"/..")
    tarball = options.jobname+".tgz"
    target = os.popen("echo ${CMSSW_BASE##*/}").readline().strip("\n")+"/"
    if (options.usetarball != "" and not os.path.isfile(options.usetarball)) or options.usetarball == "":
        print "Creating tarball of "+target+" area."
        os.system("tar -czf "+tarball+" "+target+" --exclude-caches --exclude='*.tgz'")
        if options.usetarball != "": os.system("mv "+tarball+" "+options.usetarball)
    if options.usetarball == "": os.system("mv "+tarball+" "+workingdir+"/"+options.jobname+"/configs")
    else: os.system("cp "+options.usetarball+" "+workingdir+"/"+options.jobname+"/configs/"+options.jobname+".tgz")
    os.chdir(workingdir+"/"+options.jobname)
    os.system('echo "Signature: 8a477f597d28d172789f06886806bc55" >& CACHEDIR.TAG')
    os.chdir(workingdir)

if options.kill!="":
    joblist=[]
    if options.kill=="all":
        options.numjobs=int(os.popen("/bin/ls "+options.jobname+"/xml/"+options.jobname+"_*.xml | wc -l").readline().strip('\n'))
        joblist=range(1,options.numjobs+1)
    else: joblist=makejoblist(options.kill)
    condorstatus=os.popen("condor_q -submitter $USER").read()
    for jobnumber in joblist:
        print "Killing job number: %d" %(jobnumber)
        logfile = os.popen("/bin/ls -rt "+options.jobname+"/logs/"+options.jobname+"_"+str(jobnumber)+".log | tail -1").readline().strip('\n')
        jobid = os.popen("grep submitted "+logfile+" | tail -1 | awk '{print $2}'").readline().strip("\n()").split(".")[0]
        if condorstatus.find(jobid) == -1:
            print "Error! Job "+jobid+" not found."
            continue
        subnode = getcondornode(jobid)
        if subnode == -1: print "Error! Job condor submission node not found."
        else:
            if subnode==currentnode: os.system("condor_rm "+jobid)
            else: os.system("ssh "+subnode+" 'condor_rm "+jobid+"'")
            time.sleep(0.3)
            os.system("echo 'Killed' >& "+options.jobname+"/status/"+options.jobname+"_"+str(jobnumber)+".status")

if options.clean!="":
    joblist=[]
    if options.clean=="all":
        options.numjobs=int(os.popen("/bin/ls "+options.jobname+"/xml/"+options.jobname+"_*.xml | wc -l").readline().strip('\n'))
        joblist=range(1,options.numjobs+1)
    else: joblist=makejoblist(options.clean)
    print "Cleaning %d jobs" %(len(joblist))
    for jobnumber in joblist:
        if os.path.isfile(options.jobname+"/log/"+options.jobname+"_"+str(jobnumber)+".log"): os.system("/bin/rm "+options.jobname+"/log/"+options.jobname+"_"+str(jobnumber)+".log")
        if os.path.isfile(options.jobname+"/log/"+options.jobname+"_"+str(jobnumber)+".stderr"): os.system("/bin/rm "+options.jobname+"/log/"+options.jobname+"_"+str(jobnumber)+".stderr")
        if os.path.isfile(options.jobname+"/log/"+options.jobname+"_"+str(jobnumber)+".stdout"): os.system("/bin/rm "+options.jobname+"/log/"+options.jobname+"_"+str(jobnumber)+".stdout")
        os.system("echo 'Cleaned' >& "+options.jobname+"/status/"+options.jobname+"_"+str(jobnumber)+".status")
        resultsdir = getoutputdir(options.jobname, jobnumber)
        rootfiles = getoutputfilenames(options.jobname+"/xml/"+options.jobname+"_"+str(jobnumber)+".xml").split(",")
        for rootfile in rootfiles:
            rootfile = resultsdir+"/"+rootfile.replace(".root","."+str(jobnumber)+".root")
            if os.path.isfile(rootfile): os.system("/bin/rm "+rootfile)

if options.submit!="":
    joblist=[]
    if options.submit=="all":
        options.numjobs=int(os.popen("/bin/ls "+options.jobname+"/xml/"+options.jobname+"_*.xml | wc -l").readline().strip('\n'))
        joblist=range(1,options.numjobs+1)
    else: joblist=makejoblist(options.submit)
    print "Submitting %d jobs" %(len(joblist))
    for jobnumber in joblist:
        print "Submitting job number: %d" %(jobnumber)
        if not os.path.isfile(options.jobname+"/xml/"+options.jobname+"_"+str(jobnumber)+".xml"):
            print "Error: No configuration file for jobs number "+str(jobnumber)+"!"
            exit(1)
        if os.path.isfile(options.jobname+"/logs/"+options.jobname+"_"+str(jobnumber)+".log"): os.system("/bin/rm "+options.jobname+"/logs/"+options.jobname+"_"+str(jobnumber)+".log")
        os.system("condor_submit "+options.jobname+"/configs/"+options.jobname+"_"+str(jobnumber)+".txt")
        os.system("echo 'Submitted' >& "+options.jobname+"/status/"+options.jobname+"_"+str(jobnumber)+".status")

jobstatuslist=[]
resubmitjobs=[]
if (options.status):
    print "Loading Root"
    import ROOT
    condorstatus=os.popen('condor_q -submitter $USER | grep $USER | grep "^[0-9]"').readlines()
    print "\nJob Status Summary for Task: ",options.jobname
    print "================================================================================"
    print "Job Number         Status             Additional Information"
    print "--------------------------------------------------------------------------------"
    whitespace="                                                                                "
    options.numjobs=int(os.popen("/bin/ls "+options.jobname+"/xml/"+options.jobname+"_*.xml | wc -l").readline().strip("\n"))
    statusdict={}
    for line in condorstatus:
        line = line.split()
        statusdict[line[0][:-2]]=line[5]
    for jobnumber in range(1,options.numjobs+1):
        jobinfo=""
        jobstatus=open(options.jobname+"/status/"+options.jobname+"_"+str(jobnumber)+".status").read().strip("\n")
        if jobstatus != "Created":
            jobid = ""
            if jobstatus!="Done":
                logfile = os.popen("/bin/ls -rt "+options.jobname+"/logs/"+options.jobname+"_"+str(jobnumber)+".log | tail -1").readline().strip('\n')
                jobid = os.popen("grep submitted "+logfile+" | tail -1 | awk '{print $2}'").readline().strip("\n()").split(".")[0]
                jobstatus = getjobstatus(statusdict, jobid, jobstatus)
                os.system("echo '"+jobstatus+"' >& "+options.jobname+"/status/"+options.jobname+"_"+str(jobnumber)+".status")
            jobinfo,jobstatus=getjobinfo(options.jobname,jobnumber,resubmitjobs,jobstatus,jobid)
            jobstatuslist.append(jobstatus)
            os.system("echo "+jobstatus+" >& "+options.jobname+"/status/"+options.jobname+"_"+str(jobnumber)+".status")
        print whitespace[:4]+str(jobnumber)+whitespace[:15-len(str(jobnumber))]+jobstatus+whitespace[:18-len(jobstatus)]+jobinfo
    print ""
    if jobstatuslist.count("Created")>0: print "There are "+str(jobstatuslist.count("Created"))+" Created Jobs"
    if jobstatuslist.count("Submitted")>0: print "There are "+str(jobstatuslist.count("Submitted"))+" Submitted Jobs"
    if jobstatuslist.count("Idle")>0: print "There are "+str(jobstatuslist.count("Idle"))+" Idle Jobs"
    if jobstatuslist.count("Running")>0: print "There are "+str(jobstatuslist.count("Running"))+" Running Jobs"
    if jobstatuslist.count("Completed")>0: print "There are "+str(jobstatuslist.count("Completed"))+" Completed Jobs"
    if jobstatuslist.count("Transfering Input")>0: print "There are "+str(jobstatuslist.count("Transfering Input"))+" Transfering Input Jobs"
    if jobstatuslist.count("Transfering Ouptut")>0: print "There are "+str(jobstatuslist.count("Transfering Output"))+" Transfering Output Jobs"
    if jobstatuslist.count("Held")>0: print "There are "+str(jobstatuslist.count("Held"))+" Held Jobs"
    if jobstatuslist.count("Killed")>0: print "There are "+str(jobstatuslist.count("Killed"))+" Killed Jobs"
    if jobstatuslist.count("Missing")>0: print "There are "+str(jobstatuslist.count("Error"))+" Missing Jobs"
    if jobstatuslist.count("Error")>0: print "There are "+str(jobstatuslist.count("Error"))+" Jobs in Error"
    if jobstatuslist.count("Done")>0: print "There are "+str(jobstatuslist.count("Done"))+" Done Jobs"
    if len(resubmitjobs)>0:
        print "\nThere are "+str(len(resubmitjobs))+" jobs with problems!!!!"
        print "Resubmit jobs: "+makestringlist(resubmitjobs)

print "\nBSFrame has completed execution!"
