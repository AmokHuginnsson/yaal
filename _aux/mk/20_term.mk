ifeq ($(VERBOSE),yes)
invoke = printf "%b$(NL)" '$(subst -EOC,,$(subst $(,)-EOC,,$(subst $(,)$(,),,$(1),$(2),$(3),$(4),$(5),$(6),$(7),$(8),$(9),$(10),$(11),$(12),$(13),$(14),$(15),-EOC)))' && $(subst -EOC,,$(subst $(,)-EOC,,$(subst $(,)$(,),,$(1),$(2),$(3),$(4),$(5),$(6),$(7),$(8),$(9),$(10),$(11),$(12),$(13),$(14),$(15),-EOC)))
msg = # Meaningful trailing whitespace(s).
else
invoke = $(subst -EOC,,$(subst $(,)-EOC,,$(subst $(,)$(,),,$(1),$(2),$(3),$(4),$(5),$(6),$(7),$(8),$(9),$(10),$(11),$(12),$(13),$(14),$(15),-EOC)))
msg = $(subst -EOC,,$(subst $(,)-EOC,,$(subst $(,)$(,),,$(1),$(2),$(3),$(4),$(5),$(6),$(7),$(8),$(9),$(10),$(11),$(12),$(13),$(14),$(15),-EOC)))
endif
msg_always = $(subst -EOC,,$(subst $(,)-EOC,,$(subst $(,)$(,),,$(1),$(2),$(3),$(4),$(5),$(6),$(7),$(8),$(9),$(10),$(11),$(12),$(13),$(14),$(15),-EOC)))

ifeq ($(TERMINAL),TERM)

BOLD := $(shell tput bold || tput md)
RED  := $(shell tput setaf 1 || tput AF 1)
RS   := $(shell tput sgr0 || tput me)
ifeq ($(VERBOSE),yes)
	NL=\n
else
	NL=
	CL   := $(shell tput cr;tput dl1)
endif

PROGRESS_INDICATOR_0=\\\ # Meaningful trailing whitespace(s).
PROGRESS_INDICATOR_1=| #
PROGRESS_INDICATOR_2=/ #
PROGRESS_INDICATOR_3=- #
NEXT_PROGRESS_INDICATOR_ONCE=PROGRESS_INDICATOR_0
NEXT_PROGRESS_INDICATOR_0=PROGRESS_INDICATOR_1
NEXT_PROGRESS_INDICATOR_1=PROGRESS_INDICATOR_2
NEXT_PROGRESS_INDICATOR_2=PROGRESS_INDICATOR_3
NEXT_PROGRESS_INDICATOR_3=PROGRESS_INDICATOR_0
CURR_PROGRESS_INDICATOR=PROGRESS_INDICATOR_0
CURR_DEP_PROGRESS_INDICATOR=PROGRESS_INDICATOR_0

ifneq ($(VERBOSE),yes)
DEP_PROGRESS_INDICATOR_SUFFIX= Generating dependencies ...
DEP_CL=$(CL)
else
PROGRESS_INDICATOR_0=.
NEXT_PROGRESS_INDICATOR_0=PROGRESS_INDICATOR_0
PROGRESS_INDICATOR_ONCE=Generating dependencies # Meaningful trailing whitespace(s).
CURR_DEP_PROGRESS_INDICATOR=PROGRESS_INDICATOR_ONCE
endif

define PROGRESS_INDICATOR
CUR_COUNT=$(CUR_COUNT) x
CURR_PROGRESS_INDICATOR:=$$(NEXT_$$(CURR_PROGRESS_INDICATOR))
CURR_DEP_PROGRESS_INDICATOR:=$$(NEXT_$$(CURR_DEP_PROGRESS_INDICATOR))
endef

else

NL=\n
PROGRESS_INDICATOR_0=.
PROGRESS_INDICATOR_ONCE=Generating dependencies # Meaningful trailing whitespace(s).
NEXT_PROGRESS_INDICATOR_ONCE=PROGRESS_INDICATOR_0
NEXT_PROGRESS_INDICATOR_0=PROGRESS_INDICATOR_0
CURR_PROGRESS_INDICATOR=
CURR_DEP_PROGRESS_INDICATOR=PROGRESS_INDICATOR_ONCE

define PROGRESS_INDICATOR
CUR_COUNT=$(CUR_COUNT) x
CURR_DEP_PROGRESS_INDICATOR:=$$(NEXT_$$(CURR_DEP_PROGRESS_INDICATOR))
CURR_PROGRESS_INDICATOR=
endef

endif

