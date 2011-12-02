#
# Makefile for libuml.
#

UML = libuml/libuml.o
LIBUML = libuml.a

CXXFLAGS ?= -I./ -O3

$(LIBUML): $(UML)
	$(AR) -cr $@ $?

.PHONY: clean
clean:
	-rm -f $(LIBUML) $(UML)
