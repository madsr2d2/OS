# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Linker flags
LDFLAGS = -lcrypto -pthread 

# Source files
SOURCES = extended_server_cashing_v1.c fifo.c extended_server_utils.c hashTable.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Executable output name
EXECUTABLE = extended_server_cashing_v1

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
