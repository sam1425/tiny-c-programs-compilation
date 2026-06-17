CC      := gcc
CFLAGS  := -Ofast -Wall -Wextra -std=c11 -D_GNU_SOURCE -march=native -flto -s -fno-plt -DNDEBUG
LDFLAGS := -flto -lpthread -Wl,-O1 -Wl,--as-needed -Wl,--sort-common -Wl,--hash-style=gnu
LIBGIT2_DIR := libgit2_src
PKGINCS     := -I$(LIBGIT2_DIR)/include
PKGLIBS     := $(LIBGIT2_DIR)/build/libgit2.a -lz

.PHONY: all install uninstall clean clean-all check-deps

all: check-deps slop slopd

slop: slop.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

slopd: slopd.c
	$(CC) $(CFLAGS) $(PKGINCS) -o $@ $< $(LDFLAGS) $(PKGLIBS)

install: slop slopd
	@mkdir -p $(HOME)/.local/bin
	install -m 755 slop $(HOME)/.local/bin/slop
	install -m 755 slopd $(HOME)/.local/bin/slopd
	@echo "Installed to $(HOME)/.local/bin/"

uninstall:
	rm -f $(HOME)/.local/bin/slop $(HOME)/.local/bin/slopd

clean:
	rm -f slop slopd

clean-all: clean
	rm -rf $(LIBGIT2_DIR)

check-deps:
	@pkg-config --exists libgit2 2>/dev/null || { echo "ERROR: libgit2 not found."; exit 1; }
