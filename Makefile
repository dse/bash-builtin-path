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

.PHONY: test-path-check
test-path-check: build/path-builtin.dll
	bash -c 'FOO=banana:orange:apple; enable -f build/path-builtin.dll path; path -c FOO banana'
	bash -c 'FOO=banana:orange:apple; enable -f build/path-builtin.dll path; path -c FOO orange'
	bash -c 'FOO=banana:orange:apple; enable -f build/path-builtin.dll path; path -c FOO apple'
	bash -c 'FOO=banana:orange:apple; enable -f build/path-builtin.dll path; ! path -c FOO banan'
	bash -c 'FOO=banana:orange:apple; enable -f build/path-builtin.dll path; ! path -c FOO orang'
	bash -c 'FOO=banana:orange:apple; enable -f build/path-builtin.dll path; ! path -c FOO appl'
	bash -c 'FOO=banana:orange:apple; enable -f build/path-builtin.dll path; ! path -c FOO anana'
	bash -c 'FOO=banana:orange:apple; enable -f build/path-builtin.dll path; ! path -c FOO range'
	bash -c 'FOO=banana:orange:apple; enable -f build/path-builtin.dll path; ! path -c FOO pple'
	bash -c 'FOO=banana:orange:apple; enable -f build/path-builtin.dll path; ! path -c FOO anan'
	bash -c 'FOO=banana:orange:apple; enable -f build/path-builtin.dll path; ! path -c FOO rang'
	bash -c 'FOO=banana:orange:apple; enable -f build/path-builtin.dll path; ! path -c FOO ppl'
