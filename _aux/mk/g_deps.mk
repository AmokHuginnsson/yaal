ifeq ($(strip $(foreach GOAL,clean mrproper stats tags doc,$(findstring $(GOAL),$(MAKECMDGOALS)))),)
ifeq ($(CURDIR),$(DIR_BUILD))
$(firstword $(OBJS:.$(OS)=.$(DS))) $(lastword $(OBJS:.$(OS)=.$(DS))): $(DIRS)

-include $(OBJS:.$(OS)=.$(DS))
-include $(AOBJS:.$(AOS)=.$(ADS))
endif
endif

# vim: ft=make
