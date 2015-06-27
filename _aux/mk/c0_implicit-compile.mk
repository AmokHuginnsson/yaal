# implict pattern rule

%.$(DS): %.$(SS)
	@$(call msg_always,printf "%b" "$($(CURR_DEP_PROGRESS_INDICATOR))$(DEP_PROGRESS_INDICATOR_SUFFIX)$(eval $(call PROGRESS_INDICATOR))" && ) \
	/bin/rm -f "$(@)" && \
	$(DXX) $(CXXFLAGS) -MM $(<) -MT $(@:.$(DS)=.$(OS)) -MT $(@) -MF $(@) \
	$(call msg,&& printf "%b" "$(DEP_CL)")

%.$(OS): %.$(SS)
	@$(eval $(call IDENT_HELPER,$(<))) $(call msg,printf "[%3d%%] %b" "$(shell expr $(words $(CUR_COUNT)) \* 100 / $(TOTAL))" "$($(CURR_PROGRESS_INDICATOR))$(eval $(call PROGRESS_INDICATOR))Compiling \`$(subst $(DIR_ROOT)/,,$(<))' ... " && ) \
	/bin/rm -f "$(@)" && \
	$(call invoke,$(CXX) $(CXXFLAGS) -D__ID__=$(ID) -D__TID__=$(TID) $(<) -c -o $(@) 2>&1 | tee -a make.log) && \
	test -f $(@) \
	$(call msg,&& printf "%b$(NL)" "done.$(CL)")

