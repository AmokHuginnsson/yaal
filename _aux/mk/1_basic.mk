SHELL     = /bin/sh
RELEASE   = $(VERSION).$(SUBVERSION).$(EXTRAVERSION)
DIR_BUILD ?= $(CURDIR)
FIND      = find
ARFLAGS   = r

COMA=,
quotecoma=$(subst $(COMA),$$(COMA),$(subst -EOC,,$(subst $(COMA)-EOC,,$(subst $(COMA)$(COMA),,$(1),$(2),$(3),$(4),$(5),$(6),$(7),$(8),$(9),$(10),$(11),$(12),$(13),$(14),$(15),-EOC))))

ifeq ($(VERBOSE),yes)
invoke = echo "$(subst -EOC,,$(subst $(COMA)-EOC,,$(subst $(COMA)$(COMA),,$(1),$(2),$(3),$(4),$(5),$(6),$(7),$(8),$(9),$(10),$(11),$(12),$(13),$(14),$(15),-EOC)))" && $(subst -EOC,,$(subst $(COMA)-EOC,,$(subst $(COMA)$(COMA),,$(1),$(2),$(3),$(4),$(5),$(6),$(7),$(8),$(9),$(10),$(11),$(12),$(13),$(14),$(15),-EOC)))
msg =
else
invoke = $(subst -EOC,,$(subst $(COMA)-EOC,,$(subst $(COMA)$(COMA),,$(1),$(2),$(3),$(4),$(5),$(6),$(7),$(8),$(9),$(10),$(11),$(12),$(13),$(14),$(15),-EOC)))
msg = $(subst -EOC,,$(subst $(COMA)-EOC,,$(subst $(COMA)$(COMA),,$(1),$(2),$(3),$(4),$(5),$(6),$(7),$(8),$(9),$(10),$(11),$(12),$(13),$(14),$(15),-EOC)))
endif

