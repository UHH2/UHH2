#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
*
* Usage :
* ./multicrab.py ConfigFile [options] 
*
*
"""
from fnmatch import fnmatchcase
import argparse
import os, glob, sys

from CrabScript import *
from create_dataset_xmlfile import create_dataset_xml
from readaMCatNloEntries import readEntries


if __name__ == '__main__':        
        parser = argparse.ArgumentParser(description='Scripts for a simple multicrab')
        parser.add_argument('ConfigFile', action='store', 
                            help='Config File which should be processed')
        parser.add_argument('--xml', dest='xml_create',action='store_true',
                            default=False,
                            help="create xml Files for sframe")
        parser.add_argument('--submit','-s', dest='submit_flag', action='store_true',
                            default=False,
                            help='Submit to the grid')
        parser.add_argument('--status', dest='status_flag', action='store_true',
                            default=False,
                            help='check status of all jobs. Have a lok at the dashbord')
        parser.add_argument('--command','-c', dest='crab_command', action='store',
                            default='',
                            help='execute any crab command you want')
        parser.add_argument('--options','-o',dest='crab_options', action='append',
                            default=[],
                            help='Hand any option to your crab command')
        parser.add_argument('--readEntries', dest='readEntries', action='store',
                            default=0,
                            help='read the all entries contained in all xml files. Specify how many cores you want to use.')
        parser.add_argument('--readEntriesFast', dest='readEntriesFast', action='store',
                            default=0,
                            help='read the all entries contained in all xml files. Specify how many cores you want to use. This version can be used it no negativ weights are needed')
        parser.add_argument('--postfix','-p', dest='postfix', action='store',
                           default='',
                           help="Posibility to add a postfix to the request names. Don't forget to add it too if using --xml/--readEntries")
        parser.add_argument('--filterByDatasets','-D', dest='filter_dataset', action='append',
                            default=[],
                            help='Only act on a certain file or a group of files. Filter by dataset, wildcard symbol */?/[] can be used.')
        parser.add_argument('--filterByRequest','-R', dest='filter_request', action='append',
                            default=[],
                            help='Only act on a certain file or a group of files. Filter by request-name, wildcard symbol */?/[] can be used.')

        args = parser.parse_args()

        for i in range(len(args.crab_options)):
                if '--' not in args.crab_options:
                        args.crab_options[i] = '--'+args.crab_options[i]
                
        if args.ConfigFile.endswith('.py'):
                args.ConfigFile = args.ConfigFile.replace('.py','')

	#sys.path.append(os.path.abspath("/nfs/dust/cms/user/gonvaq/CMSSW/CMSSW_7_4_15_patch1/src/UHH2/VLQToTopAndLepton/Utils/crab"))
        
        module_name = args.ConfigFile
        __import__(module_name)
        ConfigFile = sys.modules[module_name]

	if not os.path.exists(ConfigFile.config.General.workArea):
		os.makedirs(ConfigFile.config.General.workArea)



        if len(ConfigFile.requestNames) != len(ConfigFile.inputDatasets):
                print 'Number of Request-Names',len(ConfigFile.requestNames),' unequal to number of Input-Datasets',len(ConfigFile.inputDatasets)
                print 'prefere to exit'
                exit(100)
        
        if len(args.filter_dataset) > 0 or len(args.filter_request) > 0:
                setList = [] 
                filterlist =[]
                if len(args.filter_dataset) > 0 and len(args.filter_request) > 0:
                        print 'Only one filter list at a time supported. Exit'
                        exit(110)
                if len(args.filter_dataset) > 0: setList = ConfigFile.inputDatasets; filterlist = args.filter_dataset
                if len(args.filter_request) > 0: setList = ConfigFile.requestNames; filterlist = args.filter_request
                for i, dataset in reversed(list(enumerate(setList))):
                        found = False
                        for myfilter in filterlist:
                                #print dataset,myfilter,fnmatchcase(dataset,myfilter)
                                if fnmatchcase(dataset,myfilter):
                                        found = True
                        if not found:
                                #print 'deleting',i,ConfigFile.requestNames[i],ConfigFile.inputDatasets[i]
                                del ConfigFile.requestNames[i]
                                del ConfigFile.inputDatasets[i]
        


        print 'Going to print the Request-Name / Input-Dataset pairs' 
	print 'Number of Samples',len(ConfigFile.requestNames)
        for i in range(len(ConfigFile.requestNames)):
                print ConfigFile.requestNames[i],ConfigFile.inputDatasets[i]

        if args.submit_flag:
                work = CrabConfig(ConfigFile.config,'submit',args.crab_options)
                work.ByDatasets(ConfigFile.inputDatasets,ConfigFile.requestNames,args.postfix)
        if args.status_flag:
                work = CrabConfig(ConfigFile.config,'status',args.crab_options)
                work.ByDatasets(ConfigFile.inputDatasets,ConfigFile.requestNames,args.postfix)
        if args.crab_command:
                work = CrabConfig(ConfigFile.config,args.crab_command,args.crab_options)
                work.ByDatasets(ConfigFile.inputDatasets,ConfigFile.requestNames,args.postfix)

        if args.xml_create:
                for i,name in enumerate(ConfigFile.requestNames):
                        help_name = ConfigFile.inputDatasets[i].split('/')[1]
                        #print help_name
                        dirname = '/pnfs/desy.de/cms/tier2/'+ConfigFile.config.Data.outLFNDirBase+'/'+help_name+'/crab_'+name+args.postfix+'/**/**/*.root'
                        xmlname = name+'.xml'
                        #print dirname
                        print 'For',xmlname 
                        l = glob.glob(dirname)
                        #print xmlname, l
                        create_dataset_xml(dirname,xmlname)

        if args.readEntries > 0 or args.readEntriesFast > 0:
                if args.readEntries > 0 and args.readEntriesFast > 0:
                        print 'something went wrong use readEntries OR readEntriesFast!'
                        exit(120)
                fast = False
                cores = args.readEntries
                #method='"weights"'
                if args.readEntriesFast > 0:
                        fast=True
                        #method='"fast"'
                        cores = args.readEntriesFast
                fileList =[]
                for name in ConfigFile.requestNames:
                        xmlname = name +'.xml'
                        fileList.append(xmlname)
                result_list = readEntries(cores,fileList,fast)
                
                entriesFile = open("entriesFile.txt",'w+')
                if fast: 
                        entriesFile.write('The fast Method used for the number of Entries, no weights used\n')
                else:
                        entriesFile.write('Weights have been used\n')
                for i, name in enumerate(fileList):
                        entriesFile.write(name+' '+str(result_list[i])+'\n')
                        xmlFile = open(name,'a')
                        #xmlFile.write('<!-- < NumberEntries="'+str(result_list[i])+'" Method='+method+' /> -->')
                entriesFile.close()
                
