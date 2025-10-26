COMPILER_FLAGS := -Wall
LINKER_FLAGS := -l:libraylib.a -lm

all: version1 version2

.PHONY: version1
version1:
	gcc version1.c $(COMPILER_FLAGS) $(LINKER_FLAGS) -o version1

.PHONY: version2
version2:
	gcc version2.c $(COMPILER_FLAGS) $(LINKER_FLAGS) -o version2
