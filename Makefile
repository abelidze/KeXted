PROJECT = kexted
BUILD   = gcc
MAKEDIR = mkdir
CFLAGS  = -I.\include -c
LDFLAGS = -s -L. -l:pdcurses.a
OBJECTS = main.o kex.o utility.o
OUT_DIR = bin
OUTPUT  = -o ./$(OUT_DIR)/$(PROJECT)
.PHONY: directories clean

all: directories $(PROJECT) clean

$(PROJECT): $(OBJECTS)
	$(BUILD) $(OUTPUT) $(OBJECTS) $(LDFLAGS)

# .SUFFIXES: .c .o
.c.o:
	$(BUILD) $(CFLAGS) $< -o $@

directories: $(OUT_DIR)

$(OUT_DIR):
	$(MAKEDIR) $(OUT_DIR)

clean:
	rm -rf *.o