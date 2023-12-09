#    ifeq ($(MAKECMDGOALS),)
#    
#    include etc/default_target.mk
#    
#    %:
#    	cleanmake -f Makefile
#    
#    else
#    
#    $(MAKECMDGOALS): 
#    	cleanmake -f Makefile $(MAKECMDGOALS)
#    
#    endif

.PHONY: make_it

ifneq ($(MAKECMDGOALS),)
$(MAKECMDGOALS):
	+${MAKE} -f Makefile --jobs=8 $(MAKECMDGOALS)
endif

bin/bal: make_it

%:
	+${MAKE} -f Makefile --jobs=8 $@

# include Makefile

Makefile:;
	
GNUmakefile:;
	

