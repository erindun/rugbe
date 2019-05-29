CXX = g++

CXXFLAGS = -Wall -Werror

OBJS = main.o disassembler.o cpu.o instructions.o mmu.o

all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o rugbe

main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp

cpu/disassembler.o: src/disassembler.cpp
	$(CXX) $(CXXFLAGS) -c src/cpu/disassembler.cpp

cpu/cpu.o: src/cpu.cpp
	$(CXX) $(CXXFLAGS) -c src/cpu/cpu.cpp

cpu/instructions.o: src/instructions.cpp
	$(CXX) $(CXXFLAGS) -c src/cpu/instructions.cpp

mmu/mmu.o: src/mmu/mmu.cpp
	$(CXX) $(CXXFLAGS) -c src/mmu/mmu.cpp

clean:
	rm -rf *.o rugbe