CC      := gcc
CFLAGS  := -O3 -Wall -Wextra -std=c11 -D_GNU_SOURCE -march=native -flto -s
LDFLAGS := -flto -lpthread
PKGLIBS := $(shell pkg-config --libs   libgit2 2>/dev/null || echo "-lgit2")
PKGINCS := $(shell pkg-config --cflags libgit2 2>/dev/null)

TARGET  := gitstatus
SRC     := starshipslop.c

.PHONY: all install uninstall clean check-deps

all: check-deps $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(PKGINCS) -o $@ $< $(LDFLAGS) $(PKGLIBS)
	@echo "Built $(TARGET) with Cache-Free Optimizations."

install: $(TARGET)
	@mkdir -p $(HOME)/.local/bin
	install -m 755 $(TARGET) $(HOME)/.local/bin/$(TARGET)
	@echo "Installed to $(HOME)/.local/bin/$(TARGET)"

uninstall:
	rm -f $(HOME)/.local/bin/$(TARGET)

clean:
	rm -f $(TARGET)

check-deps:
	@pkg-config --exists libgit2 2>/dev/null || { echo "ERROR: libgit2 not found."; exit 1; }
