SHELL     = /bin/sh
RELEASE   = $(VERSION).$(SUBVERSION).$(EXTRAVERSION)
DIR_PKGCONFIG= $(DIR_SHARE)/pkgconfig
DIR_DOC      = $(DIR_SHARE)/doc/$(PRJNAME)
DIR_BUILD ?= $(CURDIR)
FIND      = find
ARFLAGS   = cr
VPATH     = $(DIR_ROOT)
ifeq ($(LIB_PREFIX),lib)
LIB_SUFFIX   = so.$(RELEASE)
LIB_ARCHIVE_SUFFIX = a
else
LIB_SUFFIX   = dll
LIB_ARCHIVE_SUFFIX = dll.a
endif

define IDENT_HELPER
	DIR_PREFIX=$$(subst . /,./,$$(foreach IT,$$(subst /, ,$$(subst $$(DIR_ROOT),,$$(DIR_BUILD))),/..))
	HEADER=$$(addsuffix .hxx,$$(basename $(1)))
	ID="\"$(1) $$(shell $$(GITID) ./$$(DIR_PREFIX)/$$(subst $$(DIR_ROOT)/,,$(1)))\""
	TID="\"$$(HEADER) $$(if $$(wildcard $$(HEADER)),$$(shell $$(GITID) ./$$(DIR_PREFIX)/$$(subst $$(DIR_ROOT)/,,$$(HEADER))),)\""
endef

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
	SRC_TARGETS := $$(SRC_TARGETS) $(2)
endef

COMMA=,
quotecomma=$(subst $(COMMA),$$(COMMA),$(subst -EOC,,$(subst $(COMMA)-EOC,,$(subst $(COMMA)$(COMMA),,$(1),$(2),$(3),$(4),$(5),$(6),$(7),$(8),$(9),$(10),$(11),$(12),$(13),$(14),$(15),-EOC))))

