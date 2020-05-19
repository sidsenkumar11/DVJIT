#!/bin/bash

given="$(./bin/badlang ../examples/negatives.bad)"
expected="-456"
[[ "$given" == "$expected" ]]
