# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Linker flags
LDFLAGS = -lcrypto -pthread 

# Source files
SOURCES = single_thread_server.c fifo.c extended_server_utils.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Executable output name
EXECUTABLE = single_thread_server

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
