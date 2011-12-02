#
# Makefile delegating make for the entire Utox tree
#

DIRECTORIES = libraries source interfaces
DIRECTORIES_CLEAN = $(patsubst %,%.clean,$(DIRECTORIES))

.PHONY: all clean $(DIRECTORIES) $(DIRECTORIES_CLEAN)
all: $(DIRECTORIES)
clean: $(DIRECTORIES_CLEAN)

source: libraries

$(DIRECTORIES):
	$(MAKE) -C $@

$(DIRECTORIES_CLEAN):
	$(MAKE) -C $(@:.clean=) clean
