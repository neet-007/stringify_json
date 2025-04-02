CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2
PREFIX ?= $(HOME)/.config/nvim/lua/plugins/stringify_json
DESTDIR ?= 

SRC = serializer.c tokenizer.c
BIN = serializer

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

install: $(BIN)
	mkdir -p $(DESTDIR)$(PREFIX)
	install -m 755 $(BIN) $(DESTDIR)$(PREFIX)/

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/$(BIN)

clean:
	rm -f $(BIN)
