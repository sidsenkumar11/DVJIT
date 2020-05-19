#!/bin/bash

given="$(./bin/badlang ../examples/deallocate_map.bad 2>&1)"
expected="Runtime detected attempt to overwrite register containing a dictionary while executing!"
[[ "$given" == "$expected" ]]
