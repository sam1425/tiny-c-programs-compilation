CC     = gcc
CFLAGS = -Wall -Wextra -Werror -D_POSIX_C_SOURCE=200809L -std=c11 -O2
TARGET = text
SRCS   = RopeFunctions.c handlestates.c texteditor.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
