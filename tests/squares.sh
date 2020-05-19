#!/bin/bash

given="$(./bin/badlang ../examples/squares.bad)"
expected="---------------------------
ITERATING WITH JUST INDEXES
---------------------------
0
Strings are in here too!
4
9
16
25
36
49
64
81
---------------------------
NOW ITERATING USING FORKEY
---------------------------
0
Strings are in here too!
4
9
16
25
36
49
64
81
Here's a string with an address key
---------------------------
NOW CALLING PRINT ON DICT
---------------------------
0
Strings are in here too!
4
9
16
25
36
49
64
81
Here's a string with an address key"
[[ "$given" == "$expected" ]]
