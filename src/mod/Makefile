ELFFILE     := bin/mod.elf
BINFILE     := bin/mod.bin

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

LINKSCRIPT := mod.ld

$(BINFILE): $(ELFFILE)
	@[ -d $(@D) ] || mkdir -p $(@D)
	powerpc-eabi-objcopy -O binary $< $@

$(ELFFILE): $(OBJFILES) $(LINKSCRIPT) $(MELEELD) | resources clean_unused
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(CC) $(LDFLAGS) -T$(LINKSCRIPT) -T$(MELEELD) $(OBJFILES) -o $@
	
RESOURCE_DIR_IN  := resources
RESOURCE_DIR_OUT := $(SOURCES)/resources
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
$(call get_resource_out, $1): $1 $(TOOLS)/compress_resource.py 
	cp $$< $$@
	python $(TOOLS)/compress_resource.py $$@ $$@
endef

define make_texture_rule
$(call get_texture_out, $1): $1 $(TOOLS)/compress_resource.py $(TOOLS)/encode_texture.py
	python $(TOOLS)/encode_texture.py $$< $$@
	python $(TOOLS)/compress_resource.py $$@ $$@
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
	rm -rf $(OBJDIR) $(DEPDIR) $(ELFFILE) $(BINFILE) $(RESOURCE_DIR_OUT)

# Remove unused obj/dep/resource files
.PHONY: clean_unused
clean_unused:
	$(foreach file, $(shell find $(OBJDIR) -type f), $(if $(filter $(file), $(OBJFILES)),, $(shell rm $(file))))
	$(foreach file, $(shell find $(DEPDIR) -type f), $(if $(filter $(file), $(DEPFILES)),, $(shell rm $(file))))
	$(foreach file, $(shell find $(RESOURCE_DIR_OUT) -type f), \
		$(if $(filter $(file), $(RESOURCES_OUT) $(RESOURCES_H_OUT) $(TEXTURES_OUT) $(TEXTURES_H_OUT)),, \
		$(shell rm $(file))))

-include $(DEPFILES)