# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Linker flags
LDFLAGS = -lcrypto

# Source files
SOURCES = simple_server.c hashTable.c server_utils.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Executable output name
EXECUTABLE = simple_server

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o server

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
