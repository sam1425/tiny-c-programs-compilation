CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lraylib -lm -lpthread -ldl -lrt -lX11

TARGET = game
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean

