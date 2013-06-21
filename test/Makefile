# Makefile

SRC_FILE := bitmap_test.c

OBJ_FILE := $(SRC_FILE:.c=.o)
OUTPUT_FILE := $(SRC_FILE:.c=)

all: $(OBJ_FILE)
	$(CC) -o $(OUTPUT_FILE) $(OBJ_FILE) -L../ -lmv $(GTK_LIB)

clean:
	rm *.o  *.a  -f
