#!/usr/bin/env python

import json
import sys

from Utilities.General.cmssw_das_client import get_data

def autocomplete_Datasets(data):
    result_array =[]

    for element in data:
        if '*' in element:
            jsondict = get_data(host='https://cmsweb.cern.ch',query="dataset="+element,idx=0,limit=0,threshold=300)
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
        print "No samples found"
        return []
    # Do this to remove duplicates but maintain order of insertion
    # We get duplicates because it queries ALL databases not just the main one
    # https://github.com/dmwm/DAS/issues/4287#issuecomment-390278822
    return sorted(set(result_array), key=result_array.index)

