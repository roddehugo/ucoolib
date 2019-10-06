# ucoolib - Microcontroller object oriented library.
#
# Build time configuration system.

PROJECT_CONFIG ?= Config
MODULES_CONFIG := $(wildcard $(ALL_UCOO_MODULES:%=$(UCOO_BASE)/%/Config)) \
	$(wildcard $(ALL_EXT_MODULES:%=$(BASE)/%/Config))

CONFIG_LIST := $(strip $(wildcard $(PROJECT_CONFIG)) $(MODULES_CONFIG))

# Ensure that configuration is up to date, using two mechanisms:
#  - make will make sure config.list is up to date as it is included.
#  - comparison with CONFIG_LIST ensures that the list of included
#  configuration (including the project configuration) is the same.
ifneq ($(MAKECMDGOALS),clean)
-include $(OBJDIR)/config.list
ifneq ($(CONFIG_LIST),$(CONFIG_LIST_OLD))
CONFIG_FORCE := CONFIG_FORCE
endif
endif

# This configuration is really needed, make sure to stop here if it fails.
COMPILE_ORDER_DEPS += $(OBJDIR)/config.list

clean: config-clean

.PHONY: config-clean CONFIG_FORCE

$(OBJDIR)/config.list: $(CONFIG_LIST) $(CONFIG_FORCE) | $(OBJDIR)
	@echo "CONF $(PROJECT_CONFIG)"
	$Q$(UCOO_BASE)/build/tools/config-gen -H $(OBJDIR)/config/%.hh \
		-p $(PROJECT_CONFIG) -T '$(TARGETS_SUBTARGETS)' \
		$(MODULES_CONFIG)
	$Qecho "CONFIG_LIST_OLD = $(CONFIG_LIST)" > $@

config-clean:
	rm -rf $(OBJDIR)/config $(OBJDIR)/config.list
