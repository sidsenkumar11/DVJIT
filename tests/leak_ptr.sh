#!/bin/bash

given="$(./bin/badlang ../examples/leak_ptr.bad)"
expected="Leaked Pointer"
[[ "$given" == *"$expected"* ]]
