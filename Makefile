CXX = g++

CXXFLAGS = -Wall -Werror

OBJS = main.o disassembler.o cpu.o instructions.o mmu.o

all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o rugbe

main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp

disassembler.o: src/cpu/disassembler.cpp
	$(CXX) $(CXXFLAGS) -c src/cpu/disassembler.cpp

cpu.o: src/cpu/cpu.cpp
	$(CXX) $(CXXFLAGS) -c src/cpu/cpu.cpp

instructions.o: src/cpu/instructions.cpp
	$(CXX) $(CXXFLAGS) -c src/cpu/instructions.cpp

mmu.o: src/mmu/mmu.cpp
	$(CXX) $(CXXFLAGS) -c src/mmu/mmu.cpp

clean:
	rm -rf *.o rugbe