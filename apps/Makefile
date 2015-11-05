include $(CURDIR)/../Makefile.inc

SUBDIRS=functional greybus performance stress other

all:
	$(Q)$(MAKE) -C lib
	@for d in $(SUBDIRS) ; do  \
		if [ -d $$d ] ; then \
			if [ -a $$d/Makefile ] ; then \
				$(MAKE) -C $$d ; \
				if [ $$? -ne 0 ] ; then \
					exit 1 ; \
				fi ; \
			fi ; \
		fi ; \
	done ;


clean:
	$(Q)$(MAKE) -C lib clean
	@for d in $(SUBDIRS) ; do  \
		if [ -d $$d ] ; then \
			if [ -a $$d/Makefile ] ; then \
				$(MAKE) -C $$d clean ; \
			fi ; \
		fi ; \
	done ;

.PHONY: all clean
