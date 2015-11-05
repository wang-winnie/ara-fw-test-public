# Makefile for Project Ara firmware tests

include Makefile.inc


.PHONY: all apps lava clean

all: apps lava

apps:
	$(Q)$(MAKE) -C apps
	@if [ $$? -ne 0 ] ; then exit; fi;

lava:
	$(Q)$(MAKE) -C lava
	@if [ $$? -ne 0 ] ; then exit; fi;

clean:
	$(RM) -rf $(OUTDIR)
	$(Q)$(MAKE) -C lava clean 2>&1 > /dev/null
	$(Q)$(MAKE) -C apps clean 2>&1 > /dev/null


