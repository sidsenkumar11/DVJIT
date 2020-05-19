#!/bin/bash

given="$(./bin/badlang ../examples/nested_forkey.bad)"
expected="------------------------------------------
OUTER
0
------------------------------------------
MIDDLE
0
------------------------------------------
INNER
0
------------------------------------------
INNER
1
------------------------------------------
INNER
2
------------------------------------------
MIDDLE
1
------------------------------------------
INNER
0
------------------------------------------
INNER
1
------------------------------------------
INNER
2
------------------------------------------
MIDDLE
2
------------------------------------------
INNER
0
------------------------------------------
INNER
1
------------------------------------------
INNER
2
------------------------------------------
OUTER
1
------------------------------------------
MIDDLE
0
------------------------------------------
INNER
0
------------------------------------------
INNER
1
------------------------------------------
INNER
2
------------------------------------------
MIDDLE
1
------------------------------------------
INNER
0
------------------------------------------
INNER
1
------------------------------------------
INNER
2
------------------------------------------
MIDDLE
2
------------------------------------------
INNER
0
------------------------------------------
INNER
1
------------------------------------------
INNER
2
------------------------------------------
OUTER
2
------------------------------------------
MIDDLE
0
------------------------------------------
INNER
0
------------------------------------------
INNER
1
------------------------------------------
INNER
2
------------------------------------------
MIDDLE
1
------------------------------------------
INNER
0
------------------------------------------
INNER
1
------------------------------------------
INNER
2
------------------------------------------
MIDDLE
2
------------------------------------------
INNER
0
------------------------------------------
INNER
1
------------------------------------------
INNER
2"
[[ "$given" == "$expected" ]]
