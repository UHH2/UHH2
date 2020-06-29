from xml.etree import ElementTree

# Use an instance of this to parse XML whilst keeping comments
# Usage:
#   parser = ET.XMLParser(target=CommentedTreeBuilder())
#   root = ET.fromstring(xml_str, parser)
#   or
#   root = ET.parse(filename, parser) 
#   
# Comment Elements have str(.tag) = "<function Comment at 0x....>", and .text is the comment text

# Taken from SO: https://stackoverflow.com/a/34324359

class CommentedTreeBuilder(ElementTree.TreeBuilder):
    def comment(self, data):
        self.start(ElementTree.Comment, {})
        self.data(data)
        self.end(ElementTree.Comment)
