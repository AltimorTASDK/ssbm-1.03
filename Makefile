DEVKITPATH=$(shell echo "$(DEVKITPRO)" | sed -e 's/^\([a-zA-Z]\):/\/\1/')
PATH := $(DEVKITPATH)/devkitPPC/bin:$(DEVKITPATH)/tools/bin:$(PATH)

AS  := powerpc-eabi-as
CC  := powerpc-eabi-gcc
CXX := powerpc-eabi-g++
LD  := powerpc-eabi-ld

export BINDIR    := $(abspath bin)
export TOOLS     := $(abspath tools)
export GCIDIR    := $(abspath gci)

export LDFLAGS    := -Wl,-Map=output.map -Wl,--gc-sections -flto

export CFLAGS   := -DGEKKO -mogc -mcpu=750 -meabi -mhard-float -Os -Wall \
				   -Wno-register -Wno-unused-value -Wconversion -Warith-conversion \
				   -ffunction-sections -fdata-sections -flto -mno-sdata
export CXXFLAGS := $(CFLAGS) -std=c++2b -fconcepts -fno-rtti -fno-exceptions
export INCLUDE  := -Isrc -I$(DEVKITPATH)/libogc/include

export MELEELD  := $(abspath GALE01.ld)

.PHONY: all
all: $(MELEELD)
	@cd src/loader && make
	@cd src/mod    && make

$(MELEELD): GALE01.map $(TOOLS)/map_to_linker_script.py
	python $(TOOLS)/map_to_linker_script.py
	
.PHONY: clean
clean:
	rm -rf $(BINDIR)
	@cd src/loader && make clean
	@cd src/mod    && make clean