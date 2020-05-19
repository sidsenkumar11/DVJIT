.DEFAULT_GOAL := badlang

SHELL := /bin/bash
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

# --------------------------------------------
# Asmjit
# --------------------------------------------
CFLAGS_A=-DASMJIT_STATIC
CPPS_A=$(shell find asmjit -type f -name '*.cpp')
OBJS_A=$(patsubst %.cpp, build/%.o, $(CPPS_A))
$(OBJS_A): | build/asmjit # create build dir first

build/asmjit:
	mkdir -p build/asmjit

build/asmjit/%.o: asmjit/%.cpp
	mkdir -p `dirname $@`
	g++ -std=c++11 ${CFLAGS_A} -c -o $@ $^

.PHONY: asmjit
asmjit: $(OBJS_A)

# --------------------------------------------
# Parser
# --------------------------------------------
INCLUDES_P=-I$(shell pwd)/build/parser
CFLAGS_P=-g
OBJS_P = $(addprefix build/parser/,ast.o parser.o lexer.o)
$(OBJS_P): | build/parser # create build dir first

build/parser:
	mkdir -p build/parser

build/parser/ast.o: parser/lang.def
	python3 parser/genast.py -i $< -o ast
	mv ast.cpp ast.hpp build/parser
	g++ -std=c++11 $(CFLAGS_P) -c -o $@ build/parser/ast.cpp

build/parser/parser.o: parser/parser.y
	bison -d -v -o build/parser/parser.cpp $<
	g++ -std=c++11 $(CFLAGS_P) -c -o $@ build/parser/parser.cpp

build/parser/lexer.o: parser/lexer.l
	flex -o build/parser/lexer.cpp $<
	g++ -std=c++11 $(CFLAGS_P) -c -o $@ build/parser/lexer.cpp

.PHONY: parser
parser: $(OBJS_P)

# --------------------------------------------
# Interpreter
# --------------------------------------------
INCLUDES_I=-I$(shell pwd)/
CFLAGS_I=-g -fno-inline-small-functions -no-pie -Wall
CPPS_I=$(shell find interpreter -type f -name '*.cpp')
OBJS_I=$(patsubst %.cpp, build/%.o, $(CPPS_I))
$(OBJS_I): | build/interpreter # create build dir first

build/interpreter:
	mkdir -p build/interpreter

build/interpreter/%.o: interpreter/%.cpp $(wildcard interpreter/%.hpp)
	g++ -std=c++11 ${CFLAGS_I} ${INCLUDES_I} ${INCLUDES_P} -c -o $@ $<

.PHONY: interpreter
interpreter: asmjit $(OBJS_I)

# --------------------------------------------
# General
# --------------------------------------------
LD_FLAGS=-lrt

bin:
	mkdir -p bin

bin/badlang: | bin
bin/badlang: $(OBJS_A) $(OBJS_P) $(OBJS_I)
	g++ ${CFLAGS_I} ${INCLUDES_I} -o $@ $^ ${LD_FLAGS}

badlang: bin/badlang


# --------------------------------------------
# Tests
# --------------------------------------------
TEST_DIR=../tests

# for local testing
test:
	@for f in $(shell ls ${TEST_DIR}); do \
		(../tests/$${f} 1>/dev/null && \
		/bin/echo -e ${GREEN}PASS${NC} $${f} ) || /bin/echo -e ${RED}FAIL${NC} $${f}; \
	done

# for travis, have the makefile return "error" if test fails
tests:
	@for f in $(shell ls ${TEST_DIR}); do \
		../tests/$${f} 1>/dev/null || exit 1; \
	done

.PHONY: clean
clean:
	$(RM) -r bin build
