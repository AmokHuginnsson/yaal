# implict pattern rule

%.$(OS): %.$(SS)
	@echo -n "Compiling \`$(subst $(DIR_ROOT)/,,$(<))' ... "; \
	echo -n "$(@:.$(OS)=.$(DS)) " > $(@:.$(OS)=.$(DS)); \
	$(DXX) $(CXXFLAGS) -MM $(<) -MT $(@) | grep -v '^#' >> $(@:.$(OS)=.$(DS)); \
	$(CXX) $(CXXFLAGS) $(<) -c -o $(@) 2>&1 | tee -a make.log; \
	echo $(NONL) "done.$(CL)"

