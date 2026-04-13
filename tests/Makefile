# tests/Makefile – build and run SENTINEL-D unit tests

CC      ?= gcc
TOPDIR  := ..

CFLAGS  := -std=c11 \
           -Wall -Wextra -Wpedantic \
           -g -O0 \
           -I$(TOPDIR)/include

# Sources from the main project that tests depend on
# (exclude main.c, daemon.c, http_client.c – not needed for pure unit tests)
COMMON_SRCS := $(TOPDIR)/src/config.c    \
               $(TOPDIR)/src/logger.c    \
               $(TOPDIR)/src/utils.c     \
               $(TOPDIR)/src/alert.c     \
               $(TOPDIR)/src/scheduler.c

LDFLAGS := -lssl -lcrypto

TEST_BINS := test_config test_utils test_alert_payload

.PHONY: all clean

all: $(TEST_BINS)
	@echo "--- Running test_config ---"
	./test_config
	@echo ""
	@echo "--- Running test_utils ---"
	./test_utils
	@echo ""
	@echo "--- Running test_alert_payload ---"
	./test_alert_payload
	@echo ""
	@echo "All tests passed."

# Config test: needs config.c + logger.c + utils.c
test_config: test_config.c \
             $(TOPDIR)/src/config.c \
             $(TOPDIR)/src/logger.c \
             $(TOPDIR)/src/utils.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Utils test: needs utils.c only
test_utils: test_utils.c $(TOPDIR)/src/utils.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Alert payload test: needs alert.c + config.c + logger.c + utils.c
# alert.c links OpenSSL (for webhook_post), which is fine even for unit tests
# since alert_build_payload() doesn't touch the network.
test_alert_payload: test_alert_payload.c \
                    $(TOPDIR)/src/alert.c     \
                    $(TOPDIR)/src/config.c    \
                    $(TOPDIR)/src/logger.c    \
                    $(TOPDIR)/src/utils.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TEST_BINS)
