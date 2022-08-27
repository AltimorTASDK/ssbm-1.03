ifeq ($(VERSION), 100)
ELFFILE := $(GCIBIN)/loader-ntsc100.elf
BINFILE := $(GCIBIN)/loader-ntsc100.bin
else ifeq ($(VERSION), 101)
ELFFILE := $(GCIBIN)/loader-ntsc101.elf
BINFILE := $(GCIBIN)/loader-ntsc101.bin
else ifeq ($(VERSION), 102)
ELFFILE := $(GCIBIN)/loader-ntsc102.elf
BINFILE := $(GCIBIN)/loader-ntsc102.bin
else ifeq ($(VERSION), PAL)
ELFFILE := $(GCIBIN)/loader-pal.elf
BINFILE := $(GCIBIN)/loader-pal.bin
endif

ifdef NOPAL
ZLIBBIN := $(GCIBIN)/zlib.bin
endif

CFILES   := $(foreach dir, $(SRCDIR), $(shell find $(dir) -type f -name '*.c'   2> /dev/null))
CXXFILES := $(foreach dir, $(SRCDIR), $(shell find $(dir) -type f -name '*.cpp' 2> /dev/null))
SFILES   := $(foreach dir, $(SRCDIR), $(shell find $(dir) -type f -name '*.S'   2> /dev/null))

ifdef NOPAL
ZLIB     := $(LIBDIR)/zlib
CFLAGS   += -Wno-conversion -DZLIB_CONST -DMY_ZCALLOC
CXXFLAGS += -DZLIB_CONST
INCLUDE  += -I$(ZLIB)
CFILES   += $(ZLIB)/adler32.c $(ZLIB)/crc32.c $(ZLIB)/inffast.c $(ZLIB)/inflate.c $(ZLIB)/inftrees.c $(ZLIB)/trees.c \
            $(ZLIB)/zutil.c
endif

OBJFILES := \
    $(patsubst %, $(OBJDIR)/%.o, $(CFILES)) \
    $(patsubst %, $(OBJDIR)/%.o, $(CXXFILES)) \
    $(patsubst %, $(OBJDIR)/%.o, $(SFILES))

DEPFILES := $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $(OBJFILES))

ifdef NOPAL
MGCDIR := $(GCIDIR)/a
else
MGCDIR := $(GCIDIR)/b
endif

ifeq ($(VERSION), PAL)
GCIFILE    := $(BINDIR)/$(MODNAME)-loader-PAL.gci
GCISRC     := $(GCIDIR)/ssbm-PAL.gci
MGCMAIN    := $(MGCDIR)/1.03-PAL.mgc
LINKSCRIPT := loader-PAL.ld
COMMONLD   := ../common-b.ld loader-b.ld
else
GCIFILE    := $(BINDIR)/$(MODNAME)-loader-USA.gci
GCISRC     := $(GCIDIR)/ssbm-USA.gci
MGCMAIN    := $(MGCDIR)/1.03-USA.mgc
GCIFILEJPN := $(BINDIR)/$(MODNAME)-loader-JPN.gci
GCISRCJPN  := $(GCIDIR)/ssbm-JPN.gci
MGCMAINJPN := $(MGCDIR)/1.03-JPN.mgc
ifdef NOPAL
LINKSCRIPT := loader-a-NTSC.ld
COMMONLD   := ../common-a.ld
else
LINKSCRIPT := loader-b-NTSC.ld
COMMONLD   := ../common-b.ld loader-b.ld
endif
endif

ifeq ($(VERSION), 102)
ifndef NOPAL
GCIFILE20XX := $(BINDIR)/$(MODNAME)-loader-20XX.gci
GCISRC20XX  := $(GCIDIR)/ssbm-20XX.gci
MGCMAIN20XX := $(MGCDIR)/1.03-20XX.mgc
GCIFILEUP := $(BINDIR)/$(MODNAME)-loader-UP.gci
GCISRCUP  := $(GCIDIR)/ssbm-UP.gci
MGCMAINUP := $(MGCDIR)/1.03-UP.mgc

.PHONY: all
all: loader JPN 20XX UP
else
.PHONY: all
all: loader JPN
endif
else
.PHONY: all
ifeq ($(VERSION), PAL)
all: loader
else
all: loader JPN
endif
endif

.PHONY: loader
loader: $(GCIFILE)

$(GCIFILE): $(BINFILE) $(ZLIBBIN) $(MGCMAIN) $(GCISRC)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(TOOLS)/melee-gci-compiler/melee-gci-compiler.py -i $(GCISRC) -o $@ $(MGCMAIN)

ifneq ($(VERSION), PAL)
.PHONY: JPN
JPN: $(GCIFILEJPN)

$(GCIFILEJPN): $(BINFILE) $(MGCMAINJPN) $(GCISRCJPN)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(TOOLS)/melee-gci-compiler/melee-gci-compiler.py -i $(GCISRCJPN) -o $@ $(MGCMAINJPN)
endif

ifeq ($(VERSION), 102)
ifndef NOPAL
.PHONY: 20XX
20XX: $(GCIFILE20XX)

$(GCIFILE20XX): $(BINFILE) $(MGCMAIN20XX) $(GCISRC20XX)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(TOOLS)/melee-gci-compiler/melee-gci-compiler.py -i $(GCISRC20XX) -o $@ $(MGCMAIN20XX)

.PHONY: UP
UP: $(GCIFILEUP)

$(GCIFILEUP): $(BINFILE) $(MGCMAINUP) $(GCISRCUP)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(TOOLS)/melee-gci-compiler/melee-gci-compiler.py -i $(GCISRCUP) -o $@ $(MGCMAINUP)
endif
endif

.PHONY: bin
bin: $(BINFILE) $(ZLIBBIN)

$(BINFILE): $(ELFFILE)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(OBJCOPY) -O binary -R .zlib $< $@

$(ZLIBBIN): $(ELFFILE)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(OBJCOPY) -O binary -j .zlib $< $@

$(ELFFILE): $(OBJFILES) $(LINKSCRIPT) $(MELEELD) $(COMMONLD) | clean-unused
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(CC) $(LDFLAGS) -T$(LINKSCRIPT) -T$(MELEELD) $(OBJFILES) -o $@

$(OBJDIR)/%.c.o: %.c
	@[ -d $(@D) ] || mkdir -p $(@D)
	@[ -d $(subst $(OBJDIR), $(DEPDIR), $(@D)) ] || mkdir -p $(subst $(OBJDIR), $(DEPDIR), $(@D))
	$(CC) -MMD -MP -MF $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $@) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(OBJDIR)/%.cpp.o: %.cpp
	@[ -d $(@D) ] || mkdir -p $(@D)
	@[ -d $(subst $(OBJDIR), $(DEPDIR), $(@D)) ] || mkdir -p $(subst $(OBJDIR), $(DEPDIR), $(@D))
	$(CXX) -MMD -MP -MF $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $@) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(OBJDIR)/%.S.o: %.S
	@[ -d $(@D) ] || mkdir -p $(@D)
	@[ -d $(subst $(OBJDIR), $(DEPDIR), $(@D)) ] || mkdir -p $(subst $(OBJDIR), $(DEPDIR), $(@D))
	$(CC) $(ASFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf build $(GCIBIN)

# Remove unused obj/dep files
.PHONY: clean-unused
clean-unused:
	$(foreach file, $(shell find $(OBJDIR) -type f 2> /dev/null), \
		$(if $(filter $(file), $(OBJFILES) $(OUTPUTMAP)),, \
		rm $(file);))
	$(foreach file, $(shell find $(DEPDIR) -type f 2> /dev/null), \
		$(if $(filter $(file), $(DEPFILES)),, \
		rm $(file);))

-include $(DEPFILES)