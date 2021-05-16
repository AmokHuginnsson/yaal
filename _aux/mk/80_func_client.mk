print-%:
	@$(error $* is $($*) ($(value $*)) (from $(origin $*)))

define PREPARE_TARGET
DIRS := $$(DIRS) $$(DIR_BUILD)/$$($(1))
REAL_TARGETS := $$(REAL_TARGETS) $$(DIR_BUILD)/$$($(1))/$$(EXEC_NAME)$$(STATIC_SUFFIX)
endef

define BUILD_TARGET

REAL_TARGET := $$(DIR_BUILD)/$$($(1))/$$(EXEC_NAME)$$(STATIC_SUFFIX)
REAL_DIR_$(1) = $$(if $$(realpath $$(DIR_ROOT)/$$(SRC_$(1))),$$(SRC_$(1))/,$$(dir $$(SRC_$(1))))

ifeq ($$(CURDIR),$$(DIR_BUILD))

HDRS_$(1) = $$(strip $$(sort \
	$$(if $$(realpath $$(DIR_ROOT)/$$(SRC_$(1))),\
	$$(shell cd $$(DIR_ROOT) && $$(FIND) -H ./$$(SRC_$(1)) -name '*.$$(HS)'),\
	$$(subst $$(DIR_ROOT),.,$$(wildcard $$(DIR_ROOT)/$$(SRC_$(1))*.$$(HS))) )))
SRCS_$(1) = $$(strip $$(sort \
	$$(if $$(realpath $$(DIR_ROOT)/$$(SRC_$(1))),\
	$$(shell cd $$(DIR_ROOT) && $$(FIND) -H ./$$(SRC_$(1)) -name '*.$$(SS)'),\
	$$(subst $$(DIR_ROOT),.,$$(wildcard $$(DIR_ROOT)/$$(SRC_$(1))*.$$(SS))) )))
OBJS_$(1) = $$(subst ./$$(REAL_DIR_$(1)),./$$($(1))/,$$(patsubst %.$$(SS),%.$$(OS),$$(SRCS_$(1))))
OBJS := $$(OBJS) $$(OBJS_$(1))
TOTAL := $$(words $$(OBJS))

endif

# implict pattern rule

$$($(1))/%.$$(DS): $$(REAL_DIR_$(1))%.$$(SS)
	@$$(call msg,printf "%b" "$$($$(CURR_DEP_PROGRESS_INDICATOR))$$(DEP_PROGRESS_INDICATOR_SUFFIX)$$(eval $$(call PROGRESS_INDICATOR))" && ) \
	/bin/rm -f "$$(@)"; \
	$$(DXX) $$(CXXFLAGS) $$(COMPILER_FLAGS_$(1)) -MM $$(<) -MT $$(@:.$$(DS)=.$$(OS)) -MT $$(@) -MF $$(@) \
	$$(call msg,&& printf "%b" "$$(DEP_CL)")

$$($(1))/%.$$(OS): $$(REAL_DIR_$(1))%.$$(SS)
	@$$(call msg_always,printf "[%3d%%] %b" "$$(shell expr $$(words $$(CUR_COUNT)) \* 100 / $$(TOTAL))" "$$($$(CURR_PROGRESS_INDICATOR))$$(eval $$(call PROGRESS_INDICATOR))Compiling \`$$(subst $$(DIR_ROOT)/,,$$(<))' ... " && ) \
	/bin/rm -f "$$(@)" && \
	$$(call invoke,$$(CXX) $$(CXXFLAGS) $$(COMPILER_FLAGS_$(1)) -D__ID__="\"$$(<) $$(shell $$(GITID) ./$$(subst . /,./,$$(foreach IT,$$(subst /, ,$$(subst $$(DIR_ROOT),,$$(DIR_BUILD))),/..))/$$(subst $$(DIR_ROOT)/,,$$(<)))\"" $$(<) -c -o $$(@) 2>&1 | tee -a make.log ) && \
	test -f $$(@) \
	$$(call msg,&& printf "%b$$(NL)" "done.$$(CL)")

$$(REAL_TARGET): $$(OBJS_$(1)) $$(EXTRA_DEPS_$(1))
	@$$(call msg,printf "%b" "Linking \`$$(@)' ... " && ) \
	/bin/rm -f "$$(@)"; \
	$$(call invoke,$$(LXX) $$(LXXFLAGS) $$(LXXFLAGS_$(1)) -o $$(@) $$(START_GROUP) $$(OBJS_$(1)) $$(END_GROUP) $$(if $$(STATIC),$$(STATICLIBS),$$(EXTRA_LIBS)) $$(LIBS) $$(LIBS_$(1)) 2>&1 | tee -a make.log )
ifeq ($$(READELF),readelf)
ifneq ($$(findstring $$(LIB_INFIX),-d -rd),)
	@$$(call invoke, $$(GDB_ADD_INDEX) $$(@))
endif
endif
ifdef DO_RELEASE
	@$$(call invoke,strip $$(REAL_TARGET))
endif
	@test -f $$(@) \
	$$(call msg,&& printf "%b$$(NL)" "done.$$(CL)")

endef

define MAKE_DIR
$(1):
	@umask 022;$$(MKDIR_P) $(1)
endef

