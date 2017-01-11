#!/usr/bin/env python

import json
import os, sys
import pwd

# This is the api for the das such that one can find the datasets
from das_client import get_data
from das_client import x509

def autocomplete_Datasets(data):
    result_array =[]
    ckey = x509() #os.path.join(os.environ['HOME'], '.globus/userkey.pem')
    cert = x509() #os.path.join(os.environ['HOME'], '.globus/usercert.pem')
    cpath = '/etc/grid-security/certificates/' #os.environ['SSL_CERT_DIR'] = '/etc/grid-security/certificates/'

    for element in data:
        if '*' in element:
	  jsondict = get_data('https://cmsweb.cern.ch',"dataset="+element,0,0,0,300,ckey,cert,cpath)
          #print json.dumps(jsondict, indent=4, sort_keys=True)
          #print json.dumps(jsondict['data'], indent=4, sort_keys=True)
          try:
	    for i in range(len(jsondict['data'])):
                result_array.append(jsondict['data'][i]['dataset'][0]['name'])
	  except:
	    print '='*10
	    print 'Not found',element
	    print '='*10
        else:
            result_array.append(element)
    if len(result_array) == 0: 
	print "No samples found going to exit"
	sys.exit(0)
    return result_array
