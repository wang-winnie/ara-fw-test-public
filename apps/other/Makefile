include $(CURDIR)/../../Makefile.inc

SUBDIRS=$(wildcard *)

all:
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
	@for d in $(SUBDIRS) ; do  \
		if [ -d $$d ] ; then \
			if [ -a $$d/Makefile ] ; then \
				$(MAKE) -C $$d clean ; \
			fi ; \
		fi ; \
	done ;

.PHONY: all clean
