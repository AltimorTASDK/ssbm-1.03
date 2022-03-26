ifndef DEVKITPRO
$(error Specify devkitPro install path with $$DEVKITPRO)
endif

DEVKITPATH=$(shell echo "$(DEVKITPRO)" | sed -e 's/^\([a-zA-Z]\):/\/\1/')
PATH := $(DEVKITPATH)/devkitPPC/bin:$(PATH)

export CC  := powerpc-eabi-gcc
export CXX := powerpc-eabi-g++

export DEFINES := -DGEKKO

ifneq ($(findstring __a, __$(MODVERSION)),)
export NOPAL   := 1
export DEFINES += -DNOPAL
endif

ifneq ($(findstring -beta, $(MODVERSION)),)
export DEFINES += -DBETA
else ifneq ($(findstring -rc, $(MODVERSION)),)
export DEFINES += -DBETA
endif

ifneq ($(MODVERSION),)
export MODNAME := ssbm-1.03-$(MODVERSION)
else
export MODNAME := ssbm-1.03
endif

export DEFINES += -DMODNAME=\"$(MODNAME)\"

ifeq ($(VERSION),)
$(error Specify Melee version with $$VERSION)
else ifeq ($(VERSION), 100)
export MELEELD := $(abspath GALE01r0.ld)
export DEFINES += -DNTSC100
else ifeq ($(VERSION), 101)
export MELEELD := $(abspath GALE01r1.ld)
export DEFINES += -DNTSC101
else ifeq ($(VERSION), 102)
export MELEELD := $(abspath GALE01r2.ld)
export DEFINES += -DNTSC102
else ifeq ($(VERSION), PAL)
export MELEELD := $(abspath GALP01.ld)
export DEFINES += -DPAL
else
$(error Unsupported Melee version "$(VERSION)")
endif

export BINDIR  := $(abspath bin)
export TOOLS   := $(abspath tools)
export GCIDIR  := $(abspath gci)

export OBJDIR  := obj/$(VERSION)
export DEPDIR  := dep/$(VERSION)
export SOURCES := src

export OUTPUTMAP := $(OBJDIR)/output.map
export LDFLAGS   := -Wl,-Map=$(OUTPUTMAP) -Wl,--gc-sections

export MELEEMAP := $(MELEELD:.ld=.map)

export CFLAGS   := $(DEFINES) -mogc -mcpu=750 -meabi -mhard-float -Os \
				   -Wall -Wno-switch -Wno-register -Wno-unused-value -Wconversion -Warith-conversion -Wno-multichar \
				   -ffunction-sections -fdata-sections -mno-sdata \
				   -fno-builtin-sqrt -fno-builtin-sqrtf
export ASFLAGS  := $(DEFINES) -Wa,-mregnames -Wa,-mgekko
export CXXFLAGS := $(CFLAGS) -std=c++2b -fconcepts -fno-rtti -fno-exceptions
export INCLUDE  := -I$(SOURCES) -I$(abspath src/mod/$(SOURCES)) -I$(DEVKITPATH)/libogc/include

.PHONY: all
all: loader mod

.PHONY: loader clean_tmp
loader: $(MELEELD)
	+@cd src/loader && $(MAKE)

.PHONY: mod
mod: $(MELEELD) clean_tmp
	+@cd src/mod && $(MAKE)

.PHONY: resources
resources: $(MELEELD)
	+@cd src/mod && $(MAKE) resources

$(MELEELD): $(MELEEMAP) $(TOOLS)/map_to_linker_script.py
	python $(TOOLS)/map_to_linker_script.py $(MELEEMAP) $(MELEELD)

.PHONY: clean_tmp
clean_tmp:
	@rm -rf $(GCIDIR)/tmp_*

.PHONY: clean
clean:
	rm -rf $(BINDIR)
	@cd src/loader && make clean
	@cd src/mod    && make clean