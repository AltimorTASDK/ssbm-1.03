ELFFILE     := $(GCIDIR)/1.03_data/loader.elf
BINFILE     := $(GCIDIR)/1.03_data/loader.bin

OBJDIR   := obj
DEPDIR   := dep
SOURCES  := src

CFILES   := $(foreach dir, $(SOURCES), $(shell find $(dir) -type f -name '*.c'))
CXXFILES := $(foreach dir, $(SOURCES), $(shell find $(dir) -type f -name '*.cpp'))
SFILES   := $(foreach dir, $(SOURCES), $(shell find $(dir) -type f -name '*.S'))

OBJFILES := \
    $(patsubst %, $(OBJDIR)/%.o, $(CFILES)) \
    $(patsubst %, $(OBJDIR)/%.o, $(CXXFILES)) \
    $(patsubst %, $(OBJDIR)/%.o, $(SFILES))

DEPFILES := $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $(OBJFILES))

LINKSCRIPT := loader.ld

GCIFILE     := $(BINDIR)/ssbm-1.03-loader.gci
GCISRC      := $(GCIDIR)/ssbm.gci
GCIFILE20XX := $(BINDIR)/ssbm-1.03-loader-20XX.gci
GCISRC20XX  := $(GCIDIR)/20XX.gci

MGCMAIN  := $(GCIDIR)/1.03.mgc
MGCFILES := $(shell find $(GCIDIR) -type f -name '*.mgc')

.PHONY: all
all: ssbm 20xx

.PHONY: ssbm
ssbm: $(GCIFILE)

.PHONY: 20xx
20xx: $(GCIFILE20XX)

$(GCIFILE): $(BINFILE) $(MGCFILES) $(GCISRC)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(TOOLS)/melee-gci-compiler/melee-gci-compiler.py -i $(GCISRC) -o $@ $(MGCMAIN)

$(GCIFILE20XX): $(BINFILE) $(MGCFILES) $(GCISRC20XX)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(TOOLS)/melee-gci-compiler/melee-gci-compiler.py -i $(GCISRC20XX) -o $@ $(MGCMAIN)

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
	$(AS) -mregnames -mgekko $^ -o $@

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(DEPDIR) $(ELFFILE) $(BINFILE)

# Remove unused obj/dep/resource files
.PHONY: clean_unused
clean_unused:
	$(foreach file, $(shell find $(OBJDIR) -type f), $(if $(filter $(file), $(OBJFILES)),, $(shell rm $(file))))
	$(foreach file, $(shell find $(DEPDIR) -type f), $(if $(filter $(file), $(DEPFILES)),, $(shell rm $(file))))

-include $(DEPFILES)