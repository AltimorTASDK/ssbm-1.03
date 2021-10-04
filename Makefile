DEVKITPATH=$(shell echo "$(DEVKITPRO)" | sed -e 's/^\([a-zA-Z]\):/\/\1/')
PATH := $(DEVKITPATH)/devkitPPC/bin:$(DEVKITPATH)/tools/bin:$(PATH)

AS  := powerpc-eabi-as
CC  := powerpc-eabi-gcc
CXX := powerpc-eabi-g++
LD  := powerpc-eabi-ld

SOURCES   := src
BINDIR    := bin
OBJDIR    := obj
DEPDIR    := dep
TOOLS     := tools
GCIDIR    := gci

CFILES   := $(foreach dir, $(SOURCES), $(shell find $(dir) -type f -name '*.c'))
CXXFILES := $(foreach dir, $(SOURCES), $(shell find $(dir) -type f -name '*.cpp'))
SFILES   := $(foreach dir, $(SOURCES), $(shell find $(dir) -type f -name '*.S'))

OBJFILES := \
    $(patsubst %, $(OBJDIR)/%.o, $(CFILES)) \
    $(patsubst %, $(OBJDIR)/%.o, $(CXXFILES)) \
    $(patsubst %, $(OBJDIR)/%.o, $(SFILES))

DEPFILES := $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $(OBJFILES))

LIBOGC := $(DEVKITPATH)/libogc

LINKSCRIPT := gci_bin.ld
LDFLAGS    := -Wl,-Map=output.map -Wl,--gc-sections -Wl,--print-gc-sections -nostdlib

CFLAGS   := -DGEKKO -mogc -mcpu=750 -meabi -mhard-float -O3 -Wall \
			-Wno-register -Wno-unused-value -ffunction-sections -fdata-sections
CXXFLAGS := $(CFLAGS) -std=c++2b -fconcepts -fno-rtti -fno-exceptions
INCLUDE  := -Isrc -I$(LIBOGC)/include

GCIFILE     := $(BINDIR)/ssbm-1.03.gci
GCIFILE20XX := $(BINDIR)/ssbm-1.03-20XX.gci
GCISRC20XX  := $(GCIDIR)/20XX.gci
BINFILE     := $(GCIDIR)/1.03_data/code.bin

MGCMAIN  := $(GCIDIR)/1.03.mgc
MGCFILES := $(shell find $(GCIDIR) -type f -name '*.mgc')

.PHONY: all
all: ssbm 20xx

.PHONY: ssbm
ssbm: $(GCIFILE)

.PHONY: 20xx
20xx: $(GCIFILE20XX)

$(GCIFILE): bin $(MGCFILES)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(TOOLS)/melee-gci-compiler/melee-gci-compiler.py -o $@ $(MGCMAIN)

$(GCIFILE20XX): bin $(MGCFILES) $(GCISRC20XX)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(TOOLS)/melee-gci-compiler/melee-gci-compiler.py -i $(GCISRC20XX) -o $@ $(MGCMAIN)

.PHONY: bin
bin: $(BINFILE)

$(BINFILE): $(OBJFILES) GALE01.ld $(LINKSCRIPT) | clean_unused
	$(CC) $(LDFLAGS) -T$(LINKSCRIPT) $(OBJFILES) -o $@

GALE01.ld: GALE01.map $(TOOLS)/map_to_linker_script.py
	python $(TOOLS)/map_to_linker_script.py

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
	rm -rf $(OBJDIR) $(DEPDIR) $(BINDIR) $(BINFILE)

# Remove unused obj/dep files
.PHONY: clean_unused
clean_unused:
	$(foreach file, $(shell find $(OBJDIR) -type f), $(if $(filter $(file), $(OBJFILES)),, $(shell rm $(file))))
	$(foreach file, $(shell find $(DEPDIR) -type f), $(if $(filter $(file), $(DEPFILES)),, $(shell rm $(file))))

-include $(DEPFILES)