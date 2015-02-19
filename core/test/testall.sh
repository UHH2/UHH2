#!/bin/bash

[ -f test/testall.sh ] || { echo "Execute from UHH2/core!"; exit 1; }

source test/testlib.sh


exec_checked bin/create_testtree
exec_checked sframe_main test/Test-a.xml

# metadata tests:
exec_checked sframe_main test/Test-MetadataOut.xml
exec_checked sframe_main test/Test-MetadataIn.xml
exec_checked sframe_main test/Test-MetadataIn2.xml

exec_checked bin/test

exec_checked bin/test-root-utils

#cleanup:
rm uhh2.AnalysisModuleRunner.MC.Test*.root testtree*.root test-root-utils*.root
rm -rf jobTempOutput_*
rm test.log

echo "All Tests ran successfully"
