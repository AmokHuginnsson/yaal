print-%:
	@$(error $* is $($*) ($(value $*)) (from $(origin $*)))

define PREPARE_TARGET
DIRS := $$(DIRS) $$(DIR_BUILD)/$$($(1))
REAL_TARGETS := $$(REAL_TARGETS) $$(DIR_BUILD)/$$($(1))/$$(EXEC_NAME)
endef

define BUILD_TARGET

REAL_TARGET := $$(DIR_BUILD)/$$($(1))/$$(EXEC_NAME)

ifeq ($(CURDIR),$(DIR_BUILD))
VPATH := $$(DIR_ROOT)

HDRS_$(1) = $$(strip $$(sort $$(shell cd $$(DIR_ROOT) && $$(FIND) -H ./$$(SRC_$(1)) -name '*.$(HS)')))
SRCS_$(1) = $$(strip $$(sort $$(shell cd $$(DIR_ROOT) && $$(FIND) -H ./$$(SRC_$(1)) -name '*.$(SS)')))
OBJS_$(1) = $$(subst ./$$(SRC_$(1))/,./$$($(1))/,$$(patsubst %.$(SS),%.$(OS),$$(SRCS_$(1))))
OBJS := $$(OBJS) $$(OBJS_$(1))

endif

# implict pattern rule

$$($(1))/%.$$(OS): $$(SRC_$(1))/%.$$(SS)
	@$$(call msg,echo -n "Compiling \`$$(subst $$(DIR_ROOT)/,,$$(<))' ... " && ) \
	/bin/rm -f "$$(@)"; \
	$$(DXX) $$(CXXFLAGS) $$(COMPILER_FLAGS_$(1)) -MM $$(<) -MT $$(@) -MT $$(@:.$$(OS)=.$$(DS)) -MF $$(@:.$$(OS)=.$$(DS)) && \
	$$(call invoke,$$(CXX) $$(CXXFLAGS) $$(COMPILER_FLAGS_$(1)) -D__ID__="\"$$(<) $$(shell $$(GITID) ../$$(subst $$(DIR_ROOT)/,,$$(<)))\"" $$(<) -c -o $$(@) 2>&1 | tee -a make.log ) && \
	test -f $$(@) \
	$$(call msg,&& echo $$(NONL) "done.$$(CL)")

$$(REAL_TARGET): $$(OBJS_$(1))
	@$$(call msg,echo -n "Linking \`$$(@)' ... " && ) \
	/bin/rm -f "$$(@)"; \
	$$(call invoke,$$(LXX) $$(LINKER_FLAGS) $$(LINKER_FLAGS_$(1)) -o $$(@) $$(OBJS_$(1)) $$(LIBS) $$(LIBS_$(1)) 2>&1 | tee -a make.log )
ifdef DO_RELEASE
	@strip $$(REAL_TARGET)
endif
	@test -f $$(@) \
	$$(call msg,&& echo $$(NONL) "done.$$(CL)")

endef

$(foreach IT,$(TARGETS),$(eval $(call PREPARE_TARGET,$(IT))))
