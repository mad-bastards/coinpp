export SAVEFLAGS:=$(MAKEFLAGS)

.PHONY: make_it

ifneq ($(MAKECMDGOALS),)
$(MAKECMDGOALS):
	+${MAKE} -f Makefile --jobs=8 $(MAKECMDGOALS)
endif

bin/coinpp: make_it

%:
	+${MAKE} -f Makefile --jobs=8 $@

# include Makefile

Makefile:;
	
GNUmakefile:;
	

