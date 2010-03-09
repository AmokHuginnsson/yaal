SHELL     = /bin/sh
RELEASE   = $(VERSION).$(SUBVERSION).$(EXTRAVERSION)
DIR_BUILD ?= $(CURDIR)
FIND      = find
ARFLAGS   = cr
VPATH     = $(DIR_ROOT)

# this neat trick does not really blong here
# but we want client makefiles look really simple

define NEW_TARGET
	TARGET_$(1) := $(1)
	SRC_TARGET_$(1) := $(2)
	COMPILER_FLAGS_TARGET_$(1) := $(3)
	LXXFLAGS_TARGET_$(1) := $(4)
	LIBS_TARGET_$(1) := $(5)
	EXTRA_DEPS_TARGET_$(1) := $(6)
	TARGETS := $$(TARGETS) TARGET_$(1)
endef

COMA=,
quotecoma=$(subst $(COMA),$$(COMA),$(subst -EOC,,$(subst $(COMA)-EOC,,$(subst $(COMA)$(COMA),,$(1),$(2),$(3),$(4),$(5),$(6),$(7),$(8),$(9),$(10),$(11),$(12),$(13),$(14),$(15),-EOC))))

