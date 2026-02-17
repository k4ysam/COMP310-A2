#!/usr/bin/env python3
import subprocess
import os
from pathlib import Path

def run_test(test_name, shell_path):
    test_file = f"{test_name}.txt"
    result_file = f"{test_name}_result.txt"
    
    # test input
    with open(test_file, 'r') as f:
        result = subprocess.run(
            [shell_path],
            stdin=f,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True
        )
    
    # expected output
    with open(result_file, 'r') as f:
        expected = f.read()
    
    # Compare
    if result.stdout == expected:
        return True
    else:
        print(f"\n  Expected output:")
        print("  " + "\n  ".join(expected.splitlines()))
        print(f"\n  Actual output:")
        print("  " + "\n  ".join(result.stdout.splitlines()))
        return False

def main():
    shell_path = "../project/src/mysh"
    tests = [
        "badcommand",
        "echo",
        "ls",
        "mkdir",
        "oneline",
        "oneline2",
        "prompt",
        "run",
        "set",
        "source"
    ]
    
    print("Running tests: \n")
 
    passed = 0
    failed = 0
    
    for test in tests:
        if run_test(test, shell_path):
            print(f"PASS: {test}")
            passed += 1
        else:
            print(f"FAIL: {test}")
            failed += 1
    
    print(f"Results: {passed} passed, {failed} failed out of {len(tests)} tests")

    
    return 0 if failed == 0 else 1

if __name__ == "__main__":
    exit(main())
