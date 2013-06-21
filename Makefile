# Makefile

export OUT_DIR := out

LIB_FILELIST := utils.c mat.c bmp.c wnd_gtk.c image.c
CURR_DIR := $(shell pwd)

export INC_DIR := $(CURR_DIR)/include/
export GTK_INC := `pkg-config --cflags gtk+-2.0`
export GTK_LIB := `pkg-config --libs gtk+-2.0`

export CFLAGS += -I$(INC_DIR) -I$(GTK_INC) -ggdb

LIB_FILELIST := $(LIB_FILELIST:.c=.o)

LIB_FILE := libmv.a

all: $(LIB_FILELIST)
	ar -r $(LIB_FILE) $(LIB_FILELIST)
	make test
	
.PHONY: test clean
test:
	make -C test

clean:
	rm *.o  *.a  -f
	make -C test clean
