#!/bin/bash

given="$(./bin/badlang ../examples/leak_string.bad)"
expected="Leaked Pointer"
[[ "$given" == *"$expected"* ]]
