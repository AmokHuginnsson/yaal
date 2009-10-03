ifeq ($(CURDIR),$(DIR_BUILD))
$(firstword $(OBJS:.$(OS)=.$(DS))) $(lastword $(OBJS:.$(OS)=.$(DS))): $(DIRS) $(DIR_BUILD)/version.hxx

-include $(OBJS:.$(OS)=.$(DS))
-include $(AOBJS:.$(AOS)=.$(ADS))
endif

# vim: ft=make
