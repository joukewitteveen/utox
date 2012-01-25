#
# Makefile for Utox (not the libraries).
#

CXXFLAGS ?= $(shell pkg-config opencv --cflags) \
            -I../libraries \
            -O2
LDLIBS ?= $(shell pkg-config opencv --libs) \
          -llept \
          -ltesseract$(shell ldconfig -p | grep -P "(?<=\slibtesseract)_api(?=\.)" -m1 -o) \
          -L../libraries -luml
ifdef DEBUG
CXXFLAGS += -Wall -DDEBUG
endif


utox: detector.o reader.o logic.o logic/common.o logic/class.o

.PHONY: clean
clean:
	-rm -f *.o logic/*.o utox
