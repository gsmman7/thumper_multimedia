# The compiler to use
CC=$(CCPREFIX)g++

# Compiler flags
CFLAGS=-c -Wall
	# -c: Compile or assemble the source files, but do not link. The linking stage simply is not done. The ultimate output is in the form of an object file for each source file.
	# -Wall: This enables all the warnings about constructions that some users consider questionable, and that are easy to avoid (or modify to prevent the warning), even in conjunction with macros.

# Name of executable output
EXECUTABLE=touchberry

all: $(EXECUTABLE)

$(EXECUTABLE): I2C_master.o
	$(CC) I2C_master.o -o $(EXECUTABLE)

main.o: I2C_master.cpp
	$(CC) $(CFLAGS) I2C_master.cpp

clean:
	rm -f *.o $(EXECUTABLE)
