function exec_checked () {
   echo $*
   $* > test.log 2>&1 || { echo Error executing $*; echo See test.log; exit 1; }
}

function expect_failure () {
   echo $*
   $* > test.log 2>&1 && { echo Expected that execution failed but did not of: $*; echo See test.log; exit 1; }
}
