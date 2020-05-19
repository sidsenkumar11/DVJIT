#!/bin/bash

given="$(./bin/badlang ../examples/leak_jump.bad)"
expected="VISIT
VISIT
VISIT
VISIT
VISIT
VISIT
VISIT
VISIT
VISIT
DONE!"
[[ "$given" == "$expected" ]]
