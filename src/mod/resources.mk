RESOURCES        := $(foreach dir, $(RESOURCE_DIR_IN), $(shell find $(dir) -type f))
RESOURCES        := $(filter-out %.docx, $(filter-out %.psd, $(RESOURCES)))
TEXTURES         := $(filter     %.png,  $(RESOURCES))
BIN_RESOURCES    := $(filter-out %.png,  $(RESOURCES))

define get_resource_out
$(shell printf "%s\n" $1 \
	| sed -r "s/(^|.*[/\\])resources[/\\](.*)/\\2/" \
	| awk '{print "$(RESOURCE_DIR_OUT)/" $$0}')
endef

define get_texture_out
$(shell printf "%s\n" $1 \
	| sed -r "s/(^|.*[/\\])resources[/\\]((.*)(\\.[^./\]*)|(.*))\\.png$$/\\3\\5.tex/" \
	| awk '{print "$(RESOURCE_DIR_OUT)/" $$0}')
endef

define make_bin_resource_rule
in_path  := $(word $1, $(BIN_RESOURCES))
out_path := $(word $1, $(BIN_RESOURCES_OUT))
$$(out_path): $$(in_path)
	@mkdir -p $$(dir $$@)
	cp $$< $$@
endef

define make_texture_rule
in_path  := $(word $1, $(TEXTURES))
out_path := $(word $1, $(TEXTURES_OUT))
$$(out_path): $$(in_path) $(TOOLS)/compress_resource.py $(TOOLS)/encode_texture.py
	python $(TOOLS)/encode_texture.py    $$< $$@
	python $(TOOLS)/compress_resource.py $$@ $$@
endef

BIN_RESOURCES_OUT := $(call get_resource_out, $(BIN_RESOURCES))
TEXTURES_OUT      := $(call get_texture_out,  $(TEXTURES))
$(foreach i, $(shell seq 1 $(words $(BIN_RESOURCES))), $(eval $(call make_bin_resource_rule, $(i))))
$(foreach i, $(shell seq 1 $(words $(TEXTURES))),      $(eval $(call make_texture_rule,      $(i))))

RESOURCES_OUT    := $(BIN_RESOURCES_OUT) $(TEXTURES_OUT)
RESOURCE_HEADERS := $(RESOURCES_OUT:%=%.h)

$(RESOURCE_DIR_OUT)/%.h: $(RESOURCE_DIR_OUT)/% $(TOOLS)/bin_to_header.py
	python $(TOOLS)/bin_to_header.py $< $@

.PHONY: resources
resources: $(RESOURCE_HEADERS)