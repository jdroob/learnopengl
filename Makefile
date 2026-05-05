CPP = g++
CC = gcc
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g
INC_FLAGS = -Iglad
LDFLAGS = -lGL -lglfw

CPP_SRCS = $(wildcard *.cpp)
C_SRCS = $(wildcard glad/*.c)
OBJS = $(patsubst %.cpp,obj/%.o,$(CPP_SRCS)) $(patsubst glad/%.c,obj/glad/%.o,$(C_SRCS))

build: $(OBJS)
	@mkdir -p bin
	$(CPP) $(OBJS) $(CXXFLAGS) $(INC_FLAGS) $(LDFLAGS) -o bin/prog

obj/%.o: %.cpp
	@mkdir -p obj
	$(CPP) -c $< $(CXXFLAGS) $(INC_FLAGS) -o $@

obj/glad/%.o: glad/%.c
	@mkdir -p obj/glad
	$(CC) -c $< $(INC_FLAGS) -o $@

clean:
	@rm -rf bin obj

.PHONY: clean
