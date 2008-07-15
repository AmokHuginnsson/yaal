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
USE_PCH_$(1) = -include ./$$(SRC_$(1))/header
PCH_$(1) = $$($$(COMPILER_PCH_FLAGS)_PCH_$(1))
OBJS := $$(OBJS) $$(OBJS_$(1))

endif

# implict pattern rule

$$($(1))/%.$$(OS): $$(SRC_$(1))/%.$$(SS)
	@echo -n "Compiling \`$$(subst $$(DIR_ROOT)/,,$$(<))' ... "; \
	echo -n "$$(@:.$$(OS)=.$$(DS)) " > $$(@:.$$(OS)=.$$(DS)); \
	/bin/rm -f "$$(@)"; \
	$$(DXX) $$(CXXFLAGS) $(COMPILER_FLAGS_$(1)) -MM $$(<) -MT $$(@) | grep -v '^#' >> $$(@:.$$(OS)=.$$(DS)) && test $$$${PIPESTATUS} == 0 && \
	$$(CXX) $$(CXXFLAGS) $(COMPILER_FLAGS_$(1)) $$(PCH_$(1)) $$(<) -c -o $$(@) 2>&1 | tee -a make.log && test $$$${PIPESTATUS} == 0 && \
	echo $$(NONL) "done.$$(CL)"

$$(REAL_TARGET): $$($(1))/header.gch $$(OBJS_$(1))
	@echo -n "Linking \`$$(@)' ... "
	@$(LXX) $(LINKER_FLAGS) $(LINKER_FLAGS_$(1)) -o $$(@) $$(OBJS_$(1)) $$(LIBS) 2>&1 | tee -a make.log && test $$$${PIPESTATUS} == 0
ifdef NODEBUG
	@strip $$(REAL_TARGET)
endif
	@echo $$(NONL) "done.$$(CL)"

$$($(1))/header.gch: $$(DIR_ROOT)/$$(SRC_$(1))/header $$(HDRS_$(1))
	@echo -n "Generating precompiled header \`$$(subst $$(DIR_ROOT)/,,$$(<))' ... "; \
	$$(CXX) -xc++-header $$(subst -include $$(SRC_$(1))/header,,$$(CXXFLAGS)) $$(<) -o $$(@) && \
	echo $$(NONL) "done.$$(CL)"

endef

$(foreach IT,$(TARGETS),$(eval $(call PREPARE_TARGET,$(IT))))
