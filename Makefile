# Makefile

CC = gcc

LIB_FILELIST := utils.c mat.c bmp.c wnd_gtk.c image.c jpeg.c
CURR_DIR := $(shell pwd)

INC_DIR := ./include/
GTK_INC := `pkg-config --cflags gtk+-2.0`
GTK_LIB := `pkg-config --libs gtk+-2.0`

CFLAGS += -I$(INC_DIR) -I$(GTK_INC) -ggdb

LIB_FILELIST := $(LIB_FILELIST:.c=.o)
LIB_FILE := libmv.a

all:
	make lib
	make bmp_test

lib:$(LIB_FILELIST)
	@ar -r $(LIB_FILE) $(LIB_FILELIST)

.PHONY: bmp_test clean
bmp_test:
	$(CC) -o bmp_test bmp_test.c -I$(INC_DIR) -I$(GTK_INC) -L./ -lmv $(GTK_LIB)

clean:
	rm *.o  *.a  -f
	rm -r bmp_test
