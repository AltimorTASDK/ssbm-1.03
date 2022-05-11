ifdef NOPAL
OUTDIR  := $(GCIDIR)/a
else
OUTDIR  := $(GCIDIR)/b
endif

ELFFILE := $(OUTDIR)/loader.elf
BINFILE := $(OUTDIR)/loader.bin

ifeq ($(VERSION), 100)
ELFFILE := $(OUTDIR)/loader-ntsc100.elf
BINFILE := $(OUTDIR)/loader-ntsc100.bin
else ifeq ($(VERSION), 101)
ELFFILE := $(OUTDIR)/loader-ntsc101.elf
BINFILE := $(OUTDIR)/loader-ntsc101.bin
else ifeq ($(VERSION), 102)
ELFFILE := $(OUTDIR)/loader-ntsc102.elf
BINFILE := $(OUTDIR)/loader-ntsc102.bin
else ifeq ($(VERSION), PAL)
ELFFILE := $(OUTDIR)/loader-pal.elf
BINFILE := $(OUTDIR)/loader-pal.bin
endif

ifdef NOPAL
ZLIBBIN := $(OUTDIR)/zlib.bin
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

ifeq ($(VERSION), PAL)
GCIFILE    := $(BINDIR)/$(MODNAME)-loader-PAL.gci
GCISRC     := $(GCIDIR)/ssbm-PAL.gci
MGCMAIN    := $(GCIDIR)/1.03-PAL.mgc
LINKSCRIPT := loader-PAL.ld
else
GCIFILE    := $(BINDIR)/$(MODNAME)-loader-NTSC.gci
GCISRC     := $(GCIDIR)/ssbm-NTSC.gci
ifdef NOPAL
MGCMAIN    := $(GCIDIR)/1.03-a-NTSC.mgc
LINKSCRIPT := loader-a-NTSC.ld
else
MGCMAIN    := $(GCIDIR)/1.03-b-NTSC.mgc
LINKSCRIPT := loader-b-NTSC.ld
endif
endif

ifeq ($(VERSION), 102)
ifndef NOPAL
GCIFILE20XX := $(BINDIR)/$(MODNAME)-loader-20XX.gci
GCISRC20XX  := $(GCIDIR)/ssbm-20XX.gci
MGCMAIN20XX := $(GCIDIR)/1.03-20XX.mgc
GCIFILEUP := $(BINDIR)/$(MODNAME)-loader-UP.gci
GCISRCUP  := $(GCIDIR)/ssbm-UP.gci
MGCMAINUP := $(GCIDIR)/1.03-UP.mgc

.PHONY: all
all: ssbm 20XX UP
else
.PHONY: all
all: ssbm
endif
else
.PHONY: all
all: ssbm
endif

.PHONY: ssbm
ssbm: $(GCIFILE)

$(GCIFILE): $(BINFILE) $(ZLIBBIN) $(MGCMAIN) $(GCISRC)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(TOOLS)/melee-gci-compiler/melee-gci-compiler.py -i $(GCISRC) -o $@ $(MGCMAIN)

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

$(ELFFILE): $(OBJFILES) $(LINKSCRIPT) $(MELEELD) loader.ld ../common.ld | clean_unused
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
	rm -rf $(OBJDIR) $(DEPDIR) $(ELFFILE) $(BINFILE)

# Remove unused obj/dep/resource files
.PHONY: clean_unused
clean_unused:
	$(foreach file, $(shell find $(OBJDIR) -type f 2> /dev/null), \
		$(if $(filter $(file), $(OBJFILES) $(OUTPUTMAP)),, \
		rm $(file);))
	$(foreach file, $(shell find $(DEPDIR) -type f 2> /dev/null), \
		$(if $(filter $(file), $(DEPFILES)),, \
		rm $(file);))

-include $(DEPFILES)