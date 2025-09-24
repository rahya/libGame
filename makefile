

EXEC=x64/libGame
CXX=g++
CC=gcc

# GDB option -g
C_FLAGS+= -Wall -Werror -pthread -I./source -I./
CXX_FLAGS+= -Wall -Werror -std=c++23 -pthread -I./source -I./

L_FLAGS= -lpthread

HEADERS := $(shell find . -name '*.h' | tr '\n' ' ')

SOURCES=$(patsubst %.cpp, %.o, $(wildcard         *.cpp)) \
	$(patsubst %.cpp, %.o, $(wildcard       */*.cpp)) \
	$(patsubst %.cpp, %.o, $(wildcard     */*/*.cpp)) \
	$(patsubst %.cpp, %.o, $(wildcard   */*/*/*.cpp)) \
	$(patsubst %.cpp, %.o, $(wildcard */*/*/*/*.cpp)) \
	$(patsubst %.c,   %.o, $(wildcard         *.c  )) \
	$(patsubst %.c,   %.o, $(wildcard       */*.c  )) \
	$(patsubst %.c,   %.o, $(wildcard     */*/*.c  )) \
	$(patsubst %.c,   %.o, $(wildcard   */*/*/*.c  )) \
	$(patsubst %.c,   %.o, $(wildcard */*/*/*/*.c  )) 


all: $(SOURCES) 
	@ echo "Link $(EXEC)"
	@ $(CXX) -o $(EXEC) $^ $(L_FLAGS)

deploy:
	cp pairsUSDT.txt tmp/

run:
	./$(EXEC) --server

.PHONY: test
test:
	./$(EXEC)

clean: 
	rm $(SOURCES)
	rm -rf x64

%.o: %.cpp $(HEADERS)
	@ echo "Compile $<"
	@ $(CXX) $(CXX_FLAGS) -o $@ -c $<

%.o: %.c $(HEADERS)
	@ echo "Compile $<"
	@ $(CC) $(C_FLAGS) -o $@ -c $<

