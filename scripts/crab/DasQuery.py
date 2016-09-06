#!/usr/bin/env python

import json
import os

# This is the api for the das such that one can find the datasets
from das_client import get_data

def autocomplete_Datasets(data):
    result_array =[]
    ckey = os.path.join(os.environ['HOME'], '.globus/userkey.pem')
    cert = os.path.join(os.environ['HOME'], '.globus/usercert.pem')
    os.environ['SSL_CERT_DIR'] = '/etc/grid-security/certificates/'

    for element in data:
        if '*' in element:
            jsondict = get_data('https://cmsweb.cern.ch',"dataset="+element,0,0,0,300,ckey,cert)
            #print json.dumps(jsondict, indent=4, sort_keys=True)
            #print json.dumps(jsondict['data'], indent=4, sort_keys=True)
            for i in range(len(jsondict['data'])):
                result_array.append(jsondict['data'][i]['dataset'][0]['name'])
        else:
            result_array.append(element)
    return result_array
