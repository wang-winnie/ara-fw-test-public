# common defines

SHELL = /bin/bash

# set build output dir
TOPDIR:=$(abspath $(dir $(lastword $(MAKEFILE_LIST))))
PLATFORM:=sdb
OUTDIR:=$(TOPDIR)/build/$(PLATFORM)

APPINCLUDES = \
    $(TOPDIR)/apps/include \
    $(TOPDIR)/apps/lib/include

APPLIBDIR  = $(TOPDIR)/apps/lib
APPOUTDIR  = $(OUTDIR)/apps

#$(info Building with aarch64-linux-android toolchain)
CROSSPRE := aarch64-linux-android-
GCCTOOL := /opt/android-toolchain-aarch64-4.9-host-linux-x86_64-android-21/bin/aarch64-linux-android-gcc
ARCHCFLAGS =
ARCHWARNINGS =
ARCHOPTIMIZATION =
ARCHCPUFLAGS =
ARCHINCLUDES =
ARCHDEFINES =
CC = $(CROSSPRE)gcc
CPP = $(CROSSPRE)gcc -E

TOOLDIR := $(shell dirname -- $(GCCTOOL))
export PATH := $(TOOLDIR):$(PATH)

# tool aliases
Q = @
RM = @rm
LD = @$(CROSSPRE)ld
AR = @$(CROSSPRE)ar rcs
NM = @$(CROSSPRE)nm
OBJCOPY = $(CROSSPRE)objcopy
OBJDUMP = $(CROSSPRE)objdump
OBJEXT = .o
LIBEXT = .a
EXEEXT =

# flags
EXTRADEFINES =
EXTRA_FLAGS =

#from Android.mk
#LOCAL_CFLAGS += -pie -fPIE
#LOCAL_LDFLAGS += -pie -fPIE

CFLAGS = -pie -fPIE $(ARCHCFLAGS) $(ARCHWARNINGS) $(ARCHOPTIMIZATION) $(ARCHCPUFLAGS) \
    $(ARCHINCLUDES) $(ARCHDEFINES) $(EXTRADEFINES) -pipe $(EXTRA_FLAGS)
CPPFLAGS = $(ARCHINCLUDES) $(ARCHDEFINES) $(EXTRADEFINES)
AFLAGS = $(CFLAGS) -D__ASSEMBLY__
LDFLAGS += -gc-sections -pie -fPIE

# dump settings
ifeq (0,1)
    $(info Building with the following settings:)
    $(info TOPDIR     = $(TOPDIR))
    $(info OUTDIR     = $(OUTDIR))
    $(info TOOLDIR    = $(TOOLDIR))
    $(info PATH       = $(PATH))
    $(info CROSSPRE   = $(CROSSPRE))
    $(info CC         = $(CC))
    $(info CPP        = $(CPP))
    $(info LD         = $(LD))
    $(info AR         = $(AR))
    $(info NM         = $(NM))
    $(info OBJCOPY    = $(OBJCOPY))
    $(info OBJDUMP    = $(OBJDUMP))
    $(info CFLAGS     = $(CFLAGS))
    $(info CPPFLAGS   = $(CPPFLAGS))
    $(info AFLAGS     = $(AFLAGS))
    $(info LDFLAGS    = $(LDFLAGS))
    $(info APPINCLUDE = $(APPINCLUDE))
    $(info LIBINCLUDE = $(LIBINCLUDE))
endif

# PREPROCESS - Default macro to run the C pre-processor
# Example: $(call PREPROCESS, in-file, out-file)
#   CPP - The command to invoke the C pre-processor
#   CPPFLAGS - Options to pass to the C pre-processor
define PREPROCESS
	@echo "CPP: $1->$2"
	$(Q) $(CPP) $(CPPFLAGS) $1 -o $2
endef

# COMPILE - Default macro to compile one C file
# Example: $(call COMPILE, in-file, out-file)
#   CC - The command to invoke the C compiler
#   CFLAGS - Options to pass to the C compiler
define COMPILE
	@echo "CC: $1"
	$(Q) $(CC) -c $(CFLAGS) $1 -o $2
endef

# ASSEMBLE - Default macro to assemble one assembly language file
# Example: $(call ASSEMBLE, in-file, out-file)
#   CC - By default, the C compiler is used to compile assembly language
#        files
#   AFLAGS - Options to pass to the C+compiler
define ASSEMBLE
	@echo "AS: $1"
	$(Q) $(CC) -c $(AFLAGS) $1 -o $2
endef

# ARCHIVE - Add a list of files to an archive
# Example: $(call ARCHIVE, archive-file, "file1 file2 file3 ...")
define ARCHIVE
	@echo "AR: $2"
	$(Q) $(AR) $1 $(2) || { echo "$(AR) $1 FAILED!" ; exit 1 ; }
endef

# DELFILE - Delete one file
define DELFILE
	$(Q) rm -f $1
endef

# DELDIR - Delete one directory
define DELDIR
	$(Q) rm -rf $1
endef

# MOVEFILE - Move one file
define MOVEFILE
	$(Q) mv -f $1 $2
endef

# CLEAN - Default clean target
define CLEAN
	$(Q) rm -f *$(OBJEXT) *$(LIBEXT) *~ .*.swp
endef
