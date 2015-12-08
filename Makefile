# Makefile for Project Ara firmware tests

include Makefile.inc


.PHONY: all clean

all:
	$(RM) -rf $(TOPDIR)/build
	mkdir -p $(OUTDIR)/apps
	$(Q)$(MAKE) -C apps
	@if [ $$? -ne 0 ] ; then exit; fi;
	mkdir -p $(OUTDIR)/lava
	cp -r ./lava/$(PLATFORM)/* $(OUTDIR)/lava/

clean:
	$(RM) -rf $(TOPDIR)/build
	$(Q)$(MAKE) -C apps clean 2>&1 > /dev/null


