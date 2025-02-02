# this is the makefile for all Amiga GCC2.5 (GeekGadget based)
# like ADE on aminet.
# use with command "make"

CC = gcc
BUILDDIR = build-gcc
BUILDLIBDIR = build-gcc/libobjs
BUILDEXEDIR = build-gcc/exeobjs

ASMFLAGS = I=include: 
# CFLAGS = -noixemul -m68020 -ffreestanding -nostartfiles

BINARIES = $(BUILDDIR)/keyboardview.gadget $(BUILDDIR)/keyboardviewTest
#  $(BUILDDIR)Static/keyboardviewTest_static

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
	$(CC) -c -o $@ $<  -noixemul -m68020 -ffreestanding -nostartfiles

$(BUILDEXEDIR)/%.c.o: %.c $(BUILDEXEDIR) $(HEADERS)
	$(CC) -c -o $@ $<  -m68020

$(BUILDDIR)Static/%.c.o: %.c $(BUILDDIR) $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS) -DKEYBOARDVIEW_STATICLINK=1


$(BUILDLIBDIR)/%.s.o: %.s $(BUILDLIBDIR) $(HEADERS)
	# with phxass
	phxass $< TO $@ $(ASMFLAGS)

# - - - - - - - - - for .gadget

$(BUILDDIR)/keyboardview.gadget: $(GADGETOBJS)
	$(CC) -o $@ $(GADGETOBJS) $(CFLAGS) -noixemul -m68020 -ffreestanding -nostartfiles  -Wl,-e,__start
#	slink $(GADGETOBJS) lib lib:sc.lib to $@

# - - - - - - - - - executable to test gadget

$(BUILDDIR)/keyboardviewTest: $(BUILDEXEDIR)/keyboardviewtest.c.o
	$(CC) -o $@ $(BUILDEXEDIR)/keyboardviewtest.c.o
#	slink FROM LIB:c.o $(BUILDDIR)/keyboardviewtest.c.o lib $(LIBS) to $@

# - - - - - - - - -  static test case executable

$(BUILDDIR)Static/keyboardviewTest_static: $(BUILDDIR)Static $(BUILDDIR)Static/keyboardviewtest.c.o $(GADGETOBJSSTATIC)
	$(CC) -o $@ $(BUILDDIR)Static/keyboardviewtest.c.o $(GADGETOBJSSTATIC)
#	slink FROM LIB:c.o $(BUILDDIR)Static/keyboardviewtest.c.o $(GADGETOBJSSTATIC) lib $(LIBS) to $@

# - - - - - - -
clean:
	-delete ALL FORCE $(BUILDDIR)
	-delete ALL FORCE $(BUILDDIR)Static
