# implict pattern rule

define IDENT_HELPER
	HEADER=$$(addsuffix .hxx,$$(basename $(1)))
	ID="\"$(1) $$(shell $$(GITID) ../$$(subst $$(DIR_ROOT)/,,$(1)))\""
	TID="\"$$(HEADER) $$(if $$(wildcard $$(HEADER)),$$(shell $$(GITID) ../$$(subst $$(DIR_ROOT)/,,$$(HEADER))),)\""
endef

%.$(OS): %.$(SS)
	@$(eval $(call IDENT_HELPER,$(<))) $(call msg,echo -n "Compiling \`$(subst $(DIR_ROOT)/,,$(<))' ... ";) \
	/bin/rm -f "$(@)" && \
	$(DXX) $(CXXFLAGS) -MM $(<) -MT $(@) -MT $(@:.$(OS)=.$(DS)) -MF $(@:.$(OS)=.$(DS)) && \
	$(call invoke,$(CXX) $(CXXFLAGS) -D__ID__=$(ID) -D__TID__=$(TID) $(<) -c -o $(@) 2>&1 | tee -a make.log) && \
	test -f $(@) \
	$(call msg,&& echo $(NONL) "done.$(CL)")

