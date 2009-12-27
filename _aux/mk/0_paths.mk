VPATH    = $(DIR_ROOT)
DIR_DEST =
prefix   = $(DIR_DEST)/usr/bin
doc      = $(DIR_DEST)/usr/share/doc

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

