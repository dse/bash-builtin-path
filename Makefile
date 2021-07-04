ifeq ($(OSTYPE), cygwin)
    CFLAGS  := -I/usr/include/bash
    LDFLAGS := -shared
    LIBS    := -lbash
    TARGETS := build/path-builtin.dll
else
    CFLAGS  := -I/usr/include/bash
    LDFLAGS := -shared
    LIBS    := -lbash
    TARGETS := build/path-builtin.so
endif

.PHONY: default
default: $(TARGETS)

build/%.o: src/%.c Makefile
	mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/%.dll: build/%.o Makefile
	$(CC) $(LDFLAGS) -o $@ $< $(LIBS)

build/%.so: build/%.o Makefile
	$(CC) $(LDFLAGS) -o $@ $< $(LIBS)

build/%.dylib: build/%.o Makefile
	$(CC) $(LDFLAGS) -o $@ $< $(LIBS)

.PHONY: clean
clean:
	/bin/rm *.so *.dll *.dylib || true

.PHONY: test-path-list
test-path-list: build/path-builtin.dll
	bash -c 'enable -f build/path-builtin.dll path; path PATH'
