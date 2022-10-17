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

ifneq ($(VERSION), PAL)
ALLBINS := $(GCIBIN)/loader-ntsc100.bin $(GCIBIN)/loader-ntsc101.bin $(GCIBIN)/loader-ntsc102.bin
else
ALLBINS := $(GCIBIN)/loader-pal.bin
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

ifdef NOPAL
LINKSCRIPT := loader-a-NTSC.ld
COMMONLD   := ../common-a.ld
else
ifeq ($(VERSION), PAL)
LINKSCRIPT := loader-PAL.ld
else
LINKSCRIPT := loader-b-NTSC.ld
endif
COMMONLD   := ../common-b.ld loader-b.ld
endif

GCIPREFIX := $(BINDIR)/$(MODNAME)-loader

.PHONY: gci
ifneq ($(VERSION), PAL)
gci: $(GCIPREFIX)-USA.gci
gci: $(GCIPREFIX)-JPN.gci
else
gci: $(GCIPREFIX)-PAL.gci
endif

ifeq ($(VERSION), 102)
ifndef NOPAL
gci: $(GCIPREFIX)-20XX.gci
gci: $(GCIPREFIX)-UP.gci
endif
endif

$(GCIPREFIX)-%.gci: $(MGCDIR)/1.03-%.mgc $(GCIDIR)/ssbm-%.gci $(wildcard $(ALLBINS))
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(TOOLS)/melee-gci-compiler/melee-gci-compiler.py -i $(GCIDIR)/ssbm-$*.gci -o $@ $<

.PHONY: bin
$(GCIPREFIX)-%.gci: $(BINFILE)
bin:                $(BINFILE)
ifdef NOPAL
$(GCIPREFIX)-%.gci: $(ZLIBBIN)
bin:                $(ZLIBBIN)
endif

$(BINFILE): $(ELFFILE)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(OBJCOPY) -O binary -R .zlib $< $@

ifdef NOPAL
$(ZLIBBIN): $(ELFFILE)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(OBJCOPY) -O binary -j .zlib $< $@
endif

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