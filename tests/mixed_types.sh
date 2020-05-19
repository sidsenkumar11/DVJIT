#!/bin/bash

given="$(./bin/badlang ../examples/mixed_types.bad)"
expected="4
hello world
hello world
type mismatch on reg 2!"
[[ "$given" == "$expected" ]]
