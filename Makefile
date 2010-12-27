# CSP Makefile
# Author: Arun Chaganty <arunchaganty@gmail.com>
#

ROOTDIR=./
include Makefile.inc

TARGETS=bin/helvetica
TESTS = tests/tuples tests/parser

PRJNAME=helvetica
VERSION=0.1
SRCFILES=include/ misc/ src/ tests/ Doxyfile Makefile README 
DISTFILES=$(TARGETS) tests/ doc/ README

CFLAGS += `pkg-config --cflags libxml-2.0`
LDFLAGS +=  `pkg-config --libs libxml-2.0`

LIB_OBJS=obj/CSP.o obj/CSPSolver.o obj/Log.o
BIN_OBJS=obj/main.o
GLOBAL_HEADERS=
OBJS=$(LIB_OBJS) $(BIN_OBJS)

all: $(TARGETS)

tests: $(TESTS)

bin/helvetica: ${OBJS}
	if [ ! -e bin ]; then mkdir bin; fi;
	$(CC) $(LDFLAGS) $^ -o $@

# Pattern to build obj files from src files
${LIB_OBJS}: obj/%.o : src/%.cpp include/%.h ${GLOBAL_HEADERS}
	if [ ! -e obj ]; then mkdir obj; fi;
	$(CC) $(CFLAGS) -c $< -o $@

${BIN_OBJS}: obj/%.o : src/%.cpp ${GLOBAL_HEADERS}
	if [ ! -e obj ]; then mkdir obj; fi;
	$(CC) $(CFLAGS) -c $< -o $@

${TESTS}: tests/% : tests/%.cpp ${LIB_OBJS}
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

test: tests
	./test.sh

src-dist: 
	rm -rf $(PRJNAME)-src-$(VERSION)
	mkdir $(PRJNAME)-src-$(VERSION)
	cp -rf $(SRCFILES) $(PRJNAME)-src-$(VERSION)/
	tar -czf $(PRJNAME)-src-$(VERSION).tar.gz $(PRJNAME)-src-$(VERSION)/
	rm -rf $(PRJNAME)-src-$(VERSION)

bin-dist: all
	rm -rf $(PRJNAME)-$(VERSION)
	mkdir $(PRJNAME)-$(VERSION)
	cp -rf $(DISTFILES) $(PRJNAME)-$(VERSION)/
	tar -czf $(PRJNAME)-$(VERSION).tar.gz $(PRJNAME)-$(VERSION)/
	rm -r $(PRJNAME)-$(VERSION)

.PHONY: clean clean-tests doc test

doc: 
	doxygen

clean:
	rm -rf bin/*
	rm -rf lib/*
	rm -rf obj/*

clean-tests:
	rm -rf $(TESTS)
