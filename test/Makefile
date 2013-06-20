# Makefile

SRC_FILE := bitmap_test.c

INC_DIR := ../include/
GTK_INC := `pkg-config --cflags gtk+-2.0`
GTK_LIB := `pkg-config --libs gtk+-2.0`

CFLAGS += -I$(INC_DIR) -I$(GTK_INC)

OBJ_FILE := $(SRC_FILE:.c=.o)
OUTPUT_FILE := $(SRC_FILE:.c=)

all: $(OBJ_FILE)
	$(CC) -o $(OUTPUT_FILE) $(OBJ_FILE) -L../ -lmv $(GTK_LIB)  -ggdb

clean:
	rm *.o  *.a  -f
