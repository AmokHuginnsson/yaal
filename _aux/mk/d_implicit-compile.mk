# implict pattern rule

%.$(OS): %.$(SS)
	@$(call msg,echo -n "Compiling \`$(subst $(DIR_ROOT)/,,$(<))' ... ";) \
	/bin/rm -f "$(@)" && \
	$(DXX) $(CXXFLAGS) -MM $(<) -MT $(@) -MT $(@:.$(OS)=.$(DS)) -MF $(@:.$(OS)=.$(DS)) && \
	$(call invoke,$(CXX) $(CXXFLAGS) -D__ID__="\"$(<) $(shell $(GITID) ../$(subst $(DIR_ROOT)/,,$(<)))\"" $(<) -c -o $(@) 2>&1 | tee -a make.log) && \
	test -f $(@) \
	$(call msg,&& echo $(NONL) "done.$(CL)")

