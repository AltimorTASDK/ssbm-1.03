ifeq ($(VERSION), 100)
ELFFILE := $(LOCALBIN)/mod-ntsc100.elf
BINFILE := $(LOCALBIN)/mod-ntsc100.bin
else ifeq ($(VERSION), 101)
ELFFILE := $(LOCALBIN)/mod-ntsc101.elf
BINFILE := $(LOCALBIN)/mod-ntsc101.bin
else ifeq ($(VERSION), 102)
ELFFILE := $(LOCALBIN)/mod.elf
BINFILE := $(LOCALBIN)/mod.bin
else ifeq ($(VERSION), PAL)
ELFFILE := $(LOCALBIN)/mod-pal.elf
BINFILE := $(LOCALBIN)/mod-pal.bin
endif

ALLBINS := $(LOCALBIN)/mod.bin $(LOCALBIN)/mod-ntsc100.bin.diff $(LOCALBIN)/mod-ntsc101.bin.diff
ifndef NOPAL
ALLBINS += $(LOCALBIN)/mod-pal.bin.diff
endif

GCINAME   := 103Code
GCIFILE   := $(BINDIR)/$(MODNAME).gci
BANNERSRC := banner.rgb5a3.png
BANNER    := build/banner.tex
ICON      := icon.tex
TITLE     := "Super Smash Bros. Melee         1.03 Data"
DIFFSRC   := $(LOCALBIN)/mod.bin

DOLFILE   := $(ISODIR)/sys/main.dol
PATCHES   := $(LOCALBIN)/patches.bin
DOLELF    := $(LOCALBIN)/dol.elf
DOLDATA   := build/dol_data.bin
DOLLD     := dol.ld

CFILES   := $(foreach dir, $(SRCDIR), $(shell find $(dir) -type f -name '*.c'   2> /dev/null))
CXXFILES := $(foreach dir, $(SRCDIR), $(shell find $(dir) -type f -name '*.cpp' 2> /dev/null))
SFILES   := $(foreach dir, $(SRCDIR), $(shell find $(dir) -type f -name '*.S'   2> /dev/null))

OBJFILES := \
    $(patsubst %, $(OBJDIR)/%.o, $(CFILES)) \
    $(patsubst %, $(OBJDIR)/%.o, $(CXXFILES)) \
    $(patsubst %, $(OBJDIR)/%.o, $(SFILES))

ifdef VERSION_A
OBJFILES := $(filter $(OBJDIR)/src/controller_fix/%.o, $(OBJFILES)) \
            $(filter $(OBJDIR)/src/latency/%.o,        $(OBJFILES)) \
            $(filter $(OBJDIR)/src/compat/%.o,         $(OBJFILES)) \
            $(filter $(OBJDIR)/src/version/%.o,        $(OBJFILES)) \
            $(filter $(OBJDIR)/src/stages/%.o,         $(OBJFILES)) \
            $(OBJDIR)/src/dol_init.cpp.o \
            $(OBJDIR)/src/init.S.o \
            $(OBJDIR)/src/controls/z_jump.cpp.o \
            $(OBJDIR)/src/libcxx/alloc.cpp.o \
            $(OBJDIR)/src/menu/character_select.cpp.o \
            $(OBJDIR)/src/menu/debug_menu.cpp.o \
            $(OBJDIR)/src/menu/stage_select.cpp.o \
            $(OBJDIR)/src/rules/decide_match.cpp.o \
            $(OBJDIR)/src/rules/settings_lock.cpp.o \
            $(OBJDIR)/src/rules/values.cpp.o \
            $(OBJDIR)/src/util/draw/render.cpp.o \
            $(OBJDIR)/src/util/melee/pad.cpp.o \
            $(OBJDIR)/src/util/melee/pad.S.o \
            $(OBJDIR)/src/util/gc/memcard.cpp.o \
            $(OBJDIR)/src/qol/save_data.cpp.o \
            $(OBJDIR)/src/qol/widescreen.cpp.o \
            $(OBJDIR)/src/qol/lras_macro.cpp.o \
            $(OBJDIR)/src/qol/match_results.cpp.o \
            $(OBJDIR)/src/qol/nametags.cpp.o \
            $(OBJDIR)/src/qol/nametags.S.o \
            $(OBJDIR)/src/qol/player_hud.cpp.o \
            $(OBJDIR)/src/qol/spawns.cpp.o \
            $(OBJDIR)/src/util/melee/ports.cpp.o
else ifdef STEALTH
OBJFILES := $(filter $(OBJDIR)/src/controller_fix/%.o, $(OBJFILES)) \
            $(filter $(OBJDIR)/src/latency/%.o,        $(OBJFILES)) \
            $(filter $(OBJDIR)/src/compat/%.o,         $(OBJFILES)) \
            $(filter $(OBJDIR)/src/version/%.o,        $(OBJFILES)) \
            $(OBJDIR)/src/dol_init.cpp.o \
            $(OBJDIR)/src/init.S.o \
            $(OBJDIR)/src/controls/z_jump.cpp.o \
            $(OBJDIR)/src/libcxx/alloc.cpp.o \
            $(OBJDIR)/src/menu/character_select.cpp.o \
            $(OBJDIR)/src/menu/debug_menu.cpp.o \
            $(OBJDIR)/src/rules/settings_lock.cpp.o \
            $(OBJDIR)/src/rules/values.cpp.o \
            $(OBJDIR)/src/util/draw/render.cpp.o \
            $(OBJDIR)/src/util/melee/pad.cpp.o \
            $(OBJDIR)/src/util/melee/pad.S.o \
            $(OBJDIR)/src/util/gc/memcard.cpp.o \
            $(OBJDIR)/src/qol/save_data.cpp.o \
            $(OBJDIR)/src/qol/widescreen.cpp.o
endif

DEPFILES := $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $(OBJFILES))

ifdef NOPAL
LINKSCRIPT := mod-a.ld
COMMONLD   := ../common-a.ld
else
LINKSCRIPT := mod-b.ld
COMMONLD   := ../common-b.ld
endif

CFLAGS    += -flto -fuse-linker-plugin
CXXFLAGS  += -flto -fuse-linker-plugin
LDFLAGS   += -flto

# Update build timestamp
$(shell touch src/timestamp.cpp)

.PHONY: gci
gci: $(GCIFILE)

ifeq ($(VERSION), 102)
$(GCIFILE): $(BINFILE)
else
$(GCIFILE): $(BINFILE).diff
endif
$(GCIFILE): $(wildcard $(ALLBINS)) $(BANNER) $(ICON) $(TOOLS)/bin_to_gci.py
	@[ -d $(@D) ] || mkdir -p $(@D)
ifdef NOPAL
	python $(TOOLS)/bin_to_gci.py $@ $(GCINAME) $(TITLE) $(BANNER) $(ICON) --compress $(ALLBINS)
else
	python $(TOOLS)/bin_to_gci.py $@ $(GCINAME) $(TITLE) $(BANNER) $(ICON) $(ALLBINS)
endif
	@rm -rf $(GCIDIR)/tmp_*

$(BANNER): $(BANNERSRC) $(TOOLS)/encode_texture.py
	python $(TOOLS)/encode_texture.py $< $@

ifneq ($(VERSION), 102)
.PHONY: diff
diff: $(BINFILE).diff

$(BINFILE).diff: $(BINFILE) $(TOOLS)/diff.py
	@[ -d $(@D) ] || mkdir -p $(@D)
	python $(TOOLS)/diff.py $(DIFFSRC) $< $@
endif

.PHONY: bin
bin: $(BINFILE)

$(BINFILE): $(ELFFILE)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(OBJCOPY) -O binary $< $@
ifeq ($(VERSION), 102)
	@echo "diffs are no longer valid"
	rm -f $(LOCALBIN)/mod-ntsc100.bin.diff
	rm -f $(LOCALBIN)/mod-ntsc101.bin.diff
	rm -f $(LOCALBIN)/mod-pal.bin.diff
endif

$(ELFFILE): $(OBJFILES) $(LINKSCRIPT) $(MELEELD) $(COMMONLD) | clean-unused
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(CC) $(LDFLAGS) -T$(LINKSCRIPT) -T$(MELEELD) $(OBJFILES) -o $@

.PHONY: dol
dol: $(DOLFILE)

$(DOLFILE): $(DOLELF) $(PATCHES) $(TOOLS)/patch_dol.py
	@[ -d $(@D) ] || mkdir -p $(@D)
	@[ $(DOLSRC) ] || ( echo "\$$DOLSRC must be set to the path of an NTSC 1.02 main.dol file." >& 2; exit 1 )
	$(OBJCOPY) -O binary -R .patches $< $@
	python $(TOOLS)/patch_dol.py $@ $@ $(OUTPUTMAP) $(PATCHES)

$(PATCHES): $(DOLELF)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(OBJCOPY) -O binary -j .patches $< $@

$(DOLELF): $(OBJFILES) $(DOLDATA) $(DOLLD) $(MELEELD) $(COMMONLD) | clean-unused
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(CC) $(LDFLAGS) -T$(DOLLD) -T$(MELEELD) $(OBJFILES) -o $@

$(DOLDATA): $(DOLSRC)
	@[ -d $(@D) ] || mkdir -p $(@D)
# skip dol header
	dd if=$(DOLSRC) of=$(DOLDATA) bs=256 skip=1

$(OBJDIR)/%.c.o: %.c | resources
	@[ -d $(@D) ] || mkdir -p $(@D)
	@[ -d $(subst $(OBJDIR), $(DEPDIR), $(@D)) ] || mkdir -p $(subst $(OBJDIR), $(DEPDIR), $(@D))
	$(CC) -MMD -MP -MF $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $@) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(OBJDIR)/%.cpp.o: %.cpp | resources
	@[ -d $(@D) ] || mkdir -p $(@D)
	@[ -d $(subst $(OBJDIR), $(DEPDIR), $(@D)) ] || mkdir -p $(subst $(OBJDIR), $(DEPDIR), $(@D))
	$(CXX) -MMD -MP -MF $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $@) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(OBJDIR)/%.S.o: %.S
	@[ -d $(@D) ] || mkdir -p $(@D)
	@[ -d $(subst $(OBJDIR), $(DEPDIR), $(@D)) ] || mkdir -p $(subst $(OBJDIR), $(DEPDIR), $(@D))
	$(CC) $(ASFLAGS) -c $< -o $@

RESOURCE_DIR_IN  := resources
RESOURCE_DIR_OUT := $(GENDIR)/resources
-include resources.mk

.PHONY: clean
clean:
	rm -rf build

# Remove unused obj/dep/gen files
.PHONY: clean-unused
clean-unused:
	$(foreach file, $(shell find $(OBJDIR) -type f 2> /dev/null), \
		$(if $(filter $(file), $(OBJFILES) $(OUTPUTMAP)),, \
		rm $(file);))
	$(foreach file, $(shell find $(DEPDIR) -type f 2> /dev/null), \
		$(if $(filter $(file), $(DEPFILES)),, \
		rm $(file);))
	$(foreach file, $(shell find $(GENDIR) -type f 2> /dev/null), \
		$(if $(filter $(file), $(RESOURCES_OUT) $(RESOURCE_HEADERS)),, \
		rm $(file);))

-include $(DEPFILES)