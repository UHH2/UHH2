#!/bin/bash

[ -f test/testall.sh ] || { echo "Execute from UHH2/core!"; exit 1; }

source test/testlib.sh


exec_checked bin/create_testtree
exec_checked sframe_main test/Test-a.xml

# metadata tests:

exec_checked sframe_main test/Test-MetaDataA.xml
exec_checked sframe_main test/Test-MetaDataB.xml
exec_checked sframe_main test/Test-MetaDataC.xml

# the metadata value read in test C should round-trip to the output of test C, so
# re-running test C on its output should succeed:
exec_checked sframe_main test/Test-MetaDataC-RoundTrip.xml

exec_checked bin/test

exec_checked bin/test-root-utils

#cleanup:
rm uhh2.AnalysisModuleRunner.MC.Test*.root testtree*.root test-root-utils*.root
rm -rf jobTempOutput_*
rm test.log

echo "All Tests ran successfully"
