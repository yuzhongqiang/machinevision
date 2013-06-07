# Makefile

SRC_FILES := mat.c  utils.c

OBJ_FILES := $(SRC_FILES: .c=.o)

LIB_FILE := libmv.a

all: $(OBJ_FILES)
	ar -r $(LIB_FILE) $(OBJ_FILES)

clean:
	rm *.o  *.a  -f
