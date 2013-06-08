# Makefile

SRC_FILES := utils.c mat.c bmp.c
INC_DIR := include

CFLAGS += -I$(INC_DIR)

OBJ_FILES := $(SRC_FILES:.c=.o)

LIB_FILE := libmv.a

all: $(OBJ_FILES)
	ar -r $(LIB_FILE) $(OBJ_FILES)

clean:
	rm *.o  *.a  -f
