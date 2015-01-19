#!/bin/bash

[ -f test/testall.sh ] || { echo "Execute from UHH2/common"; exit 1; }

source ../core/test/testlib.sh

exec_checked sframe_main test/JetCorrector.xml
exec_checked sframe_main test/JetSmearer.xml
expect_failure sframe_main test/JetSmearer-again.xml

#cleanup:
rm uhh2.AnalysisModuleRunner.MC.Test*.root
rm -rf jobTempOutput_*
rm test.log

echo "All tests ran successfully"
