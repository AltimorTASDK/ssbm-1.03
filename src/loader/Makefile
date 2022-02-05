ELFFILE  := $(GCIDIR)/1.03_data/loader.elf
BINFILE  := $(GCIDIR)/1.03_data/loader.bin

ifeq ($(VERSION), 100)
ELFFILE  := $(GCIDIR)/1.03_data/loader-ntsc100.elf
BINFILE  := $(GCIDIR)/1.03_data/loader-ntsc100.bin
else ifeq ($(VERSION), 101)
ELFFILE  := $(GCIDIR)/1.03_data/loader-ntsc101.elf
BINFILE  := $(GCIDIR)/1.03_data/loader-ntsc101.bin
else ifeq ($(VERSION), 102)
ELFFILE  := $(GCIDIR)/1.03_data/loader-ntsc102.elf
BINFILE  := $(GCIDIR)/1.03_data/loader-ntsc102.bin
else ifeq ($(VERSION), PAL)
ELFFILE  := $(GCIDIR)/1.03_data/loader-pal.elf
BINFILE  := $(GCIDIR)/1.03_data/loader-pal.bin
endif

OBJDIR   := obj/$(VERSION)
DEPDIR   := dep/$(VERSION)
SOURCES  := src

CFILES   := $(foreach dir, $(SOURCES), $(shell find $(dir) -type f -name '*.c'))
CXXFILES := $(foreach dir, $(SOURCES), $(shell find $(dir) -type f -name '*.cpp'))
SFILES   := $(foreach dir, $(SOURCES), $(shell find $(dir) -type f -name '*.S'))

OBJFILES := \
    $(patsubst %, $(OBJDIR)/%.o, $(CFILES)) \
    $(patsubst %, $(OBJDIR)/%.o, $(CXXFILES)) \
    $(patsubst %, $(OBJDIR)/%.o, $(SFILES))

DEPFILES := $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $(OBJFILES))

ifeq ($(VERSION), PAL)
GCIFILE    := $(BINDIR)/$(MODNAME)-loader-PAL.gci
GCISRC     := $(GCIDIR)/ssbm-PAL.gci
MGCMAIN    := $(GCIDIR)/1.03-PAL.mgc
LINKSCRIPT := loader-pal.ld
else
GCIFILE    := $(BINDIR)/$(MODNAME)-loader-NTSC.gci
GCISRC     := $(GCIDIR)/ssbm-NTSC.gci
MGCMAIN    := $(GCIDIR)/1.03-NTSC.mgc
LINKSCRIPT := loader.ld
endif

MGCFILES := $(shell find $(GCIDIR) -type f -name '*.mgc')

ifeq ($(VERSION), 102)
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

.PHONY: ssbm
ssbm: $(GCIFILE)

$(GCIFILE): $(BINFILE) $(MGCFILES) $(GCISRC)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(TOOLS)/melee-gci-compiler/melee-gci-compiler.py -i $(GCISRC) -o $@ $(MGCMAIN)

ifeq ($(VERSION), 102)
.PHONY: 20XX
20XX: $(GCIFILE20XX)

$(GCIFILE20XX): $(BINFILE) $(MGCFILES) $(GCISRC20XX)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(TOOLS)/melee-gci-compiler/melee-gci-compiler.py -i $(GCISRC20XX) -o $@ $(MGCMAIN20XX)

.PHONY: UP
UP: $(GCIFILEUP)

$(GCIFILEUP): $(BINFILE) $(MGCFILES) $(GCISRCUP)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(TOOLS)/melee-gci-compiler/melee-gci-compiler.py -i $(GCISRCUP) -o $@ $(MGCMAINUP)
endif

.PHONY: bin
bin: $(BINFILE)

$(BINFILE): $(ELFFILE)
	powerpc-eabi-objcopy -O binary $< $@

$(ELFFILE): $(OBJFILES) $(LINKSCRIPT) $(MELEELD) | clean_unused
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
	$(foreach file, $(shell find $(OBJDIR) -type f), $(if $(filter $(file), $(OBJFILES)),, $(shell rm $(file))))
	$(foreach file, $(shell find $(DEPDIR) -type f), $(if $(filter $(file), $(DEPFILES)),, $(shell rm $(file))))

-include $(DEPFILES)