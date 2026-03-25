CC      := gcc
#CFLAGS  := -O2 -Wall -Wextra -std=c11 -D_GNU_SOURCE
CFLAGS  := -O3 -Wall -Wextra -std=c11 -D_GNU_SOURCE -march=native -s
LDFLAGS := -lpthread
PKGLIBS := $(shell pkg-config --libs   libgit2 2>/dev/null || echo "-lgit2")
PKGINCS := $(shell pkg-config --cflags libgit2 2>/dev/null)

TARGET  := gitstatus
SRC     := starshipslop.c

.PHONY: all install uninstall clean check-deps

all: check-deps $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(PKGINCS) -o $@ $< $(LDFLAGS) $(PKGLIBS)
	@echo "Built $(TARGET) — run 'make install' to copy to ~/.local/bin"

install: $(TARGET)
	@mkdir -p $(HOME)/.local/bin
	install -m 755 $(TARGET) $(HOME)/.local/bin/$(TARGET)
	@echo "Installed to $(HOME)/.local/bin/$(TARGET)"
	@echo "Make sure ~/.local/bin is in your PATH"

uninstall:
	rm -f $(HOME)/.local/bin/$(TARGET)

clean:
	rm -f $(TARGET)

check-deps:
	@command -v pkg-config >/dev/null 2>&1 || \
		{ echo "ERROR: pkg-config not found"; exit 1; }
	@pkg-config --exists libgit2 2>/dev/null || \
		{ echo "ERROR: libgit2 not found."; \
		  echo "  Ubuntu/Debian : sudo apt install libgit2-dev"; \
		  echo "  Arch          : sudo pacman -S libgit2"; \
		  echo "  Fedora        : sudo dnf install libgit2-devel"; \
		  exit 1; }
	@echo "Dependencies OK (libgit2 $(shell pkg-config --modversion libgit2))"
