DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -g
else
	CFLAGS += -O3
endif

mbc0/mbc0: mbc0/mbc0.o common.o

menu/mbcMenu: menu/mbcMenu.o common.o

%.o: %.asm
	cd $(dir $@);	rgbasm -o $(notdir $@) $(notdir $<)

%.gb: %.o
	cd $(dir $@);	rgblink -o $(notdir $@) $(notdir $<)
	cd $(dir $@);	rgbfix -v -p 0 $(notdir $@)

.PHONY: all debug clean mbc0 menu

mbc0: mbc0/mbc0
menu: menu/mbcMenu menu/menu.gb

all: mbc0 menu

clean:
	-rm *.o
	-rm mbc0/mbc0 mbc0/*.o
	-rm menu/mbcMenu menu/menu.gb mbcMenu/*.o
