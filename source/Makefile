#
# Makefile for Utox (not the libraries).
#

CXXFLAGS ?= $(shell pkg-config opencv --cflags --libs) \
            -llept \
            -ltesseract$(shell ldconfig -p | grep -P "(?<=\slibtesseract)_api(?=\.)" -m1 -o) \
            -I../libraries -L../libraries \
            -O2
ifdef DEBUG
CXXFLAGS += -Wall -DDEBUG
endif


# libuml contains a virtual function, so it should be linked lastly.
utox: detector.o reader.o logic.o logic/common.o logic/class.o
	$(CXX) $(CXXFLAGS) $@.cpp $^ -luml -o $@

.PHONY: clean
clean:
	-rm -f *.o logic/*.o utox
