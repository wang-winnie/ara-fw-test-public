include $(CURDIR)/../../Makefile.inc

LIB=libfwtest.a
OBJS=$(patsubst %.c, %.o, $(wildcard *.c))
HDRS=$(wildcard *.h)
#$(info OBJS=$(OBJS))

CFLAGS += -static

all: $(LIB)

$(LIB): $(OBJS)
	$(Q)$(AR) $@ $^

%.o: %.c $(HDRS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -f *.o *.a

.PHONY: all clean

