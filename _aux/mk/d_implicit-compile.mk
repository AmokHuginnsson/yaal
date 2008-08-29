# implict pattern rule

%.$(OS): %.$(SS)
	@echo -n "Compiling \`$(subst $(DIR_ROOT)/,,$(<))' ... "; \
	/bin/rm -f "$(@)"; \
	$(DXX) $(CXXFLAGS) -MM $(<) -MT $(@) -MT $(@:.$(OS)=.$(DS)) -MF $(@:.$(OS)=.$(DS)) && \
	$(CXX) $(CXXFLAGS) -D__ID__="\"$(<) $(shell $(GITID) ../$(subst $(DIR_ROOT)/,,$(<)))\"" $(<) -c -o $(@) 2>&1 | tee -a make.log && \
	test -f $(@) && \
	echo $(NONL) "done.$(CL)"

