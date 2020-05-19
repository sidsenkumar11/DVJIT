#!/bin/bash

given="$(./bin/badlang ../examples/invalid_regno.bad 2>&1)"
expected="Register 128 out of range!
Must be in range [0, 128)"
[[ "$given" == "$expected" ]]
