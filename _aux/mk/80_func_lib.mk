define PREPARE_VARIABLES
	DIR_SRC_$(1) := $(2)
	DIR_$(1) := $(1)
ifeq ($$(CURDIR),$$(DIR_BUILD))
	SONAME_$(1) := $$(LIB_PREFIX)$(3)$(1)$$(LIB_INFIX).so.$$(VERSION).$$(SUBVERSION)
	TARGET_$(1) := $$(LIB_PREFIX)$(3)$(1)$$(LIB_INFIX).$$(LIB_SUFFIX)
	TARGETS += $$(TARGET_$(1))
	CXXFLAGS_$(1) := -D__$$(call TO_UPPER,$(3))$$(call TO_UPPER,$(1))_BUILD__
	HDR_$(1) += $$(strip $$(sort $$(shell cd $$(DIR_ROOT) && $$(FIND) ./$$(DIR_SRC_$(1)) -name "*.$$(HS)")))
	HDRS += $$(HDR_$(1))
	SRC_$(1) = $$(strip $$(sort $$(shell cd $$(DIR_ROOT) && $$(FIND) ./$$(DIR_SRC_$(1)) -name "*.$$(SS)")))
	SRCS += $$(SRC_$(1))
	DIRS += $$(abspath $$(addprefix $$(DIR_BUILD)/,$$(sort $$(dir $$(SRC_$(1)) $$(HDR_$(1))))))
	OBJS_$(1) := $$(patsubst ./$$(DIR_SRC_$(1))/%,./$$(DIR_$(1))/%,$$(patsubst %.$$(SS),%.$$(OS),$$(SRC_$(1))))
	OBJS += $$(OBJS_$(1))
	TOTAL:=$$(words $$(SRCS))
endif

# implict pattern rule

# $$(DIR_$(1)/)... prefix is required for other (non-yaal) libraries
# that could be clients to this (yaal) build system.

$$(DIR_$(1))/%.$$(DS): $$(DIR_SRC_$(1))/%.$$(SS)
	@$$(call msg_always,printf "%b" "$$($$(CURR_DEP_PROGRESS_INDICATOR))$$(DEP_PROGRESS_INDICATOR_SUFFIX)$$(eval $$(call PROGRESS_INDICATOR))" && ) \
	/bin/rm -f "$$(@)" && \
	$$(DXX) $$(CXXFLAGS) $$(CXXFLAGS_$(1)) -MM $$(<) -MT $$(@:.$$(DS)=.$$(OS)) -MT $$(@) -MF $$(@) \
	$$(call msg,&& printf "%b" "$$(DEP_CL)")

$$(DIR_$(1))/%.$$(OS): $$(DIR_SRC_$(1))/%.$$(SS)
	@$$(eval $$(call IDENT_HELPER,$$(<))) $$(call msg,printf "[%3d%%] %b" "$$(shell expr $$(words $$(CUR_COUNT)) \* 100 / $$(TOTAL))" "$$($$(CURR_PROGRESS_INDICATOR))$$(eval $$(call PROGRESS_INDICATOR))Compiling \`$$(subst $$(DIR_ROOT)/,,$$(<))' ... " && ) \
	/bin/rm -f "$$(@)" && \
	$$(call invoke,$$(CXX) $$(CXXFLAGS) $$(CXXFLAGS_$(1)) -D__ID__=$$(ID) -D__TID__=$$(TID) $$(<) -c -o $$(@) 2>&1 | tee -a make.log) && \
	test -f $$(@) \
	$$(call msg,&& printf "%b$$(NL)" "done.$$(CL)")

endef

define CONFIGURE_DRIVER
	SONAME_$(1)_driver := $$(LIB_PREFIX)$(2)$(1)_driver$$(LIB_INFIX).so.$$(VERSION).$$(SUBVERSION)
	TARGET_$(1)_driver := $$(LIB_PREFIX)$(2)$(1)_driver$$(LIB_INFIX).$$(LIB_SUFFIX)
	TARGETS += $$(TARGET_$(1)_driver)
	SRCS += $$(patsubst %,$$(DIR_ROOT)/$$(DIR_dbwrapper)/%.$$(SS),$(1)_driver)
	OBJS_$(1)_driver := ./$$(patsubst %,$$(DIR_dbwrapper)/%.$$(OS), $(1)_driver)
	OBJS += $$(OBJS_$(1)_driver)
	TOTAL:=$$(words $$(SRCS))
endef

define MAKE_DIR
$(1):
	@umask 022;$$(MKDIR_P) $(1)
endef

