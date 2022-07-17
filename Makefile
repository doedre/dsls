include config.mk

.PHONY: options clean

.SUFFIXES: .c .o

_default: dsls

options:
	@echo "Build options:"
	@echo "CC	= $(CC)"
	@echo "CFLAGS	= $(CFLAGS)"
	@echo "LDFLAGS	= $(LDFLAGS)"
	@echo "SRC	= $(SRC)"
	@echo "OBJ	= $(OBJ)"
	@echo

.c.o: config.h
	@echo [CC] $<
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -r *.o dsls

dsls: $(OBJ)
	@echo [LD] $@
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $@

install: dsls
	mkdir -p "$(PREFIX)/bin"
	cp -f dsls "$(PREFIX)/bin"
	chmod 755 "$(PREFIX)/bin/dsls"

$(OBJ_DIR):
	mkdir -p $@

