TARGET=webserver

CC=gcc
CFLAGS=-g -Wall -pedantic -I.
LINKER=gcc -o
LFLAGS=-g -Wall -I. -lrt -lpthread

SRCDIR=src
OBJDIR=objs
BINDIR=bin

SOURCES := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

.PHONY: clean clobber

$(BINDIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(LINKER) $@ $(LFLAGS) $(OBJECTS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir	-p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clobber: clean
	/bin/rm -rf $(BINDIR)
clean:
	/bin/rm -rf $(OBJDIR)
