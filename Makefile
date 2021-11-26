DEVKITPATH=$(shell echo "$(DEVKITPRO)" | sed -e 's/^\([a-zA-Z]\):/\/\1/')
PATH := $(DEVKITPATH)/devkitPPC/bin:$(DEVKITPATH)/tools/bin:$(PATH)

export AS  := powerpc-eabi-as
export CC  := powerpc-eabi-gcc
export CXX := powerpc-eabi-g++
export LD  := powerpc-eabi-ld

export BINDIR  := $(abspath bin)
export TOOLS   := $(abspath tools)
export GCIDIR  := $(abspath gci)

export LDFLAGS := -Wl,-Map=output.map -Wl,--gc-sections

export DEFINES := -DGEKKO

ifeq ($(VERSION),)
$(error Specify Melee version with VERSION)
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

export MELEEMAP := $(MELEELD:.ld=.map)

export CFLAGS   := $(DEFINES) -mogc -mcpu=750 -meabi -mhard-float -Os \
				   -Wall -Wno-register -Wno-unused-value -Wconversion -Warith-conversion \
				   -ffunction-sections -fdata-sections -mno-sdata \
				   -fno-builtin-sqrt -fno-builtin-sqrtf
export ASFLAGS  := $(DEFINES) -Wa,-mregnames -Wa,-mgekko
export CXXFLAGS := $(CFLAGS) -std=c++2b -fconcepts -fno-rtti -fno-exceptions
export INCLUDE  := -Isrc -I$(abspath src/mod/src) -I$(DEVKITPATH)/libogc/include

.PHONY: all
all: loader mod

.PHONY: loader
loader: $(MELEELD)
	+@cd src/loader && $(MAKE)

.PHONY: mod
mod: $(MELEELD)
	+@cd src/mod && $(MAKE)

.PHONY: resources
resources: $(MELEELD)
	+@cd src/mod && $(MAKE) resources

$(MELEELD): $(MELEEMAP) $(TOOLS)/map_to_linker_script.py
	python $(TOOLS)/map_to_linker_script.py $(MELEEMAP) $(MELEELD)

.PHONY: clean
clean:
	rm -rf $(BINDIR)
	@cd src/loader && make clean
	@cd src/mod    && make clean