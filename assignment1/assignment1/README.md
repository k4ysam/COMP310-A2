To run test cases use this command: "cd test-cases && python3 run_tests.py"

To run test cases without the script: "cd test-cases && for test in badcommand echo ls mkdir oneline oneline2 prompt run set source; do echo \"Testing $test...\"; ../project/src/mysh < ${test}.txt > /tmp/${test}_output.txt 2>&1; if diff -q /tmp/${test}_output.txt ${test}_result.txt > /dev/null; then echo \"PASS: $test\"; else echo \"FAIL: $test\"; fi; done"
