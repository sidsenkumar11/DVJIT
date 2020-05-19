#!/bin/bash

given="$(./bin/badlang ../examples/print_bytes.bad)"
expected=`echo -e "\x01\""`
[[ "$given" == "$expected" ]]
