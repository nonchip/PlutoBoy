DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -g3
else
	CFLAGS += -O3
endif

mbc0: mbc0.o common.o

.PHONY: all debug clean

all: mbc0

clean:
	-rm mbc0 *.o
