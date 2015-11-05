# Makefile for Project Ara firmware test LAVA files

SHELL = /bin/bash

# set build output dir
TOPDIR:=$(shell cd ..;pwd)
OUTDIR:=$(TOPDIR)/build

# add processing tools bin dir to shell path
PPTOOL := $(shell which awk)
TOOLDIR := $(shell dirname -- $(PPTOOL))
export PATH := $(TOOLDIR):$(PATH)

# tool aliases
Q = @
RM = @rm
AR = @ar rcs

# dump settings
ifeq (0,1)
    $(info Building with the following settings:)
    $(info TOPDIR     = $(TOPDIR))
    $(info OUTDIR     = $(OUTDIR))
    $(info TOOLDIR    = $(TOOLDIR))
    $(info PATH       = $(PATH))
endif

all: json yaml

json:

yaml:

clean:
	$(RM) -rf $(OUTDIR)

.PHONY: all json yaml clean
