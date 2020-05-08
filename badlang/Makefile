# very very lazy makefile

asmjit_cpps = $(shell find asmjit/ -type f -name '*.cpp')
asmjit_os = $(patsubst %.cpp, %.o, $(asmjit_cpps))
asmjit_bin_os = $(patsubst %, bin/%, $(asmjit_os))

.PHONY: build
build: bin bin/badlang
	
LD_FLAGS=-lrt
INCLUDE_FLAGS=-I$(shell pwd)/
C_FLAGS=-g -fno-inline-small-functions -no-pie

bin/badlang: src/main.cpp bin/helpers.o bin/jit_compile.o $(asmjit_bin_os)
	g++ ${C_FLAGS} -std=c++11 -Wall ${INCLUDE_FLAGS} -o bin/badlang $^ ${LD_FLAGS}

bin/helpers.o: src/helpers.cpp src/helpers.hpp
	g++ ${C_FLAGS} -std=c++11 -Wall -c ${INCLUDE_FLAGS} -o $@ src/helpers.cpp

bin/jit_compile.o: src/jit_compile.hpp src/jit_compile.cpp
	g++ ${C_FLAGS} -std=c++11 -Wall ${INCLUDE_FLAGS} -c -o $@ src/jit_compile.cpp


.PHONY: asmjit
asmjit: $(asmjit_bin_os)


ASMJIT_FLAGS=-DASMJIT_STATIC

asmjit/%.o: asmjit/%.cpp
	g++ -std=c++11 ${ASMJIT_FLAGS} -c -o $@ $^

bin/asmjit/%.o: asmjit/%.o
	mkdir -p `dirname $@`
	mv $^ `dirname $@`

bin:
	mkdir -p ./bin

.PHONY: clean
clean:
	rm -r bin/*
