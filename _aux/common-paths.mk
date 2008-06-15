VPATH					= $(DIR_ROOT)/src
DIR_DEST 			=
prefix				= $(DIR_DEST)/usr/bin
doc						= $(DIR_DEST)/usr/share/doc

# this neat trick does not really blong here
# but we want client makefiles look really simple

TARGET_NO = 0
define NEW_TARGET
	TARGET_$$(TARGET_NO) := $(1)
	SRC_TARGET_$$(TARGET_NO) := $(2)
	COMPILER_FLAGS_TARGET_$$(TARGET_NO) := $(3)
	LINKER_FLAGS_TARGET_$$(TARGET_NO) := $(4)
	TARGETS := $$(TARGETS) TARGET_$$(TARGET_NO)
	TARGET_NO := $$(TARGET_NO)0
endef

