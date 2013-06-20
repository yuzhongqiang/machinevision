# Makefile

LIB_FILELIST := utils.c mat.c bmp.c wnd_gtk.c image.c

INC_DIR := include/
GTK_INC := `pkg-config --cflags gtk+-2.0`

CFLAGS += -I$(INC_DIR) -I$(GTK_INC)

LIB_FILELIST := $(LIB_FILELIST:.c=.o)

LIB_FILE := libmv.a

all: $(LIB_FILELIST)
	ar -r $(LIB_FILE) $(LIB_FILELIST)
	
.PHONY: test clean
test:
	make -C test

clean:
	rm *.o  *.a  -f
