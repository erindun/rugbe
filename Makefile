CXX = g++

CXXFLAGS = -Wall -Werror

OBJS = main.o disassembler.o

all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o rugbe

main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp

disassembler.o: src/disassembler.cpp
	$(CXX) $(CXXFLAGS) -c src/disassembler.cpp

clean:
	rm -rf *.o rugbe