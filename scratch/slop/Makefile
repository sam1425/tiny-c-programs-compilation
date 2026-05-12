CC      := gcc
CFLAGS  := -Ofast -Wall -Wextra -std=c11 -D_GNU_SOURCE -march=native -flto -s -fno-plt -DNDEBUG
LDFLAGS := -flto -lpthread -Wl,-O1 -Wl,--as-needed -Wl,--sort-common -Wl,--hash-style=gnu
LIBGIT2_DIR := libgit2_src
PKGINCS     := -I$(LIBGIT2_DIR)/include
PKGLIBS     := $(LIBGIT2_DIR)/build/libgit2.a -lz

TARGET  := gitstatus
SRC     := starshipslop.c

.PHONY: all install uninstall clean clean-all check-deps

all: check-deps $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(PKGINCS) -o $@ $< $(LDFLAGS) $(PKGLIBS)
	@echo "Built $(TARGET) with Local Static libgit2."

install: $(TARGET)
	@mkdir -p $(HOME)/.local/bin
	install -m 755 $(TARGET) $(HOME)/.local/bin/$(TARGET)
	@echo "Installed to $(HOME)/.local/bin/$(TARGET)"

uninstall:
	rm -f $(HOME)/.local/bin/$(TARGET)

clean:
	rm -f $(TARGET)

clean-all: clean
	rm -rf $(LIBGIT2_DIR)

check-deps:
	@pkg-config --exists libgit2 2>/dev/null || { echo "ERROR: libgit2 not found."; exit 1; }
