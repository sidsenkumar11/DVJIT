# very very lazy makefile

asmjit_cpps = $(shell find asmjit/ -type f -name '*.cpp')
asmjit_os = $(patsubst %.cpp, %.o, $(asmjit_cpps))
asmjit_bin_os = $(patsubst %, bin/%, $(asmjit_os))

.PHONY: build
build: bin bin/badlang
	
LD_FLAGS=-lrt

bin/badlang: src/main.cpp bin/helpers.o $(asmjit_bin_os)
	g++ -std=c++11 -Wall -o bin/badlang $^ ${LD_FLAGS}

bin/helpers.o: src/helpers.cpp src/helpers.hpp
	g++ -std=c++11 -Wall -c -o $@ src/helpers.cpp


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
