#!/bin/bash

given="$(./bin/badlang ../examples/hello_world.bad)"
expected="hello, world"
[[ "$given" == "$expected" ]]
