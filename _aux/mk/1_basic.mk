SHELL     = /bin/sh
RELEASE   = $(VERSION).$(SUBVERSION).$(EXTRAVERSION)
DIR_BUILD ?= $(CURDIR)
FIND      = find
ARFLAGS   = r

COMA=,
ifeq ($(VERBOSE),yes)
invoke = echo "$(1)$(2)$(3)$(4)$(5)$(6)$(7)$(8)$(9)$(10)" && $(1)$(2)$(3)$(4)$(5)$(6)$(7)$(8)$(9)$(10)
msg =
else
invoke = $(1)$(2)$(3)$(4)$(5)$(6)$(7)$(8)$(9)$(10)
msg = $(1)$(2)$(3)$(4)$(5)$(6)$(7)$(8)$(9)$(10)
endif

