ifndef DEVKITPRO
$(error Specify devkitPro install path with $$DEVKITPRO)
endif

DEVKITPATH=$(shell echo "$(DEVKITPRO)" | sed -e 's/^\([a-zA-Z]\):/\/\1/')
PATH := $(DEVKITPATH)/devkitPPC/bin:$(PATH)

export CC      := powerpc-eabi-gcc
export CXX     := powerpc-eabi-g++
export OBJCOPY := powerpc-eabi-objcopy

export DEFINES := $(foreach def, $(USERDEFS), -D$(def))
export DEFINES += -DGEKKO

ifdef MODVERSION
ifneq ($(shell echo "$(MODVERSION)" | grep -P '^a($$|[\d-])'),)
export NOPAL := 1
endif
ifneq ($(shell echo "$(MODVERSION)" | grep -P '(^|-)(beta|rc)($$|[\d-])'),)
export BETA := 1
endif
export MODNAME := ssbm-1.03-$(MODVERSION)
else
export MODNAME := ssbm-1.03
endif

ifdef NOPAL
export DEFINES += -DNOPAL
endif
ifdef BETA
export DEFINES += -DBETA
endif

export DEFINES += -DMODNAME=\"$(MODNAME)\"

ifndef VERSION
$(warning Melee $$VERSION not specified. Defaulting to 102.)
export VERSION := 102
endif

ifeq ($(VERSION), 100)
export MELEELD := $(abspath GALE01r0.ld)
export DEFINES += -DNTSC100
else ifeq ($(VERSION), 101)
export MELEELD := $(abspath GALE01r1.ld)
export DEFINES += -DNTSC101
else ifeq ($(VERSION), 102)
export MELEELD := $(abspath GALE01r2.ld)
export DEFINES += -DNTSC102
else ifeq ($(VERSION), PAL)
ifdef NOPAL
$(error $$VERSION set to PAL, but $$NOPAL specified.)
endif
export MELEELD := $(abspath GALP01.ld)
export DEFINES += -DPAL
else
$(error Unsupported Melee version "$(VERSION)")
endif

export ISODIR  := $(abspath iso)
export TOOLS   := $(abspath tools)
export GCIDIR  := $(abspath gci)

ifdef NOPAL
export BINDIR  := $(abspath bin/a)
export OBJDIR  := obj/a/$(VERSION)
export DEPDIR  := dep/a/$(VERSION)
else
export BINDIR  := $(abspath bin/b)
export OBJDIR  := obj/b/$(VERSION)
export DEPDIR  := dep/b/$(VERSION)
endif
export SOURCES := src

export OUTPUTMAP = $(OBJDIR)/output.map
export LDFLAGS   = -Wl,-Map=$(OUTPUTMAP) -Wl,--gc-sections

export MELEEMAP  = $(MELEELD:.ld=.map)

export CFLAGS    = $(DEFINES) -mogc -mcpu=750 -meabi -mhard-float -Os \
				   -Wall -Wno-switch -Wno-unused-value -Wconversion -Warith-conversion -Wno-multichar \
				   -Wno-pointer-arith \
				   -ffunction-sections -fdata-sections -mno-sdata \
				   -fno-builtin-sqrt -fno-builtin-sqrtf
export ASFLAGS   = $(DEFINES) -Wa,-mregnames -Wa,-mgekko
export CXXFLAGS  = $(CFLAGS) -std=c++2b -fconcepts -fno-rtti -fno-exceptions
export INCLUDE  := -I$(SOURCES) -I$(abspath src/mod/$(SOURCES)) -I$(DEVKITPATH)/libogc/include

.PHONY: all
all: loader mod

.PHONY: loader
loader: $(MELEELD) | clean_tmp
	+@cd src/loader && $(MAKE)

.PHONY: mod
mod: $(MELEELD)
	+@cd src/mod && $(MAKE)

.PHONY: dol
dol: export OBJDIR  := obj/DOL
dol: export DEPDIR  := dep/DOL
dol: export DEFINES += -DDOL -DNOPAL
dol: $(MELEELD)
	@[[ $(VERSION) == 102 ]] || ( echo "DOL builds are supported only for NTSC 1.02." >& 2; exit 1 )
	+@cd src/mod && $(MAKE) dol

.PHONY: resources
resources:
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