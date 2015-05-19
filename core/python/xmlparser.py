#!/usr/bin/env python
import StringIO
import xml.sax
from xml.sax.handler import ContentHandler

# Inheriting from EntityResolver and DTDHandler is not necessary
class TestHandler(ContentHandler):
    def __init__(self, inputvalue):
        self.keyvalue=inputvalue
        self.returnlist=[]

    def resolveEntity(self, publicID, systemID):
        #print "TestHandler.resolveEntity(): %s" % (systemID)
        return systemID

    def startElement(self, name, attrs):
        for attrName in attrs.keys():
            #print 'Attribute -- Name: %s  Value: %s\n' %(attrName, attrs.get(attrName))
            if attrName==self.keyvalue: self.returnlist.append(str(attrs.get(attrName)))

    def getreturnlist(self):
        return self.returnlist

def parse(xml_string, inputvalue, debug=0):
    try:
        parser = xml.sax.make_parser()
        curHandler = TestHandler(inputvalue)
        parser.setContentHandler(curHandler)
        parser.setEntityResolver(curHandler)
        parser.setDTDHandler(curHandler)

        stream = StringIO.StringIO(xml_string)
        parser.parse(stream)
        stream.close()
        return curHandler.getreturnlist()
    except xml.sax.SAXParseException, e:
        print "*** PARSER error: %s" % e

#XML = open("ZprimePreSelectionCycleElectrons_config_simple_MC.xml").read()
#parse(XML,"FileName")

