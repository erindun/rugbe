CXX = g++

CXXFLAGS = -Wall -Werror

LFLAGS = -lmingw32 -lSDL2main -lSDL2

OBJS = main.o disassembler.o cpu.o instructions.o mmu.o ppu.o gameboy.o

all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LFLAGS) -o rugbe

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

ppu.o: src/ppu/ppu.cpp
	$(CXX) $(CXXFLAGS) -c src/ppu/ppu.cpp

gameboy.o: src/gameboy.cpp
	$(CXX) $(CXXFLAGS) -c src/gameboy.cpp

clean:
	rm -rf *.o rugbe