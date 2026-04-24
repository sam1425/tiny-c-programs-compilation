CC = gcc
CFLAGS = -Wall -Wextra -std=c23
LDFLAGS = -lncurses

TARGET = typing_test
SRC = typingtest_ncurse.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
