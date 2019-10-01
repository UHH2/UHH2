#!python
import pickle
import json
pickle_off = open("/nfs/dust/cms/user/karavdia/lwtnn_TEST/CMSSW_10_2_11/src/UHH2/LwtnnExample/KerasDNNModel/variable_names.pkl","rb")
emp = pickle.load(pickle_off)
print(emp)

data = {}
data['inputs'] = []
for i in range(len(emp)):
    data['inputs'].append({'name': emp[i], 'offset': 0, 'scale': 1})


#FixMe: read output nodes infomation from some input file prepared during training
#classes = {0: ['QCD_Mu'], 1: ['TTbar'], 2:['WJets'], 3:['DYJets+ST']}

data['class_labels'] = ['QCD_Mu','TTbar','WJets','DYJets+ST']




with open('variables.json', 'w') as outfile:
    json.dump(data, outfile)
