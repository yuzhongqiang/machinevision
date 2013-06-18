# Makefile

SRC_FILES := utils.c mat.c bmp.c wnd_gtk.c
INC_DIR := include/
GTK_INC := `pkg-config --cflags gtk+-2.0`

CFLAGS += -I$(INC_DIR) -I$(GTK_INC)

OBJ_FILES := $(SRC_FILES:.c=.o)

LIB_FILE := libmv.a

all: $(OBJ_FILES)
	ar -r $(LIB_FILE) $(OBJ_FILES)

clean:
	rm *.o  *.a  -f
