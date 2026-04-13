# SENTINEL-D – Makefile
#
# Targets:
#   make           → release binary  (bin/sentinel-d)
#   make debug     → debug binary    with ASan + UBSan
#   make test      → build & run unit tests
#   make clean     → remove build artifacts
#   make install   → install to /usr/local/bin

CC      := gcc
BINARY  := bin/sentinel-d

# Source and include directories
SRC_DIR     := src
INCLUDE_DIR := include
BIN_DIR     := bin

SRCS := $(SRC_DIR)/main.c       \
        $(SRC_DIR)/config.c     \
        $(SRC_DIR)/daemon.c     \
        $(SRC_DIR)/http_client.c\
        $(SRC_DIR)/alert.c      \
        $(SRC_DIR)/scheduler.c  \
        $(SRC_DIR)/logger.c     \
        $(SRC_DIR)/utils.c

OBJS := $(SRCS:.c=.o)

# Flags
BASE_CFLAGS := -std=c11 \
               -Wall -Wextra -Wpedantic \
               -Wshadow -Wformat=2 -Wstrict-prototypes \
               -I$(INCLUDE_DIR)

RELEASE_CFLAGS := $(BASE_CFLAGS) -O2 -DNDEBUG
DEBUG_CFLAGS   := $(BASE_CFLAGS) -g -O0 \
                  -fsanitize=address,undefined \
                  -fno-omit-frame-pointer \
                  -DDEBUG

LDFLAGS := -lssl -lcrypto

# -----------------------------------------------------------------------
# Default: release build
# -----------------------------------------------------------------------
.PHONY: all
all: CFLAGS = $(RELEASE_CFLAGS)
all: $(BINARY)

# -----------------------------------------------------------------------
# Debug build (ASan + UBSan)
# -----------------------------------------------------------------------
.PHONY: debug
debug: CFLAGS = $(DEBUG_CFLAGS)
debug: LDFLAGS += -fsanitize=address,undefined
debug: clean $(BINARY)

# -----------------------------------------------------------------------
# Link
# -----------------------------------------------------------------------
$(BINARY): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built: $@"

# -----------------------------------------------------------------------
# Compile (pattern rule)
# -----------------------------------------------------------------------
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# -----------------------------------------------------------------------
# Directories
# -----------------------------------------------------------------------
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# -----------------------------------------------------------------------
# Tests
# -----------------------------------------------------------------------
.PHONY: test
test:
	$(MAKE) -C tests CC=$(CC)

# -----------------------------------------------------------------------
# Install
# -----------------------------------------------------------------------
PREFIX     ?= /usr/local
INSTALL_DIR := $(DESTDIR)$(PREFIX)/bin

.PHONY: install
install: all
	install -d $(INSTALL_DIR)
	install -m 755 $(BINARY) $(INSTALL_DIR)/sentinel-d
	@echo "Installed to $(INSTALL_DIR)/sentinel-d"

# -----------------------------------------------------------------------
# Clean
# -----------------------------------------------------------------------
.PHONY: clean
clean:
	rm -f $(SRCS:.c=.o) $(BINARY)
	$(MAKE) -C tests clean 2>/dev/null || true
	@echo "Clean done"
