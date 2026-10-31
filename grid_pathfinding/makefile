CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

SRCS = grid_pathfinding.cpp \
       algorithms/brute_force.cpp \
       algorithms/dynamic_programming.cpp \
       algorithms/greedy.cpp

all: exe
exe: $(SRCS) path_result.h
	$(CXX) $(CXXFLAGS) $(SRCS) -o exe
clean:
	rm -f exe
.PHONY: all clean
