define DIR_GEN
MODULE_DEPS=$$(filter ./$(1)/%,$$(OBJS))
$$(firstword $$(MODULE_DEPS:.$$(OS)=.$$(DS))) $$(lastword $$(MODULE_DEPS:.$$(OS)=.$$(DS))): $$(DIR_BUILD)/dirs.d
endef

ifeq ($(strip $(foreach GOAL,clean mrproper purge-local stats tags doc,$(findstring $(GOAL),$(MAKECMDGOALS)))),)
ifeq ($(CURDIR),$(DIR_BUILD))

DIR_MODULES=$(patsubst %/,%,$(patsubst ./%,%,$(strip $(sort $(dir $(OBJS))))))
$(foreach D,$(DIR_MODULES),$(eval $(call DIR_GEN,$(D))))

$(DIR_BUILD)/dirs.d:
	@$(MKDIR_P) $(foreach D,$(DIR_MODULES),$(DIR_BUILD)/$(D)) && touch $(@)

-include $(OBJS:.$(OS)=.$(DS))
-include $(AOBJS:.$(AOS)=.$(ADS))
endif
endif

# vim: ft=make
