# EYES Makefile

export OPENCV_DIR := /home/yuzhongqiang/opencv-2.4.6.1/
export OPENCV_BUILD_DIR := ${OPENCV_DIR}/build
export OPENCV_INC_DIR := $(shell pkg-config --cflags opencv)
export OPENCV_LIB_DIR := $(shell pkg-config --libs opencv)

CFLAGS += ${OPENCV_INC_DIR) ${OPENCV_LIB_DIR}

SRC_FILES := eyes.cpp
EXE_FILES := eyes

all:
	gcc -o ${EXE_FILES}  ${SRC_FILES}  ${OPENCV_INC_DIR} ${OPENCV_LIB_DIR}

clean:
	rm *.o ${EXE_FILES} -rf
