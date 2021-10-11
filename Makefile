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
LDFLAGS    := -Wl,-Map=output.map -Wl,--gc-sections -flto

CFLAGS   := -DGEKKO -mogc -mcpu=750 -meabi -mhard-float -Os -Wall \
			-Wno-register -Wno-unused-value -Wconversion -Warith-conversion \
			-ffunction-sections -fdata-sections -flto
CXXFLAGS := $(CFLAGS) -std=c++2b -fconcepts -fno-rtti -fno-exceptions
INCLUDE  := -Isrc -I$(LIBOGC)/include

GCIFILE     := $(BINDIR)/ssbm-1.03.gci
GCISRC      := $(GCIDIR)/ssbm.gci
GCIFILE20XX := $(BINDIR)/ssbm-1.03-20XX.gci
GCISRC20XX  := $(GCIDIR)/20XX.gci
ELFFILE     := $(GCIDIR)/1.03_data/code.elf
BINFILE     := $(GCIDIR)/1.03_data/code.bin

MGCMAIN  := $(GCIDIR)/1.03.mgc
MGCFILES := $(shell find $(GCIDIR) -type f -name '*.mgc')

.PHONY: all
all: ssbm 20xx

.PHONY: ssbm
ssbm: $(GCIFILE)

.PHONY: 20xx
20xx: $(GCIFILE20XX)

$(GCIFILE): $(BINFILE) $(MGCFILES)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(TOOLS)/melee-gci-compiler/melee-gci-compiler.py -i $(GCISRC) -o $@ $(MGCMAIN)

$(GCIFILE20XX): $(BINFILE) $(MGCFILES) $(GCISRC20XX)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(TOOLS)/melee-gci-compiler/melee-gci-compiler.py -i $(GCISRC20XX) -o $@ $(MGCMAIN)

.PHONY: bin
bin: $(BINFILE)

$(BINFILE): $(ELFFILE)
	powerpc-eabi-objcopy -O binary $< $@

$(ELFFILE): $(OBJFILES) GALE01.ld $(LINKSCRIPT) | resources clean_unused
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
	
RESOURCE_DIR_IN  := resources
RESOURCE_DIR_OUT := src/resources
RESOURCES        := $(foreach dir, $(RESOURCE_DIR_IN), $(shell find $(dir) -type f))
RESOURCES        := $(filter-out %.psd, $(RESOURCES))
TEXTURES         := $(filter     %.png, $(RESOURCES))
RESOURCES        := $(filter-out %.png, $(RESOURCES))

define get_resource_out
$(subst $(RESOURCE_DIR_IN), $(RESOURCE_DIR_OUT), $1)
endef

define get_texture_out
$(shell echo $(subst $(RESOURCE_DIR_IN), $(RESOURCE_DIR_OUT), $1) | sed -r "s/([^.]*)\\.?.*\\.png/\\1.tex/")
endef

define make_resource_rule
$(call get_resource_out, $1): $1
	cp $$< $$@
endef

define make_texture_rule
$(call get_texture_out, $1): $1 $(TOOLS)/encode_texture.py
	python $(TOOLS)/encode_texture.py $$< $$@
endef

define make_header_rule
$1.h: $1 $(TOOLS)/bin_to_header.py
	python $(TOOLS)/bin_to_header.py $$< $$@
endef

RESOURCES_OUT := $(foreach resource, $(RESOURCES), $(call get_resource_out, $(resource)))
RESOURCES_H_OUT := $(foreach out, $(RESOURCES_OUT), $(out).h)
TEXTURES_OUT := $(foreach texture, $(TEXTURES), $(call get_texture_out, $(texture)))
TEXTURES_H_OUT := $(foreach out, $(TEXTURES_OUT), $(out).h)

.PHONY: resources
resources: $(RESOURCES_OUT) $(RESOURCES_H_OUT) $(TEXTURES_OUT) $(TEXTURES_H_OUT)

$(foreach resource, $(RESOURCES), $(eval $(call make_resource_rule, $(resource))))
$(foreach texture, $(TEXTURES), $(eval $(call make_texture_rule, $(texture))))

$(foreach resource, $(RESOURCES_OUT) $(TEXTURES_OUT), $(eval $(call make_header_rule, $(resource))))

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(DEPDIR) $(BINDIR) $(BINFILE) $(RESOURCE_DIR_OUT)

# Remove unused obj/dep/resource files
.PHONY: clean_unused
clean_unused:
	$(foreach file, $(shell find $(OBJDIR) -type f), $(if $(filter $(file), $(OBJFILES)),, $(shell rm $(file))))
	$(foreach file, $(shell find $(DEPDIR) -type f), $(if $(filter $(file), $(DEPFILES)),, $(shell rm $(file))))
	$(foreach file, $(shell find $(RESOURCE_DIR_OUT) -type f), \
		$(if $(filter $(file), $(RESOURCES_OUT) $(RESOURCES_H_OUT) $(TEXTURES_OUT) $(TEXTURES_H_OUT)),, \
		$(shell rm $(file))))

-include $(DEPFILES)