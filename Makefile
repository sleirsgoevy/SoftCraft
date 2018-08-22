.PHONY: debug clean
OBJECTS  := $(shell find . -name '*.cpp' | sed 's/[.]cpp/.o/g')
OPTFLAGS ?= -O3 -flto
CXXFLAGS := $(OPTFLAGS) -std=c++14
CXX      := clang++

main: $(OBJECTS)
	$(CXX) $(OBJECTS) $(OPTFLAGS) -Wno-odr -o main -lSDL2 -pthread

pgo_full:
	make clean
	make pgo_instrument
	make pgo_collect
	make clean
	make pgo_enabled PGO_PROFILE=pgo.profdata

pgo_instrument:
	make OPTFLAGS='-O3 -flto -g -fprofile-instr-generate' main

pgo_collect: main
	LLVM_PROFILE_FILE='pgo-%m.profraw' ./main
	llvm-profdata merge -output=pgo.profdata *.profraw

pgo_enabled:
	make OPTFLAGS='$(OPTFLAGS) -fprofile-instr-use=$(PGO_PROFILE)' main

debug:
	make OPTFLAGS='-O0 -g' main

clean:
	rm -f main *.o *.profraw
