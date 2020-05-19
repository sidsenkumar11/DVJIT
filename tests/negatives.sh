#!/bin/bash

exit 0; # TODO: remove me when negatives are implemented

given="$(./bin/badlang ../examples/negatives.bad)"
expected="-456"
[[ "$given" == "$expected" ]]
