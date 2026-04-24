CC = gcc
CFLAGS = -std=c23 -Wall -Werror

TARGET = birthday
SRC = birthday.c daysuntil.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
