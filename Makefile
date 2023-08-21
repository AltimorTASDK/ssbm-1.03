MAKEFLAGS += --no-builtin-rules

ifndef DEVKITPRO
$(error Specify devkitPro install path with $$DEVKITPRO)
endif

ifndef VERSION
else ifeq ($(VERSION), 100)
else ifeq ($(VERSION), 101)
else ifeq ($(VERSION), 102)
else ifeq ($(VERSION), PAL)
ifdef NOPAL
$(error $$VERSION set to PAL, but $$NOPAL specified.)
endif
else
$(error Unsupported Melee version "$(VERSION)")
endif

DEVKITPATH=$(shell echo "$(DEVKITPRO)" | sed -e 's/^\([a-zA-Z]\):/\/\1/')
PATH := $(DEVKITPATH)/devkitPPC/bin:$(PATH)

export CC      := powerpc-eabi-gcc
export CXX     := powerpc-eabi-g++
export OBJCOPY := powerpc-eabi-objcopy

ifdef MODVERSION
ifneq ($(shell echo "$(MODVERSION)" | grep -P '(^|-)a($$|[\d-])'),)
export NOPAL := 1
endif
ifneq ($(shell echo "$(MODVERSION)" | grep -P '(^|-)(beta|rc)($$|[\d-])'),)
export BETA := 1
endif
ifeq ($(shell echo "$(MODVERSION)" | grep -P '(^|-)LE($$|[\d-])'),)
export TOURNAMENT := 1
endif
ifneq ($(shell echo "$(MODVERSION)" | grep -P '(^|-)SE($$|[\d-])'),)
export STEALTH := 1
endif
export MODNAME := ssbm-1.03-$(MODVERSION)
else
export MODNAME := ssbm-1.03
endif

export DEFINES  = $(foreach def, $(USERDEFS), -D$(def))
export DEFINES += -DGEKKO

ifdef NOPAL
export DEFINES += -DNOPAL
endif
ifdef BETA
export DEFINES += -DBETA
endif
ifdef TOURNAMENT
export DEFINES += -DTOURNAMENT
endif
ifdef STEALTH
export DEFINES += -DSTEALTH
endif

export DEFINES += -DMODNAME=\"$(MODNAME)\"

MELEELD_100 := $(abspath GALE01r0.ld)
DEFINES_100 := -DNTSC100
MELEELD_101 := $(abspath GALE01r1.ld)
DEFINES_101 := -DNTSC101
MELEELD_102 := $(abspath GALE01r2.ld)
DEFINES_102 := -DNTSC102
MELEELD_PAL := $(abspath GALP01.ld)
DEFINES_PAL := -DPAL

export MELEELD  = $(MELEELD_$(VERSION))
export DEFINES += $(DEFINES_$(VERSION))

ifdef STEALTH
export EDITION := SE
else ifdef TOURNAMENT
export EDITION := TE
else
export EDITION := LE
endif

ifdef NOPAL
export SUBDIR := a/$(EDITION)
else
export SUBDIR := b/$(EDITION)
endif

export TOOLS    := $(abspath tools)
export GCIDIR   := $(abspath gci)
export GENDIR   := build/gen
export LIBDIR   := lib
export SRCDIR   := src $(GENDIR)
export LOCALBIN := build/bin/$(SUBDIR)
export BINDIR   := $(abspath bin/$(SUBDIR))
export OBJDIR    = build/obj/$(SUBDIR)/$(VERSION)
export DEPDIR    = build/dep/$(SUBDIR)/$(VERSION)
export GCIBIN   := $(GCIDIR)/$(SUBDIR)/bin

export OUTPUTMAP = $(OBJDIR)/output.map
export LDFLAGS   = -Wl,-Map=$(OUTPUTMAP) -Wl,--gc-sections

export MELEEMAP  = $(MELEELD:.ld=.map)

export CFLAGS    = $(DEFINES) -mogc -mcpu=750 -meabi -mhard-float -Os \
				   -Wall -Wno-switch -Wno-unused-value -Wconversion -Warith-conversion -Wno-multichar \
				   -Wno-pointer-arith \
				   -ffunction-sections -fdata-sections -mno-sdata \
				   -fno-builtin-sqrt -fno-builtin-sqrtf
export ASFLAGS   = $(DEFINES) -Wa,-mregnames -Wa,-mgekko
export CXXFLAGS  = $(CFLAGS) -std=c++23 -fconcepts -fno-rtti -fno-exceptions
export INCLUDE  := $(foreach dir, $(SRCDIR), -I$(dir)) -I$(abspath src/mod/src) -I$(DEVKITPATH)/libogc/include

.PHONY: gci
gci: loader mod

ifndef VERSION

ALLVERSIONS := 100 101 102
ifndef NOPAL
ALLVERSIONS += PAL
endif

$(foreach version,$(ALLVERSIONS),$(eval %-$(version): export VERSION := $(version)))

# Make gcis for all versions
.PHONY: mod
mod: export VERSION := 102
mod: mod-bin-102 mod-diff-101 mod-diff-100
ifndef NOPAL
mod: mod-diff-PAL
endif
	+@cd src/mod && $(MAKE) gci

.PHONY: $(foreach version,$(ALLVERSIONS),mod-diff-$(version))
$(foreach version,$(ALLVERSIONS),$(eval mod-diff-$(version): mod-bin-$(version)))
$(foreach version,$(ALLVERSIONS),mod-diff-$(version)): mod-bin-102
	+@cd src/mod && $(MAKE) diff

.PHONY: $(foreach version,$(ALLVERSIONS),mod-bin-$(version))
$(foreach version,$(ALLVERSIONS),mod-bin-$(version)):
	+@cd src/mod && $(MAKE) bin

.PHONY: loader
loader: loader-NTSC
ifndef NOPAL
loader: loader-PAL
endif

.PHONY: loader-NTSC
loader-NTSC: export VERSION := 102
loader-NTSC: loader-bin-100 loader-bin-101 loader-bin-102
	+@cd src/loader && $(MAKE) gci

ifndef NOPAL
.PHONY: loader-PAL
loader-PAL: export VERSION := PAL
loader-PAL:
	+@cd src/loader && $(MAKE) gci
endif

.PHONY: $(foreach version,$(ALLVERSIONS),loader-bin-$(version))
$(foreach version,$(ALLVERSIONS),loader-bin-$(version)):
	+@cd src/loader && $(MAKE) bin

else

# Make gcis for specific version
.PHONY: loader
loader: $(MELEELD)
	+@cd src/loader && $(MAKE)

.PHONY: mod
mod: $(MELEELD)
	+@cd src/mod && $(MAKE)

endif

.PHONY: dol
dol: export OBJDIR  := build/obj/dol/$(EDITION)
dol: export DEPDIR  := build/dep/dol/$(EDITION)
dol: export ISODIR  := $(abspath iso/$(EDITION))
dol: export DEFINES += -DDOL -DNTSC102 -DNOPAL
dol: export MELEELD := $(abspath GALE01r2.ld)
dol: $(MELEELD)
	+@cd src/mod && $(MAKE) dol

.PHONY: resources
resources:
	+@cd src/mod && $(MAKE) resources

$(MELEELD): $(MELEEMAP) $(TOOLS)/map_to_linker_script.py
	python $(TOOLS)/map_to_linker_script.py $(MELEEMAP) $(MELEELD)

.PHONY: clean
clean:
	rm -rf $(BINDIR)
	@cd src/loader && $(MAKE) clean
	@cd src/mod    && $(MAKE) clean