COMPILER_FLAGS := -Wall
LINKER_FLAGS := -l:libraylib.a -lm

all: 
	gcc main.c $(COMPILER_FLAGS) $(LINKER_FLAGS) -o main
