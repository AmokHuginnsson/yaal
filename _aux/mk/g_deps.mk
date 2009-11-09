define DIR_GEN
MODULE_DEPS=$$(filter ./$(1)/%,$$(OBJS))
$$(firstword $$(MODULE_DEPS:.$$(OS)=.$$(DS))) $$(lastword $$(MODULE_DEPS:.$$(OS)=.$$(DS))): $$(filter %/$(1),$$(DIRS))
endef

ifeq ($(strip $(foreach GOAL,clean mrproper stats tags doc,$(findstring $(GOAL),$(MAKECMDGOALS)))),)
ifeq ($(CURDIR),$(DIR_BUILD))

DIR_MODULES=$(strip $(sort $(dir $(OBJS))))
$(foreach D,$(DIR_MODULES),$(eval $(call DIR_GEN,$(subst /,,$(subst ./,,$(D))))))

-include $(OBJS:.$(OS)=.$(DS))
-include $(AOBJS:.$(AOS)=.$(ADS))
endif
endif

# vim: ft=make
