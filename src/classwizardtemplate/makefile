# this is the makefile for all Amiga GCC2.5 (GeekGadget based)
# like ADE on aminet.
# Update os-include and os-lib with NDK3.2 include and lib.
# use with command "make"

CC = gcc
BUILDDIR = build-gcc
BUILDLIBDIR = build-gcc/libobjs
BUILDEXEDIR = build-gcc/exeobjs

ASMFLAGS = I=include: 
CFLAGS = -m68000 -Os -noixemul

BINARIES = \
 $(BUILDDIR)/keyboardview.gadget\
 $(BUILDDIR)/keyboardviewTest\
 $(BUILDDIR)Static/keyboardviewTest_static

HEADERS = class_keyboardview.h class_keyboardview_private.h  minialib.h

GADGETOBJS = \
 $(BUILDLIBDIR)/classinit.s.o \
 $(BUILDLIBDIR)/class_keyboardview_lib.c.o \
 $(BUILDLIBDIR)/class_keyboardview_dispatch.c.o \
 $(BUILDLIBDIR)/class_keyboardview_attribs.c.o \
 $(BUILDLIBDIR)/class_keyboardview_render.c.o \
 $(BUILDLIBDIR)/class_keyboardview_handleinput.c.o

GADGETOBJSSTATIC = \
 $(BUILDDIR)Static/class_keyboardview_lib.c.o \
 $(BUILDDIR)Static/class_keyboardview_dispatch.c.o \
 $(BUILDDIR)Static/class_keyboardview_attribs.c.o \
 $(BUILDDIR)Static/class_keyboardview_render.c.o \
 $(BUILDDIR)Static/class_keyboardview_handleinput.c.o

all: $(BINARIES)

# - - - - - - - - -

$(BUILDDIR):
	-makedir $(BUILDDIR)

$(BUILDLIBDIR): $(BUILDDIR)
	-makedir $(BUILDLIBDIR)

$(BUILDEXEDIR): $(BUILDDIR)
	-makedir $(BUILDEXEDIR)

$(BUILDDIR)Static:
	-makedir $(BUILDDIR)Static

# - - - - - - - - 

$(BUILDLIBDIR)/%.c.o: %.c $(BUILDLIBDIR) $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS) -ffreestanding -nostartfiles

$(BUILDEXEDIR)/%.c.o: %.c $(BUILDEXEDIR) $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILDDIR)Static/%.c.o: %.c $(BUILDDIR) $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS) -DKEYBOARDVIEW_STATICLINK=1


$(BUILDLIBDIR)/%.s.o: %.s $(BUILDLIBDIR) $(HEADERS)
	# use phxass as assembler
	phxass $< TO $@ $(ASMFLAGS)
	#gcc assembler will not make it.
	#as $< -o $@ -m68000
# - - - - - - - - - for .gadget

$(BUILDDIR)/keyboardview.gadget: $(GADGETOBJS)
	$(CC) -o $@ $(GADGETOBJS) $(CFLAGS) -ffreestanding -nostartfiles  -Wl,-e,__start

# - - - - - - - - - executable to test gadget

$(BUILDDIR)/keyboardviewTest: $(BUILDEXEDIR)/keyboardviewtest.c.o
	$(CC) -o $@ $(CFLAGS) $(BUILDEXEDIR)/keyboardviewtest.c.o  -s


# - - - - - - - - -  static test case executable

$(BUILDDIR)Static/keyboardviewTest_static: $(BUILDDIR)Static $(BUILDDIR)Static/keyboardviewtest.c.o $(GADGETOBJSSTATIC)
	$(CC) -o $@ $(CFLAGS) $(BUILDDIR)Static/keyboardviewtest.c.o $(GADGETOBJSSTATIC)


# - - - - - - -
clean:
	-delete ALL FORCE $(BUILDDIR)
	-delete ALL FORCE $(BUILDDIR)Static
