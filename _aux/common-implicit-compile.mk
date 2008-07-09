# implict pattern rule

%.$(OS): %.$(SS)
	@echo -n "Compiling \`$(subst $(DIR_ROOT)/,,$(<))' ... "; \
	echo -n "$(@:.$(OS)=.$(DS)) " > $(@:.$(OS)=.$(DS)); \
	/bin/rm -f "$(@)"; \
	$(DXX) $(CXXFLAGS) -MM $(<) -MT $(@) | grep -v '^#' >> $(@:.$(OS)=.$(DS)) && test $${PIPESTATUS} == 0 && \
	$(CXX) $(CXXFLAGS) $(<) -c -o $(@) 2>&1 | tee -a make.log && test $${PIPESTATUS} == 0 && \
	echo $(NONL) "done.$(CL)"

