#!/bin/bash
# exit 0;
# cd ../exploits/getshell
# python3 gen_exploit.py
# cd ../../badlang
given=`echo "echo 'hello from inside';exit" | ./bin/badlang ../exploits/getshell/exploit.bad`
expected="hello from inside"
[[ "$given" == "$expected" ]]
