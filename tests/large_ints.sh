#!/bin/bash

given="$(./bin/badlang ../examples/large_ints.bad)"
expected="1311768467294899695"
[[ "$given" == "$expected" ]]
