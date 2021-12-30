
ARCH := $(shell uname -s)
CC = g++
DEBUG = -g3
PROFILE =
STATIC = -static
INCLUDES = -I. -Icables
DEFINES = -D__USE_REENTRANT -D_REENTRANT -D_THREAD_SAFE -D_LIBC_REENTRANT \
		-D_GNU_SOURCE 
OPTIMIZATION = -march=i386 -mtune=i386 \
		 -fno-common -pipe -Wno-char-subscripts -O3
# -fomit-frame-pointer
CFLAGS = -Wall $(DEFINES) $(INCLUDES) $(DEBUG) $(PROFILE) $(OPTIMIZATION)
LIBS = -lfltk -L/usr/X11/lib -lX11 -lXpm -lXft -lXext -lXrender -lfreetype \
		-lfontconfig -lexpat -lz -lpthread
LDFLAGS = $(STATIC) $(DEBUG) $(PROFILE) -Wl,--start-group $(LIBS) \
	-Wl,--end-group
INSTALL_DIR = /usr/local/bin
TARGET = fjtag

OBJS = Fl_History_Input.o main.o ui.o prefs.o flash.o fjtag.o status.o \
	altera.o parport.o tap.o part.o e1c_maxloader_bus.o bus.o cfi.o \
	jedec.o amd.o nios_bus.o

ifneq ($(ARCH),Linux)
endif

all:	do-it-all

ifeq (.depend,$(wildcard .depend))
include .depend
do-it-all:	$(TARGET)
else
do-it-all:	depend $(TARGET)
endif

$(TARGET):	$(OBJS)
		$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

.C.o:
		$(CC) $(CFLAGS) -c $< -o $@

clean:
		@rm -f .depend *.o *~ *core $(TARGET) && echo Clean Ok.
		
install:	all
		@cp -af $(TARGET) $(INSTALL_DIR) && echo Install Ok.

dist:		dep clean all

strip:
		@strip -s $(TARGET)
		@objcopy -R .comment -R .note $(TARGET) $(TARGET).stripped
		@sstrip $(TARGET).stripped
		@upx $(TARGET).stripped
		@sstrip $(TARGET).stripped
		@mv -f $(TARGET).stripped $(TARGET)

dep depend:
		$(CC) $(CFLAGS) -M *.C > .depend

