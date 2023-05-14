CC = gcc
CFLAGS = -march=native -std=c11
SRCDIR = src
LIBDIR = $(wildcard ./lib/*/src)
BINDIR = bin
BUILDDIR = build

INCLUDE_DIRS := ./include
INCLUDE_DIRS += $(wildcard ./lib/*/include)
INC_PARAMS=$(INCLUDE_DIRS:%=-I%)

# List all the source files in the src directory
SRC := $(wildcard $(SRCDIR)/*.c)

# List all the source files in the lib/isocline/src directory
#SRC += $(wildcard $(LIBDIR)/*.c)
SRC += ./lib/isocline/src/isocline.c

# Create object file names by replacing .c extension with .o
OBJ := $(patsubst %.c, $(BUILDDIR)/%.o, $(notdir $(SRC)))

all: debug
debug: CFLAGS += -ggdb -DDEBUG
debug: executables

release: CFLAGS += -O3
release: strip
executables: $(BINDIR)/clox

run: release
	./$(BINDIR)/clox ${ARGS}

strip: $(BINDIR)/clox
	strip -s $(BINDIR)/clox
$(BINDIR)/clox: $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC_PARAMS) $^ -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC_PARAMS) -c $< -o $@

$(BUILDDIR)/%.o: $(LIBDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC_PARAMS) -c $< -o $@

clean:
	rm -rf $(BINDIR) $(BUILDDIR)

.PHONY: all clean
