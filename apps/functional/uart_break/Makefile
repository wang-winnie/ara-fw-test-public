include $(CURDIR)/../../../Makefile.inc

APP=$(notdir $(CURDIR))

OBJS=$(patsubst %.c, %.o, $(wildcard *.c))
HDRS=$(wildcard *.h)

APPLIBS     += $(APPLIBDIR)/libfwtest.a
APPLIBDIRS  += $(APPLIBDIR)
#APPINCLUDES +=

LDLIBS   += $(APPLIBS)
LDFLAGS  += $(patsubst %,-L%,$(subst ' ', ,$(APPLIBDIRS)))
CFLAGS   += -static $(patsubst %,-I%,$(subst ' ', ,$(APPINCLUDES)))

#$(info CFLAGS=$(CFLAGS))
#$(info LDFLAGS=$(LDFLAGS))
#$(info LDLIBS=$(LDLIBS))

default: $(APP)
	@mkdir -p $(APPOUTDIR)
	@cp $(APP) $(APPOUTDIR)

all: default

%.o: %.c $(HDRS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(APP): $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) -f *.o *.a $(APP)

.PHONY: all clean


